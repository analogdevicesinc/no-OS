/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ltm4686 examples.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"

struct no_os_uart_init_param ltm4686_uart_ip = {
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

struct no_os_i2c_init_param ltm4686_i2c_ip = {
	.device_id = 1,
	.max_speed_hz = 100000,
	.platform_ops = I2C_OPS,
	.slave_address = LTM4686_ADDRESS,
	.extra = I2C_EXTRA,
};

struct ltm4686_init_param ltm4686_ip = {
	.i2c_init = &ltm4686_i2c_ip,
	.crc_en = true,
	.id = ID_LTM4686,
};
