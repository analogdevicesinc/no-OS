/**************************************************************************//**
*   @file   main.c
*   @brief  main file
*   @author ATofan (alexandru.tofan@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ad7193.h"
#include "system_config.h"

int main(void)
{
    // Configure system
	SystemConfiguration();

    // Configure SPI
	SPI_Init(SPI_BASEADDR, 0, 1, 1);

    // Run demo program
	DemoProgram();

	return(0);
}
