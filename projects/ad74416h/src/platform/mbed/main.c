/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Mbed platform of ad74416h project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/***************************************************************************//**
 * @brief Main function for Mbed platform.
 *
 * @return ret - Result of the enabled examples.
*******************************************************************************/

int main()
{
	int ret;
	ad74416h_ip.spi_ip = ad74416h_spi_ip;

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc* uart;
	ret = no_os_uart_init(&uart, &ad74416h_uart_ip);
	if (ret) {
		no_os_uart_remove(uart);
		return ret;
	}
	no_os_uart_stdio(uart);
	ret = basic_example_main();
	if (ret) {
		no_os_uart_remove(uart);
		return ret;
	}
#endif

#if (IIO_EXAMPLE+BASIC_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif
	return 0;
}
