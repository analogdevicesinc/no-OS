/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _STM32_CAPI_SPI_PRIV_H_
#define _STM32_CAPI_SPI_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "stm32_capi_spi.h"

/**
 * @brief Padding bookkeeping for one DMA transfer.
 *
 * SPI clocks max(tx_size, rx_size) frames, so whichever direction is short
 * (or absent) has to be padded before the DMA channels can be armed. Only the
 * cases that need a bounce buffer allocate one; a direction that is entirely
 * dummy is pointed at a single static byte with increment disabled instead.
 */
struct stm32_spi_dma_pad {
	/** Padded TX bounce buffer (tx_buf copied in, remainder zeroed), or
	 *  NULL when the caller's tx_buf is used directly. */
	uint8_t *tx_tmp;
	/** Oversized RX bounce buffer, or NULL when the caller's rx_buf is
	 *  used directly. */
	uint8_t *rx_tmp;
	/** Caller RX buffer that rx_tmp must be copied back into on
	 *  completion, or NULL when no copy-back is needed. */
	uint8_t *rx_dst;
	/** Number of bytes to copy from rx_tmp back into rx_dst. */
	uint32_t rx_copy_len;
};

/**
 * @brief STM32 SPI private handle with full feature support
 */
struct stm32_spi_priv_handle {
	/** Active HAL SPI handle. Points at the caller-supplied handle (the same
	 *  object the NVIC vector services) so IT/DMA transfers and HAL completion
	 *  callbacks all act on one handle. Falls back to hspi_storage when the
	 *  caller provides only an identifier. */
	SPI_HandleTypeDef *hspi;
	/** Backing storage used only when no external handle is supplied. */
	SPI_HandleTypeDef hspi_storage;
	/** SPI input clock frequency */
	uint32_t input_clock;
	/** Chip select alternate function */
	uint32_t alternate;
	/** Chip select GPIO port handle */
	struct capi_gpio_port_handle *cs_port_handle;
	/** Chip select GPIO pin descriptor */
	struct capi_gpio_pin chip_select;
	/** DMA handle for async operations */
	struct capi_dma_handle *dma_handle;
	/** RX DMA Channel */
	struct capi_dma_chan *rxdma_ch;
	/** TX DMA Channel */
	struct capi_dma_chan *txdma_ch;
	/** Per-channel DMA configuration attached to each DMA transfer */
	struct stm32_dma_chan_extra_config *rxdma_extra;
	struct stm32_dma_chan_extra_config *txdma_extra;
	/** Minimum transfer length (bytes) at which transceive()/transceive_async()
	 *  dispatch to DMA instead of PIO/IT. */
	uint32_t dma_threshold;
	/** CAPI callback */
	capi_spi_callback_t callback;
	/** CAPI callback argument */
	void *callback_arg;
	/** Current transfer */
	struct capi_spi_transfer *current_transfer;
	/** Transfer queue */
	struct capi_spi_transfer *transfer_queue;
	/** Number of transfers */
	uint32_t transfer_count;
	/** Current transfer index */
	uint32_t current_transfer_index;
	/** Async transfer in progress flag */
	bool async_in_progress;
	/** DMA completion flag */
	bool dma_done;
	/** DMA user callback */
	void (*dma_user_cb)(void *ctx);
	/** DMA user callback context */
	void *dma_user_ctx;
	/** TX DMA transfer descriptors */
	struct capi_dma_transfer *tx_ch_xfer;
	/** RX DMA transfer descriptors */
	struct capi_dma_transfer *rx_ch_xfer;
	/** Per-transfer padding bookkeeping, transfer_count entries. */
	struct stm32_spi_dma_pad *dma_pad;
#ifdef HAL_TIM_MODULE_ENABLED
	/** Timer handle for PWM-based CS control */
	struct capi_timer_handle *cs_timer;
	/** CS timer channel number */
	uint32_t cs_timer_channel;
	/** Timer handle for PWM-based TX clocking */
	struct capi_timer_handle *tx_timer;
	/** TX timer channel number */
	uint32_t tx_timer_channel;
#endif
	/** GPIO CS configuration */
	struct stm32_capi_gpio_port_config cs_gpio_config;
	/** Flag indicating if CS GPIO is initialized */
	bool cs_initialized;
	/** True once the sync-path peripheral config has been applied for this
	 *  controller. Zeroed at init so a fresh (or re-initialized) controller
	 *  always configures on its first transfer - must not persist across a
	 *  deinit/init cycle. */
	bool sync_cfg_cached;
	/** Key of the last device config applied in the sync path. Reconfigure
	 *  when it changes (CS, clock mode, bit order or speed). */
	uint64_t sync_cfg_key;
};

#define CAPI_SPI_CONTROLLER_HANDLE_STM32_INIT() \
	(&(struct capi_spi_controller_handle){ \
		.ops = NULL, \
		.init_allocated = false, \
		.priv = &(struct stm32_spi_priv_handle){ .hspi = NULL }})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _STM32_CAPI_SPI_PRIV_H_ */
