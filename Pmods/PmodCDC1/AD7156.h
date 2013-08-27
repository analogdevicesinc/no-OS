/**************************************************************************//**
*   @file   AD7156.h
*   @brief  AD7156 Driver Header file.
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

#ifndef AD7156_H_
#define AD7156_H_

#include "xparameters.h"

#define I2C_BASEADDR XPAR_AXI_IIC_1_BASEADDR
#define AD7156_I2C_ADDR				0x48
/******************************************************************************/
/* ADXL345                                                                    */
/******************************************************************************/
// AD7156 Registers
#define AD7156_STATUS				0x00
#define AD7156_CH1_DATA_HIGH		0x01
#define AD7156_CH1_DATA_LOW			0x02
#define AD7156_CH2_DATA_HIGH		0x03
#define AD7156_CH2_DATA_LOW			0x04
#define AD7156_CH1_AVERAGE_HIGH		0x05
#define AD7156_CH1_AVERAGE_LOW		0x06
#define AD7156_CH2_AVERAGE_HIGH		0x07
#define AD7156_CH2_AVERAGE_LOW		0x08
#define AD7156_CH1_THRESH_HIGH		0x09
#define AD7156_CH1_THRESH_LOW		0x0A
#define AD7156_CH1_SETUP			0x0B
#define AD7156_CH2_THRESH_HIGH		0x0C
#define AD7156_CH2_THRESH_LOW		0x0D
#define AD7156_CH2_SETUP			0x0E
#define AD7156_CONFIGURATION		0x0F
#define AD7156_POWER_DOWN_TIMER		0x10
#define AD7156_CH1_CAPDAC			0x11
#define AD7156_CH2_CAPDAC			0x12
#define AD7156_SERIAL_NR_3			0x13
#define AD7156_SERIAL_NR_2			0x14
#define AD7156_SERIAL_NR_1			0x15
#define AD7156_SERIAL_NR_0			0x16
#define AD7156_CHIP_ID				0x17

// Register specific bits
// Status Register 0x00
#define AD7156_PwrDown				7
#define AD7156_DacStep2				6
#define AD7156_OUT2					5
#define AD7156_DacStep1				4
#define AD7156_OUT1					3
#define AD7156_C1_C2				2
#define AD7156_nRDY2				1
#define AD7156_nRDY1				0
// Channel 1 Data High 0x01
// Channel 1 Data Low 0x02
// Channel 2 Data High 0x03
// Channel 2 Data Low 0x04
// Channel 1 Average High 0x05
// Channel 1 Average Low 0x06
// Channel 2 Average High 0x07
// Channel 2 Average Low 0x08
// Channel 1 Threshold High 0x09
// Channel 1 Threshold Low 0x0A
// Channel 1 Threshold High 0x0C
// Channel 1 Threshold Low 0x0D
// Channel 1 Setup 0x0B
// Channel 2 Setup 0x0E
#define AD7156_RngH					7
#define AD7156_RngL					6
#define AD7156_nHyst				4
#define AD7156_ThrSettling			0
// Configuration Register 0x0F
#define AD7156_ThrFixed				7
#define AD7156_ThrMD1				6
#define AD7156_ThrMD0				5
#define AD7156_EnCh1				4
#define AD7156_EnCh2				3
#define AD7156_MD2					2
#define AD7156_MD1					1
#define AD7156_MD0					0
// Power Down Timer Register 0x10
#define AD7156_PowerDownValue		0
// Channel 1 CAPDAC Register 0x11
// Channel 2 CAPDAC Register 0x12
#define AD7156_DacEn				7
#define AD7156_DacAuto				6
#define AD7156_DacValue				0

/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/
void AD7156_DisplayMainMenu(void);
void AD7156_DisplayMenu(void);
int AD7156_ReadReg(int reg);
int AD7156_BurstReadReg(int reg, char size);
void AD7156_WriteReg(int reg, unsigned char txData);
void AD7156_BurstWriteReg(int reg, char size, int txData);
void AD7156_DisplayCurrentSettings(void);
void AD7156_Init(void);
void AD7156_ChangeRange(char range, char chan);
void AD7156_SetRange(void);
void AD7156_PrintCap(char channel);
void AD7156_PrintAverageCap(char channel);
void AD7156_Print(char channel);
void AD7156_PrintAverage(char channel);
void AD7156_SelChannels(void);
void AD7156_SelThreshold(void);
void AD7156_SetThreshold(void);
void ADITimerIntrFunction(void);
void ADIExtIntrFunction(void);
void DemoProgram(void);

#endif /* AD7156_H_ */
