/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by eval-adxl355-pmdz project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param adxl355_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

#ifdef GPIO_DRDY_PORT_NUM
/* Initialization for Sync pin GPIO. */
struct no_os_gpio_init_param adxl355_gpio_drdy_ip = {
	.port = GPIO_DRDY_PORT_NUM,
	.number = GPIO_DRDY_PIN_NUM,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};
#endif

struct max_gpio_init_param adxl355_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param adxl355_spi_extra_ip  = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
