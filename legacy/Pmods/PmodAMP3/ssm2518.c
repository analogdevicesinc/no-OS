/***************************************************************************//**
*   @file   ssm2518.c
*   @brief  Driver File for SSM2518 Driver.
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

#include "xil_io.h"
#include "audio_data.h"
#include "ssm2518.h"
#include "i2c_axi.h"

volatile int rxData = 0x00;

/*****************************************************************************
* @brief Write to an internal SSM2518 Register.
*
* @param regAddr - register address.
* @param data - data to be written.
*
* @return None.
******************************************************************************/
void SSM2518_WriteReg(char regAddr, unsigned char data)
{
	unsigned char txBuffer[1] = {0x00};

	txBuffer[0] = data;
	I2C_Write_axi(I2C_BASEADDR, SSM2518_I2C_ADDR, regAddr, 1, txBuffer);
}

/*****************************************************************************
* @brief Read from an internal SSM2518 Register.
*
* @param regAddr - register address.
*
* @return rxBuffer[0] - data read from device.
******************************************************************************/
unsigned char SSM2518_ReadReg(char regAddr)
{
	unsigned char rxBuffer[1] = {0x00};

	I2C_Read_axi(I2C_BASEADDR, SSM2518_I2C_ADDR, regAddr, 1, rxBuffer);

	return(rxBuffer[0]);
}

/*****************************************************************************
* @brief Initialize SSM2518
*
* @param None.
*
* @return None.
******************************************************************************/
void SSM2518_Init(void)
{
	xil_printf("\n\rInitializing SSM2518 via I2C...");
	SSM2518_WriteReg(SSM2518_Reset_Power_Control, 					0x04);
	SSM2518_WriteReg(SSM2518_Edge_Clock_Control, 					0x00);
	SSM2518_WriteReg(SSM2518_Serial_Interface_Sample_Rate_Control, 	0x02);
	SSM2518_WriteReg(SSM2518_Serial_Interface_Control, 				0x00);
	SSM2518_WriteReg(SSM2518_Channel_Mapping_Control, 				0x01);
	SSM2518_WriteReg(SSM2518_Left_Volume_Control, 					0x40);
	SSM2518_WriteReg(SSM2518_Right_Volume_Control, 					0x40);
	SSM2518_WriteReg(SSM2518_Volume_Mute_Control, 					0x80);
	SSM2518_WriteReg(SSM2518_Fault_Control_1, 						0x0C);
	SSM2518_WriteReg(SSM2518_Power_Fault_Control, 					0x80);
	xil_printf(" Done!\n\r");
}
