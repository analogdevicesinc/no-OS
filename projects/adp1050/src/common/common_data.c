/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by adp1050 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"

struct no_os_uart_init_param adp1050_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA,
};

struct no_os_i2c_init_param adp1050_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 100000,
	.platform_ops = I2C_OPS,
	.slave_address = ADP1050_PMBUS_10KOHM_ADDRESS,
	.extra = I2C_EXTRA,
};

struct no_os_gpio_init_param adp1050_pg_alt_ip = {
	.port = GPIO_PG_ALT_PORT,
	.number = GPIO_PG_ALT_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct adp1050_init_param adp1050_ip = {
	.i2c_param = &adp1050_i2c_ip,
	.pg_alt_param = &adp1050_pg_alt_ip,
	.flgi_param = NULL,
	.syni_param = NULL,
	.on_off_config = ADP1050_ON_OFF_DEFAULT_CFG,
};
