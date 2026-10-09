/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by MAX34451 project.
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

struct max_uart_init_param max34451_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_i2c_init_param max34451_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
