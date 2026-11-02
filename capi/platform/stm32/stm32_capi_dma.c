/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stm32_capi_dma_priv.h"
#include "capi_alloc.h"
#include <errno.h>
#include <stddef.h>
#include <string.h>

#define STM32_CAPI_DMA_POLL_TIMEOUT_MS	1000

static struct capi_dma_handle *dma_handle_singleton = NULL;
static struct stm32_dma_chan_priv chan_priv_pool[STM32_CAPI_DMA_MAX_CHANNELS];

static int stm32_capi_dma_isr_chan(struct capi_dma_chan *chan);

/**
 * @brief Get the channel private data from its embedded HAL DMA handle.
 * @param hdma - Pointer to the HAL DMA handle.
 * @return Pointer to the channel private data.
 */
static struct stm32_dma_chan_priv *to_chan_priv(DMA_HandleTypeDef *hdma)
{
	return (struct stm32_dma_chan_priv *)((uint8_t *)hdma -
					      offsetof(struct stm32_dma_chan_priv, hdma));
}

/**
 * @brief Enable the clocks of every DMA controller present on the part.
 */
static void stm32_capi_dma_clk_enable(void)
{
#if defined(__HAL_RCC_DMA1_CLK_ENABLE)
	__HAL_RCC_DMA1_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_DMA2_CLK_ENABLE)
	__HAL_RCC_DMA2_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_DMAMUX1_CLK_ENABLE)
	__HAL_RCC_DMAMUX1_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPDMA1_CLK_ENABLE)
	__HAL_RCC_GPDMA1_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPDMA2_CLK_ENABLE)
	__HAL_RCC_GPDMA2_CLK_ENABLE();
#endif
}

/**
 * @brief HAL DMA transfer complete callback.
 * @param hdma - Pointer to the HAL DMA handle.
 */
static void stm32_capi_dma_xfer_cplt_callback(DMA_HandleTypeDef *hdma)
{
	struct stm32_dma_chan_priv *chan_priv = to_chan_priv(hdma);
	struct capi_dma_chan *chan = chan_priv->capi_chan;

	chan_priv->completed = true;

	if (chan && chan->xfer_complete_cb)
		chan->xfer_complete_cb(0, chan->xfer_complete_ctx);
}

/**
 * @brief HAL DMA transfer error callback.
 * @param hdma - Pointer to the HAL DMA handle.
 */
static void stm32_capi_dma_xfer_error_callback(DMA_HandleTypeDef *hdma)
{
	struct stm32_dma_chan_priv *chan_priv = to_chan_priv(hdma);
	struct capi_dma_chan *chan = chan_priv->capi_chan;

	chan_priv->completed = true;

	if (chan && chan->error_cb)
		chan->error_cb(0, chan->error_ctx);
}

#if defined(STM32_CAPI_DMA_FAST_CHANNEL)
#define STM32_DMA_CCR_IE_MASK	(DMA_CCR_TCIE | DMA_CCR_HTIE | DMA_CCR_TEIE)

/**
 * @brief Bit offset of the channel's flags in the controller ISR/IFCR.
 * @param hdma - Pointer to the HAL DMA handle.
 * @return Flag offset.
 */
static inline uint32_t stm32_dma_flag_shift(const DMA_HandleTypeDef *hdma)
{
	return hdma->ChannelIndex & 0x1CU;
}

/**
 * @brief Program address/length and enable the channel at register level.
 *
 * The static configuration (request, direction, widths, increments) is left
 * in CCR by HAL_DMA_Init() and is not touched here.
 *
 * @param chan_priv - Pointer to the channel private data.
 * @param ie - Interrupt enable bits to set in CCR.
 */
static void stm32_dma_fast_start(struct stm32_dma_chan_priv *chan_priv,
				 uint32_t ie)
{
	DMA_HandleTypeDef *hdma = &chan_priv->hdma;
	DMA_Channel_TypeDef *ch = hdma->Instance;
	uint32_t ccr = ch->CCR & ~(DMA_CCR_EN | STM32_DMA_CCR_IE_MASK);

	ch->CCR = ccr;
	hdma->DmaBaseAddress->IFCR = DMA_ISR_GIF1 << stm32_dma_flag_shift(hdma);
#if defined(DMAMUX1)
	hdma->DMAmuxChannelStatus->CFR = hdma->DMAmuxChannelStatusMask;
#endif
	ch->CNDTR = chan_priv->length;
	if (hdma->Init.Direction == DMA_MEMORY_TO_PERIPH) {
		ch->CPAR = (uint32_t)chan_priv->dst;
		ch->CMAR = (uint32_t)chan_priv->src;
	} else {
		ch->CPAR = (uint32_t)chan_priv->src;
		ch->CMAR = (uint32_t)chan_priv->dst;
	}
	ch->CCR = ccr | ie | DMA_CCR_EN;
}

/**
 * @brief Disable the channel and clear its flags.
 * @param chan_priv - Pointer to the channel private data.
 */
static void stm32_dma_fast_stop(struct stm32_dma_chan_priv *chan_priv)
{
	DMA_HandleTypeDef *hdma = &chan_priv->hdma;
	DMA_Channel_TypeDef *ch = hdma->Instance;

	ch->CCR &= ~(DMA_CCR_EN | STM32_DMA_CCR_IE_MASK);
	hdma->DmaBaseAddress->IFCR = DMA_ISR_GIF1 << stm32_dma_flag_shift(hdma);
}

/**
 * @brief Read the channel's flags, aligned to the channel 1 bit positions.
 * @param chan_priv - Pointer to the channel private data.
 * @return Channel flags (DMA_ISR_GIF1 .. DMA_ISR_TEIF1).
 */
static inline uint32_t stm32_dma_fast_flags(const struct stm32_dma_chan_priv
		*chan_priv)
{
	const DMA_HandleTypeDef *hdma = &chan_priv->hdma;

	return (hdma->DmaBaseAddress->ISR >> stm32_dma_flag_shift(hdma)) & 0xFU;
}

/**
 * @brief Register-level channel interrupt handler.
 * @param chan_priv - Pointer to the channel private data.
 */
static void stm32_dma_fast_isr(struct stm32_dma_chan_priv *chan_priv)
{
	DMA_HandleTypeDef *hdma = &chan_priv->hdma;
	DMA_Channel_TypeDef *ch = hdma->Instance;
	struct capi_dma_chan *chan = chan_priv->capi_chan;
	uint32_t ccr = ch->CCR;
	uint32_t flags;

	/* Only act on channels armed for interrupts, pollers own the rest */
	if (!(ccr & STM32_DMA_CCR_IE_MASK))
		return;

	flags = stm32_dma_fast_flags(chan_priv);

	if ((flags & DMA_ISR_TEIF1) && (ccr & DMA_CCR_TEIE)) {
		stm32_dma_fast_stop(chan_priv);
		chan_priv->completed = true;
		if (chan->error_cb)
			chan->error_cb(0, chan->error_ctx);
		return;
	}

	if ((flags & DMA_ISR_TCIF1) && (ccr & DMA_CCR_TCIE)) {
		if (!(ccr & DMA_CCR_CIRC))
			ch->CCR = ccr & ~(DMA_CCR_EN | STM32_DMA_CCR_IE_MASK);
		hdma->DmaBaseAddress->IFCR = DMA_ISR_GIF1 << stm32_dma_flag_shift(hdma);
		chan_priv->completed = true;
		if (chan->xfer_complete_cb)
			chan->xfer_complete_cb(0, chan->xfer_complete_ctx);
	}
}
#endif /* STM32_CAPI_DMA_FAST_CHANNEL */

/**
 * @brief Initialize the STM32 DMA controller.
 * @param handle - Pointer to store the allocated DMA handle.
 * @param config - Pointer to the DMA configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_init(struct capi_dma_handle **handle,
			       const struct capi_dma_config *config)
{
	struct capi_dma_handle *dma_handle;
	struct stm32_dma_priv_handle *dma_priv;

	if (!handle || !config)
		return -EINVAL;

	if (config->num_chans > STM32_CAPI_DMA_MAX_CHANNELS)
		return -EINVAL;

	if (dma_handle_singleton) {
		*handle = dma_handle_singleton;
		return 0;
	}

	if (*handle == NULL) {
		dma_handle = (struct capi_dma_handle *)capi_calloc(1, sizeof(*dma_handle));
		if (!dma_handle)
			return -ENOMEM;

		dma_priv = (struct stm32_dma_priv_handle *)capi_calloc(1, sizeof(*dma_priv));
		if (!dma_priv) {
			capi_free(dma_handle);
			return -ENOMEM;
		}

		dma_handle->priv = dma_priv;
		dma_handle->init_allocated = true;
	} else {
		dma_handle = *handle;
		dma_handle->init_allocated = false;

		if (!dma_handle->priv)
			return -EINVAL;

		dma_priv = dma_handle->priv;
		memset(dma_priv, 0, sizeof(*dma_priv));
	}

	dma_priv->num_chans = config->num_chans;
	dma_handle->ops = config->ops;

	stm32_capi_dma_clk_enable();

	dma_handle_singleton = dma_handle;
	*handle = dma_handle;

	return 0;
}

/**
 * @brief Deinitialize the STM32 DMA controller.
 * @param handle - Pointer to the DMA handle.
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
			if (dma_priv->chan_privs[i] && dma_priv->chan_privs[i]->hw_ready)
				HAL_DMA_DeInit(&dma_priv->chan_privs[i]->hdma);
			dma_priv->chan_privs[i] = NULL;
		}
	}

	memset(chan_priv_pool, 0, sizeof(chan_priv_pool));

	if (handle->init_allocated) {
		capi_free(dma_priv);
		capi_free(handle);
	}

	dma_handle_singleton = NULL;

	return 0;
}

/**
 * @brief Initialize a DMA channel.
 *
 * Channel storage is static, indexed by the channel ID.
 *
 * @param handle - Pointer to the DMA handle.
 * @param chan_ptr - Pointer to store the channel.
 * @param id - Channel identifier.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_init_chan(struct capi_dma_handle *handle,
				    struct capi_dma_chan **chan_ptr,
				    uint32_t id)
{
	struct stm32_dma_chan_priv *chan_priv;
	struct stm32_dma_priv_handle *dma_priv;
	struct capi_dma_chan *chan;

	if (!handle || !chan_ptr)
		return -EINVAL;

	dma_priv = handle->priv;
	if (!dma_priv)
		return -EINVAL;

	if (id >= dma_priv->num_chans)
		return -EINVAL;

	if (dma_priv->chan_privs[id])
		return -EBUSY;

	chan_priv = &chan_priv_pool[id];
	memset(chan_priv, 0, sizeof(*chan_priv));

	chan = &chan_priv->chan_storage;
	chan->owned_by_app = false;
	chan->handle = handle;
	chan->id = id;
	chan->extra = chan_priv;

	chan_priv->capi_chan = chan;
	chan_priv->completed = true;

	dma_priv->chan_privs[id] = chan_priv;

	*chan_ptr = chan;

	return 0;
}

/**
 * @brief Deinitialize a DMA channel.
 * @param chan - Pointer to the DMA channel.
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

	if (!chan_priv)
		return 0;

	if (chan_priv->hw_ready)
		HAL_DMA_DeInit(&chan_priv->hdma);

	if (dma_priv && chan->id < dma_priv->num_chans)
		dma_priv->chan_privs[chan->id] = NULL;

	memset(chan_priv, 0, sizeof(*chan_priv));

	return 0;
}

/**
 * @brief Configure a DMA transfer on a channel.
 *
 * The static part of the configuration (request, direction, widths,
 * increments, mode) only reaches HAL_DMA_Init() when it changes; repeated
 * transfers with the same shape only record their addresses and length.
 *
 * @param chan - Pointer to the DMA channel.
 * @param xfer - Pointer to the transfer descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_config_xfer(struct capi_dma_chan *chan,
				      struct capi_dma_transfer *xfer)
{
	struct stm32_dma_chan_priv *chan_priv;
	struct stm32_dma_chan_extra_config *chan_extra;
	DMA_InitTypeDef init;

	if (!chan || !xfer || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;
	chan_extra = xfer->extra;

	if (chan_extra && chan_extra != chan_priv->applied_extra) {
		chan_priv->ch_num = chan_extra->ch_num;
		chan_priv->mem_increment = chan_extra->mem_increment;
		chan_priv->per_increment = chan_extra->per_increment;
		chan_priv->mem_data_alignment = chan_extra->mem_data_alignment;
		chan_priv->per_data_alignment = chan_extra->per_data_alignment;
		chan_priv->dma_mode = chan_extra->dma_mode;
		chan_priv->trig = chan_extra->trig;

		if (chan_extra->hdma) {
			if (chan_priv->hw_ready)
				HAL_DMA_DeInit(&chan_priv->hdma);
			memcpy(&chan_priv->hdma, chan_extra->hdma, sizeof(DMA_HandleTypeDef));
			chan_priv->hw_ready = false;
		}

		chan_priv->applied_extra = chan_extra;
		chan_priv->cfg_valid = false;
	}

	if (!chan_priv->hdma.Instance)
		return -EINVAL;

	if (chan_priv->hw_ready && chan_priv->cfg_valid &&
	    xfer->xfer_type == chan_priv->cfg_type &&
	    xfer->src_inc == chan_priv->cfg_src_inc &&
	    xfer->dst_inc == chan_priv->cfg_dst_inc)
		goto set_addr;

	init = chan_priv->hdma.Init;

#if defined(STM32F2) || defined(STM32F4) || defined(STM32F7)
	init.Channel = chan_priv->ch_num;
#else
	init.Request = chan_priv->ch_num;
#endif

	switch (chan_priv->dma_mode) {
	case CAPI_DMA_CIRCULAR_MODE:
		init.Mode = DMA_CIRCULAR;
		break;
	case CAPI_DMA_NORMAL_MODE:
	default:
		init.Mode = DMA_NORMAL;
		break;
	}

	switch (xfer->xfer_type) {
	case CAPI_DMA_MEM_TO_MEM:
		init.Direction = DMA_MEMORY_TO_MEMORY;
		init.MemInc = (xfer->dst_inc == CAPI_DMA_BYTE_INCREMENT) ?
			      DMA_MINC_ENABLE : DMA_MINC_DISABLE;
		init.PeriphInc = (xfer->src_inc == CAPI_DMA_BYTE_INCREMENT) ?
				 DMA_PINC_ENABLE : DMA_PINC_DISABLE;
		break;
	case CAPI_DMA_MEM_TO_DEV:
		init.Direction = DMA_MEMORY_TO_PERIPH;
		init.MemInc = (xfer->src_inc == CAPI_DMA_BYTE_INCREMENT) ?
			      DMA_MINC_ENABLE : DMA_MINC_DISABLE;
		init.PeriphInc = (xfer->dst_inc == CAPI_DMA_BYTE_INCREMENT) ?
				 DMA_PINC_ENABLE : DMA_PINC_DISABLE;
		break;
	case CAPI_DMA_DEV_TO_MEM:
		init.Direction = DMA_PERIPH_TO_MEMORY;
		init.MemInc = (xfer->dst_inc == CAPI_DMA_BYTE_INCREMENT) ?
			      DMA_MINC_ENABLE : DMA_MINC_DISABLE;
		init.PeriphInc = (xfer->src_inc == CAPI_DMA_BYTE_INCREMENT) ?
				 DMA_PINC_ENABLE : DMA_PINC_DISABLE;
		break;
	default:
		return -EINVAL;
	}

	switch (chan_priv->mem_data_alignment) {
	case CAPI_DMA_DATA_ALIGN_HALF_WORD:
		init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		break;
	case CAPI_DMA_DATA_ALIGN_WORD:
		init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		break;
	case CAPI_DMA_DATA_ALIGN_BYTE:
	default:
		init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		break;
	}

	switch (chan_priv->per_data_alignment) {
	case CAPI_DMA_DATA_ALIGN_HALF_WORD:
		init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		break;
	case CAPI_DMA_DATA_ALIGN_WORD:
		init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		break;
	case CAPI_DMA_DATA_ALIGN_BYTE:
	default:
		init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		break;
	}

	if (!chan_priv->hw_ready ||
	    memcmp(&init, &chan_priv->hdma.Init, sizeof(init))) {
		chan_priv->hdma.Init = init;
		chan_priv->hdma.XferCpltCallback = stm32_capi_dma_xfer_cplt_callback;
		chan_priv->hdma.XferErrorCallback = stm32_capi_dma_xfer_error_callback;

		if (HAL_DMA_Init(&chan_priv->hdma) != HAL_OK) {
			chan_priv->hw_ready = false;
			return -EIO;
		}
		chan_priv->hw_ready = true;
	}

	chan_priv->cfg_valid = true;
	chan_priv->cfg_type = xfer->xfer_type;
	chan_priv->cfg_src_inc = xfer->src_inc;
	chan_priv->cfg_dst_inc = xfer->dst_inc;

set_addr:
	chan_priv->src = xfer->src;
	chan_priv->dst = xfer->dst;
	chan_priv->length = xfer->length;
	chan_priv->completed = false;

	chan->xfer = xfer;

	return 0;
}

/**
 * @brief Start a configured DMA transfer.
 *
 * Interrupts are armed only for the events that have a registered callback.
 * A memory-to-memory transfer on a channel without an IRQ and without a
 * completion callback is completed by polling before returning.
 *
 * @param chan - Pointer to the DMA channel.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_xfer_start(struct capi_dma_chan *chan)
{
	struct stm32_dma_chan_priv *chan_priv;
	bool polled;
	int ret;

	if (!chan || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;
	if (!chan_priv->hw_ready)
		return -EINVAL;

	polled = !chan->irq_num && !chan->xfer_complete_cb && chan->xfer &&
		 chan->xfer->xfer_type == CAPI_DMA_MEM_TO_MEM;

	chan_priv->completed = false;

#if defined(STM32_CAPI_DMA_FAST_CHANNEL)
	if (!polled) {
		uint32_t ie = 0;

		if (chan->xfer_complete_cb)
			ie |= DMA_CCR_TCIE | DMA_CCR_TEIE;
		if (chan->error_cb)
			ie |= DMA_CCR_TEIE;

		stm32_dma_fast_start(chan_priv, ie);

		return 0;
	}
#endif

	if (chan_priv->hdma.State == HAL_DMA_STATE_BUSY)
		HAL_DMA_Abort(&chan_priv->hdma);

	if (polled) {
		ret = HAL_DMA_Start(&chan_priv->hdma,
				    (uint32_t)chan_priv->src,
				    (uint32_t)chan_priv->dst,
				    chan_priv->length);
		if (ret == HAL_OK) {
			ret = HAL_DMA_PollForTransfer(&chan_priv->hdma,
						      HAL_DMA_FULL_TRANSFER,
						      STM32_CAPI_DMA_POLL_TIMEOUT_MS);
			if (ret == HAL_OK)
				chan_priv->completed = true;
		}
	} else {
		ret = HAL_DMA_Start_IT(&chan_priv->hdma,
				       (uint32_t)chan_priv->src,
				       (uint32_t)chan_priv->dst,
				       chan_priv->length);
		if (ret == HAL_OK && !chan->xfer_complete_cb)
			__HAL_DMA_DISABLE_IT(&chan_priv->hdma, DMA_IT_TC);
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

	if (!chan || !chan->extra)
		return -EINVAL;

	chan_priv = chan->extra;

	if (!chan_priv->hw_ready) {
		chan_priv->completed = true;
		return 0;
	}

#if defined(STM32_CAPI_DMA_FAST_CHANNEL)
	stm32_dma_fast_stop(chan_priv);
	chan_priv->hdma.State = HAL_DMA_STATE_READY;
#else
	if ((HAL_DMA_Abort(&chan_priv->hdma) != HAL_OK) &&
	    (chan_priv->hdma.ErrorCode != HAL_DMA_ERROR_NO_XFER))
		return -EIO;
#endif

	chan_priv->completed = true;

	return 0;
}

/**
 * @brief Check if a DMA channel transfer has completed.
 * @param chan - Pointer to the DMA channel.
 * @return true if completed, false otherwise.
 */
static bool stm32_capi_dma_chan_is_completed(const struct capi_dma_chan *chan)
{
	struct stm32_dma_chan_priv *chan_priv;

	if (!chan || !chan->extra)
		return true;

	chan_priv = chan->extra;

#if defined(STM32_CAPI_DMA_FAST_CHANNEL)
	if (!chan_priv->completed && chan_priv->hw_ready &&
	    (stm32_dma_fast_flags(chan_priv) & (DMA_ISR_TCIF1 | DMA_ISR_TEIF1)))
		chan_priv->completed = true;
#endif

	return chan_priv->completed;
}

/**
 * @brief DMA controller interrupt handler, services every channel.
 * @param handle - Pointer to the DMA handle.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_isr(struct capi_dma_handle *handle)
{
	struct stm32_dma_priv_handle *dma_priv;
	uint32_t i;

	if (!handle || !handle->priv)
		return -EINVAL;

	dma_priv = handle->priv;

	for (i = 0; i < dma_priv->num_chans; i++)
		if (dma_priv->chan_privs[i] && dma_priv->chan_privs[i]->hw_ready)
			stm32_capi_dma_isr_chan(dma_priv->chan_privs[i]->capi_chan);

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

#if defined(STM32_CAPI_DMA_FAST_CHANNEL)
	stm32_dma_fast_isr(chan_priv);
#else
	HAL_DMA_IRQHandler(&chan_priv->hdma);
#endif

	return 0;
}

/**
 * @brief Register the transfer complete callback of a channel.
 * @param chan - Pointer to the DMA channel.
 * @param callback - Callback, NULL to remove it.
 * @param xfer_complete_ctx - Callback context.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_register_complete_callback(struct capi_dma_chan
		*chan,
		capi_dma_xfer_complete_cb callback,
		void *xfer_complete_ctx)
{
	if (!chan)
		return -EINVAL;

	chan->xfer_complete_cb = callback;
	chan->xfer_complete_ctx = xfer_complete_ctx;

	return 0;
}

/**
 * @brief Register the error callback of a channel.
 * @param chan - Pointer to the DMA channel.
 * @param callback - Callback, NULL to remove it.
 * @param error_ctx - Callback context.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_dma_register_error_callback(struct capi_dma_chan *chan,
		capi_dma_error_cb callback,
		void *error_ctx)
{
	if (!chan)
		return -EINVAL;

	chan->error_cb = callback;
	chan->error_ctx = error_ctx;

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
	.register_complete_callback = stm32_capi_dma_register_complete_callback,
	.register_error_callback = stm32_capi_dma_register_error_callback,
};
