/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by max42500 project.
 *   @author Joshua Maniti (Joshua.Maniti@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

const struct max_i2c_init_param max42500_i2c_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

const struct max_gpio_init_param max42500_gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH
};
