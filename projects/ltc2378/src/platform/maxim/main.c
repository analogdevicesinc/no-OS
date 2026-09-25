/********************************************************************************
 *   @file   main.c
 *   @brief  Main file for Maxim platform of ltc2378 project.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/*******************************************************************************
 * @brief Main function execution for LTC2378 platform.
 *
 * @return ret - Result of the enabled examples execution.
 *******************************************************************************/
int main()
{
	int ret;
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &uip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart);
	ret = example_main();
	if (ret)
		goto error;

error:
	no_os_uart_remove(uart);

	return ret;
}
