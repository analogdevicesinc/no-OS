/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by ltc4296 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "parameters.h"

struct max_uart_init_param ltc4296_uart_extra = {
	.flow = UART_FLOW_DIS,
};

struct max_spi_init_param ltc4296_spi_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
