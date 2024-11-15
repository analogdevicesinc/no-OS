/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for aducm3029 platform of eval-ltc4306-pmdz project.
 *   @author JSanbuen (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"
#include "basic_example.h"

/***************************************************************************//**
 * @brief Main function execution for ADUCM3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	ltc4306_user_init.i2c_init = iip;
	max538x_user_init.i2c_init = iip;

	ret = platform_init();
	if (ret)
		goto error;

	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &uip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart);

	ret = basic_example_main();
	if (ret)
		goto error_uart;
error_uart:
	return no_os_uart_remove(uart);


#if (BASIC_EXAMPLE != 1)
#error Please enable the example and rebuild the project.
#endif

error:
	return ret;
}
