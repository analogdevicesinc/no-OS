/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by MAX25603 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "parameters.h"

struct max_uart_init_param max25603_uart_extra = {
	.flow = UART_FLOW_DIS,
};

struct max_gpio_init_param max25603_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_pwm_init_param max25603_pwm_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};
