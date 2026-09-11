/*******************************************************************************
 *   @file   maxim_capi_uart_priv.h
 *   @brief  Header file for the UART private handle
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_UART_PRIV_H_
#define MAXIM_CAPI_UART_PRIV_H_

#include "maxim_capi_uart.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_uart_priv
 * @brief Private structure for UART
 */
struct max_capi_uart_priv {
	/** Identifier */
	uint32_t id;
	/** UART registers */
	mxc_uart_regs_t *uart;
	/** Clock source storage */
	mxc_uart_clock_t clk_src;
	/** Line config storage */
	struct capi_uart_line_config line_config;
	/** DMA handle */
	struct capi_dma_handle *dma_handle;
	/** Async callback */
	capi_uart_callback callback;
	/** Callback arg */
	void *callback_arg;
	/** UART request storage for async */
	mxc_uart_req_t async_req;
	/** DMA transfer storage for async */
	struct capi_dma_transfer dma_xfer;
	/** DMA transfer extra struct storage for async */
	struct max_capi_dma_xfer_extra dma_xfer_extra;
	/** Async transfer in progress flag */
	volatile bool async_transfer_in_progress;
	/** DMA completion flag */
	volatile bool dma_completed;
	/** TX DMA channel */
	struct capi_dma_chan *dma_channel_tx;
	/** RX DMA channel */
	struct capi_dma_chan *dma_channel_rx;
	/** Whether to enable IRQ connection on init */
	bool use_irq;
};

#define CAPI_UART_HANDLE_MAXIM_INIT()			\
	(&(struct capi_uart_handle) {			\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct max_capi_uart_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_UART_PRIV_H_ */
