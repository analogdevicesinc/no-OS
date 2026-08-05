/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of iio_demo project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "platform_includes.h"
#include "common_data.h"

#ifdef CONFIG_IIO_DEMO_IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef CONFIG_IIO_DEMO_IIO_SW_TRIGGER_EXAMPLE
#include "iio_trigger_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -1;

#if TARGET_NUM==32660
#error TARGET MAX32660 not supported
#endif

#ifdef CONFIG_IIO_DEMO_IIO_EXAMPLE
	ret = iio_example_main();
#endif

#ifdef CONFIG_IIO_DEMO_IIO_SW_TRIGGER_EXAMPLE
#error Software trigger is not supported over UART.
#endif

#ifdef CONFIG_IIO_DEMO_IIO_TIMER_TRIGGER_EXAMPLE
	ret = iio_timer_trigger_example_main();
#endif

#if (CONFIG_IIO_DEMO_IIO_EXAMPLE + CONFIG_IIO_DEMO_IIO_SW_TRIGGER_EXAMPLE + CONFIG_IIO_DEMO_IIO_TIMER_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (CONFIG_IIO_DEMO_IIO_EXAMPLE + CONFIG_IIO_DEMO_IIO_SW_TRIGGER_EXAMPLE + CONFIG_IIO_DEMO_IIO_TIMER_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}
