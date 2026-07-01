/***************************************************************************//**
 * @file main.c
 * @brief Main file for Xilinx platform of capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <xil_cache.h>
extern int example_main(void);

/**
 * @brief Main function execution for Xilinx platform.
 * @return Result of the enabled example execution.
 */
int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();
	return example_main();
}
