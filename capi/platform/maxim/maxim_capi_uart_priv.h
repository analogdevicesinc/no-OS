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

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "maxim_capi_uart.h"

/**
 * @struct max_capi_uart_priv
 * @brief Private structure for UART
 */
struct max_capi_uart_priv {
	/** Identifier */
	uint32_t id;
	/** UART registers */
	mxc_uart_regs_t *uart;
	/**
	 * Clock source storage. Held as a raw uint32_t so the struct is portable
	 * to parts whose MXC_UART_Init() takes no mxc_uart_clock_t argument; it is
	 * cast back to mxc_uart_clock_t at the call sites that need it.
	 */
	uint32_t clk_src;
	/** Line config storage */
	struct capi_uart_line_config line_config;
	/** DMA handle */
	struct capi_dma_handle *dma_handle;
	/** Async callback */
	capi_uart_callback callback;
	/** Callback arg */
	void *callback_arg;
	/**
	 * UART request storage for async. TX and RX are kept in SEPARATE
	 * requests, not one combined request: the CAPI API arms transmit and
	 * receive as two independent operations, and MSDK's RevB async engine
	 * tracks them independently (AsyncTxRequests[]/AsyncRxRequests[]). A
	 * single shared request would be re-pointed by the second arm while the
	 * driver still holds a pointer to it for the first direction, corrupting
	 * the in-flight transfer. On the external-loopback fixture both
	 * directions are live at once, so both slots are needed.
	 */
	mxc_uart_req_t async_tx_req;
	mxc_uart_req_t async_rx_req;
	/** DMA transfer storage for async */
	struct capi_dma_transfer dma_xfer;
#if MAX_CAPI_UART_HAS_DMA
	/** DMA transfer extra struct storage for async */
	struct max_capi_dma_xfer_extra dma_xfer_extra;
#endif
	/** Async transmit in progress flag */
	volatile bool tx_in_progress;
	/** Async receive in progress flag */
	volatile bool rx_in_progress;
	/** DMA completion flag */
	volatile bool dma_completed;
	/** TX DMA channel */
	struct capi_dma_chan *dma_channel_tx;
	/** RX DMA channel */
	struct capi_dma_chan *dma_channel_rx;
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
