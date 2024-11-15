/***************************************************************************//**
*   @file   main.c
*   @brief  Main File for ADXL362 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "adxl362.h"
#include "system_config.h"

/*****************************************************************************/
/********************** Variable Definitions *********************************/
/*****************************************************************************/

/******************************************************************************
* @brief Main function.
*
* @return Always returns 0.
******************************************************************************/
int main(void)
{
    // Configure system
	SystemConfiguration();

    // Configure SPI
	SPI_Init(SPI_BASEADDR, 0, 0, 0);

    // Initialize Interrupts
	InterruptsInit();

	// Initialize ADXL362
	ADXL362_Init();

    // Run demo program
	DemoProgram();

	return(0);
}

