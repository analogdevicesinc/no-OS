/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for xilinx platform of adf4377 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "no_os_error.h"
extern int example_main();
/**
 * @brief Main function execution for XILINX platform.
 * @return ret - Result of the enabled examples execution.
 */
int main()
{
	int ret = -EINVAL;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

	/* Execute the example main function */
	ret = example_main();
	if (ret)
		return ret;

	/* Disable the instruction cache. */
	Xil_ICacheDisable();
	/* Disable the data cache. */
	Xil_DCacheDisable();

	return ret;
}
