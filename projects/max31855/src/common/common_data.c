/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by max31855 examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common_data.h"

struct no_os_uart_init_param uip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

const struct no_os_spi_init_param max31855_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_MAX_SPEED,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.parent = NULL,
};

struct max31855_init_param max31855_ip = {
	.spi_init = max31855_spi_ip,
};
