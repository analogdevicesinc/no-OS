/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADF4030 examples.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param adf4030_uart_ip = {
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

struct no_os_spi_init_param adf4030_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 3000000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
};

struct adf4030_init_param adf4030_ip = {
	.spi_init = &adf4030_spi_ip,
	.spi_4wire_en = true,
	.cmos_3v3 = false,
	.ref_freq = 125000000,
	.vco_freq = 2500000000,
	.bsync_freq = 100000000,
	.ref_div = 1,
	.chip_addr = 0,
};

