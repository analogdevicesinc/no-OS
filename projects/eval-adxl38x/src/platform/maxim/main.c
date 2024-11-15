/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of eval-adxl38x project.
 *   @author BRajendran (balarupini.rajendran@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef BASIC_EXAMPLE
#include "basic_example_main.h"
#endif

#ifdef SELFTEST_EXAMPLE
#include "selftest_example_main.h"
#endif

#ifdef FIFO_EXAMPLE
#include "fifo_example_main.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

	adxl38x_ip.comm_init.spi_init = adxl38x_spi_ip;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adxl38x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

#ifdef BASIC_EXAMPLE
	ret = basic_example_main();
#endif

#ifdef SELFTEST_EXAMPLE
	ret = selftest_example_main();
#endif

#ifdef FIFO_EXAMPLE
	ret = fifo_example_main();
#endif

#if (BASIC_EXAMPLE + SELFTEST_EXAMPLE + FIFO_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE + SELFTEST_EXAMPLE + FIFO_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	no_os_uart_remove(uart_desc);
	return ret;
}

