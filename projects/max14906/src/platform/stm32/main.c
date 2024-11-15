/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of max14906 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "no_os_error.h"
#include "platform_includes.h"
#include "common_data.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/*******************************************************************************
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

	max14906_spi_extra_ip.get_input_clock = HAL_RCC_GetPCLK1Freq;
	stm32_init();

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart;

	ret = no_os_uart_init(&uart, &max14906_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);
	ret = basic_example_main();

	no_os_uart_remove(uart);
#endif

#if (IIO_EXAMPLE + BASIC_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}
