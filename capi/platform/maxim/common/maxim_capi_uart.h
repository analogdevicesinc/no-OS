/*******************************************************************************
 *   @file   maxim_capi_uart.h
 *   @brief  Header file for the MAX32650/MAX32690 CAPI UART functions
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_UART_H_
#define MAXIM_CAPI_UART_H_

#include <stdbool.h>
#include <stdint.h>
#include "capi_uart.h"
#include "maxim_capi_gpio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define MAX_CAPI_UART_DEFAULT_BAUD 115200

/**
 * @struct max_capi_uart_extra
 * @brief Platform specific UART configuration
 */
struct max_capi_uart_extra {
	/** Voltage level of the UART pins */
	enum max_capi_gpio_vssel vssel;
	/** Connect the UART IRQ, needed by the interrupt driven async API */
	bool use_irq;
	/** CAPI DMA channel IDs, used when config->dma_handle is set.
	 *  Both 0 selects channels 0 (RX) and 1 (TX). */
	uint32_t rx_dma_ch_id;
	uint32_t tx_dma_ch_id;
};

extern const struct capi_uart_ops max_capi_uart_ops;

int max_capi_uart_stdio_enable(struct capi_uart_handle *handle);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_UART_H_ */
