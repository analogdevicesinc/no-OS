/********************************************************************************
 *   @file   parameters.c
 *   @brief  Definition of maxim platform data used by ltc3337 project.
 *   @author Brent Kowal (brent.kowal@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "parameters.h"

struct max_uart_init_param max_uart_extra = {
	.flow = UART_FLOW_DIS,
};

struct max_i2c_init_param max_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
