/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by the ADIN1110 examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param adin1110_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

struct no_os_gpio_init_param adin1110_reset_gpio_ip = {
	.port = RST_GPIO_PORT,
	.number = RST_GPIO_NUM,
	.pull = NO_OS_PULL_UP,
	.platform_ops = GPIO_OPS,
	.extra = RST_GPIO_EXTRA,
};
