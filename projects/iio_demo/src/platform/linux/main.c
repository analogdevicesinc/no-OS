/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for linux platform of iio_demo project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "common_data.h"
#include "no_os_error.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef IIO_SW_TRIGGER_EXAMPLE
#include "iio_sw_trigger_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for linux platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#endif

#ifdef IIO_SW_TRIGGER_EXAMPLE
	ret = iio_sw_trigger_example_main();
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
#error Timer trigger example is not supported on linux platform.
#endif

#if (IIO_EXAMPLE + IIO_SW_TRIGGER_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (IIO_EXAMPLE + IIO_SW_TRIGGER_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

	return ret;
}
