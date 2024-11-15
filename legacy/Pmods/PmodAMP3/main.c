/***************************************************************************//**
*   @file   main.c
*   @brief  Main File for SSM2518 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "xparameters.h"
#include "xil_cache.h"
#include "xil_io.h"
#include "audio_data.h"
#include "ssm2518.h"
#include "i2c_axi.h"

#define DDR_BASEADDR  		XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define VDMA_BASEADDR 		XPAR_AXI_VDMA_1_BASEADDR
#define PMOD_DATA_BASEADDR	DDR_BASEADDR + 0xA000000
#define CF_BASEADDR			XPAR_AXI_SSM2518_0_BASEADDR

int main()
{
	int i = 0;
	int audioSize = 0;
	int sendValDac = 0;

	Xil_DCacheFlush();
	Xil_DCacheDisable();

	xil_printf("PmodAMP3 Demonstration Project\n\r");

	audioSize = (sizeof(audio_data) / sizeof(u32));

	xil_printf("Preparing Audio Data...");

	for(i = 0; i < audioSize; i++)
	{
	  sendValDac = audio_data[i];
	  Xil_Out32((PMOD_DATA_BASEADDR + (i * 0x04)), sendValDac);
	}

	xil_printf("Done!\n\r");

	xil_printf("Programming VDMA Core...");

	Xil_Out32((VDMA_BASEADDR + 0x000), 0x00000000);
	Xil_Out32((VDMA_BASEADDR + 0x000), 0x00000003); // enable circular mode
	while((Xil_In32(VDMA_BASEADDR + 0x04) & 0x01) != 0x00);
	Xil_Out32((VDMA_BASEADDR + 0x018), 0x01); // frm store
	Xil_Out32((VDMA_BASEADDR + 0x05c), PMOD_DATA_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x058), 63928); // h offset bytes
	Xil_Out32((VDMA_BASEADDR + 0x054), 63928); // h size bytes
	Xil_Out32((VDMA_BASEADDR + 0x050), 12); // v size

	xil_printf("Done!\n\r");

	xil_printf("Enabling SSM2518 Core...");

	Xil_Out32(CF_BASEADDR + 0x4040, 0x01);
	Xil_Out32(CF_BASEADDR + 0x4044, 0x01);

	xil_printf("Done!\n\r");


	while(1)
	{
	}

    return 0;
}
