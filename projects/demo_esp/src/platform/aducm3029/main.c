/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for aducm3029 platform of demo_esp project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include "basic_example.h"

/***************************************************************************//**
 * @brief Main function execution for ADUCM3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	ret = platform_init();
	if (ret)
		goto error;

	ret = basic_example_main();
	if (ret)
		goto error;

#if (BASIC_EXAMPLE != 1)
#error Please enable the example and rebuild the project.
#endif

error:
	return ret;
}
