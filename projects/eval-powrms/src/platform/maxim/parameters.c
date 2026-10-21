/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by EVAL-POWRMS project.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param demo_uart0_map_B_extra_ip = {
	.flow = MAX_UART_FLOW_DIS,
	.map = MAX_UART_MAP_B
};

struct max_uart_init_param demo_uart0_map_A_extra_ip = {
	.flow = MAX_UART_FLOW_DIS,
	.map = MAX_UART_MAP_A,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};


struct max_i2c_init_param oled_display_i2c_maxim_extra_param = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

struct max_gpio_init_param adc_gpio_extra_param = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

