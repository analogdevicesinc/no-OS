/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by cn0531 examples.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param cn0531_uart_ip = {
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

struct ad5791_init_param cn0531_ad5791_ip = {
	.act_device = ID_AD5791,
	.gpio_clr = {
		.number = GPIO_CLR_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_ldac = {
		.number = GPIO_LDAC_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.gpio_reset = {
		.number = GPIO_RESET_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	},
	.spi_init = {
		.device_id = SPI_DEVICE_ID,
		.chip_select = SPI_CS,
		.max_speed_hz = SPI_BAUDRATE,
		.mode = NO_OS_SPI_MODE_3,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	},
};

struct ad5791_iio_init_param cn0531_ad5791_iio_ip = {
	.ad5791_initial = &cn0531_ad5791_ip,
	.vref_mv = 5000,
	.vref_neg_mv = 5000,
};
