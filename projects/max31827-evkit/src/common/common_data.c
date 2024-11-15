/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by max31827 examples.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

const struct no_os_i2c_init_param max31827_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 100000,
	.slave_address = 0x42,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA,
};

struct max31827_init_param max31827_ip = {
	.i2c_init_param = max31827_i2c_ip,
	.comp_int = 0,
	.alarm_pol = 0,
	.fault_q = 1,
	.timeout_enable = 0,
};
