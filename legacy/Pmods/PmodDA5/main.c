/***************************************************************************//**
*   @file   main.c
*   @brief  Main File for AD5781 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "ad5781.h"
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
	SPI_Init(SPI_BASEADDR, 0, 0, 1);

    // Initialize Interrupts
	InterruptsInit();

	// Initialize DAC
	AD5781_InitDAC();

    // Run demo program
	DemoProgram();

	return(0);
}

