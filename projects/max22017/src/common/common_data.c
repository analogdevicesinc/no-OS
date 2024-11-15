/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by max22017 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"

struct no_os_uart_init_param max22017_uart_ip = {
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

struct no_os_spi_init_param max22017_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.extra = SPI_EXTRA,
	.max_speed_hz = SPI_BAUDRATE,
	.platform_ops = SPI_OPS,
	.chip_select = SPI_CS,
};

struct no_os_gpio_init_param max22017_rstb_ip = {
	.port = GPIO_RSTB_PORT_NUM,
	.pull = NO_OS_PULL_NONE,
	.number = GPIO_RSTB_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct max22017_init_param max22017_ip = {
	.comm_param = &max22017_spi_ip,
	.crc_en = false,
	.ext_dac_ref = false,
	.rstb_param = &max22017_rstb_ip,
};
