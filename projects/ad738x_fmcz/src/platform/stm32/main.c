/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for STM32 platform of eval-ad738x project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#elif defined(IIO_EXAMPLE)
#include "iio_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for STM32 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
#define AD738X_ADC_REF_VOLTAGE 3300

int main()
{
	int ret;
	struct no_os_uart_desc *uart;

	stm32_init();
	ret = no_os_uart_init(&uart, &ad738x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

#ifdef BASIC_EXAMPLE
	ret = basic_example_main();
#elif defined(IIO_EXAMPLE)

	ret = iio_example_main();
#else
#error At least one example has to be selected using y value in Makefile.
#endif
}
