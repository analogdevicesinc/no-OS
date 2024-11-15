/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by max14914 example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"

struct no_os_uart_init_param max14914_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct no_os_gpio_init_param max14914_fault_ip = {
	.port = GPIO_FAULT_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_FAULT_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param max14914_doilvl_ip = {
	.port = GPIO_DOILVL_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_DOILVL_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param max14914_di_en_ip = {
	.port = GPIO_DI_EN_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_DI_EN_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param max14914_in_ip = {
	.port = GPIO_IN_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_IN_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param max14914_pp_ip = {
	.port = GPIO_PP_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_PP_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct max14914_init_param max14914_ip = {
	.di_en_init_param = &max14914_di_en_ip,
	.doilvl_init_param = &max14914_doilvl_ip,
	.fault_init_param = &max14914_fault_ip,
	.in_init_param = &max14914_in_ip,
	.pp_init_param = &max14914_pp_ip
};
