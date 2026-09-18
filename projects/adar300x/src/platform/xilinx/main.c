/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Xilinx platform of ADAR300X project.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <xil_cache.h>
#include "basic_example.h"

int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	return basic_example_main();
}
