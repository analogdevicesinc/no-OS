/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definitions specific to Maxim platform used by ad7124-8pmdz
 *           project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param ad7124_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_spi_init_param ad7124_spi_extra_ip = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
