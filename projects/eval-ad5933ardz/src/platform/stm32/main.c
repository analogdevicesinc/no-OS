/********************************************************************************
 *   @file   main.c
 *   @brief  Main file for STM32 platform of eval-ad5933ardz project.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
 ********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include <stdio.h>
#include <no_os_alloc.h>

int example_main(void);

/***************************************************************************//**
 * @brief Main function execution for MAX32690 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main(void)
{
	stm32_init();

#ifdef IIO_SUPPORT
	return example_main();
#endif


	int ret;
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &uip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	return example_main();
}
