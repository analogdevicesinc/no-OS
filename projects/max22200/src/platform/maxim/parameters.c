/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by MAX22200 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "parameters.h"

struct max_uart_init_param max22200_uart_extra = {
	.flow = UART_FLOW_DIS,
};

struct max_gpio_init_param max22200_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param max22200_spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
