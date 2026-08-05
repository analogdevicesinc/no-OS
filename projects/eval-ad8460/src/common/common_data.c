/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad8460 examples.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "common_data.h"
#include <stdbool.h>

struct no_os_uart_init_param uip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

const struct no_os_spi_init_param ad8460_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_MAX_SPEED,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.parent = NULL,
};

const struct no_os_gpio_init_param ad8460_gpio_rstn = {
	.port = GPIO_RSTN_PORT_NUM,
	.number = GPIO_RSTN_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct ad8460_init_param ad8460_ip = {
	.spi_init_param = ad8460_spi_ip,
	.gpio_rstn = ad8460_gpio_rstn,
	.refio_1p2v_mv = 1200,
	.ext_resistor_ohms = 2000,
};
