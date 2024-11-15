/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by adt7420-pmdz examples.
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
	.extra = &xuip,
};

const struct no_os_i2c_init_param iip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 100000,
	.slave_address = 0x48,
	.extra = NULL,
	.platform_ops = I2C_OPS,
	.extra = &adt7420_i2c_extra,
};

struct adt7420_init_param adt7420_user_init = {
	.interface_init = iip,
	.active_device = ID_ADT7420
};
