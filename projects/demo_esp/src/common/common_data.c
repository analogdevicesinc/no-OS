/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by demo_esp examples.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_CONFIG_BAUDRATE,
	.irq_id = UART_CONFIG_IRQ_ID,
	.asynchronous_rx = true,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = UART_OPS,
	.extra = UART_EXTRA
};

struct no_os_irq_init_param irq_ip = {
	.irq_ctrl_id = 0,
	.platform_ops = IRQ_OPS,
	.extra = 0
};

struct no_os_timer_init_param timer_ip = {
	.id = TIMER_ID,
	.freq_hz = TIMER_FREQ,
	.ticks_count = 0,
	.platform_ops = TIMER_OPS,
	.extra = TIMER_EXTRA
};

struct no_os_gpio_init_param gpio_wifi_rst_ip = {
	.port = GPIO_PORT,
	.number = GPIO_NR,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA
};
