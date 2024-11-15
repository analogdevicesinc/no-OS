/***************************************************************************//**
 *   @file   max14906/src/common/common_data.c
 *   @brief  Defines common data to be used by max14906 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"

struct no_os_uart_init_param max14906_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct no_os_spi_init_param max14906_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.extra = SPI_EXTRA,
	.max_speed_hz = SPI_BAUDRATE,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
};

#ifdef BASIC_EXAMPLE

struct no_os_gpio_init_param max14906_fault_gpio_param = {
	.port = GPIO_FAULT_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_FAULT_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct max149x6_init_param max14906_ip = {
	.chip_address = 0,
	.comm_param = &max14906_spi_ip,
	.fault_gpio_param = &max14906_fault_gpio_param,
	.crc_en = true,
};

#endif

#ifdef IIO_SUPPORT

struct max149x6_init_param max14906_ip = {
	.chip_address = 0,
	.comm_param = &max14906_spi_ip,
	.crc_en = true,
};

#endif
