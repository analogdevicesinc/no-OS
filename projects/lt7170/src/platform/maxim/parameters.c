/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by lt7170 project.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

struct max_uart_init_param lt7170_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_i2c_init_param lt7170_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
