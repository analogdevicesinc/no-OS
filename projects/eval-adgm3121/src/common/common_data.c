/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adgm3121 example.
 *   @author Ciobanu Radu (Radu-rares.Ciobanu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_gpio.h"

struct no_os_uart_init_param adgm3121_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct adgm3121_init_param adgm3121_ip = {
	.type = ID_ADGM3121,
	.gpio_pin_spi = {
		.port = GPIO_PIN_SPI_PORT_NUM,
		.number = GPIO_PIN_SPI_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in1 = {
		.port = GPIO_SW1_PORT_NUM,
		.number = GPIO_SW1_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in2 = {
		.port = GPIO_SW2_PORT_NUM,
		.number = GPIO_SW2_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in3 = {
		.port = GPIO_SW3_PORT_NUM,
		.number = GPIO_SW3_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_in4 = {
		.port = GPIO_SW4_PORT_NUM,
		.number = GPIO_SW4_PIN_NUM,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.mode = ADGM3121_MODE_PARALLEL,
};
