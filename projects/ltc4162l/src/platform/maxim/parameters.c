/********************************************************************************
 *   @file   parameters.c
 *   @brief  Definition of maxim platform data used by ltc4162l project.
 *   @author Marc Paolo Sosa (marcpaolo.sosa@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#include "parameters.h"

struct max_uart_init_param max_uart_extra = {
	.flow = MAX_UART_FLOW_DIS,
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct max_i2c_init_param max_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
