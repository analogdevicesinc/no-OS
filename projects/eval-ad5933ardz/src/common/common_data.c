/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by eval-ad5933ardz examples.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "ad5933.h"

struct no_os_uart_init_param uip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = &uart_extra_ip,
};

const struct no_os_i2c_init_param ad5933_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_MAX_SPEED,
	.slave_address = AD5933_ADDRESS,
	.platform_ops = I2C_OPS,
	.extra = &i2c_extra_ip,
};

struct ad5933_init_param ad5933_user_init = {
	.i2c_init = ad5933_i2c_ip,
	.clock_freq = EXTERNAL_CLK_SPEED,
	.clock_source = AD5933_CLOCK_SOURCE_EXTERNAL,
	.pga_gain = AD5933_GAIN_X1,
	.output_range = AD5933_RANGE_2000mVpp,
	.settling_cycles = AD5933_SETTLING_CYCLES,
};
