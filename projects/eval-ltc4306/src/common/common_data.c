/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-ltc4306-pmdz examples.
 *   @author JSanbuen (jose.sanbuenaventura@analog.com)
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

const struct no_os_i2c_init_param iip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 100000,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA,
};

struct ltc4306_init_param ltc4306_user_init = {};

struct max538x_init_param max538x_user_init = {
	.active_device = MAX_DUT,
};
