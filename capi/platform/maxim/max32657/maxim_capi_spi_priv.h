/*******************************************************************************
 *   @file   maxim_capi_spi_priv.h
 *   @brief  Header file for private SPI handle
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_SPI_PRIV_H_
#define MAXIM_CAPI_SPI_PRIV_H_

#include "maxim_capi_spi.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_spi_fifo_async
 * @brief Data structure for async FIFO SPI
 */
struct max_capi_spi_fifo_async {
	/* TX buffer */
	const uint8_t *tx_buf;
	/* RX buffer */
	uint8_t *rx_buf;
	/* TX bytes to send */
	uint32_t tx_size;
	/* RX bytes to receive */
	uint32_t rx_size;
	/* max(tx_effective, rx_effective) */
	uint32_t clk_len;
	/* chars queued to TX FIFO so far */
	uint32_t tx_cnt;
	/* chars drained from RX FIFO so far */
	uint32_t rx_cnt;
	/* RX bytes to discard before rx_buf */
	uint32_t rx_skip;
	/* FIFO async in progress */
	bool active;
};

/**
 * @struct max_capi_spi_priv
 * @brief Private structure for SPI
 */
struct max_capi_spi_priv {
	/** SPI controller handle */
	struct capi_spi_controller_handle *spi_handle;
	/** SPI device ID; only 0 is valid for MAX32657 */
	uint32_t identifier;
	/** DMA controller handle */
	struct capi_dma_handle *dma_handle;
	/** Choose which chip select(s) gets enabled */
	uint8_t chip_select;
	/** SPI delays */
	struct max_capi_spi_delays platform_delays;
	/** Whether to enable IRQ connection during init */
	bool use_irq;
	/** Clock frequency */
	uint32_t clock_freq;
	/** DMA callback function */
	capi_spi_callback_t callback;
	/** DMA callback arg */
	void *callback_arg;
	/** FIFO async */
	struct max_capi_spi_fifo_async fifo_async;
	/** DMA completion count */
	volatile uint8_t dma_completed_count;
	/** RX DMA channel */
	struct capi_dma_chan *dma_channel_rx;
	/** TX DMA channel */
	struct capi_dma_chan *dma_channel_tx;
	/** Transfer in progress flag */
	volatile bool transfer_in_progress;
	/** Async transfer in progress flag */
	volatile bool async_transfer_in_progress;
};

#define CAPI_SPI_HANDLE_MAXIM_INIT()			\
	(&(struct capi_spi_controller_handle) {		\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct max_capi_spi_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_SPI_PRIV_H_ */
