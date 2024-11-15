/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of max22017 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
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

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

int main()
{
	int ret = -EINVAL;

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &max22017_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = basic_example_main();

	no_os_uart_remove(uart_desc);
#endif

#if (BASIC_EXAMPLE + IIO_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE + IIO_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable ony one example and rebuild thhe project.
#endif

	return ret;
}
