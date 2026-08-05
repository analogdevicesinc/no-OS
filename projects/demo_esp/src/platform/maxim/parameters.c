/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of maxim platform data used by demo_esp
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
