/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for xilinx platform of pulsar-adc project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
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

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#elif defined(IIO_EXAMPLE)
#include "iio_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for xilinx platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

#ifdef BASIC_EXAMPLE
	ret = basic_example_main();
#elif defined(IIO_EXAMPLE)
	ret = iio_example_main();
#else
#error At least one example has to be selected using y value in Makefile.
#endif
	return ret;
}
