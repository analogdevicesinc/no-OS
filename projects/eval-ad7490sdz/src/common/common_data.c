/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Common data used within the eval-ad7490sdz project.
 *   @author Radu Sabau (radu.sabau@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "common_data.h"
#include <stdbool.h>

struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.asynchronous_rx = true,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

struct no_os_spi_init_param ad7490_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.mode = NO_OS_SPI_MODE_2,
	.chip_select = SPI_CS,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
};

struct ad7490_init_param ad7490_ip = {
	.spi_param = &ad7490_spi_ip,
	.op_mode = AD7490_MODE_NORMAL,
	.vdd = AD7490_VDD_5V,
};
