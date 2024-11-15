/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ad74413r examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
struct no_os_uart_init_param adt75_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

const struct no_os_i2c_init_param i2c_param = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 400000,
	.platform_ops = I2C_OPS,
	.slave_address = ADT75_ADDR(0),
	.extra = I2C_EXTRA,
};

struct adt75_init_param adt75_ip = {
	.comm_param = i2c_param,
};
