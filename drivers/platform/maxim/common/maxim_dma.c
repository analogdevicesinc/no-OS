/***************************************************************************//**
 *   @file   maxim_dma.c
 *   @brief  Maxim platform specific implementation DMA API.
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
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_util.h"
#include "no_os_irq.h"
#include "maxim_irq.h"
#include "maxim_dma.h"
#include "mxc_sys.h"
#include "dma.h"

#define MAX_DMA_GCR_RST_POS	0

/**
* @brief There is only one DMA controller
*/
static struct no_os_dma_desc *dma_descriptor;

/**
 * @brief Initialize a DMA controller.
 * @param desc - Descriptor to be initialized.
 * @param param - Initialization parameter for the decriptor.
 * @return 0 in case of success
 * 	   -ENOMEM if there is not enough free memory
 */
static int maxim_dma_init(struct no_os_dma_desc **desc,
			  struct no_os_dma_init_param *param)
{
	struct no_os_dma_desc *descriptor;
	uint32_t i;
	int ret;

	struct no_os_irq_init_param irq_param = {
		.irq_ctrl_id = 0,
		.platform_ops = &max_irq_ops,
		.extra = NULL
	};

	if (dma_descriptor) {
		/*
		 * There is only one DMA controller and it's already
		 * initialized.
		 */
		*desc = dma_descriptor;
		return 0;
	}

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->channels = no_os_calloc(param->num_ch,
					    sizeof(*descriptor->channels));
	if (!descriptor->channels) {
		ret = -ENOMEM;
		goto free_descriptor;
	}

	if (!MXC_SYS_IsClockEnabled(MXC_SYS_PERIPH_CLOCK_DMA)) {
		MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_DMA);
		MXC_SYS_Reset_Periph(MAX_DMA_GCR_RST_POS);
	}

	descriptor->id = param->id;
	descriptor->num_ch = param->num_ch;
	descriptor->extra = MAX_DMA;
	for (i = 0; i < param->num_ch; i++) {
		descriptor->channels[i].id = i;
		descriptor->channels[i].irq_num = max_dma_get_irq(descriptor->id, i);
		descriptor->channels[i].extra = (void *)&MAX_DMA->ch[i];
		descriptor->channels[i].free = true;
	}

	no_os_irq_ctrl_init(&descriptor->irq_ctrl, &irq_param);
	MAX_DMA->cn |= MAX_DMA_IRQ_EN_ALL_CH;

	*desc = descriptor;

	return 0;

free_descriptor:
	no_os_free(descriptor);
	descriptor = NULL;

	return ret;
}

/**
 * @brief Free the resources allocated for a DMA descriptor.
 * @param desc - Descriptor to be freed.
 * @return 0
 */
static int maxim_dma_remove(struct no_os_dma_desc *desc)
{
	uint32_t i;

	MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_DMA);
	MAX_DMA->cn &= MAX_DMA_IRQ_EN_ALL_CH;
	no_os_irq_ctrl_remove(desc->irq_ctrl);

	no_os_free(desc->channels);
	no_os_free(desc);
	dma_descriptor = NULL;

	return 0;
}

/**
 * @brief Free the resources allocated for a DMA descriptor.
 * @param desc - Descriptor for the DMA controller.
 * @param ch - The index of the acquired channel.
 * @return 0 if a channel was acquired
 * 	   -EBUSY if there are no free channels
 */
static int maxim_dma_acquire_free_ch(struct no_os_dma_desc *desc, uint32_t *ch)
{
	struct max_dma_ch_regs *dma_ch;
	uint32_t i;

	/* Find a free DMA channel */
	for (i = 0; i < desc->num_ch; i++) {
		dma_ch = desc->channels[i].extra;
		if (dma_ch->st & MAX_DMA_ENABLED || !desc->channels[i].free ||
		    desc->channels[i].sync_lock)
			continue;

		*ch = i;
		desc->channels[i].free = false;
		dma_ch->cfg |= MAX_DMA_IRQ_CTZ_EN;

		return 0;
	}

	return -EBUSY;
}

/**
 * @brief Free DMA channel.
 * @param desc - Structure containing the state of the DMA controller.
 * @param ch - Index in the DMA controller struct's channel array for the
 * 	       currently acquired channel.
 * @return 0 in case of success, negative error code otherwise.
 */
static int maxim_dma_release_ch(struct no_os_dma_desc *desc, uint32_t ch)
{
	struct no_os_dma_ch channel = desc->channels[ch];
	struct max_dma_ch_regs *dma_ch;

	dma_ch = channel.extra;
	dma_ch->cfg &= ~MAX_DMA_ENABLE;

	return 0;
}

/**
 * @brief Configure a DMA channel for a transfer.
 * @param channel - The DMA channel descriptor.
 * @param xfer - Descriptor for the transfer.
 * @return 0 in case of success
 */
static int maxim_dma_config_xfer(struct no_os_dma_ch *channel,
				 struct no_os_dma_xfer_desc *xfer)
{
	struct max_dma_ch_regs *dma_ch = channel->extra;

	dma_ch->src = (uint32_t)xfer->src;
	dma_ch->dst = (uint32_t)xfer->dst;
	dma_ch->cnt = xfer->length;

	/* Configure source and destination increments */
	dma_ch->cfg |= MAX_DMA_DST_INC;
	dma_ch->cfg |= MAX_DMA_SRC_INC;
	dma_ch->cfg &= ~MAX_DMA_REQSEL_MASK;

	switch (xfer->xfer_type) {
	case MEM_TO_MEM:
		break;
	case MEM_TO_DEV:
		dma_ch->cfg |= no_os_field_prep(MAX_DMA_REQSEL_MASK,
						dma_ch->dst);
		break;
	case DEV_TO_MEM:
		dma_ch->cfg |= no_os_field_prep(MAX_DMA_REQSEL_MASK,
						dma_ch->src);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Start a DMA transfer for a specific channel .
 * @param desc - Descriptor for the DMA controller.
 * @param ch - The index of the channel.
 * @return 0
 */
static int maxim_dma_xfer_start(struct no_os_dma_desc *desc,
				struct no_os_dma_ch *ch)
{
	struct max_dma_ch_regs *max_dma_ch = ch->extra;

	max_dma_ch->cfg |= MAX_DMA_ENABLE;

	return 0;
}

/**
 * @brief Disable a DMA channel.
 * @param desc - Descriptor for the DMA controller.
 * @param ch - The index of the channel.
 * @return 0
 */
static int maxim_dma_xfer_abort(struct no_os_dma_desc *desc,
				struct no_os_dma_ch *ch)
{
	struct max_dma_ch_regs *max_dma_ch = ch->extra;

	max_dma_ch->cfg &= ~MAX_DMA_ENABLE;

	return 0;
}

/**
 * @brief Whether or not the channel has an ongoing DMA transfer.
 * @param desc - DMA controller descriptor.
 * @param ch - The channel for which we want to do the checking.
 * @return true if the channel is busy, false otherwise.
 */
static bool maxim_dma_in_progress(struct no_os_dma_desc *desc,
				  struct no_os_dma_ch *ch)
{
	struct max_dma_ch_regs *dma_ch = ch->extra;

	return dma_ch->st & MAX_DMA_ENABLED;
}

/**
 * @brief Maxim platform specific callbacks for the DMA API
 */
struct no_os_dma_platform_ops max_dma_ops = {
	.dma_init = maxim_dma_init,
	.dma_remove = maxim_dma_remove,
	.dma_config_xfer = maxim_dma_config_xfer,
	.dma_xfer_start = maxim_dma_xfer_start,
	.dma_xfer_abort = maxim_dma_xfer_abort,
	.dma_acquire_ch = maxim_dma_acquire_free_ch,
	.dma_release_ch = maxim_dma_release_ch,
	.dma_ch_in_progress = maxim_dma_in_progress,
};
