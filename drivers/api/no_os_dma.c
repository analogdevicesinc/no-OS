/***************************************************************************//**
 *   @file   no_os_dma.c
 *   @brief  Platform independent implementation for the DMA API.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include <inttypes.h>
#include "no_os_dma.h"
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_mutex.h"
#include "no_os_irq.h"
#include "no_os_alloc.h"
#include "no_os_list.h"

/**
 * @brief Default handler for cycling though the channel's list of transfers
 * @param context - structure which stores the state of the current channel
 * 		    and DMA controller state.
 */
static void default_sg_callback(void *context)
{
	struct no_os_dma_default_handler_data *data = context;
	struct no_os_dma_xfer_desc *next_xfer;
	struct no_os_dma_xfer_desc *old_xfer;
	uint32_t list_size;
	int ret;

	/* Handle the next transfer from the SG list */
	ret = no_os_list_get_first(data->channel->sg_list, (void **)&old_xfer);
	if (ret) {
		/*
		 * This should only happen if the list descriptor is NULL.
		 * The case in which there is no transfer left in the list should
		 * have been handled in the previous interrupt.
		 */
		no_os_dma_xfer_abort(data->desc, data->channel);
		return;
	}

	no_os_list_read_first(data->channel->sg_list, (void **)&next_xfer);
	no_os_list_get_size(data->channel->sg_list, &list_size);
	if (old_xfer->xfer_complete_cb)
		old_xfer->xfer_complete_cb(old_xfer, next_xfer,
					   old_xfer->xfer_complete_ctx);

	if (!list_size) {
		no_os_irq_disable(data->desc->irq_ctrl, data->channel->irq_num);
		data->channel->free = true;
		return;
	}

	data->desc->platform_ops->dma_config_xfer(data->channel, next_xfer);

	no_os_dma_xfer_start(data->desc, data->channel);
}

/**
 * @brief Initialize the DMA controller.
 * @param desc - Structure containing the state of the DMA controller
 * @param param - Initialization parameter for the DMA controller.
 * @return 0 in case of success, negative error code otherwise.
 */
int no_os_dma_init(struct no_os_dma_desc **desc,
		   struct no_os_dma_init_param *param)
{
	int ret;
	uint32_t i, j;
	void *mutex;

	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->dma_init)
		return -ENOSYS;

	no_os_mutex_init(&mutex);

	no_os_mutex_lock(mutex);
	ret = param->platform_ops->dma_init(desc, param);
	if (ret)
		goto unlock;

	no_os_mutex_init(&(*desc)->mutex);

	(*desc)->platform_ops = param->platform_ops;

	for (i = 0; i < param->num_ch; i++) {
		ret = no_os_list_init(&(*desc)->channels[i].sg_list, NO_OS_LIST_QUEUE, NULL);
		if (ret)
			goto list_err;

		no_os_mutex_init(&(*desc)->channels[i].mutex);
	}

	(*desc)->ref++;
	no_os_mutex_unlock(mutex);

	return 0;

list_err:
	for (j = 0; j < i; j++)
		no_os_list_remove((*desc)->channels[i].sg_list);

	no_os_dma_remove(*desc);
unlock:
	no_os_mutex_unlock(mutex);

	return ret;
}

/**
 * @brief Remove resources allocated for the DMA controller.
 * @param desc - Structure containing the state of the DMA controller
 * @return 0 in case of success, negative error code otherwise.
 */
int no_os_dma_remove(struct no_os_dma_desc *desc)
{
	uint32_t i;
	int ret;

	if (!desc)
		return -EINVAL;

	if (!desc->ref)
		return 0;

	for (i = 0; i < desc->num_ch; i++) {
		ret = no_os_list_remove(desc->channels->sg_list);
		if (ret)
			return ret;

		no_os_mutex_remove(desc->channels[i].mutex);
		if (desc->irq_ctrl && desc->channels[i].cb_desc.handle) {
			no_os_irq_unregister_callback(desc->irq_ctrl,
						      desc->channels[i].irq_num,
						      &desc->channels[i].cb_desc);
		}

	}
	no_os_mutex_remove(desc->mutex);

	ret = desc->platform_ops->dma_remove(desc);
	if (ret)
		return ret;

	desc->ref--;

	return 0;
}

/**
 * @brief Get a free DMA channel.
 * @param desc - Structure containing the state of the DMA controller.
 * @param channel - Reference to the acquired channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int no_os_dma_acquire_channel(struct no_os_dma_desc *desc,
			      struct no_os_dma_ch **channel)
{
	uint32_t ch_num;
	int ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->dma_acquire_ch)
		return -ENOSYS;

	no_os_mutex_lock(desc->mutex);

	ret = desc->platform_ops->dma_acquire_ch(desc, &ch_num);
	if (ret)
		goto unlock;

	*channel = &desc->channels[ch_num];

unlock:
	no_os_mutex_unlock(desc->mutex);

	return ret;
}

/**
 * @brief Free DMA channel.
 * @param desc - Structure containing the state of the DMA controller.
 * @param channel - Reference to the currently acquired channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int no_os_dma_release_channel(struct no_os_dma_desc *desc,
			      struct no_os_dma_ch *channel)
{
	int ret;

	if (!desc || !desc->platform_ops || !channel)
		return -EINVAL;

	if (!desc->platform_ops->dma_release_ch)
		return -ENOSYS;

	no_os_mutex_lock(channel->mutex);
	ret = desc->platform_ops->dma_release_ch(desc, channel->id);
	no_os_mutex_unlock(channel->mutex);

	return ret;
}

/**
 * @brief Acquire a channel and configure the list of transfers.
 * @param desc - Structure containing the state of the DMA controller
 * @param xfer - Array of DMA transfers.
 * @param len - The number of transfers in the xfer list.
 * @param ch - Previously acquired channel, for which the transfer will be configured.
 * @return 0 in case of success, negative error code otherwise.
 */
int no_os_dma_config_xfer(struct no_os_dma_desc *desc,
			  struct no_os_dma_xfer_desc *xfer,
			  uint32_t len, struct no_os_dma_ch *ch)
{
	uint32_t i;
	int ret;
	void *discard;
	struct no_os_callback_desc *sg_callback;

	if (!desc || !xfer || !len || !ch)
		return -EINVAL;

	no_os_mutex_lock(ch->mutex);

	/*
	 * Add the transfers to the channel's SG list. It's safe to do so, since
	 * there are no ongoing transfers on this channel.
	 */
	for (i = 0; i < len; i++)
		no_os_list_add_last(ch->sg_list, &xfer[i]);

	if (desc->irq_ctrl) {
		sg_callback = &ch->cb_desc;
		sg_callback->ctx = &ch->irq_ctx;
		sg_callback->handle = (void *)ch->id;
		sg_callback->peripheral = xfer[0].periph;
		ch->irq_ctx.desc = desc;
		ch->irq_ctx.channel = ch;

		switch (xfer[0].xfer_type) {
		case MEM_TO_DEV:
		case MEM_TO_MEM:
			sg_callback->event = NO_OS_EVT_DMA_TX_COMPLETE;
			break;
		case DEV_TO_MEM:
			sg_callback->event = NO_OS_EVT_DMA_RX_COMPLETE;
			break;
		default:
			ret = -EINVAL;
			goto err;
		}

		ret = desc->platform_ops->dma_config_xfer(ch, xfer);
		if (ret)
			goto err;

		if (desc->sg_handler)
			sg_callback->callback = desc->sg_handler;
		else
			sg_callback->callback = default_sg_callback;

		ret = no_os_irq_register_callback(desc->irq_ctrl,
						  ch->irq_num,
						  sg_callback);
		if (ret)
			goto err;

		no_os_irq_set_priority(desc->irq_ctrl, ch->irq_num, xfer[0].irq_priority);
	}

	no_os_mutex_unlock(ch->mutex);
	return 0;
err:
	for (i = 0; i < len; i++)
		no_os_list_get_first(ch->sg_list, &discard);
	no_os_mutex_unlock(ch->mutex);

	return ret;
}

/**
 * @brief Lock a DMA channel, so it won't be acquired even if it's free.
 * @param ch - Reference to the DMA channel
 * @return 0 in case of success, -EINVAL if a NULL channel reference is provided.
 */
int no_os_dma_chan_lock(struct no_os_dma_ch *ch)
{
	no_os_mutex_lock(ch->mutex);

	if (!ch)
		return -EINVAL;

	ch->sync_lock = true;

	no_os_mutex_unlock(ch->mutex);

	return 0;
}

/**
 * @brief Unlock a DMA channel, marking it available for acquisition.
 * @param ch - Reference to the DMA channel
 * @return 0 in case of success, -EINVAL if a NULL channel reference is provided.
 */
int no_os_dma_chan_unlock(struct no_os_dma_ch *ch)
{
	no_os_mutex_lock(ch->mutex);

	if (!ch)
		return -EINVAL;

	ch->sync_lock = false;

	no_os_mutex_unlock(ch->mutex);

	return 0;
}

/**
 * @brief Acquire a channel and configure the list of transfers.
 * @param desc - Structure containing the state of the DMA controller
 * @param ch - Index for the channel we want to start the transfer on.
 * @return 0 in case of success, negative error code otherwise.
 */
int no_os_dma_xfer_start(struct no_os_dma_desc *desc, struct no_os_dma_ch *ch)
{
	int ret;

	if (!desc || !desc->platform_ops || !ch)
		return -EINVAL;

	if (!desc->platform_ops->dma_xfer_start)
		return -ENOSYS;

	no_os_mutex_lock(ch->mutex);

	if (desc->irq_ctrl)
		no_os_irq_enable(desc->irq_ctrl, ch->irq_num);

	ret = desc->platform_ops->dma_xfer_start(desc, ch);

	no_os_mutex_unlock(ch->mutex);

	return ret;
}

/**
 * @brief Abort the ongoing and any other future transfers scheduled for a channel.
 * @param desc - Structure containing the state of the DMA controller
 * @param ch - Index for the channel we want to abort the transfer.
 * @return 0 in case of success, negative error code otherwise.
 */
int no_os_dma_xfer_abort(struct no_os_dma_desc *desc, struct no_os_dma_ch *ch)
{
	void *discard;
	int ret;

	if (!desc || !desc->platform_ops || !ch)
		return -EINVAL;

	if (!desc->platform_ops->dma_xfer_abort)
		return -ENOSYS;

	no_os_mutex_lock(ch->mutex);

	if (desc->irq_ctrl)
		no_os_irq_disable(desc->irq_ctrl, ch->irq_num);

	do {
		ret = no_os_list_get_first(ch->sg_list, &discard);
	} while (!ret);

	ret = desc->platform_ops->dma_xfer_abort(desc, ch);

	no_os_mutex_unlock(ch->mutex);

	return ret;
}

/**
 * @brief Get the state of a DMA channel (free or not).
 * @param desc - Reference to the DMA controller.
 * @param ch - Reference to the DMA channel.
 * @return true if a channel is free, false otherwise.
 */
bool no_os_dma_is_completed(struct no_os_dma_desc *desc,
			    struct no_os_dma_ch *ch)
{
	(void)desc;

	if (!ch)
		return -EINVAL;

	return ch->free;
}

/**
 * @brief Whether or not there is a transfer in progress on a specific channel.
 * @param desc - Reference to the DMA controller.
 * @param ch - Reference to the DMA channel.
 * @return true if a channel is free, false otherwise.
 */
bool no_os_dma_in_progress(struct no_os_dma_desc *desc, struct no_os_dma_ch *ch)
{
	int ret;

	if (!desc || !desc->platform_ops || !ch)
		return -EINVAL;

	if (!desc->platform_ops->dma_ch_in_progress)
		return -ENOSYS;

	no_os_mutex_lock(ch->mutex);
	ret = desc->platform_ops->dma_ch_in_progress(desc, ch);
	no_os_mutex_unlock(ch->mutex);

	return ret;
}
