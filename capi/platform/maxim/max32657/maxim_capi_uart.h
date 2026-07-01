/*******************************************************************************
 *   @file   maxim_capi_uart.h
 *   @brief  Header file for UART functions with CAPI
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_UART_H_
#define MAXIM_CAPI_UART_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "uart.h"
#include "capi_dma.h"
#include "capi_uart.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_irq.h"
#include "maxim_capi_gpio.h"

#define MAX_CAPI_UART_DEFAULT_BAUD 115200

/**
 * @struct max_capi_uart_extra
 * @brief MAX32657-specific UART extra configuration
 */
struct max_capi_uart_extra {
	/** GPIO voltage selection */
	enum max_capi_gpio_vssel vssel;
	/** OPTIONAL - DMA config */
	struct capi_dma_config *dma_config;
};

extern struct capi_uart_ops max_capi_uart_ops;

/**
 * @brief Enable stdio redirection of UART
 * @param handle The UART handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_stdio_enable(struct capi_uart_handle *handle);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_UART_H_ */
