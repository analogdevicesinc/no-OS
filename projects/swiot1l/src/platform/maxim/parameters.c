/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by the swiot1l project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "parameters.h"

struct max_uart_init_param adin1110_uart_extra_ip = {
	.flow = UART_FLOW_DIS,
	.vssel = 1
};

struct max_spi_init_param adin1110_spi_extra_ip  = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param vddioh_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_i2c_init_param vddioh_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
