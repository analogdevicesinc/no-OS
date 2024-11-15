/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by adp1050 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "parameters.h"

struct max_uart_init_param adp1050_uart_extra = {
	.flow = UART_FLOW_DIS,
};

struct max_i2c_init_param adp1050_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct max_gpio_init_param adp1050_pg_alt_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
