/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by eval-adis1646x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param adis1646x_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct max_gpio_init_param adis1646x_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param adis1646x_spi_extra_ip  = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

/* Initialization for Sync pin GPIO. */
struct no_os_gpio_init_param adis_gpio_drdy_ip = {
	.port = GPIO_DRDY_PORT_NUM,
	.number = GPIO_DRDY_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct max_gpio_init_param adis_gpio_drdy_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
