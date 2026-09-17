/*******************************************************************************
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ltc4162l examples.
 *   @author Marc Paolo Sosa (marcpaolo.sosa@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

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

const struct no_os_i2c_init_param ltc4162l_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 100000,
	.slave_address = 0x68,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA,
};

struct ltc4162l_init_param ltc4162l_ip = {
	.i2c_init_param = ltc4162l_i2c_ip,
	.rsnsb = 1000,
	.rsnsi = 1000,
	.id = ID_LTC4162L,
};
