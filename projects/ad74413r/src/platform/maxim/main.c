/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of ad74413r project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include "no_os_error.h"

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
#endif

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

#ifdef DUMMY_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ad74413r_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	ret = dummy_example_main();
#endif

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

#ifdef IIO_TRIGGER_EXAMPLE
#error IIO trigger example is not supported on Maxim platform.
#endif

#if (DUMMY_EXAMPLE + IIO_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (DUMMY_EXAMPLE + IIO_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}
