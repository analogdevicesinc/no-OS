/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by ltm4700 project.
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

struct max_uart_init_param uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_i2c_init_param i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
