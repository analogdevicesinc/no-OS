/**************************************************************************//**
*   @file   main.c
*   @brief  Implementation of main program for AD7091R.
*   @author ATofan (alexandru.tofan@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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

#define DMA_BASEADDR XPAR_AXIDMA_0_BASEADDR
#define CF_BASEADDR  XPAR_AXI_AD7091R_0_BASEADDR

/*****************************************************************************/
/************************ Variables Definitions ******************************/
/*****************************************************************************/
volatile u32 startAcquisition;              			//starts one acquisition cycle
volatile u32* pStartAcquisition = &startAcquisition; 	//pointer to the startAcquisition variable
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

void printVin(u32 data);

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

    ReadAdcData( point , (8096) );

    for (i = 0 ; i < 8096;i++)
    {
    	printVin(point[i]);
    }

    return 0;
}

void printVin(u32 data)
{
	int rxData = 0;
	float value = 0;
	int whole = 0;
	int thousands = 0;

   	rxData = data & 0xFFF;

   	// converting data for display
   	value = ((float)rxData/4096)*2.5;
   	whole = value;
   	thousands = ((value - whole)*1000);

	if(thousands > 99)
	{
		xil_printf("Vin = %d.%3d %c\n\r",whole, thousands,'V');
	}
	else if(thousands > 9)
	{
		xil_printf("Vin = %d.0%2d %c\n\r",whole, thousands,'V');
	}
	else
	{
		xil_printf("Vin = %d.00%1d %c\n\r",whole, thousands,'V');
	}
}
