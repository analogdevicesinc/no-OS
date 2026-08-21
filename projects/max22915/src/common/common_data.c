/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by MAX22915 example.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "max22915.h"

struct no_os_uart_init_param max22915_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &max22915_uart_extra_ip,
	.platform_ops = &uart_platform_ops,
};

struct no_os_spi_init_param max22915_spi_init = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 50000,
	.mode = NO_OS_SPI_MODE_0,
	.chip_select = GPIO_CS_PIN,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = &spi_platform_ops,
	.extra = &max22915_spi_extra_ip
};

struct max22915_init_param max22915_ip = {
	.spi_init = &max22915_spi_init,
	.sw_state = {
		SWITCH_OPEN, SWITCH_CLOSE, SWITCH_CLOSE, SWITCH_CLOSE,
		SWITCH_CLOSE, SWITCH_CLOSE, SWITCH_CLOSE, SWITCH_OPEN
	},
	.ow_config = {
		{.owoff_enable = true, .owon_enable = false},
		{.owoff_enable = true, .owon_enable = false},
		{.owoff_enable = true, .owon_enable = false},
		{.owoff_enable = true, .owon_enable = false},
		{.owoff_enable = true, .owon_enable = true},
		{.owoff_enable = true, .owon_enable = false},
		{.owoff_enable = true, .owon_enable = false},
		{.owoff_enable = false, .owon_enable = false},
	},
	.shtvdd_enable = {
		true, true, false, true,
		true, true, true, true
	},
	.curr_config = {
		.curr_average = 0x0,
		.curr_one_shot_en = false,
		.curr_cont = false,
	},
	.temp_config = {
		.temp_average = 0x4,
		.therm_warn_en = true,
		.temp_cont = true
	},
	.crc_en = false,
	.chip_address = 0,
};
