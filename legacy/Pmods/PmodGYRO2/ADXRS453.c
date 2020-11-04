/***************************************************************************//**
 *   @file   ADXRS453.c
 *   @brief  Implementation of ADXRS453 Driver.
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

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include "system_config.h"
#include "ADXRS453.h"

volatile int rxData = 0;

extern u32 (*SPI_Init)(u32, char, char, char);
extern u32 (*SPI_TransferData)(u32, char, char*, char, char*, char);

/******************************************************************************
* @brief Send and Read Data over SPI to/from the ADXRS453
*
* @param txData - Data to be sent.
*
* @return retValue - Data read from the ADXRS453.
******************************************************************************/
int ADXRS453_SendData(int txData)
{
	char txBuffer[4] = {0x00, 0x00, 0x00, 0x00};
	char rxBuffer[4] = {0x00, 0x00, 0x00, 0x00};
	int cfgValue = txData;
	int retValue = 0;

	txBuffer[0] = ((cfgValue & 0xFF000000) >> 24);
	txBuffer[1] = ((cfgValue & 0x00FF0000) >> 16);
	txBuffer[2] = ((cfgValue & 0x0000FF00) >> 8);
	txBuffer[3] = ((cfgValue & 0x000000FF) >> 0);

	SPI_TransferData(SPI_BASEADDR, 4, txBuffer, 4, rxBuffer, 1);

	retValue = ((rxBuffer[0] << 24) | (rxBuffer[1] << 16) | (rxBuffer[2] << 8) | (rxBuffer[3] << 0));

	return(retValue);
}

/******************************************************************************
* @brief Initialize the ADXRS453.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXRS453_Init(void)
{
	ADXRS453_SendData(0x20000003);
	delay_ms(50);
	ADXRS453_SendData(0x20000000);
	delay_ms(50);
	ADXRS453_SendData(0x20000000);
	ADXRS453_SendData(0x20000000);
}

/******************************************************************************
* @brief Read degrees/second register from the ADXRS453
*
* @param None.
*
* @return rxDeg - Value of register.
******************************************************************************/
int ADXRS453_ReadDegrees(void)
{
	int rxDeg = 0;
	rxDeg = ADXRS453_SendData(0x20000000);

	if(((rxDeg & 0xC000000) == 0x4000000)&&((rxDeg & 0x00000004) != 0x00000004))
	{
		return(rxDeg);
	}

	return(rxDeg);
}

/******************************************************************************
* @brief Print the Degrees/Second.
*
* @param x - Degrees/Second
******************************************************************************/
void PrintDegreesPos(int x)
{
	char c[] = {' ',' ',' ','0','.','0','0','0','0',0};
	int  nr;
	int  i   = 8;

	// If the number is in negative degree range
	// set '-' as the beginning character in string
	// and set the corresponding value
	if (x > 32767 * 125)
	{
		nr = (0x10000 * 125) - x;
		c[0] = '-';
	}

	// If the number is in positive degree range
	// set '+' as the beginning character in string
	// and set the corresponding value
	else
	{
		nr = x;
		c[0] = '+';
	}

	// Extract mod 10 values from the number
	// and place them in the corresponding
	// position in the string, with ASCII conversion
	while(nr > 0)
	{
		c[i] = (nr % 10) + 48;
		nr = nr / 10;
		i = i - 1;
		// Skip the 5th position (it is pre-loaded with '.'
		if(i == 4)
		{
			i = i-1;
		}
	}

	// Print the corresponding value on UART
	xil_printf("Degrees / Second = %s\n\r",c);
}

/******************************************************************************
* @brief Demo program for the ADXRS453
*
* @param None.
*
* @return None.
******************************************************************************/
void DemoProgram(void)
{
	xil_printf("PmodGYRO2 Demo Program\n\r");

	while(1)
	{
		rxData = ADXRS453_ReadDegrees();
	    PrintDegreesPos(((rxData & 0x3FFFC00) >> 10)*125);
	}
}
