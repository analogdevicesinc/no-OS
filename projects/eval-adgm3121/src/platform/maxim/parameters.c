/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by eval-adgm3121 project.
 *   @author Ciobanu Radu (Radu-rares.Ciobanu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param adgm3121_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct max_gpio_init_param adgm3121_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
