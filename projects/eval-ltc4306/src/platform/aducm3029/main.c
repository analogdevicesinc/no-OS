/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for aducm3029 platform of eval-ltc4306-pmdz project.
 *   @author JSanbuen (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"

extern int example_main();

/***************************************************************************//**
 * @brief Main function execution for ADUCM3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	ltc4306_user_init.i2c_init = iip;
	max538x_user_init.i2c_init = iip;

	ret = platform_init();
	if (ret)
		return ret;

	return example_main();
}
