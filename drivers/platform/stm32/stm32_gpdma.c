/***************************************************************************//**
 *   @file   stm32_gpdma.c
 *   @brief  Implementation of STM32 GPDMA functionality.
 * 	 @author Swaroop P Kukkillaya (swaroop.kukkillaya@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "stm32_gpdma.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "stm32_irq.h"

struct stm32_dma_ch_priv_data {
	DMA_NodeTypeDef *nodes;
	DMA_QListTypeDef *llist;
};

/**
* @brief There is only one DMA controller
*/
static struct no_os_dma_desc *gpdma_descriptor;

/**
 * @brief Fill the GPDMA channel trigger structure for a transfer.
 * @param sdma_ch - The STM32 GPDMA channel descriptor.
 * @param trig - The filled trigger configuration.
 * @return 0 in case of success, negative error codes otherwise
 */
static int stm32_gpdma_trigger_config(struct stm32_dma_channel *sdma_ch,
				      DMA_TriggerConfTypeDef *trig)
{
	if (!sdma_ch || !trig)
		return -EINVAL;

	if (sdma_ch->trig) {
		switch (sdma_ch->trig->mode) {
		case STM32_DMA_BLOCK_XFER_MODE:
			trig->TriggerMode = DMA_TRIGM_BLOCK_TRANSFER;
			break;
		case STM32_DMA_REP_BLOCK_XFER_MODE:
			trig->TriggerMode = DMA_TRIGM_REPEATED_BLOCK_TRANSFER;
			break;
		case STM32_DMA_LLI_LINK_XFER_MODE:
			trig->TriggerMode = DMA_TRIGM_LLI_LINK_TRANSFER;
			break;
		case STM32_DMA_SINGLE_BURST_MODE:
			trig->TriggerMode = DMA_TRIGM_SINGLE_BURST_TRANSFER;
			break;
		default:
			return -EINVAL;
		}
		switch (sdma_ch->trig->polarity) {
		case STM32_DMA_TRIG_MASKED:
			trig->TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
			break;
		case STM32_DMA_TRIG_RISING:
			trig->TriggerPolarity = DMA_TRIG_POLARITY_RISING;
			break;
		case STM32_DMA_TRIG_FALLING:
			trig->TriggerPolarity = DMA_TRIG_POLARITY_FALLING;
			break;
		default:
			return -EINVAL;
		}
		trig->TriggerSelection = sdma_ch->trig->id;
	}

	return 0;
}

/**
 * @brief Configure GPDMA channel trigger for a transfer.
 * @param sdma_ch - The STM32 GPDMA channel descriptor.
 * @param node_config - Configuration for the nodes in the linked list.
 * @param num_nodes - Number of nodes in the linked list.
 * @return 0 in case of success, negative error codes otherwise
 */
static int stm32_gpdma_llist_config(struct stm32_dma_channel *sdma_ch,
				    DMA_NodeConfTypeDef *node_config,
				    uint32_t num_nodes)
{
	int i;
	struct stm32_dma_ch_priv_data *ch_priv_data;
	ch_priv_data = sdma_ch->priv_data;

	/* Remove all the previous nodes */
	if (ch_priv_data) {
		/* Reset, Unlink and DeInit the previous list */
		if (ch_priv_data->llist) {
			if (HAL_DMAEx_List_ResetQ(ch_priv_data->llist) != HAL_OK)
				return -EINVAL;
			if (HAL_DMAEx_List_DeInit(sdma_ch->hdma) != HAL_OK)
				return -EINVAL;

			no_os_free(ch_priv_data->llist);
			ch_priv_data->llist = NULL;
		}
		if (ch_priv_data->nodes) {
			no_os_free(ch_priv_data->nodes);
			ch_priv_data->nodes = NULL;
		}
	} else {
		sdma_ch->priv_data = (void *) no_os_calloc(1,
				     sizeof(struct stm32_dma_ch_priv_data));
		ch_priv_data = sdma_ch->priv_data;
	}

	ch_priv_data->llist = (DMA_QListTypeDef *) no_os_calloc(1,
			      sizeof(*ch_priv_data->llist));
	ch_priv_data->nodes = (DMA_NodeTypeDef *) no_os_calloc(num_nodes,
			      sizeof(*ch_priv_data->nodes));

	for (i = 0; i < num_nodes; i++) {
		if (HAL_DMAEx_List_BuildNode(&node_config[i],
					     &ch_priv_data->nodes[i]) != HAL_OK) {
			return -EINVAL;
		}
		if (HAL_DMAEx_List_InsertNode(ch_priv_data->llist, NULL,
					      &ch_priv_data->nodes[i]) != HAL_OK) {
			return -EINVAL;
		}
	}
	if (HAL_DMAEx_List_SetCircularMode(ch_priv_data->llist) != HAL_OK) {
		return -EINVAL;
	}

	sdma_ch->hdma->InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;
	if (HAL_DMAEx_List_Init(sdma_ch->hdma) != HAL_OK) {
		return -EINVAL;
	}
	if (HAL_DMAEx_List_LinkQ(sdma_ch->hdma, ch_priv_data->llist) != HAL_OK) {
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Configure a General Purpose DMA channel alignment for a transfer.
 * @param sdma_ch - The STM32 GPDMA channel descriptor.
 * @param xfer - Descriptor for the transfer.
 * @return 0 in case of success, negative error codes otherwise
 */
static int stm32_gpdma_fill_xfer_alignment(struct stm32_dma_channel *sdma_ch,
		struct no_os_dma_xfer_desc *xfer)
{
	sdma_ch->hdma->Instance = (DMA_Channel_TypeDef *) sdma_ch->ch_num;

	/* Set the direction to be Memory-to-Peripheral */
	sdma_ch->hdma->Init.Direction = DMA_MEMORY_TO_PERIPH;

	switch (xfer->xfer_type) {
	case MEM_TO_MEM:
		sdma_ch->hdma->Init.Direction = DMA_MEMORY_TO_MEMORY;
	case MEM_TO_DEV:
		switch (sdma_ch->per_data_alignment) {
		case DATA_ALIGN_BYTE:
			sdma_ch->hdma->Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
			break;
		case DATA_ALIGN_HALF_WORD:
			sdma_ch->hdma->Init.DestDataWidth = DMA_DEST_DATAWIDTH_HALFWORD;
			break;
		case DATA_ALIGN_WORD:
			sdma_ch->hdma->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
			break;
		default:
			return -EINVAL;
		}

		switch (sdma_ch->mem_data_alignment) {
		case DATA_ALIGN_BYTE:
			sdma_ch->hdma->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
			break;
		case DATA_ALIGN_HALF_WORD:
			sdma_ch->hdma->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_HALFWORD;
			break;
		case DATA_ALIGN_WORD:
			sdma_ch->hdma->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
			break;
		default:
			return -EINVAL;
		}
		sdma_ch->hdma->Init.SrcInc = sdma_ch->mem_increment ? DMA_SINC_INCREMENTED :
					     DMA_SINC_FIXED;
		sdma_ch->hdma->Init.DestInc = sdma_ch->per_increment ? DMA_DINC_INCREMENTED :
					      DMA_DINC_FIXED;
		break;
	case DEV_TO_MEM:
		sdma_ch->hdma->Init.Direction = DMA_PERIPH_TO_MEMORY;

		switch (sdma_ch->mem_data_alignment) {
		case DATA_ALIGN_BYTE:
			sdma_ch->hdma->Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
			break;
		case DATA_ALIGN_HALF_WORD:
			sdma_ch->hdma->Init.DestDataWidth = DMA_DEST_DATAWIDTH_HALFWORD;
			break;
		case DATA_ALIGN_WORD:
			sdma_ch->hdma->Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
			break;
		default:
			return -EINVAL;
		}

		switch (sdma_ch->per_data_alignment) {
		case DATA_ALIGN_BYTE:
			sdma_ch->hdma->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
			break;
		case DATA_ALIGN_HALF_WORD:
			sdma_ch->hdma->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_HALFWORD;
			break;
		case DATA_ALIGN_WORD:
			sdma_ch->hdma->Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
			break;
		default:
			return -EINVAL;
		}
		sdma_ch->hdma->Init.SrcInc = sdma_ch->per_increment ? DMA_SINC_INCREMENTED :
					     DMA_SINC_FIXED;
		sdma_ch->hdma->Init.DestInc = sdma_ch->mem_increment ? DMA_DINC_INCREMENTED :
					      DMA_DINC_FIXED;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Configure a General Purpose DMA channel for a transfer.
 * @param channel - The GPDMA channel descriptor.
 * @param xfer - Descriptor for the transfer.
 * @return 0 in case of success, negative error codes otherwise
 */
int stm32_gpdma_config_xfer(struct no_os_dma_ch *channel,
			    struct no_os_dma_xfer_desc *xfer)
{
	int ret;
	struct stm32_dma_channel* sdma_ch;
	DMA_NodeConfTypeDef xfer_node_config = {0};
	DMA_TriggerConfTypeDef trigger_config;

	if (!channel || !xfer || !channel->extra)
		return -EINVAL;

	sdma_ch = channel->extra;

	ret = stm32_gpdma_fill_xfer_alignment(sdma_ch, xfer);
	if (ret)
		return ret;

	ret = stm32_gpdma_trigger_config(sdma_ch, &trigger_config);
	if (ret)
		return ret;

	switch (sdma_ch->dma_mode) {
	case DMA_NORMAL_MODE:
		sdma_ch->hdma->Init.Mode = DMA_NORMAL;

		sdma_ch->src = xfer->src;
		sdma_ch->dst = xfer->dst;
		sdma_ch->length = xfer->length;

		ret = HAL_DMA_Init(sdma_ch->hdma);
		if (ret != HAL_OK)
			return -EINVAL;

		if (sdma_ch->trig) {
			ret = HAL_DMAEx_ConfigTrigger(sdma_ch->hdma, &trigger_config);
			if (ret != HAL_OK)
				return -EINVAL;
		}
		break;
	case DMA_CIRCULAR_MODE:
		xfer_node_config.NodeType = DMA_GPDMA_LINEAR_NODE;
		xfer_node_config.Init = sdma_ch->hdma->Init;

		xfer_node_config.SrcAddress = (uint32_t) xfer->src;
		xfer_node_config.DstAddress = (uint32_t) xfer->dst;
		xfer_node_config.DataSize = (uint32_t) xfer->length;

		if (sdma_ch->trig) {
			xfer_node_config.TriggerConfig.TriggerMode = trigger_config.TriggerMode;
			xfer_node_config.TriggerConfig.TriggerPolarity = trigger_config.TriggerPolarity;
			xfer_node_config.TriggerConfig.TriggerSelection =
				trigger_config.TriggerSelection;
		}

		ret = stm32_gpdma_llist_config(sdma_ch, &xfer_node_config, 1);
		if (ret)
			return -EINVAL;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Initialize a General Purpose DMA controller.
 * @param desc - Descriptor to be initialized.
 * @param param - Initialization parameter for the decriptor.
 * @return 0 in case of success
 * 	   -ENOMEM if there is not enough free memory
 */
int stm32_gpdma_init(struct no_os_dma_desc** desc,
		     struct no_os_dma_init_param* param)
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

	if (gpdma_descriptor) {
		/*
		 * There is only one DMA controller and it's already
		 * initialized.
		 */
		*desc = gpdma_descriptor;
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
	descriptor->sg_handler = param->sg_handler;

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
 * @brief Start a General Purpose DMA transfer for a specific channel.
 * @param desc - Descriptor for the GPDMA controller.
 * @param chan - The GPDMA Channel
 * @return 0 in case of success, negative error codes otherwise
 */
int stm32_gpdma_xfer_start(struct no_os_dma_desc *desc,
			   struct no_os_dma_ch *chan)
{
	int ret;
	struct stm32_dma_channel* sdma_chan;

	if (!desc || !chan || !chan->extra)
		return -EINVAL;

	sdma_chan = chan->extra;

	switch (sdma_chan->dma_mode) {
	case DMA_CIRCULAR_MODE:
		if (chan->irq_num) {
			ret = HAL_DMAEx_List_Start_IT(sdma_chan->hdma);
		} else {
			ret = HAL_DMAEx_List_Start(sdma_chan->hdma);
		}
		break;
	case DMA_NORMAL_MODE:
		if (chan->irq_num) {
			ret = HAL_DMA_Start_IT(sdma_chan->hdma, (uint32_t)sdma_chan->src,
					       (uint32_t)sdma_chan->dst, sdma_chan->length);
		} else {
			ret = HAL_DMA_Start(sdma_chan->hdma, (uint32_t)sdma_chan->src,
					    (uint32_t)sdma_chan->dst, sdma_chan->length);
		}
		break;
	default:
		return -EINVAL;
	}

	if (ret != HAL_OK)
		return -EINVAL;

	return 0;
}

/**
 * @brief Disable a General Purpose DMA channel.
 * @param desc - Descriptor for the GPDMA controller.
 * @param chan - The GPDMA Channel
 * @return 0 in case of success, negative error codes otherwise
 */
int stm32_gpdma_xfer_abort(struct no_os_dma_desc *desc,
			   struct no_os_dma_ch *chan)
{
	int ret;
	struct stm32_dma_channel *sdma_chan;

	if (!desc || !chan || !chan->extra)
		return -EINVAL;

	sdma_chan = chan->extra;

	ret = HAL_DMA_Abort(sdma_chan->hdma);

	if (ret != HAL_OK)
		return -EINVAL;

	return 0;
}

/**
 * @brief Free the resources allocated for a General Purpose DMA descriptor.
 * @param desc - Descriptor to be freed.
 * @return 0 in case of success, negative error code otherwise
 */
int stm32_gpdma_remove(struct no_os_dma_desc* desc)
{
	no_os_irq_ctrl_remove(desc->irq_ctrl);

	no_os_free(desc->channels);

	no_os_free(desc);

	gpdma_descriptor = NULL;

	return 0;
}

/**
 * @brief stm32 platform specific General Purpose DMA platform ops structure
 */
const struct no_os_dma_platform_ops stm32_gpdma_ops = {
	.dma_init = stm32_gpdma_init,
	.dma_config_xfer = stm32_gpdma_config_xfer,
	.dma_xfer_start = stm32_gpdma_xfer_start,
	.dma_xfer_abort = stm32_gpdma_xfer_abort,
	.dma_remove = stm32_gpdma_remove
};

