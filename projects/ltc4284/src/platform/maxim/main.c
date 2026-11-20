/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main entry point for LTC4284 project on Maxim platform
 *   @author Carlos Jones Jr <carlosjr.jones@analog.com>
 *   @author Christopher de Guzman <christopher.deguzman@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_print_log.h"

#if defined(CONFIG_LTC4284_BASIC_EXAMPLE)
#include "basic_example.h"
#endif

#if defined(CONFIG_LTC4284_IIO_EXAMPLE)
#include "iio_example.h"
#endif

#if defined(CONFIG_LTC4284_OVERCURRENT_EXAMPLE)
#include "overcurrent_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution.
 *
 * @return ret - Result of the enabled example execution.
*******************************************************************************/
int main()
{
	int ret;
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ltc4284_uart_ip);
	if (ret) {
		pr_err("UART initialization failed: %d\n", ret);
		return ret;
	}

	no_os_uart_stdio(uart_desc);

#if defined(CONFIG_LTC4284_BASIC_EXAMPLE)
	return basic_example_main();
#elif defined(CONFIG_LTC4284_IIO_EXAMPLE)
	return iio_example_main();
#elif defined(CONFIG_LTC4284_OVERCURRENT_EXAMPLE)
	return overcurrent_example_main();
#endif

	return 0;
}
