/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of the ADIN1140 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

#define _STR(x) #x
#define STR(x)  _STR(x)

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adin1140_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	pr_info("Running " STR(EXAMPLE) " on " STR(TARGET) "\n");

	return example_main();
}
