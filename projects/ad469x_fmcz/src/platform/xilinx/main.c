/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for xilinx platform of ad469x_fmcz project.
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
#ifdef IIO_EXAMPLE
#include "iio_example.h"
#elif defined(BASIC_EXAMPLE)
#include "basic_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for xilinx platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret = -EINVAL;

	Xil_DCacheDisable();
	Xil_ICacheDisable();

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
#elif defined (BASIC_EXAMPLE)
	ret = basic_example_main();
#else
#error At least one example has to be selected using y value in Makefile.
#endif

	return ret;
}
