/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by eval-adg1712 project.
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param adg1712_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct max_gpio_init_param adg1712_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
