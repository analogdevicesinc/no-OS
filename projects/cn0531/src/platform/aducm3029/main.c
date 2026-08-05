/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for ADuCM3029 platform of cn0531 project.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for ADuCM3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	ret = platform_init();
	if (ret)
		return ret;

	return example_main();
}
