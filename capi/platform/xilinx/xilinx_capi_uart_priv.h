/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform UART private driver contract.
 *
 * Include this from Xilinx UART backend sources and tests that intentionally
 * inspect Xilinx-private state or preallocate Xilinx UART storage.
 */

#ifndef _XILINX_CAPI_UART_PRIV_H_
#define _XILINX_CAPI_UART_PRIV_H_

#include <xilinx_capi_uart.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct capi_uart_xilinx_handle
 * @brief Xilinx UART private handle.
 */
struct capi_uart_xilinx_handle {
	/** Xilinx driver instance (XUartPs*, XUartPsv*, XUartLite*, XUartNs550*) */
	void *instance;
	/** Cached line configuration */
	struct capi_uart_line_config line_config;
	/** User callback for async operations */
	capi_uart_callback callback;
	/** User callback argument */
	void *callback_arg;
	/** IRQ ID (only valid if use_irq is true) */
	uint32_t irq_id;
	/** True if IRQ was successfully connected during init */
	bool use_irq;
	/** Last interrupt reason (captured in ISR) */
	enum capi_uart_interrupt_reason last_interrupt_reason;
};

/**
 * @brief Declare a stack-allocated Xilinx UART handle.
 *
 * Declares `name` (struct capi_uart_handle) with embedded private state.
 * Pass &name to capi_uart_init(). The backend skips allocation and sets
 * init_allocated to false.
 */
#define CAPI_UART_HANDLE_XILINX_DEFINE(name)                                   \
	struct capi_uart_handle name = {                                       \
		.ops = NULL,                                                   \
		.init_allocated = false,                                       \
		.priv = &(struct capi_uart_xilinx_handle){0}                   \
	}

/**
 * @brief Xilinx UART operations tables. Select one via config->ops.
 */
extern struct capi_uart_ops capi_uart_xilinx_ps_ops;
extern struct capi_uart_ops capi_uart_xilinx_psv_ops;
extern struct capi_uart_ops capi_uart_xilinx_pl_lite_ops;
extern struct capi_uart_ops capi_uart_xilinx_pl_ns550_ops;

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_UART_PRIV_H_ */
