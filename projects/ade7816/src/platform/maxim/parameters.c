/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by ade7816 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "parameters.h"

struct max_uart_init_param ade7816_uart_extra = {
	.flow = UART_FLOW_DIS,
};

struct max_spi_init_param ade7816_spi_extra = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct max_gpio_init_param ade7816_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
