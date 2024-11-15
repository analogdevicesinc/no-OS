/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Mbed platform of adt7420-pmdz project.
 *   @author CMinajigi (chandrakant.minajigi@analog.com)
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

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
#endif

/***************************************************************************//**
 * @brief Main function for Mbed platform.
 *
 * @return ret - Result of the enabled examples.
*******************************************************************************/

int main()
{
	int ret;
#ifdef IIO_EXAMPLE
	ret = iio_example_main();
	if (ret)
		return ret;
#endif

#ifdef DUMMY_EXAMPLE
	struct no_os_uart_desc* uart;
	ret = no_os_uart_init(&uart, &uip);
	if (ret) {
		no_os_uart_remove(uart);
		return ret;
	}
	no_os_uart_stdio(uart);
	ret = dummy_example_main();
	if (ret) {
		no_os_uart_remove(uart);
		return ret;
	}
#endif

#if (IIO_EXAMPLE+DUMMY_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif
	return 0;
}
