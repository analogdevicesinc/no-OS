/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adg1736 project.
 *   @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"

struct no_os_uart_init_param adg1736_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

static struct no_os_gpio_init_param gpio_en_ip = {
	.port = GPIO_EN_PORT,
	.number = GPIO_EN_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct adg1736_init_param adg1736_ip = {
	.type = ADG1736,
	.gpio_in1 = {
		.port = GPIO_IN1_PORT,
		.number = GPIO_IN1_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in2 = {
		.port = GPIO_IN2_PORT,
		.number = GPIO_IN2_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_en = &gpio_en_ip,
};

/* LEDs */
struct no_os_gpio_init_param led_red_ip = {
	.port = LED_RED_PORT,
	.number = LED_RED_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param led_green_ip = {
	.port = LED_GREEN_PORT,
	.number = LED_GREEN_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

/* Test signal GPIOs */
struct no_os_gpio_init_param gpio_d_ip = {
	.port = GPIO_D_PORT,
	.number = GPIO_D_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param gpio_s_ip = {
	.port = GPIO_S_PORT,
	.number = GPIO_S_PIN,
	.pull = NO_OS_PULL_DOWN,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};
