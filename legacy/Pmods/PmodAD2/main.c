/***************************************************************************//**
*   @file   main.c
*   @brief  Main File for AD7991 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
********************************************************************************
*   SVN Revision: $WCREV$
*******************************************************************************/

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

