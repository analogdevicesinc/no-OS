/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by iio_demo_freeRTOS examples.
 *   @author Robert Budai (robert.budai@analog.com)
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
#ifdef ENABLE_LOOPBACK
static uint16_t loopback_buffs[DEMO_CHANNELS][SAMPLES_PER_CHANNEL];
#endif

struct no_os_gpio_init_param gpio_ip = {
	.port = GPIO_PORT,
	.number = 0,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_uart_init_param iio_demo_uart_ip = {
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

struct adc_demo_init_param adc_init_par = {
	.ext_buff_len = SAMPLES_PER_CHANNEL,
	.ext_buff = (uint16_t **)loopback_buffs,
	.dev_global_attr = 3333,
	.dev_ch_attr = {
		1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118,
		1119, 1120, 1121, 1122, 1123, 1124, 1125, 1126
	}
};

struct dac_demo_init_param dac_init_par = {
	.loopback_buffer_len = SAMPLES_PER_CHANNEL,
	.loopback_buffers = (uint16_t **)loopback_buffs,
	.dev_global_attr = 4444,
	.dev_ch_attr = {
		1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118,
		1119, 1120, 1121, 1122, 1123, 1124, 1125, 1126
	}
};

uint8_t in_buff[MAX_SIZE_BASE_ADDR] = {0};
uint8_t out_buff[MAX_SIZE_BASE_ADDR] = {0};
