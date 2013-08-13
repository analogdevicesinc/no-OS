/**************************************************************************//**
*   @file   AD5110.c
*   @brief  Implementation of AD5110 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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
******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "AD511x.h"
#include "Communication.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
unsigned char ad511xDevAddr = 0;

/***************************************************************************//**
 * @brief Initializes the communication with the device.
 *
 * @param devAddr - AD511x device address.
 * 					Example: AD511x_I2C_ADDR - Model AD511X BCPZ Y;
 * 							 AD511x_1_I2C_ADDR - Model AD511X BCPZ Y-1.
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
char AD511x_Init(unsigned char devAddr)
{
	char status = -1;

	ad511xDevAddr = devAddr;
	status = I2C_Init(100000);

	return status;
}

/***************************************************************************//**
 * @brief Write the content of serial register data to RDAC.
 *
 * @param rdacValue - Value of the serial register.
 *
 * @return none.
*******************************************************************************/
void AD511x_WriteRdac(unsigned char rdacValue)
{
	unsigned char dataBuffer[2];

	dataBuffer[0] = CMD_WR_RDAC;
	dataBuffer[1] = rdacValue;
	I2C_Write(ad511xDevAddr,
			  dataBuffer,
			  2,
			  1);
}

/***************************************************************************//**
 * @brief Read the content of RDAC register.
 *
 * @return rdacValue - Value read from register.
*******************************************************************************/
unsigned char AD511x_ReadRdac(void)
{
	unsigned char dataBuffer[2];

	dataBuffer[0] = CMD_RD_RDAC;
	dataBuffer[1] = 0;
	I2C_Write(ad511xDevAddr,
			  dataBuffer,
			  2,
			  1);

	I2C_Read(ad511xDevAddr,
			 dataBuffer,
			 1,
			 1);

	return dataBuffer[0];
}

/***************************************************************************//**
 * @brief Write the content of RDAC register to EEPROM.
 *
 * @return none.
*******************************************************************************/
void AD511x_WriteRdacEeprom(void)
{
	unsigned char dataBuffer[2];

	dataBuffer[0] = CMD_WR_RDAC_EEPROM;
	dataBuffer[1] = 0;
	I2C_Write(ad511xDevAddr,
			  dataBuffer,
			  2,
			  1);
}

/***************************************************************************//**
 * @brief Read wiper position from EEPROM.
 *
 * @return wiperValue - Value read from EEPROM.
*******************************************************************************/
unsigned char AD511x_ReadWiper(void)
{
	unsigned char dataBuffer[2];

	dataBuffer[0] = CMD_RD_EEPROM;
	dataBuffer[1] = WIPER_POSITION;
	I2C_Write(ad511xDevAddr,
			  dataBuffer,
			  2,
			  1);

	I2C_Read(ad511xDevAddr,
			 dataBuffer,
			 1,
			 1);

	return dataBuffer[0];
}

/***************************************************************************//**
 * @brief Read resistor tolerance from EEPROM.
 *
 * @return resTolerance - Value read from EEPROM.
*******************************************************************************/
unsigned char AD511x_ReadResTolerance(void)
{
	unsigned char dataBuffer[2];

	dataBuffer[0] = CMD_RD_EEPROM;
	dataBuffer[1] = RESISTOR_TOLERANCE;
	I2C_Write(ad511xDevAddr,
			  dataBuffer,
			  2,
			  1);

	I2C_Read(ad511xDevAddr,
			 dataBuffer,
			 1,
			 1);

	return dataBuffer[0];
}

/***************************************************************************//**
 * @brief Software reset; makes a refresh of RDAC register with EEPROM.
 *
 * @return none.
*******************************************************************************/
void AD511x_Reset(void)
{
	unsigned char dataBuffer[2];

	dataBuffer[0] = CMD_RESET;
	dataBuffer[1] = 0;
	I2C_Write(ad511xDevAddr,
			  dataBuffer,
			  2,
			  1);
}

/***************************************************************************//**
 * @brief Software shutdown.
 *
 * @param value - the value written to shut down register.
 * 				Example: ShutDownOff - the device is on.
 * 						 ShutDownOn  - the device is shut down.
 *
 * @return none.
*******************************************************************************/
void AD511x_ShutDown(unsigned char value)
{
	unsigned char dataBuffer[2];

	dataBuffer[0] = CMD_SHUT_DOWN;
	dataBuffer[1] = value;
	I2C_Write(ad511xDevAddr,
			  dataBuffer,
			  2,
			  1);
}
