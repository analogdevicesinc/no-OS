/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for XILINX platform of eval-adxl367z project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
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

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for XILINX platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
	if (ret < 0)
		goto error;
#endif

#ifdef DUMMY_EXAMPLE
	ret = dummy_example_main();
	if (ret < 0)
		goto error;
#endif

	/* Disable the instruction cache. */
	Xil_DCacheDisable();
	/* Disable the data cache. */
	Xil_ICacheDisable();

#if (IIO_EXAMPLE+DUMMY_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and rebuild the project.
#endif

error:
	return 0;
}
