/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by issd1306 example.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct display_dev *oled_display;
struct no_os_i2c_desc *oled_display_i2c_desc;


struct no_os_uart_init_param demo_uart_ip = {
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

struct no_os_i2c_init_param oled_display_i2c_init_param = {
	.device_id = 1,
	.max_speed_hz = 400000,
	.slave_address = SSD1306_I2C_ADDR,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA,
};

ssd_1306_extra oled_display_extra = {
	.comm_type = SSD1306_I2C,
	.i2c_ip = &oled_display_i2c_init_param,
};

struct display_init_param oled_display_ini_param = {
	.cols_nb = 128,
	.rows_nb = 64,
	.controller_ops = &ssd1306_ops,
	.extra = &oled_display_extra,
};

