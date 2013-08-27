/***************************************************************************//**
*   @file   ADT7420.h
*   @brief  Header File for ADT7420 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
********************************************************************************
*   SVN Revision: $WCREV$
*******************************************************************************/

#ifndef ADT7420_H_
#define ADT7420_H_

#include "xparameters.h"

//ADT7420 IIC Address (JP1 - OPEN, JP2 - OPEN)
#define I2C_BASEADDR		XPAR_AXI_IIC_1_BASEADDR
#define ADT7420_IIC_ADDR 	0x4B

//REGISTERS
#define TEMP_REG 			0x00
#define STATUS_REG 			0x02
#define CONFIG_REG 			0x03
#define TH_SETP_MSB 		0x04
#define TH_SETP_LSB 		0x05
#define TL_SETP_MSB 		0x06
#define TL_SETP_LSB 		0x07
#define TCRIT_SETP_MSB 		0x08
#define TCRIT_SETP_LSB 		0x09
#define T_HYST_SETP 		0x0A
#define ID_REG 				0x0B
#define SOFT_RST_REG 		0x2F

//STATUS_REG BITS
#define UNDER_TLOW 			4
#define OVER_THIGH 			5
#define OVER_TCRIT 			6
#define CNV_READY_N 		7

//CONFIG_REG BITS
#define FAULT_QUEUE 		0
#define CT_POL 				2
#define INT_POL 			3
#define INT_CT 				4
#define OP_MODE 			5
#define RESOLUTION 			7

//ID_REG BITS
#define REVISION_ID 		0x07
#define MANUFACTURE_ID 		0xF8

//FUNCTION PROTOTYPES
void ADT7420_Init(void);
void ADT7420_DisplayMainMenu(void);
void ADT7420_DisplayMenu(void);
void ADT7420_PrintID(void);
void ADT7420_DisplayResolutionMenu(void);
void ADT7420_SetResolution(void);
unsigned char ADT7420_GetResolution(char display);
int ADT7420_ReadTemp(void);
void Display_Temp (short int data);
int ADT7420_ConsoleRead(void);
void ADT7420_DisplaySetTHighMenu(void);
void ADT7420_SetTHigh (int THigh);
void ADT7420_PrintTHigh(void);
void ADT7420_DisplaySetTLowMenu(void);
void ADT7420_SetTLow (int TLow);
void ADT7420_PrintTLow(void);
void ADT7420_DisplaySetTCritMenu(void);
void ADT7420_SetTCrit(int TCrit);
void ADT7420_PrintTCrit(void);
void ADT7420_DisplaySetTHystMenu(void);
void ADT7420_SetHysteresis(int degrees);
void ADT7420_PrintHysteresis(void);
void ADT7420_DisplaySetFaultQueueMenu(void);
void ADT7420_PrintFaultQueue(void);
void ADT7420_SetAlertPolarity(char CT, char INT);
void SetAlertModeComparator(void);
void ADT7420_PrintAlertMode(void);
void ADT7420_PrintCTPolarity(void);
void ADT7420_PrintINTPolarity(void);
void ADT7420_DisplaySettings(void);
void DemoProgram(void);

#endif /* ADT7420_H_ */
