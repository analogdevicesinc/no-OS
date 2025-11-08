/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_uart.h
 * @brief MAX32655 platform-specific UART CAPI implementation header
 * @author Claude Code (noreply@anthropic.com)
 */

#ifndef MAXIM_CAPI_UART_H_
#define MAXIM_CAPI_UART_H_

#include "capi_uart.h"
#include "max32655.h"
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Private handle structure for MAX32655 UART CAPI implementation
 */
struct maxim_capi_uart_handle {
	struct capi_uart_handle capi_handle;  /**< Must be first field */
	mxc_uart_regs_t *uart_regs;          /**< Maxim UART registers */
	uint32_t device_id;                   /**< UART device ID */
	capi_uart_callback callback;         /**< User callback */
	void *callback_arg;                   /**< User callback argument */
	struct capi_uart_line_config line_config; /**< Current line configuration */
	bool fifo_enabled;                    /**< FIFO enable state */
	mxc_uart_req_t async_req;            /**< Async request structure */
};

#define CAPI_UART_HANDLE &(struct capi_uart_handle){		\
		.priv = &(struct maxim_capi_uart_handle){},	\
	}

/**
 * @brief MAX32655 UART CAPI operations structure
 *
 * This structure contains function pointers for all UART CAPI operations
 * specific to the MAX32655 platform.
 */
extern const struct capi_uart_ops maxim_uart_ops;

#ifdef __cplusplus
}
#endif

#endif /* MAXIM_CAPI_UART_H_ */