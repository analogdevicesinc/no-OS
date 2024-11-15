/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of ad7091r8-sdz project.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "no_os_error.h"
#include "common_data.h"
#include "platform_includes.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
#include "iio_timer_trigger_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
	ret = iio_timer_trigger_example_main();
#endif

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ad7091r8_uart_ip);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	no_os_uart_stdio(uart_desc);
	ret = basic_example_main();
	no_os_uart_remove(uart_desc);
#endif

#if (BASIC_EXAMPLE + IIO_EXAMPLE + IIO_TIMER_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE + IIO_EXAMPLE + IIO_TIMER_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}
