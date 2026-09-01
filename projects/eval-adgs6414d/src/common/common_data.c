/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by adgs6414d examples.
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param adgs6414d_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_spi_init_param adgs6414d_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.chip_select = SPI_CS,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
};

struct adgs6414d_init_param adgs6414d_ip = {
	.spi_init = &adgs6414d_spi_ip,
	.initial_state = 0x00,
	.crc_en = false,
	.sclk_err_en = true,
	.rw_err_en = true,
};
