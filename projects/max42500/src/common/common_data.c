/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by max42500 examples.
 *   @author Joshua Maniti (Joshua.Maniti@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "maxim_gpio.h"
#include "maxim_i2c.h"
#include "maxim_uart.h"

struct no_os_uart_init_param uart_ip = {
	.device_id             = 0,
	.asynchronous_rx       = false,
	.baud_rate             = 115200,
	.size                  = NO_OS_UART_CS_8,
	.parity                = NO_OS_UART_PAR_NO,
	.stop                  = NO_OS_UART_STOP_1_BIT,
	.extra                 = &uart_extra_ip,
	.platform_ops          = &max_uart_ops,
};

struct max42500_init_param max42500_ip = {
	.comm_param = {
		.device_id      = MAX42500_I2C_DEVICE_ID,
		.max_speed_hz   = MAX42500_I2C_CLK_SPEED,
		.slave_address  = MAX42500_I2C_ADDR,
		.platform_ops   = &max_i2c_ops,
		.extra          = (void *)&max42500_i2c_ip,
	},
	.en0_param = {
		.port           = MAX42500_EN0_PORT,
		.number         = MAX42500_EN0_PIN,
		.pull           = NO_OS_PULL_DOWN,
		.platform_ops   = &max_gpio_ops,
		.extra          = (void *)&max42500_gpio_extra,
	},
	.en1_param = {
		.port           = MAX42500_EN1_PORT,
		.number         = MAX42500_EN1_PIN,
		.pull           = NO_OS_PULL_DOWN,
		.platform_ops   = &max_gpio_ops,
		.extra          = (void *)&max42500_gpio_extra,
	},
	.addr_param = {
		.port           = MAX42500_ADDR_PORT,
		.number         = MAX42500_ADDR_PIN,
		.pull           = NO_OS_PULL_DOWN,
		.platform_ops   = &max_gpio_ops,
		.extra          = (void *)&max42500_gpio_extra,
	},
	.addr_sel   = MAX42500_ADDR_SEL,
	.pece       = MAX42500_PECE,
	.vmon_en    = MAX42500_VMON_EN,
	.vmon_vmpd  = MAX42500_VMON_VMPD,
	.reset_map  = MAX42500_RESET_MAP,
};
