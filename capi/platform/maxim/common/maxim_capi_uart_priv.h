/*******************************************************************************
 *   @file   maxim_capi_uart_priv.h
 *   @brief  Private data of the MAX32650/MAX32690 CAPI UART driver
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_UART_PRIV_H_
#define MAXIM_CAPI_UART_PRIV_H_

#include "capi_dma.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_uart.h"
#include "uart.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_uart_priv
 * @brief UART private data
 */
struct max_capi_uart_priv {
	uint32_t id;
	mxc_uart_regs_t *uart;
	struct capi_uart_line_config line_config;
	capi_uart_callback callback;
	void *callback_arg;
	mxc_uart_req_t async_req;
	volatile bool async_in_progress;
	bool use_irq;
	/** DMA channels, NULL when DMA is not configured */
	struct capi_dma_chan *rx_dma;
	struct capi_dma_chan *tx_dma;
	struct capi_dma_transfer dma_xfer;
	struct max_capi_dma_xfer_extra rx_req;
	struct max_capi_dma_xfer_extra tx_req;
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
