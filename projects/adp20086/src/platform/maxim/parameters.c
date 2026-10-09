/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Source file of Maxim platform data used by adp20086 project.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

struct max_uart_init_param adp20086_uart_extra = {
	.flow = MAX_UART_FLOW_DIS
};

struct max_i2c_init_param adp20086_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct max_gpio_init_param adp20086_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};