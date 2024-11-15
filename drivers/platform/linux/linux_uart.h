/*******************************************************************************
 *   @file   linux/linux_uart.h
 *   @brief  Header containing extra types used by the UART driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef LINUX_UART_H_
#define LINUX_UART_H_

#include "no_os_uart.h"

/**
 * @struct linux_uart_init_param
 * @brief Structure holding the initialization parameters for Linux platform
 * specific UART parameters.
 */
struct linux_uart_init_param {
	/** UART device ID (/dev/"device_id") */
	const char *device_id;
};

/**
 * @brief Linux platform specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops linux_uart_ops;

#endif // LINUX_UART_H_
