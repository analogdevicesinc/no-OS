/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform UART driver for CAPI
 *
 * Backends (select via config.ops):
 *   capi_uart_xilinx_ps_ops       - XUartPs   (Zynq-7000 / ZynqMP PS UART)
 *   capi_uart_xilinx_psv_ops      - XUartPsv  (Versal PS UART)
 *   capi_uart_xilinx_pl_lite_ops  - XUartLite (AXI UartLite, fixed 8N1)
 *   capi_uart_xilinx_pl_ns550_ops - XUartNs550 (AXI 16550)
 */

#ifndef _XILINX_CAPI_UART_H_
#define _XILINX_CAPI_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include <capi_uart.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct capi_uart_xilinx_config
 * @brief Optional Xilinx platform-specific UART configuration.
 *
 * Passed via config->extra during init. use_irq decides whether the driver
 * connects irq_id through the CAPI IRQ singleton. IRQ ID 0 is valid for an
 * INTC input, so it is passed through unchanged.
 */
struct capi_uart_xilinx_config {
	/** Enable CAPI IRQ connection during init */
	bool use_irq;
	/** Normalized CAPI IRQ ID. Zero is valid for INTC input 0. */
	uint32_t irq_id;
};

/**
 * @struct capi_uart_xilinx_handle
 * @brief Xilinx UART handle.
 *
 * Embeds the base CAPI handle as first member for safe casting.
 */
struct capi_uart_xilinx_handle {
	/** Base handle - MUST be first for container_of pattern */
	struct capi_uart_handle handle;
	/** Xilinx driver instance (XUartPs*, XUartPsv*, or XUartLite*) */
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
 * @brief Xilinx UART operations tables. Select one via config->ops.
 */
extern struct capi_uart_ops
	capi_uart_xilinx_ps_ops;       /**< XUartPs  (Zynq/ZynqMP PS) */
extern struct capi_uart_ops
	capi_uart_xilinx_psv_ops;      /**< XUartPsv (Versal PS) */
extern struct capi_uart_ops
	capi_uart_xilinx_pl_lite_ops;  /**< XUartLite (AXI UartLite) */
extern struct capi_uart_ops
	capi_uart_xilinx_pl_ns550_ops; /**< XUartNs550 (AXI 16550) */

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_UART_H_ */
