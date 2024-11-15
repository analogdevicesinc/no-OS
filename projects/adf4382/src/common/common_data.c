/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by adf4382 examples.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common_data.h"

struct no_os_uart_init_param adf4382_uart_ip = {
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

struct no_os_spi_init_param adf4382_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 1500000,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
};

struct adf4382_init_param adf4382_ip = {
	.spi_init = &adf4382_spi_ip,
	.spi_3wire_en = false,
	.cmos_3v3 = false,
	.ref_freq_hz = 125000000,
	.freq = 2875000000,
	.ref_doubler_en = 1,
	.ref_div = 1,
	.cp_i = 15,
	.bleed_word = 4903,
	.ld_count = 10,
	.id = ID_ADF4382A,
};
