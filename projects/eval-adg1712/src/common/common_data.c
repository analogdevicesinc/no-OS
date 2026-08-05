/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adg1712 examples.
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_gpio.h"

struct no_os_uart_init_param adg1712_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct adg1712_init_param adg1712_init_param = {
	.chip_id = ID_ADG1712,
	.gpio_in1 = {
		.port = GPIO_SW_PORT_NUM,
		.number = GPIO_SW1_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in2 = {
		.port = GPIO_SW_PORT_NUM,
		.number = GPIO_SW2_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in3 = {
		.port = GPIO_SW_PORT_NUM,
		.number = GPIO_SW3_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in4 = {
		.port = GPIO_SW_PORT_NUM,
		.number = GPIO_SW4_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	}
};
