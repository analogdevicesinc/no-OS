/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by lt8722 examples.
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"

struct no_os_uart_init_param lt8722_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct no_os_spi_init_param lt8722_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.extra = SPI_EXTRA,
	.max_speed_hz = SPI_BAUDRATE,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0
};

struct no_os_gpio_init_param lt8722_swen_ip = {
	.port = GPIO_SWEN_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_SWEN_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct no_os_gpio_init_param lt8722_en_ip = {
	.port = GPIO_EN_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_EN_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct lt8722_init_param lt8722_ip = {
	.spi = &lt8722_spi_ip,
	.gpio_swen = &lt8722_swen_ip,
	.gpio_en = &lt8722_en_ip,
	.start_up_sequence = true
};
