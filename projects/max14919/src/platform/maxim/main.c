/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of max14919 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"
#include "basic_example.h"

int main()
{
	int ret = -EINVAL;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &max14919_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = basic_example_main();

	no_os_uart_remove(uart_desc);

	return ret;
}
