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
#include "no_os_error.h"

#if defined(CONFIG_LTC4284_BASIC_EXAMPLE)
#include "basic_example.h"
#endif

#if defined(CONFIG_LTC4284_IIO_EXAMPLE)
#include "iio_example.h"
#endif

#if defined(CONFIG_LTC4284_OVERCURRENT_EXAMPLE)
#include "overcurrent_example.h"
#endif

#if defined(CONFIG_LTC4284_DUAL_CORE_EXAMPLE)
#include "dual_core_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution.
 *
 * @return ret - Result of the enabled example execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

#if defined(CONFIG_LTC4284_IIO_EXAMPLE)
	/* iio_app_init() initializes the UART itself. */
	ret = iio_example_main();
#else
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ltc4284_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

#if defined(CONFIG_LTC4284_BASIC_EXAMPLE)
	ret = basic_example_main();
#elif defined(CONFIG_LTC4284_OVERCURRENT_EXAMPLE)
	ret = overcurrent_example_main();
#elif defined(CONFIG_LTC4284_DUAL_CORE_EXAMPLE)
	ret = dual_core_example_main();
#endif

	no_os_uart_remove(uart_desc);
#endif

	return ret;
}
