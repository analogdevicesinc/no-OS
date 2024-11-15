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

#include "ad5933.h"
#include "system_config.h"

int main(void)
{
    // Configure system
	SystemConfiguration();

    // Configure I2C
	I2C_Init(I2C_BASEADDR, AD5933_I2C_ADDR);

    // Initialize Interrupts
	InterruptsInit();

    // Run demo program
	DemoProgram();

	return(0);
}
