/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by MAX22007 example.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "max22007.h"

struct no_os_uart_init_param max22007_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &max22007_uart_extra_ip,
	.platform_ops = &uart_platform_ops,
};

struct no_os_spi_init_param max22007_spi_init = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 5000000,
	.mode = NO_OS_SPI_MODE_0,
	.chip_select = GPIO_CS_PIN,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = &spi_platform_ops,
	.extra = &max22007_spi_extra_ip
};

struct max22007_init_param max22007_ip = {
	.comm_param = &max22007_spi_init,
	.crc_en = false,
	.ref_mode = INTERNAL_REF,
	.timeout_config = {
		.timeout_en = false,
		.timeout_sel = MAX22007_TIMEOUT_100MS,
		.timeout_cnfg = TIMEOUT_RESET,
	},
	.channel_config = {
		[0] = {
			.channel_mode = MAX22007_VOLTAGE_MODE,
			.latch_mode = TRANSPARENT_LATCH,
			.channel_power = MAX22007_CH_POWER_ON,
		},
		[1] = {
			.channel_mode = MAX22007_VOLTAGE_MODE,
			.latch_mode = LDAC_CONTROL,
			.channel_power = MAX22007_CH_POWER_OFF,
		},
		[2] = {
			.channel_mode = MAX22007_VOLTAGE_MODE,
			.latch_mode = LDAC_CONTROL,
			.channel_power = MAX22007_CH_POWER_OFF,
		},
		[3] = {
			.channel_mode = MAX22007_VOLTAGE_MODE,
			.latch_mode = LDAC_CONTROL,
			.channel_power = MAX22007_CH_POWER_ON,
		},
	}
};
