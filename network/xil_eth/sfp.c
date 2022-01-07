/*
 * Copyright (C) 2013 - 2019 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */
/*****************************************************************************/
/**
* @file sfp.c
*
* This file programs sfp phy chip.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date	 Changes
* ----- ---- -------- ---------------------------------------------------------
* 1.0   srt  10/19/13 Initial Version
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xparameters.h"
#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || \
	XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
#include "xil_printf.h"
#include "xiicps.h"
#include "sleep.h"
#include "xscugic.h"

/************************** Constant Definitions *****************************/
#define IIC_SLAVE_ADDR		0x56
#define IIC_MUX_ADDRESS		0x74
#define IIC_CHANNEL_ADDRESS	0x01

#define XIIC	XIicPs
#define INTC	XScuGic
/**************************** Type Definitions *******************************/
typedef struct {
	XIIC I2cInstance;
	INTC IntcInstance;
	volatile u8 TransmitComplete;   /* Flag to check completion of Transmission */
	volatile u8 ReceiveComplete;    /* Flag to check completion of Reception */
	volatile u32 TotalErrorCount;
} XIIC_LIB;
/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
int I2cWriteData(XIIC_LIB *I2cLibPtr, u8 *WrBuffer, u16 ByteCount, u16 SlaveAddr);
int I2cReadData(XIIC_LIB *I2cLibPtr, u8 *RdBuffer, u16 ByteCount, u16 SlaveAddr);
int I2cPhyWrite(XIIC_LIB *I2cLibPtr, u8 PhyAddr, u8 Reg, u16 Data, u16 SlaveAddr);
int I2cPhyRead(XIIC_LIB *I2cLibPtr, u8 PhyAddr, u8 Reg, u16 *Data, u16 SlaveAddr);
int I2cSetupHardware(XIIC_LIB *I2cLibPtr);
/************************** Function Definitions *****************************/

/*****************************************************************************/
/**
 * This function initializes ZC706 MUX.
 *
 * @param	I2cLibPtr contains a pointer to the instance of the IIC library
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 ******************************************************************************/
int ZC706MuxInit(XIIC_LIB *I2cLibInstancePtr)
{
	u8 WrBuffer;
	int Status;

	WrBuffer = IIC_CHANNEL_ADDRESS;

	Status = I2cWriteData(I2cLibInstancePtr, &WrBuffer, 1, IIC_MUX_ADDRESS);
	if (Status != XST_SUCCESS) {
		xil_printf("SFP_PHY: Writing failed\n\r");
		return XST_FAILURE;
 	}
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function program SFP PHY.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 ******************************************************************************/
int ProgramSfpPhy(void)
{
	XIIC_LIB I2cLibInstance;
	int Status;
	u8 WrBuffer[2];
	u16 phy_read_val;

	Status = I2cSetupHardware(&I2cLibInstance);
	if (Status != XST_SUCCESS) {
		xil_printf("Fail!!!\n\r");
		xil_printf("SFP_PHY: Configuring HW failed\n\r");
		return XST_FAILURE;
	}
	Status = ZC706MuxInit(&I2cLibInstance);
	if (Status != XST_SUCCESS) {
		xil_printf("SFP_PHY: Mux Init failed\n\r");
		return XST_FAILURE;
	}

	WrBuffer[0] = 0;
	Status = I2cWriteData(&I2cLibInstance, WrBuffer, 1, IIC_SLAVE_ADDR);
	if (Status != XST_SUCCESS) {
		xil_printf("SFP_PHY: Writing failed\n\r");
		return XST_FAILURE;
	}

#if XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
	/* Enabling 1000BASEX */
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x1B, 0x9088, IIC_SLAVE_ADDR);
#else
	/* Enabling SGMII */
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x1B, 0x9084, IIC_SLAVE_ADDR);
#endif

	/* Apply Soft Reset */
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x00, 0x9140, IIC_SLAVE_ADDR);

	/* Enable 1000BaseT Full Duplex capabilities */
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x09, 0x0E00, IIC_SLAVE_ADDR);

	/* Apply Soft Reset */
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x00, 0x9140, IIC_SLAVE_ADDR);

	/* Advertise 10/100 Capabilities else change the capabilities */
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x04, 0x0141, IIC_SLAVE_ADDR);

	/* Apply Soft Reset */
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x00, 0x9140, IIC_SLAVE_ADDR);

	/* Apply Soft Reset */
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x00, 0x9140, IIC_SLAVE_ADDR);

	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x10, 0xF079, IIC_SLAVE_ADDR);
	/* Apply Soft Reset */
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x00, 0x9140, IIC_SLAVE_ADDR);

	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x16, 0x0001, IIC_SLAVE_ADDR);
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x00, 0x9140, IIC_SLAVE_ADDR);
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x00, 0x9340, IIC_SLAVE_ADDR);
	usleep(1);

	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x16, 0x0, IIC_SLAVE_ADDR);
	phy_read_val = 0x0;

	while((phy_read_val & 0x0C00) != 0x0C00) {
		I2cPhyRead(&I2cLibInstance, IIC_SLAVE_ADDR, 0x11, &phy_read_val, IIC_SLAVE_ADDR);
	}
	usleep(1);

	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x16, 0x0, IIC_SLAVE_ADDR);
	I2cPhyRead(&I2cLibInstance, IIC_SLAVE_ADDR, 0x11, &phy_read_val, IIC_SLAVE_ADDR);
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x16, 0x0001, IIC_SLAVE_ADDR);

	/* configure speed */
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x14, 0x0c61, IIC_SLAVE_ADDR);
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x00, 0x9340, IIC_SLAVE_ADDR);
	I2cPhyWrite(&I2cLibInstance, IIC_SLAVE_ADDR, 0x16, 0x0, IIC_SLAVE_ADDR);

	return XST_SUCCESS;
}
#endif
#endif
