/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for aducm3029 platform of eval-adxl355-pmdz project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
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

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
#endif

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for ADUCM3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;
	adxl355_ip.comm_init.spi_init = adxl355_spi_ip;

	ret = platform_init();
	if (ret)
		goto error;

#ifdef DUMMY_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adxl355_uart_ip);
	if (ret)
		goto error;

	no_os_uart_stdio(uart_desc);

	ret = dummy_example_main();
	if (ret)
		goto error_uart;
error_uart:
	return no_os_uart_remove(uart_desc);
#endif

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
	if (ret)
		goto error;
#endif

#ifdef IIO_TRIGGER_EXAMPLE

	struct no_os_gpio_desc *adxl355_gpio_desc;

	/* Initialize DATA READY pin */
	ret = no_os_gpio_get_optional(&adxl355_gpio_desc, &adxl355_gpio_drdy_ip);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_input(adxl355_gpio_desc);
	if (ret)
		goto error_gpio;

	ret = iio_trigger_example_main();
	if (ret)
		goto error_gpio;

error_gpio:
	return no_os_gpio_remove(adxl355_gpio_desc);
#endif

#if (DUMMY_EXAMPLE + IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (DUMMY_EXAMPLE + IIO_EXAMPLE + IIO_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

error:
	return ret;
}
