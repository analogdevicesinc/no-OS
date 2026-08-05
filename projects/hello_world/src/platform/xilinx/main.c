/***************************************************************************//**
 *   @file   main.c
 *   @brief  Entry point for the Xilinx platform build of hello_world.
 *
 *           Xilinx Zynq and ZynqMP processors start with both the instruction
 *           cache and data cache disabled.  Enabling them here gives a
 *           significant performance boost before handing off to the platform-
 *           agnostic application code.
 *
 *   @author Analog Devices, Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <xil_cache.h>
#include "basic_example.h"

/***************************************************************************//**
 * @brief Main function execution for Xilinx platform.
 *
 * @return ret - Result of basic_example_main() execution.
*******************************************************************************/
int main()
{
	/* Enable caches for better performance. */
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	return basic_example_main();
}
