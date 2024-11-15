/***************************************************************************//**
*   @file   main.c
*   @brief  Main File for AD5628 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "ad5628.h"
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

	// Enable Reference
	AD5628_RefON();

    // Run demo program
	DemoProgram();

	return(0);
}

