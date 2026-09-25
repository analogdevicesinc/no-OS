/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main entry point for LTC4284 project on Maxim platform
 *   @author Carlos Jones Jr <carlosjr.jones@analog.com>
 *   @author Christopher de Guzman <christopher.deguzman@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_print_log.h"

extern int example_main();

int main()
{
	int ret;
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ltc4284_uart_ip);
	if (ret) {
		pr_err("UART initialization failed: %d\n", ret);
		return ret;
	}

	no_os_uart_stdio(uart_desc);
	return example_main();
}
