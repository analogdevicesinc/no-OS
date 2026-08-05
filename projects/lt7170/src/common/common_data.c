/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by lt7170 examples.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"

struct no_os_uart_init_param lt7170_uart_ip = {
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

struct no_os_i2c_init_param lt7170_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 100000,
	.platform_ops = I2C_OPS,
	.slave_address = LT7170_PMBUS_ADDRESS,
	.extra = I2C_EXTRA,
};

struct lt7170_init_param lt7170_ip = {
	.i2c_init = &lt7170_i2c_ip,
	.pg_param = NULL,
	.run_param = NULL,
	.alert_param = NULL,
	.fault_param = NULL,
	.fault_cfg = LT7170_FAULT_PIN_OUTPUT,
	.chip_id = ID_LT7170,
	.external_clk_en = false,
	.crc_en = false,
};
