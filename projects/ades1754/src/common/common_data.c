/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ades1754 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"

struct no_os_uart_init_param ades1754_uart_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.platform_ops = UART_OPS,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA,
};

struct no_os_uart_init_param ades1754_comm_ip = {
	.device_id = COMM_DEVICE_ID,
	.baud_rate = COMM_BAUDRATE,
	.asynchronous_rx = false,
	.size = NO_OS_UART_CS_8,
	.platform_ops = COMM_OPS,
	.parity = NO_OS_UART_PAR_ODD,
	.stop = NO_OS_UART_STOP_2_BIT,
	.extra = COMM_EXTRA,
};

struct ades1754_init_param ades1754_ip = {
	.uart_param = &ades1754_comm_ip,
	.id = ID_ADES1754,
	.uart_bridge = true,
	.dev_addr = 0,
	.no_dev = 1,
};
