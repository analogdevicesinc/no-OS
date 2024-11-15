/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of ADF4382 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

/**
 * @brief Main function execution for STM32 platform.
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	int ret = -EINVAL;
	adf4382_spi_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;

	stm32_init();

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &adf4382_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);
	ret = basic_example_main();
	if (ret)
		no_os_uart_remove(uart_desc);
#endif

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

	return ret;
}
