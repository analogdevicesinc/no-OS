/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of ltc4296 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

int main()
{
	int ret = -EINVAL;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ltc4296_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	ret = basic_example_main();
	no_os_uart_remove(uart_desc);

#if (BASIC_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#endif

	return ret;
}
