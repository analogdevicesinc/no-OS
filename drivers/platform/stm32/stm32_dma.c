/***************************************************************************//**
 *   @file   stm32/stm32_dma.c
 *   @brief  Implementation of stm32 DMA functionality.
 * 	 @author Janani Sunil (janani.sunil@analog.com)
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

#include "stm32_dma.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "stm32_irq.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
* @brief There is only one DMA controller
*/
static struct no_os_dma_desc *dma_descriptor;

/**
 * @brief Configure a DMA channel for a transfer.
 * @param channel - The DMA channel descriptor.
 * @param xfer - Descriptor for the transfer.
 * @return 0 in case of success, negative error codes otherwise
 */
int stm32_dma_config_xfer(struct no_os_dma_ch *channel,
			  struct no_os_dma_xfer_desc *xfer)
{
	struct stm32_dma_channel* sdma_ch;
	int ret;

	if (!channel || !xfer || !channel->extra)
		return -EINVAL;

	sdma_ch = channel->extra;

	/* Note: Channel number is assigned via the Instance for MCU
	 * families other than STM32F2, STM32F4 and STM32F7 */
#if defined (STM32F2) || defined (STM32F4) || defined (STM32F7)
	sdma_ch->hdma->Init.Channel = sdma_ch->ch_num;
#else
	sdma_ch->hdma->Instance = sdma_ch->ch_num;
#endif
	sdma_ch->hdma->Init.MemInc = sdma_ch->mem_increment ? DMA_MINC_ENABLE :
				     DMA_MINC_DISABLE;
	sdma_ch->hdma->Init.PeriphInc = sdma_ch->per_increment ? DMA_PINC_ENABLE :
					DMA_PINC_DISABLE;

	switch (sdma_ch->mem_data_alignment) {
	case DATA_ALIGN_BYTE:
		sdma_ch->hdma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		break;
	case DATA_ALIGN_HALF_WORD:
		sdma_ch->hdma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		break;
	case DATA_ALIGN_WORD:
		sdma_ch->hdma->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		break;
	default:
		return -EINVAL;
	}

	switch (sdma_ch->per_data_alignment) {
	case DATA_ALIGN_BYTE:
		sdma_ch->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		break;
	case DATA_ALIGN_HALF_WORD:
		sdma_ch->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		break;
	case DATA_ALIGN_WORD:
		sdma_ch->hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		break;
	default:
		return -EINVAL;
	}

	switch (sdma_ch->dma_mode) {
	case DMA_NORMAL_MODE:
		sdma_ch->hdma->Init.Mode = DMA_NORMAL;
		break;
	case DMA_CIRCULAR_MODE:
		sdma_ch->hdma->Init.Mode = DMA_CIRCULAR;
		break;
	default:
		return -EINVAL;
	}

	switch (xfer->xfer_type) {
	case MEM_TO_MEM:
		sdma_ch->hdma->Init.Direction = DMA_MEMORY_TO_MEMORY;
		break;
	case MEM_TO_DEV:
		sdma_ch->hdma->Init.Direction = DMA_MEMORY_TO_PERIPH;
		break;
	case DEV_TO_MEM:
		sdma_ch->hdma->Init.Direction = DMA_PERIPH_TO_MEMORY;
		break;
	default:
		return -EINVAL;
	}

	sdma_ch->src = xfer->src;
	sdma_ch->dst = xfer->dst;
	sdma_ch->length = xfer->length;

	ret = HAL_DMA_Init(sdma_ch->hdma);
	if (ret != HAL_OK)
		return -EINVAL;

	return 0;
}

/**
 * @brief Initialize a DMA controller.
 * @param desc - Descriptor to be initialized.
 * @param param - Initialization parameter for the decriptor.
 * @return 0 in case of success
 * 	   -ENOMEM if there is not enough free memory
 */
int stm32_dma_init(struct no_os_dma_desc** desc,
		   const struct no_os_dma_init_param* param)
{

	struct no_os_dma_desc* descriptor;
	int ret;
	uint8_t i;
	struct no_os_irq_init_param irq_param = {
		.irq_ctrl_id = 0,
		.platform_ops = &stm32_irq_ops,
		.extra = NULL
	};

	if (!desc)
		return -EINVAL;

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

	descriptor->id = param->id;
	descriptor->num_ch = param->num_ch;

	for (i = 0; i < param->num_ch; i++) {
		descriptor->channels[i].free = true;
	}

	no_os_irq_ctrl_init(&descriptor->irq_ctrl, &irq_param);

	*desc = descriptor;

	return 0;

free_descriptor:
	no_os_free(descriptor);
	descriptor = NULL;

	return ret;
}

/**
 * @brief Start a DMA transfer for a specific channel .
 * @param desc - Descriptor for the DMA controller.
 * @param chan - The DMA Channel
 * @return 0 in case of success, negative error codes otherwise
 */
int stm32_dma_xfer_start(struct no_os_dma_desc *desc,
			 struct no_os_dma_ch *chan)
{
	int ret;
	struct stm32_dma_channel* sdma_chan;

	if (!desc || !chan || !chan->extra)
		return -EINVAL;

	sdma_chan = chan->extra;

	if (chan->irq_num) {
		ret = HAL_DMA_Start_IT(sdma_chan->hdma, (uint32_t)sdma_chan->src,
				       (uint32_t )sdma_chan->dst, sdma_chan->length);
	} else {
		ret = HAL_DMA_Start(sdma_chan->hdma, (uint32_t )sdma_chan->src,
				    (uint32_t )sdma_chan->dst, sdma_chan->length);
	}

	if (ret != HAL_OK)
		return -EINVAL;

	return 0;
}

/**
 * @brief Disable a DMA channel.
 * @param desc - Descriptor for the DMA controller.
 * @param chan - The DMA Channel
 * @return 0 in case of success, negative error codes otherwise
 */
int stm32_dma_xfer_abort(struct no_os_dma_desc* desc,
			 struct no_os_dma_ch* chan)
{
	int ret;
	struct stm32_dma_channel* sdma_chan;

	if (!desc || !chan || !chan->extra)
		return -EINVAL;

	sdma_chan = chan->extra;

	if (chan->irq_num)
		ret = HAL_DMA_Abort_IT(sdma_chan->hdma);
	else
		ret = HAL_DMA_Abort(sdma_chan->hdma);

	if (ret != HAL_OK)
		return -EINVAL;

	return 0;
}

/**
 * @brief Free the resources allocated for a DMA descriptor.
 * @param desc - Descriptor to be freed.
 * @return 0 in case of success, negative error code otherwise
 */
int stm32_dma_remove(struct no_os_dma_desc* desc)
{
	no_os_irq_ctrl_remove(desc->irq_ctrl);

	no_os_free(desc->channels);

	no_os_free(desc);

	dma_descriptor = NULL;

	return 0;
}

/**
 * @brief stm32 platform specific DMA platform ops structure
 */
const struct no_os_dma_platform_ops stm32_dma_ops = {
	.dma_init = stm32_dma_init,
	.dma_config_xfer = stm32_dma_config_xfer,
	.dma_xfer_start = stm32_dma_xfer_start,
	.dma_xfer_abort = stm32_dma_xfer_abort,
	.dma_remove = stm32_dma_remove
};

