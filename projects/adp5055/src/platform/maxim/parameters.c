/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by adp5055 project.
 *   @author Angelo Catapang (angelo.catapang@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

struct max_uart_init_param adp5055_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
};

struct max_i2c_init_param adp5055_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
