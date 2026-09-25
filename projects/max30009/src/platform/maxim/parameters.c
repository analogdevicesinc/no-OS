/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by max30009 project
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_i2c_init_param max30009_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIO,
};

struct max_spi_init_param max30009_spi_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIO,
	.num_slaves = 1,
};

struct max_uart_init_param max30009_uart_extra_ip = {
	.flow = MXC_UART_FLOW_DIS,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param max30009_gpio_csb_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIO,
};

struct max_gpio_init_param max30009_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIO,
};
