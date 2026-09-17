/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of adp1055 project.
 *   @author Ivan Gil Mercano (ivangil.mercano@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

int main()
{
	int ret = -EINVAL;

	struct no_os_uart_desc *uart_desc;

	/** Initialize UART interface */
	ret = no_os_uart_init(&uart_desc, &adp1055_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = example_main();
	if (ret)
		return ret;

	return ret;
}
