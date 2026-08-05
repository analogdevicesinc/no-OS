/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for aducm3029 platform of ad7124-8pmdz project.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for aducm3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	ret = platform_init();
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	return example_main();
}
