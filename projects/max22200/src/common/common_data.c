/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Common data used by the MAX22200 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"

struct no_os_uart_init_param max22200_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct no_os_spi_init_param max22200_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.extra = SPI_EXTRA,
	.max_speed_hz = SPI_BAUDRATE,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
};

struct no_os_gpio_init_param max22200_cmd_ip = {
	.port = GPIO_CMD_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_CMD_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct max22200_init_param max22200_ip = {
	.comm_param = &max22200_spi_ip,
	.cmd_param = &max22200_cmd_ip,
	.ch_config = {
		MAX22200_INDEPENDENT_MODE,
		MAX22200_INDEPENDENT_MODE,
		MAX22200_INDEPENDENT_MODE,
		MAX22200_INDEPENDENT_MODE,
	}
};
