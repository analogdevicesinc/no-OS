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

#include "adt7420.h"
#include "system_config.h"

int main(void)
{
    // Configure system
	SystemConfiguration();

    // Configure I2C
	I2C_Init(I2C_BASEADDR, ADT7420_IIC_ADDR);

	// Initialize ADT7420 Device
	ADT7420_Init();

    // Initialize Interrupts
	InterruptsInit();

    // Run demo program
	DemoProgram();

	return(0);
}
