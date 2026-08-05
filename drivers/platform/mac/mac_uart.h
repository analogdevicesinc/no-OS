/***************************************************************************//**
 *   @file   mac_uart.h
 *   @brief  Implementation of Mac platform UART Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef MAC_UART_H_
#define MAC_UART_H_

#include "no_os_uart.h"

/**
 * @struct mac_uart_init_param
 * @brief Structure holding the initialization parameters for Mac platform
 * specific UART parameters.
 */
struct mac_uart_init_param {
	/** UART device ID (/dev/"device_id") */
	const char *device_id;
};

/**
 * @brief Mac platform specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops mac_uart_ops;

#endif // MAC_UART_H_
