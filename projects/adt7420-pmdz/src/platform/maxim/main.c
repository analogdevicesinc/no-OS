/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for MAX32650 platform of adt7420-pmdz project.
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for MAX32655 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
	if (ret)
		goto error;
#endif

#ifdef DUMMY_EXAMPLE
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &uip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart);
	ret = dummy_example_main();
	if (ret)
		goto error;
#endif

#if (IIO_EXAMPLE+DUMMY_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

error:
#ifdef DUMMY_EXAMPLE
	no_os_uart_remove(uart);
#endif
	return 0;
}
