/***************************************************************************//**
 *   @file   max14906/src/platform/maxim/parameters.c
 *   @brief  Definition of Maxim platform data used by max14906 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "parameters.h"

struct max_uart_init_param max14906_uart_extra = {
	.flow = UART_FLOW_DIS,
};

#ifdef BASIC_EXAMPLE
struct max_gpio_init_param max14906_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIO
};
#endif

struct max_spi_init_param max14906_spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
