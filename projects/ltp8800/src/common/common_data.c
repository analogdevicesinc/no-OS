/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ltp8800 examples.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"

struct no_os_uart_init_param ltp8800_uart_ip = {
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

struct no_os_i2c_init_param ltp8800_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = 100000,
	.platform_ops = I2C_OPS,
	.slave_address = LTP8800_ADDRESS,
	.extra = I2C_EXTRA,
};

struct no_os_gpio_init_param ltp8800_ctrl_gpio_ip = {
	.port = CTRL_GPIO_PORT,
	.number = CTRL_GPIO_NUMBER,
	.pull = NO_OS_PULL_UP,
	.platform_ops = CTRL_GPIO_OPS,
	.extra = CTRL_GPIO_EXTRA,
};

struct ltp8800_init_param ltp8800_ip = {
	.i2c_init = &ltp8800_i2c_ip,
	.smbalert_param = NULL,
	.ctrl_param = &ltp8800_ctrl_gpio_ip,
	.ext_clk_param = NULL,
	.external_clk_en = false,
	.crc_en = true,
};
