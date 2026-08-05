/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by LTM3360B example.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "no_os_uart.h"
#include "no_os_i2c.h"
#include "ltm3360b.h"

struct no_os_uart_init_param ltm3360b_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_i2c_init_param ltm3360b_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_BAUDRATE,
	.slave_address = LTM3360B_I2C_ADDR,
	.platform_ops = I2C_OPS,
};

struct ltm3360b_init_param ltm3360b_ip = {
	.i2c_init = &ltm3360b_i2c_ip,
	.auto_enable = true,
	.imon_threshold = LTM3360B_IMON_TH_79_PERCENT,
	.vout_mv = 1000
};
