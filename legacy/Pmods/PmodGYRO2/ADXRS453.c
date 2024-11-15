/***************************************************************************//**
 *   @file   ADXRS453.c
 *   @brief  Implementation of ADXRS453 Driver.
 *   @author ATofan (alexandru.tofan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
