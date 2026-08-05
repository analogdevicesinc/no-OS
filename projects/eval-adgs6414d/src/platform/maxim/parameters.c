/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by adgs6414d project.
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param adgs6414d_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct max_spi_init_param adgs6414d_spi_extra_ip = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
