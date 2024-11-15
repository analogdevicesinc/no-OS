/********************************************************************************
 *   @file   main.c
 *   @brief  Main file for Maxim platform of max31855 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform_includes.h"
#include "common_data.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/*******************************************************************************
 * @brief Main function execution for MAX32655 platform.
 *
 * @return ret - Result of the enabled examples execution.
 *******************************************************************************/
int main()
{
	int ret;

#ifdef IIO_EXAMPLE
	return iio_example_main();
#endif

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &uip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart);
	ret = basic_example_main();
	if (ret)
		goto error;
#endif

#if (IIO_EXAMPLE + BASIC_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

error:
#ifdef BASIC_EXAMPLE
	no_os_uart_remove(uart);
#endif
	return 0;
}
