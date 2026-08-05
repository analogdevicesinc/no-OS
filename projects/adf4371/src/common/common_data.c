/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by adf4371 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"

struct no_os_uart_init_param adf4371_uart_ip = {
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

struct no_os_spi_init_param adf4371_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
	.extra = SPI_EXTRA,
};

struct no_os_gpio_init_param adf4371_ce_ip = {
	.port = GPIO_CE_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_CE_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

struct adf4371_chan_spec chan_spec[1] = {
	{
		.num = 0,
		.power_up_frequency = 8000000000,
	},
};

struct adf4371_init_param adf4371_ip = {
	.spi_init = &adf4371_spi_ip,
	.channels = chan_spec,
	.spi_3wire_enable = false,
	.clkin_frequency = 50000000,
	.muxout_select = 11,
	.num_channels = 1,
};
