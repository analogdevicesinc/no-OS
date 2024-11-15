/***************************************************************************//**
*   @file   main.c
*   @brief  Main File for AD7991 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdio.h>
#include "xparameters.h"
#include "xil_cache.h"
#include "i2c_axi.h"
#include "AD7991.h"
#include "xil_io.h"

/*****************************************************************************/
/************************** Macros Definitions *******************************/
/*****************************************************************************/
// Reference Voltage value
// Actual value * 10000
// E.g. VREF = 2.048 * 10000
#define VREF 20480

volatile int rxData;

/******************************************************************************
* @brief Main function.
*
* @return Always returns 0.
******************************************************************************/
int main() 
{

   Xil_ICacheEnable();
   Xil_DCacheEnable();

   I2C_Init_axi(IIC_BASEADDR, AD7991IICAddr);

   unsigned char rxBuffer[2] = {0x00, 0x00};

   // Configure AD7991
   /*
    	Use external reference (2V, selected from Jumper JP1)
    	Read all 3 Channels
    	4th Channel used as reference
    */
   AD7991_Config(0,1,1,1,1,0,0,0);

	   // Print the value read on one of the channels
	   AD7991_Print(20,VREF);

   Xil_DCacheDisable();
   Xil_ICacheDisable();

   return 0;
}

