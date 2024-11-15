/***************************************************************************//**
*   @file   AD7991.h
*   @brief  Header File for AD7991 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "xil_types.h"
#include "xparameters.h"

/*****************************************************************************/
/*********************** AD7911 Definitions **********************************/
/*****************************************************************************/
// ADP5589 IIC Address
#define AD7991IICAddr		0x28
#define IIC_BASEADDR  		XPAR_AXI_IIC_1_BASEADDR
#define UART_BASEADDR		XPAR_PS7_UART_1_BASEADDR

/*****************************************************************************/
/******************* AD7911 Registers Bits ***********************************/
/*****************************************************************************/
// Configuration Register
#define CH3                 7
#define CH2                 6
#define CH1                 5
#define CH0                 4
#define REF_SEL             3
#define FLTR                2
#define bitTrialDelay       1
#define sampleDelay         0
// Conversion Result Register
#define LeadingZeros        0xC000
#define CHID                0x3000
#define ADCValue            0xFFF

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/
void AD7991_Init(void);
void AD7991_Config(char chan3, char chan2, char chan1, char chan0, char ref, char filter, char bit, char sample);
void AD7991_Print(u32 nr_cnv, u32 vref);



