/***************************************************************************//**
 *   @file   maxim_usb_uart.h
 *   @brief  Header file of USB CDC ACM driver for maxim platform.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MAXIM_USB_UART_H_
#define MAXIM_USB_UART_H_

#include "no_os_irq.h"
#include "no_os_uart.h"

/**
 * @brief Aditional UART config parameters
 */
struct max_usb_uart_init_param {
	uint16_t vid;
	uint16_t pid;
};

/**
 * @brief Platform specific UART state
 */
struct max_usb_uart_desc {
	/** Controller that handles UART interrupts */
	struct no_os_irq_ctrl_desc *nvic;
};

/**
 * @brief Maxim specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops max_usb_uart_ops;

#endif
