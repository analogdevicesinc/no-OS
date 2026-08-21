/*******************************************************************************
 *   @file   win_uart.h
 *   @brief  Header containing extra types used by the UART driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef WIN_UART_H_
#define WIN_UART_H_

#include "no_os_uart.h"

/**
 * @struct win_uart_init_param
 * @brief Structure holding the initialization parameters for Windows platform
 * specific UART parameters.
 */
struct win_uart_init_param {
	/** COM device ID */
	const char *device_id;
};

/**
 * @brief Windows platform specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops win_uart_ops;

#endif // WIN_UART_H_
