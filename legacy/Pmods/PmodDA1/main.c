/***************************************************************************//**
*   @file   main.c
*   @brief  Main file for AD7303 driver
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
#include "ad7303.h"

#define DDR_BASEADDR  		XPAR_PS7_DDR_0_S_AXI_BASEADDR
#define VDMA_BASEADDR 		XPAR_AXI_VDMA_1_BASEADDR
#define PMOD_DATA_BASEADDR	DDR_BASEADDR + 0xA000000
#define CF_BASEADDR			XPAR_AXI_AD7303_0_BASEADDR

int main()
{
	int i = 0;
	int sendValDac0 = 0;
	int sendValDac1 = 0;

	Xil_DCacheFlush();
	Xil_DCacheDisable();

	xil_printf("PmodDA1 Demonstration Project\n\r");

	xil_printf("Preparing DAC Data...");

	for(i = 0; i < 256; i++)
	{
	  sendValDac0 = (0x0300 | (sinetable[i%256] & 0xFF)) << 16;
	  sendValDac1 = (0x0300 | (triangletable[i%256] & 0xFF));

	  Xil_Out32(PMOD_DATA_BASEADDR + i * 0x04, sendValDac0 | sendValDac1);
	}

	xil_printf("Done!\n\r");

	xil_printf("Programming VDMA Core...");

	Xil_Out32((VDMA_BASEADDR + 0x000), 0x00000004); // reset
	Xil_Out32((VDMA_BASEADDR + 0x000), 0x00000003); // enable circular mode
	while((Xil_In32(VDMA_BASEADDR + 0x04) & 0x01) == 0x01);
	Xil_Out32((VDMA_BASEADDR + 0x018), 0x03); // frm store
	Xil_Out32((VDMA_BASEADDR + 0x05c), PMOD_DATA_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x060), PMOD_DATA_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x064), PMOD_DATA_BASEADDR); // start address
	Xil_Out32((VDMA_BASEADDR + 0x058), 256 * 4); // h offset (2048 * 4) bytes
	Xil_Out32((VDMA_BASEADDR + 0x054), 256 * 4); // h size (1920 * 4) bytes
	Xil_Out32((VDMA_BASEADDR + 0x050), 1); // v size (1080)

	xil_printf("Done!\n\r");

	xil_printf("Enabling AD7303 Core...");

	Xil_Out32(CF_BASEADDR + 0x4040, 0x01);
	Xil_Out32(CF_BASEADDR + 0x4044, 0x01);

	xil_printf("Done!\n\r");


	while(1)
	{
	}

    return 0;
}
