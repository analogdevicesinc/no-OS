/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of the basic example.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "common_data.h"

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int basic_example_main()
{
	struct no_os_uart_desc *uart_desc;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	printf("Hello world");

	return 0;
}
