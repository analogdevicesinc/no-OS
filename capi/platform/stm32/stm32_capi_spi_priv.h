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
 * @brief STM32 SPI private handle with full feature support
 */
struct stm32_spi_priv_handle {
	/** SPI HAL Handle */
	SPI_HandleTypeDef hspi;
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
};

#define CAPI_SPI_CONTROLLER_HANDLE_STM32_INIT()                                \
	(&(struct capi_spi_controller_handle){                                 \
		.ops = NULL,                                                   \
		.init_allocated = false,                                       \
		.priv = &(struct stm32_spi_priv_handle){0}})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _STM32_CAPI_SPI_PRIV_H_ */
