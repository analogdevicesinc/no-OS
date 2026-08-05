/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by max17616 examples.
 *   @author Carlos Jones (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"

struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEVICE_ID,
	.asynchronous_rx = UART_ASYNC_RX,
	.baud_rate = UART_BAUDRATE,
	.size = UART_SIZE,
	.parity = UART_PARITY,
	.stop = UART_STOP,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_i2c_init_param i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_CLK_SPEED,
	.slave_address = I2C_ADDR,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA
};

struct max17616_init_param max17616_ip = {
	.i2c_init = &i2c_ip,
	.chip_id = ID_MAX17616,
};
