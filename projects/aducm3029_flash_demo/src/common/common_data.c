/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by aducm3029_flash_demo examples.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param flash_demo_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_irq_init_param flash_demo_irq_ip = {
	.irq_ctrl_id = IRQ_CTRL_ID,
	.platform_ops = IRQ_OPS,
	.extra = IRQ_EXTRA,
};

struct no_os_flash_init_param flash_demo_flash_ip = {
	.id = FLASH_DEVICE_ID,
};
