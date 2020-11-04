/**************************************************************************//**
*   @file   main.c
*   @brief  main file
*   @author ATofan (alexandru.tofan@analog.com)
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
*   SVN Revision: $WCREV$
******************************************************************************/

#include <stdio.h>
#include "xparameters.h"
#include "xil_io.h"
#include "admp441.h"

#define DMA_BASEADDR	 XPAR_AXIDMA_0_BASEADDR
#define CF_BASEADDR      XPAR_AXI_ADMP441_0_BASEADDR

unsigned int nrSamples = 488000;
unsigned int nrSeconds = 10;
volatile int admp441Data[1000044] = {0,};
volatile int* admp441DataPointer = &admp441Data[0];
volatile int admp441DataLitte;
volatile int ampFactor = 1;
volatile unsigned int i = 0;

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

/******************************************************************************
* @brief Main function.
*
* @return Always returns 0.
******************************************************************************/
int main()
{
	unsigned long dataMask = 0x7FFFFF80;
	int dmaRepeat = 0;
	unsigned long rxData = 0;

	Xil_ICacheEnable();
	Xil_DCacheEnable();

	xil_printf("PmodMIC2 Demo Program\n\r");

	// Prepare WAV Header
	xil_printf("Preparing WAV File Header...");
	admp441Data[0]  = CHUNK_RIFF_ID;
	admp441Data[1]  = CHUNK_SIZE;
	admp441Data[2]  = WAVE_FORMAT;
	admp441Data[3]  = SUBCHUNK1_ID;
	admp441Data[4]  = SUBCHUNK1_SIZE;
	admp441Data[5]  = (AUDIO_FORMAT << 16) | (NR_CHANNELS);
	admp441Data[6]  = SAMPLE_RATE;
	admp441Data[7]  = BYTE_RATE;
	admp441Data[8]  = (BLOCK_ALIGN << 16) | (NR_BITS);
	admp441Data[9]  = SUBCHUNK2_ID;
	admp441Data[10] = SUBCHUNK2_SIZE;
	xil_printf(" Done!\n\r");

	// Set number of samples
	nrSamples = nrSeconds * 48800;

	// Start from admp441Data[11]
	xil_printf("Receiving Data... ");

    dmaRepeat = nrSamples / 8096;
    i = 0;

    while(i < dmaRepeat)
    {
    	ReadAdcData( point + i*8096 , (8096) );
    	i += 1;
    }

	xil_printf("Done!\n\r");
	xil_printf("Samples Acquired: %d\n\r", nrSamples);

	// Convert data and save in RAM
    for (i = 11; i < 8096 * dmaRepeat + 11; i++)
    {
    	rxData = (point[i - 11] & dataMask) << 1;
		admp441DataLitte = bigToLittle(rxData * ampFactor);
		admp441Data[i] = admp441DataLitte;
	}

	while(1)
	{

	}

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	return 0;
}
