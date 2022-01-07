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
* @file si5324.c
*
* This file programs si5324 chip which generates clock for the peripherals.
*
* Please refer to Si5324 Datasheet for more information
* http://www.silabs.com/Support%20Documents/TechnicalDocs/Si5324.pdf
*
* Tested on Zynq ZC706 platform
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
#define IIC_SLAVE_ADDR		0x68
#define IIC_MUX_ADDRESS		0x74
#define IIC_CHANNEL_ADDRESS	0x10

#define XIIC	XIicPs
#define INTC	XScuGic
/**************************** Type Definitions *******************************/
typedef struct SI324Info
{
	u32 RegIndex;	/* Register Number */
	u32 Value;		/* Value to be Written */
} SI324Info;

typedef struct {
	XIIC I2cInstance;
	INTC IntcInstance;
	volatile u8 TransmitComplete;   /* Flag to check completion of Transmission */
	volatile u8 ReceiveComplete;    /* Flag to check completion of Reception */
	volatile u32 TotalErrorCount;
} XIIC_LIB;
/************************** Function Prototypes *****************************/
int I2cWriteData(XIIC_LIB *I2cLibPtr, u8 *WrBuffer, u16 ByteCount, u16 SlaveAddr);
int I2cReadData(XIIC_LIB *I2cLibPtr, u8 *RdBuffer, u16 ByteCount, u16 SlaveAddr);
int I2cPhyWrite(XIIC_LIB *I2cLibPtr, u8 PhyAddr, u8 Reg, u16 Data, u16 SlaveAddr);
int I2cPhyRead(XIIC_LIB *I2cLibPtr, u8 PhyAddr, u8 Reg, u16 *Data, u16 SlaveAddr);
int I2cSetupHardware(XIIC_LIB *I2cLibPtr);
/************************* Global Definitions *****************************/
/*
 * These configuration values generates 125MHz clock
 * For more information please refer to Si5324 Datasheet.
 */
SI324Info InitTable[] = {
		{  0, 0x54},	/* Register 0 */
		{  1, 0xE4},	/* Register 1 */
		{  2, 0x12},	/* Register 2 */
		{  3, 0x15},	/* Register 3 */
		{  4, 0x92},	/* Register 4 */
		{  5, 0xed},	/* Register 5 */
		{  6, 0x2d},	/* Register 6 */
		{  7, 0x2a},	/* Register 7 */
		{  8, 0x00},	/* Register 8 */
		{  9, 0xc0},	/* Register 9 */
		{ 10, 0x08},	/* Register 10 */
		{ 11, 0x40},	/* Register 11 */
		{ 19, 0x29},	/* Register 19 */
		{ 20, 0x3e},	/* Register 20 */
		{ 21, 0xff},	/* Register 21 */
		{ 22, 0xdf},	/* Register 22 */
		{ 23, 0x1f},	/* Register 23 */
		{ 24, 0x3f},	/* Register 24 */
		{ 25, 0x60},	/* Register 25 */
		{ 31, 0x00},	/* Register 31 */
		{ 32, 0x00},	/* Register 32 */
		{ 33, 0x05},	/* Register 33 */
		{ 34, 0x00},	/* Register 34 */
		{ 35, 0x00},	/* Register 35 */
		{ 36, 0x05},	/* Register 36 */
		{ 40, 0xc2},	/* Register 40 */
		{ 41, 0x22},	/* Register 41 */
		{ 42, 0xdf},	/* Register 42 */
		{ 43, 0x00},	/* Register 43 */
		{ 44, 0x77},	/* Register 44 */
		{ 45, 0x0b},	/* Register 45 */
		{ 46, 0x00},	/* Register 46 */
		{ 47, 0x77},	/* Register 47 */
		{ 48, 0x0b},	/* Register 48 */
		{ 55, 0x00},	/* Register 55 */
		{131, 0x1f},	/* Register 131 */
		{132, 0x02},	/* Register 132 */
		{137, 0x01},	/* Register 137 */
		{138, 0x0f},	/* Register 138 */
		{139, 0xff},	/* Register 139 */
		{142, 0x00},	/* Register 142 */
		{143, 0x00},	/* Register 143 */
		{136, 0x40}		/* Register 136 */
};

/************************** Function Definitions *****************************/
int MuxInit(XIIC_LIB *I2cLibInstancePtr)
{
	u8 WrBuffer[0];
	int Status;

	WrBuffer[0] = IIC_CHANNEL_ADDRESS;

	Status = I2cWriteData(I2cLibInstancePtr,
				WrBuffer, 1, IIC_MUX_ADDRESS);
	if (Status != XST_SUCCESS) {
		xil_printf("Si5324: Writing failed\n\r");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int ProgramSi5324(void)
{
	XIIC_LIB I2cLibInstance;
	int Index;
	int Status;
	u8 WrBuffer[2];

	Status = I2cSetupHardware(&I2cLibInstance);
	if (Status != XST_SUCCESS) {
		xil_printf("Si5324: Configuring HW failed\n\r");
		return XST_FAILURE;
	}

	Status = MuxInit(&I2cLibInstance);
	if (Status != XST_SUCCESS) {
		xil_printf("Si5324: Mux Init failed\n\r");
		return XST_FAILURE;
	}

	for (Index = 0; Index < sizeof(InitTable)/8; Index++) {
		WrBuffer[0] = InitTable[Index].RegIndex;
		WrBuffer[1] = InitTable[Index].Value;

		Status = I2cWriteData(&I2cLibInstance, WrBuffer, 2, IIC_SLAVE_ADDR);
		if (Status != XST_SUCCESS) {
			xil_printf("Si5324: Writing failed\n\r");
			return XST_FAILURE;
		}
	}
	return XST_SUCCESS;
}
#endif
#endif
