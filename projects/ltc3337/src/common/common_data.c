/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ltc3337 examples.
 *   @author Brent Kowal (brent.kowal@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common_data.h"

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

const struct no_os_i2c_init_param ltc3337_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_MAX_SPEED,
	.slave_address = LTC3337_I2C_ADDR,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA,
};

struct ltc3337_init_param ltc3337_ip = {
	.prescale = 10,
	.i2c_init = ltc3337_i2c_ip,
};
