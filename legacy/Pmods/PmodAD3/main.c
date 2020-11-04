/**************************************************************************//**
*   @file   main.c
*   @brief  Implementation of main program for AD7780.
*   @author ATofan (adrian.costina@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*   SVN Revision: $WCWREV$
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "xil_io.h"
#include "xparameters.h"

/*****************************************************************************/
/************************ Constants Definitions ******************************/
/*****************************************************************************/
#define SAMPLE_NO 8*1024

#define DMA_BASEADDR 	 XPAR_AXIDMA_0_BASEADDR
#define CF_BASEADDR      XPAR_AXI_AD7780_0_BASEADDR

/*****************************************************************************/
/************************ Variables Definitions ******************************/
/*****************************************************************************/
volatile u32 startAcquisition;                      //starts one acquisition cycle
volatile u32* pStartAcquisition = &startAcquisition;
volatile u32* point =  XPAR_PS7_DDR_0_S_AXI_BASEADDR + 1024*1024 ; // pointer to data memory location

void Xil_DCacheFlush(void);

void delay_ms(u32 ms_count) {
  u32 count;
  for (count = 0; count < ((ms_count * 10000000) + 1); count++) {
    asm("nop");
  }
}
/******************************************************************************
* @brief Reads data from an ADC usign AXI_DMA core
*
* @param buf - Start address of the buffer where to store the read data
* @param size - Size in bytes of the data to read
* @return None
******************************************************************************/
void ReadAdcData(u32 buf, u32 size)
{
// program DMA engine
      Xil_Out32((DMA_BASEADDR + 0x030), 0); // Clear dma operations
      Xil_Out32((DMA_BASEADDR + 0x030), 1); // Enable Run bit
      Xil_Out32((DMA_BASEADDR + 0x048), buf); // Configure DMA with the destination address
      Xil_Out32((DMA_BASEADDR + 0x058), (size * 4 )); // Number of bites to be written

      // program the DMA module in the ADC core
      Xil_Out32((CF_BASEADDR + 0x040), 0x1); // Power up the core
      Xil_Out32((CF_BASEADDR + 0x088), 0x7); // Reset overflows
      Xil_Out32((CF_BASEADDR + 0x080), 0x0); // DMA stop

      Xil_Out32((CF_BASEADDR + 0x084), size * 4 );  // Program the number of bytes to be captured
      Xil_Out32((CF_BASEADDR + 0x080), 0x1); // Start capturing data

      // Wait for data transfer to finish
      do {
             delay_ms(1);
      }
      while ((Xil_In32(CF_BASEADDR + 0x088) & 0x1) == 1);
        if (Xil_In32(CF_BASEADDR + 0x088) != 0x00) {
          xil_printf("overflow occured, capture may be corrupted\n\r");
          xil_printf ("%x\n", Xil_In32(CF_BASEADDR + 0x088) );
        }
      Xil_DCacheFlush();

}

void AD7780_PrintVin(u32 value);

/**************************************************************************//**
* @brief Main function implementation.
*
* @return None
******************************************************************************/
int main()
{
    int i;

    xil_printf("\n-------------------------------------\n\r");
    xil_printf("Waiting for command from the PC...\n\r");
    
    xil_printf("Starting a new data acquisition...\n\r");

    xil_printf("Reading data from the ADC...\n\r");
    ReadAdcData( point , (256) );

    for (i = 0 ; i < 256; i++)
    {
    	AD7780_PrintVin(point[i]);
    }

    return 0;
}

/******************************************************************************
* @brief Display Vin.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7780_PrintVin(u32 data)
{
	unsigned char calcGain = 0x00;
	unsigned int inputV = 0x00;
	float value = 0x00;
	float vref = 3300;
	int whole = 0x00;
	int thousands = 0x00;
	int fullScale = 8388608;

	calcGain = 1;

	// Process Voltage data from 32 bit stream
	inputV = (data & 0xFFFFFF00) >> 8;

	// Calculate float value of Vin based on datasheet formula
	value = (((float)(vref/1000) * ((float)inputV - fullScale)) / (calcGain * fullScale));

	// If voltage is positive
	if(value >= 0)
	{
		xil_printf("Vin = +");
	}
	// If voltage is negative
	else
	{
		xil_printf("Vin = -");
		value = value * (-1);
	}
	// Process whole and thousands
	whole = (int) value;
	thousands = (value - whole) * 1000;
	// Properly display whole and thousands
	if(thousands > 99)
	{
		xil_printf("%d.%d V\n\r", whole, thousands);
	}
	else if(thousands > 9)
	{
		xil_printf("%d.0%d V\n\r", whole, thousands);
	}
	else
	{
		xil_printf("%d.00%d V\n\r", whole, thousands);
	}
}
