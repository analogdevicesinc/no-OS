/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Platform parameters for LTC4284 project
 *   @author Carlos Jones Jr <carlosjr.jones@analog.com>
 *   @author Christopher de Guzman <christopher.deguzman@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

/* CN2 FTDI USB-to-UART Bridge - Simple UART configuration */
struct max_uart_init_param ltc4284_uart_extra_ip = {
	.flow = MXC_UART_FLOW_DIS
};

struct max_i2c_init_param ltc4284_i2c_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
