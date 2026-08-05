/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ltc2378 examples.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "common_data.h"
#include <stdbool.h>
#include "parameters.h"

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

const struct no_os_spi_init_param ltc2378_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_MAX_SPEED,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA,
	.parent = NULL,
};

const struct no_os_gpio_init_param ltc2378_gpio_cnv = {
	.port = GPIO_CNV_PORT_NUM,
	.number = GPIO_CNV_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

const struct no_os_gpio_init_param ltc2378_gpio_busy = {
	.port = GPIO_BUSY_PORT_NUM,
	.number = GPIO_BUSY_PIN_NUM,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};

const struct ltc2378_init_param ltc2378_ip = {
	.spi_init = &ltc2378_spi_ip,
	.gpio_cnv_init = &ltc2378_gpio_cnv,
	.gpio_busy_init = &ltc2378_gpio_busy,
	.vref_uv = LTC2378_DEFAULT_VREF_UV,
	.input_mode = LTC2378_UNIPOLAR
};
