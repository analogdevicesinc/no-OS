/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adl8113 project.
 *   @author Raul Geo <Raul.Georgian@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param adl8113_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_gpio_init_param led_red_ip = {
	.port = LED_RED_PORT,
	.number = LED_RED_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param led_green_ip = {
	.port = LED_GREEN_PORT,
	.number = LED_GREEN_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param led_blue_ip = {
	.port = LED_BLUE_PORT,
	.number = LED_BLUE_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param button1_ip = {
	.port = BUTTON1_PORT,
	.number = BUTTON1_PIN,
	.pull = NO_OS_PULL_UP,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param button2_ip = {
	.port = BUTTON2_PORT,
	.number = BUTTON2_PIN,
	.pull = NO_OS_PULL_UP,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct adl8113_init_param adl8113_param = {
	.gpio_va = {
		.port = GPIO_VA_PORT,
		.number = GPIO_VA_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	},
	.gpio_vb = {
		.port = GPIO_VB_PORT,
		.number = GPIO_VB_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	},
	.initial_mode = ADL8113_INTERNAL_AMPLIFIER
};
