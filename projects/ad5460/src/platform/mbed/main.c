/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Mbed platform of ad5460 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

#ifdef CHANNEL_OUTPUT_EXAMPLE
#include "channel_output_example.h"
#endif

#ifdef VOLTAGE_OUTPUT_EXAMPLE
#include "voltage_output_example.h"
#endif

#ifdef CURRENT_OUTPUT_EXAMPLE
#include "current_output_example.h"
#endif

/***************************************************************************//**
 * @brief Main function for Mbed platform.
 *
 * @return ret - Result of the enabled examples.
*******************************************************************************/

int main()
{
	int ret;
	ad5460_ip.spi_ip = ad5460_spi_ip;

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc* uart;
	ret = no_os_uart_init(&uart, &ad5460_uart_ip);
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

#ifdef CHANNEL_OUTPUT_EXAMPLE
	struct no_os_uart_desc* uart;
	ret = no_os_uart_init(&uart, &ad5460_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);
	ret = channel_output_example_main();
	no_os_uart_remove(uart);
	if (ret)
		return ret;
#endif

#ifdef VOLTAGE_OUTPUT_EXAMPLE
	struct no_os_uart_desc* uart;
	ret = no_os_uart_init(&uart, &ad5460_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);
	ret = voltage_output_example_main();
	no_os_uart_remove(uart);
	if (ret)
		return ret;
#endif

#ifdef CURRENT_OUTPUT_EXAMPLE
	struct no_os_uart_desc* uart;
	ret = no_os_uart_init(&uart, &ad5460_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);
	ret = current_output_example_main();
	no_os_uart_remove(uart);
	if (ret)
		return ret;
#endif

#if (BASIC_EXAMPLE+CHANNEL_OUTPUT_EXAMPLE+VOLTAGE_OUTPUT_EXAMPLE+CURRENT_OUTPUT_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif
	return 0;
}
