/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ltc7841 examples.
 *   @author Marvin Cabuenas (marvinneil.cabuenas@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"

struct no_os_uart_init_param uart_ip = {
	.device_id             = 2,
	.asynchronous_rx       = false,
	.baud_rate             = 115200,
	.size                  = NO_OS_UART_CS_8,
	.parity                = NO_OS_UART_PAR_NO,
	.stop                  = NO_OS_UART_STOP_1_BIT,
	.extra                 = &uart_extra_ip,
	.platform_ops          = UART_OPS,
};

struct no_os_i2c_init_param ltc7841_i2c_ip = {
	.device_id      = LTC7841_I2C_DEVICE_ID,
	.max_speed_hz   = LTC7841_I2C_CLK_SPEED,
	.slave_address  = LTC7841_I2C_ADDR,
	.platform_ops   = I2C_OPS,
	.extra          = &ltc7841_i2c_extra,
};

struct ltc7841_init_param ltc7841_ip = {
	.comm_param = &ltc7841_i2c_ip,
};
