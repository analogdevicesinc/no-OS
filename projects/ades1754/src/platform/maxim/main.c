/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of ades1754 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "common_data.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

extern int example_main();

int main()
{
	int ret = -EINVAL;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ades1754_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = example_main();

	no_os_uart_remove(uart_desc);

	return ret;
}
