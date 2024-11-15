/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by lt7182s project.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "parameters.h"

struct max_uart_init_param lt7182s_uart_extra = {
	.flow = UART_FLOW_DIS,
};

struct max_i2c_init_param lt7182s_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
