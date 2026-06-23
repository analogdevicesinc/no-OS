/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stm32_capi_dma.h"
#include "capi_dma.h"
#include "capi_alloc.h"
#include <errno.h>
#include <string.h>

/**
 * @struct stm32_dma_priv_handle
 * @brief STM32 platform specific DMA private handle
 */
struct stm32_dma_priv_handle {
	/** Number of channels */
	uint32_t num_chans;
	/** Array of channel private data pointers */
	struct stm32_dma_chan_priv **chan_privs;
};

/**
 * @struct stm32_dma_chan_priv
 * @brief STM32 platform specific DMA channel private data
 */
struct stm32_dma_chan_priv {
	/** DMA HAL Handle */
	DMA_HandleTypeDef hdma;
	/** Channel Number */
	uint32_t ch_num;
	/** Memory Increment */
	bool mem_increment;
	/** Peripheral Increment */
	bool per_increment;
	/** Memory Data Alignment */
	enum stm32_capi_dma_data_alignment mem_data_alignment;
	/** Peripheral Data Alignment */
	enum stm32_capi_dma_data_alignment per_data_alignment;
	/** DMA Mode */
	enum stm32_capi_dma_mode dma_mode;
	/** Trigger configuration */
	struct stm32_capi_dma_trigger *trig;
	/** Source Address for the data */
	capi_dma_glbl_addr_t src;
	/** Destination Address for the data */
	capi_dma_glbl_addr_t dst;
	/** Transfer length in Bytes */
	size_t length;
	/** Transfer completed flag */
	volatile bool completed;
	/** Back-reference to the CAPI channel */
	struct capi_dma_chan *capi_chan;
};

#define MAX_DMA_CHANNELS 16

static struct capi_dma_handle *dma_handle_singleton = NULL;
static struct stm32_dma_chan_priv *chan_priv_map[MAX_DMA_CHANNELS];

/**
 * @brief Find channel private data by HAL DMA handle.
 * @param hdma - Pointer to the HAL DMA handle.
 * @return Pointer to the matching channel private data, or NULL if not found.
 */
static struct stm32_dma_chan_priv *find_chan_priv_by_hdma(
	DMA_HandleTypeDef *hdma)
{
	uint32_t i;

	for (i = 0; i < MAX_DMA_CHANNELS; i++) {
		if (chan_priv_map[i] && &chan_priv_map[i]->hdma == hdma)
			return chan_priv_map[i];
	}

	return NULL;
}

/**
 * @brief HAL DMA transfer complete callback.
 * @param hdma - Pointer to the HAL DMA handle that completed.
 */
static void stm32_capi_dma_xfer_cplt_callback(DMA_HandleTypeDef *hdma)
{
	struct stm32_dma_chan_priv *chan_priv;

	chan_priv = find_chan_priv_by_hdma(hdma);
	if (!chan_priv || !chan_priv->capi_chan)
		return;

	chan_priv->completed = true;

	if (chan_priv->capi_chan->xfer_complete_cb)
		chan_priv->capi_chan->xfer_complete_cb(0,
						       chan_priv->capi_chan->xfer_complete_ctx);
}

/**
 * @brief HAL DMA transfer error callback.
 * @param hdma - Pointer to the HAL DMA handle that errored.
 */
static void stm32_capi_dma_xfer_error_callback(DMA_HandleTypeDef *hdma)
{
	struct stm32_dma_chan_priv *chan_priv;

	chan_priv = find_chan_priv_by_hdma(hdma);
	if (!chan_priv)
		return;

	chan_priv->completed = true;
}

/**
 * @brief Initialize the STM32 DMA controller.
 * @param handle - Pointer to the DMA handle pointer to be allocated.
 * @param config - Pointer to the DMA configuration structure.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_init(struct capi_dma_handle **handle,
			       const struct capi_dma_config *config)
{
	struct capi_dma_handle *dma_handle;
	struct stm32_dma_priv_handle *dma_priv;

	if (!handle || !config)
		return -EINVAL;

	if (dma_handle_singleton) {
		*handle = dma_handle_singleton;
		return 0;
	}

	dma_handle = (struct capi_dma_handle *)capi_calloc(1, sizeof(*dma_handle));
	if (!dma_handle)
		return -ENOMEM;

	dma_priv = (struct stm32_dma_priv_handle *)capi_calloc(1, sizeof(*dma_priv));
	if (!dma_priv) {
		capi_free(dma_handle);
		return -ENOMEM;
	}

	dma_priv->num_chans = config->num_chans;
	dma_priv->chan_privs = (struct stm32_dma_chan_priv **)capi_calloc(
				       config->num_chans, sizeof(struct stm32_dma_chan_priv *));
	if (!dma_priv->chan_privs) {
		capi_free(dma_priv);
		capi_free(dma_handle);
		return -ENOMEM;
	}

	dma_handle->init_allocated = true;
	dma_handle->ops = config->ops;
	dma_handle->priv = dma_priv;

	dma_handle_singleton = dma_handle;
	*handle = dma_handle;

	return 0;
}

/**
 * @brief Deinitialize the STM32 DMA controller.
 * @param handle - Pointer to the DMA handle to deinitialize.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_deinit(struct capi_dma_handle *handle)
{
	struct stm32_dma_priv_handle *dma_priv;
	uint32_t i;

	if (!handle)
		return -EINVAL;

	dma_priv = handle->priv;

	if (dma_priv) {
		for (i = 0; i < dma_priv->num_chans; i++) {
			if (dma_priv->chan_privs[i]) {
				HAL_DMA_DeInit(&dma_priv->chan_privs[i]->hdma);
				capi_free(dma_priv->chan_privs[i]);
			}
		}
		capi_free(dma_priv->chan_privs);
		capi_free(dma_priv);
	}

	memset(chan_priv_map, 0, sizeof(chan_priv_map));

	if (handle->init_allocated)
		capi_free(handle);

	dma_handle_singleton = NULL;

	return 0;
}

/**
 * @brief Initialize a DMA channel.
 * @param handle - Pointer to the DMA handle.
 * @param chan_ptr - Pointer to the channel pointer to be allocated.
 * @param id - Channel identifier.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_init_chan(struct capi_dma_handle *handle,
				    struct capi_dma_chan **chan_ptr,
				    uint32_t id)
{
	struct capi_dma_chan *chan;
	struct stm32_dma_chan_priv *chan_priv;
	struct stm32_dma_priv_handle *dma_priv;

	if (!handle || !chan_ptr)
		return -EINVAL;

	dma_priv = handle->priv;
	if (!dma_priv)
		return -EINVAL;

	if (id >= dma_priv->num_chans)
		return -EINVAL;

	if (dma_priv->chan_privs[id])
		return -EBUSY;

	chan = (struct capi_dma_chan *)capi_calloc(1, sizeof(*chan));
	if (!chan)
		return -ENOMEM;

	chan_priv = (struct stm32_dma_chan_priv *)capi_calloc(1, sizeof(*chan_priv));
	if (!chan_priv) {
		capi_free(chan);
		return -ENOMEM;
	}

	chan->owned_by_app = false;
	chan->handle = handle;
	chan->id = id;
	chan->extra = chan_priv;

	chan_priv->capi_chan = chan;
	chan_priv->completed = true;

	dma_priv->chan_privs[id] = chan_priv;

	if (id < MAX_DMA_CHANNELS)
		chan_priv_map[id] = chan_priv;

	*chan_ptr = chan;

	return 0;
}

/**
 * @brief Deinitialize a DMA channel.
 * @param chan - Pointer to the DMA channel to deinitialize.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_deinit_chan(struct capi_dma_chan *chan)
{
	struct stm32_dma_chan_priv *chan_priv;
	struct stm32_dma_priv_handle *dma_priv;

	if (!chan || !chan->handle)
		return -EINVAL;

	chan_priv = chan->extra;
	dma_priv = chan->handle->priv;

	if (chan_priv) {
		HAL_DMA_DeInit(&chan_priv->hdma);

		if (chan->id < MAX_DMA_CHANNELS)
			chan_priv_map[chan->id] = NULL;

		if (dma_priv && chan->id < dma_priv->num_chans)
			dma_priv->chan_privs[chan->id] = NULL;

		capi_free(chan_priv);
	}

	if (!chan->owned_by_app)
		capi_free(chan);

	return 0;
}

/**
 * @brief Configure a DMA transfer.
 * @param chan - Pointer to the DMA channel.
 * @param xfer - Pointer to the DMA transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_config_xfer(struct capi_dma_chan *chan,
				      struct capi_dma_transfer *xfer)
{
	struct stm32_dma_chan_priv *chan_priv;
	struct stm32_dma_chan_extra_config *chan_extra;
	int ret;

	if (!chan || !xfer || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;
	chan_extra = xfer->extra;

	if (chan_extra) {
		chan_priv->ch_num = chan_extra->ch_num;
		chan_priv->mem_increment = chan_extra->mem_increment;
		chan_priv->per_increment = chan_extra->per_increment;
		chan_priv->mem_data_alignment = chan_extra->mem_data_alignment;
		chan_priv->per_data_alignment = chan_extra->per_data_alignment;
		chan_priv->dma_mode = chan_extra->dma_mode;
		chan_priv->trig = chan_extra->trig;

		if (chan_extra->hdma)
			memcpy(&chan_priv->hdma, chan_extra->hdma, sizeof(DMA_HandleTypeDef));
	}

#if defined(STM32F2) || defined(STM32F4) || defined(STM32F7)
	chan_priv->hdma.Init.Channel = chan_priv->ch_num;
#else
	chan_priv->hdma.Instance = (DMA_Channel_TypeDef *)(uintptr_t)chan_priv->ch_num;
#endif

	if (xfer->src_inc == CAPI_DMA_BYTE_INCREMENT)
		chan_priv->hdma.Init.MemInc = DMA_MINC_ENABLE;
	else
		chan_priv->hdma.Init.MemInc = DMA_MINC_DISABLE;

	if (xfer->dst_inc == CAPI_DMA_BYTE_INCREMENT)
		chan_priv->hdma.Init.PeriphInc = DMA_PINC_ENABLE;
	else
		chan_priv->hdma.Init.PeriphInc = DMA_PINC_DISABLE;

	switch (chan_priv->mem_data_alignment) {
	case CAPI_DMA_DATA_ALIGN_BYTE:
		chan_priv->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		break;
	case CAPI_DMA_DATA_ALIGN_HALF_WORD:
		chan_priv->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		break;
	case CAPI_DMA_DATA_ALIGN_WORD:
		chan_priv->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		break;
	default:
		chan_priv->hdma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		break;
	}

	switch (chan_priv->per_data_alignment) {
	case CAPI_DMA_DATA_ALIGN_BYTE:
		chan_priv->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		break;
	case CAPI_DMA_DATA_ALIGN_HALF_WORD:
		chan_priv->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		break;
	case CAPI_DMA_DATA_ALIGN_WORD:
		chan_priv->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		break;
	default:
		chan_priv->hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		break;
	}

	switch (chan_priv->dma_mode) {
	case CAPI_DMA_NORMAL_MODE:
		chan_priv->hdma.Init.Mode = DMA_NORMAL;
		break;
	case CAPI_DMA_CIRCULAR_MODE:
		chan_priv->hdma.Init.Mode = DMA_CIRCULAR;
		break;
	default:
		chan_priv->hdma.Init.Mode = DMA_NORMAL;
		break;
	}

	switch (xfer->xfer_type) {
	case CAPI_DMA_MEM_TO_MEM:
		chan_priv->hdma.Init.Direction = DMA_MEMORY_TO_MEMORY;
		break;
	case CAPI_DMA_MEM_TO_DEV:
		chan_priv->hdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
		break;
	case CAPI_DMA_DEV_TO_MEM:
		chan_priv->hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
		break;
	default:
		return -EINVAL;
	}

	chan_priv->src = xfer->src;
	chan_priv->dst = xfer->dst;
	chan_priv->length = xfer->length;
	chan_priv->completed = false;

	chan->xfer = xfer;

	chan_priv->hdma.XferCpltCallback = stm32_capi_dma_xfer_cplt_callback;
	chan_priv->hdma.XferErrorCallback = stm32_capi_dma_xfer_error_callback;

	ret = HAL_DMA_Init(&chan_priv->hdma);
	if (ret != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief Start a DMA transfer.
 * @param chan - Pointer to the DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_xfer_start(struct capi_dma_chan *chan)
{
	struct stm32_dma_chan_priv *chan_priv;
	int ret;

	if (!chan || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;
	chan_priv->completed = false;

	if (chan->irq_num) {
		ret = HAL_DMA_Start_IT(&chan_priv->hdma,
				       (uint32_t)chan_priv->src,
				       (uint32_t)chan_priv->dst,
				       chan_priv->length);
	} else {
		ret = HAL_DMA_Start(&chan_priv->hdma,
				    (uint32_t)chan_priv->src,
				    (uint32_t)chan_priv->dst,
				    chan_priv->length);
	}

	if (ret != HAL_OK)
		return -EIO;

	return 0;
}

/**
 * @brief Abort an ongoing DMA transfer.
 * @param chan - Pointer to the DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_xfer_abort(struct capi_dma_chan *chan)
{
	struct stm32_dma_chan_priv *chan_priv;
	int ret;

	if (!chan || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;

	ret = HAL_DMA_Abort(&chan_priv->hdma);

	if ((ret != HAL_OK) &&
	    (chan_priv->hdma.ErrorCode != HAL_DMA_ERROR_NO_XFER))
		return -EIO;

	chan_priv->completed = true;

	return 0;
}

/**
 * @brief Check if a DMA channel transfer is completed.
 * @param chan - Pointer to the DMA channel.
 * @return true if the transfer is completed, false otherwise.
 */
static bool stm32_capi_dma_chan_is_completed(const struct capi_dma_chan *chan)
{
	struct stm32_dma_chan_priv *chan_priv;

	if (!chan || !chan->extra)
		return true;

	chan_priv = chan->extra;

	return chan_priv->completed;
}

/**
 * @brief DMA controller interrupt handler.
 * @param handle - Pointer to the DMA handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_isr(struct capi_dma_handle *handle)
{
	(void)handle;
	return 0;
}

/**
 * @brief DMA channel interrupt handler.
 * @param chan - Pointer to the DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_isr_chan(struct capi_dma_chan *chan)
{
	struct stm32_dma_chan_priv *chan_priv;

	if (!chan || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;

	HAL_DMA_IRQHandler(&chan_priv->hdma);

	return 0;
}

const struct capi_dma_ops stm32_capi_dma_ops = {
	.init = stm32_capi_dma_init,
	.deinit = stm32_capi_dma_deinit,
	.init_chan = stm32_capi_dma_init_chan,
	.deinit_chan = stm32_capi_dma_deinit_chan,
	.config_xfer = stm32_capi_dma_config_xfer,
	.xfer_start = stm32_capi_dma_xfer_start,
	.xfer_abort = stm32_capi_dma_xfer_abort,
	.chan_is_completed = stm32_capi_dma_chan_is_completed,
	.isr = stm32_capi_dma_isr,
	.isr_chan = stm32_capi_dma_isr_chan,
};
