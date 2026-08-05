/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by ssd1306 project.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct max_uart_init_param demo_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct max_i2c_init_param oled_display_i2c_maxim_extra_param = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

