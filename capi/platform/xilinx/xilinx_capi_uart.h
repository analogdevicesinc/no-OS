/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform UART driver for CAPI
 *
 * Backends are selected via config.ops. Platform driver code and tests that
 * need backend ops symbols or private handle storage should include
 * xilinx_capi_uart_priv.h.
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

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_UART_H_ */
