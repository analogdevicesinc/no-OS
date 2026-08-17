/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Platform-specific parameters for MAXM86161 on MAX32655.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param maxm86161_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_i2c_init_param maxm86161_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIO,
};

struct max_gpio_init_param maxm86161_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIO,
};
