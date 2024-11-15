/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for pico platform of eval-adis1654x project.
 *   @author RBolboac (ramona.gradinariu@analog.com)
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

#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger_example.h"
#endif

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/******************************************************************************/
/************************* Functions Definitions ******************************/
/******************************************************************************/

/**
 * @brief Main function execution for pico platform.
 *
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	int ret = -EINVAL;
	adis1654x_ip.spi_init = &adis1654x_spi_ip;

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adis1654x_uart_ip);
	if (ret)
		return ret;

	ret = basic_example_main();
	no_os_uart_remove(uart_desc);
#endif

#ifdef IIO_TRIGGER_EXAMPLE
	ret = iio_trigger_example_main();
#endif

#if (BASIC_EXAMPLE + IIO_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE + IIO_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}
