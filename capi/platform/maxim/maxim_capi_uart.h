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

/*
 * Per-target capability gates.
 *
 * TARGET_NUM is the MSDK per-part selector (pulled in via uart.h -> mxc_device.h
 * -> max<n>.h). MAX32657 is the reference target; other parts are switched on as
 * they are brought up and verified on hardware.
 *
 *  - MAX_CAPI_UART_HAS_DMA : a Maxim CAPI DMA backend exists for this part, so
 *      the DMA-backed async path (and the maxim_capi_dma.h dependency, which
 *      carries a MAX32657 TrustZone-secure #error) is compiled in.
 *  - MAX_CAPI_UART_CFG_PINS : the board provides the gpio_cfg_uart global and
 *      UART pin mux is configured here.
 *  - MAX_CAPI_UART_CLOCK_ARG : MXC_UART_Init()/MXC_UART_SetFrequency() take an
 *      explicit mxc_uart_clock_t argument on this part.
 */
#if (TARGET_NUM == 32657)
#define MAX_CAPI_UART_HAS_DMA	1
#define MAX_CAPI_UART_CFG_PINS	1
#else
#define MAX_CAPI_UART_HAS_DMA	0
#define MAX_CAPI_UART_CFG_PINS	0
#endif

#if (TARGET_NUM == 32520) || (TARGET_NUM == 32570) || (TARGET_NUM == 32650) || \
    (TARGET_NUM == 32660) || (TARGET_NUM == 32665)
#define MAX_CAPI_UART_CLOCK_ARG	0
#else
#define MAX_CAPI_UART_CLOCK_ARG	1
#endif

#if MAX_CAPI_UART_HAS_DMA
#include "maxim_capi_dma.h"
#endif
#include "maxim_capi_irq.h"
#include "maxim_capi_gpio.h"

#define MAX_CAPI_UART_DEFAULT_BAUD 115200

/**
 * @struct max_capi_uart_extra
 * @brief Maxim-specific UART extra configuration
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
