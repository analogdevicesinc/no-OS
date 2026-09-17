/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ada4250_ardz examples.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param ada4250_uart_ip = {
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

struct no_os_spi_init_param ada4250_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.chip_select = SPI_CS,
	.max_speed_hz = SPI_BAUDRATE,
	.mode = NO_OS_SPI_MODE_3,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
};

struct ada4250_init_param ada4250_ip = {
	.device_id = ADA4250,
	.spi_init = &ada4250_spi_ip,
	.refbuf_en = ADA4250_BUF_DISABLE,
	.bias = ADA4250_BIAS_DISABLE,
	.gain = ADA4250_GAIN_8,
	.avdd_v = 5000,
	.offset_nv = 0,
};
