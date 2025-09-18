/*******************************************************************************
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by MAX20362 project.
 *   @author Wilbert Jethro R. Limjoco (Wilbertjethro.Limjoco@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"
#include "maxim_gpio.h"
#include "maxim_i2c.h"
#include "maxim_uart.h"

struct max_uart_init_param uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

const struct max_i2c_init_param max20362_i2c_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};

const struct max_gpio_init_param max20362_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
