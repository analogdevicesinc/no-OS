/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  ADIOL100 project platform-specific parameters (Maxim).
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param adiol100_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_spi_init_param adiol100_spi_extra = {
	.num_slaves = 2,
	.polarity   = SPI_SS_POL_LOW,
	.vssel      = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param adiol100_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
