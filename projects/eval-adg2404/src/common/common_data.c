/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-adg2404 project.
 *   @author Raul Geo <Raul.Georgian@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param adg2404_uart_ip = {
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

struct adg2404_init_param adg2404_ip = {
	.gpio_a0 = {
		.port = GPIO_A0_PORT,
		.number = GPIO_A0_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	},
	.gpio_a1 = {
		.port = GPIO_A1_PORT,
		.number = GPIO_A1_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	},
	.gpio_en = {
		.port = GPIO_EN_PORT,
		.number = GPIO_EN_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA
	}
};
