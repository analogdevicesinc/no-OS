/***************************************************************************//**
 *   @file   ADXL362.c
 *   @brief  Implementation of ADXL362 Driver.
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
#include "ADXL362.h"

/*****************************************************************************/
/********************** Variable Definitions *********************************/
/*****************************************************************************/
volatile char rxData;
volatile char mode;

/******************************************************************************
* @brief Generates a delay in ms.
*
* @param ms_count - Number of ms desired.
*
* @return None.
******************************************************************************/
void delay_ms(u32 ms_count)
{
	u32 count;
	for (count = 0; count < ((ms_count * 6670) + 1); count++)
	{
	  asm("nop");
	}
}

/******************************************************************************
* @brief Displays Main Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL362_DisplayMainMenu(void)
{
	xil_printf("\n\r\n\rPmod-ACL2 Demo Program\n\r");
	xil_printf("\n\r");
	xil_printf("Options: \n\r");
	xil_printf("    [a] Display acceleration on All Axes\n\r");
	xil_printf("    [x] Display acceleration on X Axis\n\r");
	xil_printf("    [y] Display acceleration on Y Axis\n\r");
	xil_printf("    [z] Display acceleration on Z Axis\n\r");
	xil_printf("    [t] Display temperature\n\r");
	xil_printf("    [r] Select measurement range\n\r");
	xil_printf("    [s] Switch resolution\n\r");
	xil_printf("    [i] Print device ID\n\r");
	xil_printf("    [m] Display main menu\n\r");
	xil_printf("Please select desired option\n\r");

	rxData = 0;
}

/******************************************************************************
* @brief Displays Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL362_DisplayMenu(void)
{
	xil_printf("\n\rOptions: \n\r");
	xil_printf("    [a] Display acceleration on All Axes\n\r");
	xil_printf("    [x] Display acceleration on X Axis\n\r");
	xil_printf("    [y] Display acceleration on Y Axis\n\r");
	xil_printf("    [z] Display acceleration on Z Axis\n\r");
	xil_printf("    [t] Display temperature\n\r");
	xil_printf("    [r] Select measurement range\n\r");
	xil_printf("    [s] Switch resolution\n\r");
	xil_printf("    [i] Print device ID\n\r");
	xil_printf("    [m] Display main menu\n\r");
	xil_printf("Please select desired option\n\r");

	rxData = 0;
}

/******************************************************************************
* @brief Displays a float value divided in whole and thousands.
*
* @param whole - whole value.
* @param thousands - thousands value.
* @param sign - 0 = +, 1 = -
* @param temp - 0 = acceleration, 1 = celsius
*
* @return None.
******************************************************************************/
void ADXL362_Display(int whole, int thousands, int sign, char temp)
{
	if(thousands > 99)
	{
		xil_printf("%c%d.%3d %c\n\r", (sign == 0)?'+':'-', whole, thousands, (temp == 0)?'g':'C');
	}
	else if(thousands > 9)
	{
		xil_printf("%c%d.0%2d %c\n\r", (sign == 0)?'+':'-', whole, thousands, (temp == 0)?'g':'C');
	}
	else
	{
		xil_printf("%c%d.00%1d %c\n\r", (sign == 0)?'+':'-', whole, thousands, (temp == 0)?'g':'C');
	}
}

/******************************************************************************
* @brief Writes to an ADXL362 Register.
*
* @param addr - register address.
* @param data - data to be written
*
* @return None.
******************************************************************************/
void ADXL362_WriteReg(char addr, char data)
{
	char txBuffer[3] = {0x0A, addr, data};
	char rxBuffer[3] = {0x00, 0x00, 0x00};

	SPI_TransferData(SPI_BASEADDR, 3, txBuffer, 3, rxBuffer, 1);
}

/******************************************************************************
* @brief Reads from an ADXL362 Register.
*
* @param addr - register address.
*
* @return rx - data read from register.
******************************************************************************/
char ADXL362_ReadReg(char addr)
{
	char txBuffer[3] = {0x0B, addr, 0x00};
	char rxBuffer[3] = {0x00, 0x00, 0x00};
	char rx = 0x00;

	SPI_TransferData(SPI_BASEADDR, 3, txBuffer, 3, rxBuffer, 1);

	rx = rxBuffer[2];

	return(rx);
}

/******************************************************************************
* @brief Prints ADXL362 internal ID Registers.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL362_PrintID(void)
{
	xil_printf("\n\r");
	xil_printf("Device ID:  %x\n\r",ADXL362_ReadReg(ADXL362_DEVID_AD) & 0xFF);
	xil_printf("Device ID:  %x\n\r",ADXL362_ReadReg(ADXL362_DEVID_MST) & 0xFF);
	xil_printf("Part ID:    %x\n\r",ADXL362_ReadReg(ADXL362_PARTID) & 0xFF);
	xil_printf("Silicon ID: %x\n\r",ADXL362_ReadReg(ADXL362_REVID) & 0xFF);

	rxData = 0;

	xil_printf("\n\r");
	ADXL362_DisplayMenu();
}

/******************************************************************************
* @brief Reads temperature from ADXL362.
*
* @param None.
*
* @return Temperature value.
******************************************************************************/
int ADXL362_ReadTemp(void)
{
	char temp[2] = {0x00, 0x00};
	int result = 0;

	temp[0] = ADXL362_ReadReg(ADXL362_TEMP_H);
	temp[1] = ADXL362_ReadReg(ADXL362_TEMP_L);

	result = ((temp[0] & 0xFF) << 8) | ((temp[1]) & 0xFF);

	return(result);
}

/******************************************************************************
* @brief Displays read temperature from ADXL362.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL362_PrintTemp(void)
{
	float 	value 		= 0;
	int 	rxTemp 		= 0;
	char 	sign 		= 0;
	int 	whole 		= 0;
	int 	thousands 	= 0;

	rxTemp = ADXL362_ReadTemp();
	value = (float)((rxTemp & 0x7FF) * 0.065);
	sign = (rxTemp & 0x800) >> 11;

	whole = value;
	thousands = (value - whole) * 1000;

	xil_printf("\n\rADXL362 Temperature is: ");
	ADXL362_Display(whole, thousands, sign, 1);
	//xil_printf("\n\r");
	rxData = 0;

	xil_printf("\n\r");
	ADXL362_DisplayMenu();
}

/******************************************************************************
* @brief Checks if ADXL362 Conversion is complete.
*
* @param None.
*
* @return 1 - Data is ready, 0 - Data is not ready.
******************************************************************************/
char ADXL362_IsDataReady(void)
{
	if((ADXL362_ReadReg(ADXL362_STATUS) & 0x01) == 0x01)
	{
		return(1);
	}
	else
	{
		return(0);
	}
}

/******************************************************************************
* @brief Read X Axis 8 MSB.
*
* @param None.
*
* @return rx - read value.
******************************************************************************/
char ADXL362_ReadXSmall(void)
{
	char rx;

	while(!ADXL362_IsDataReady());
	rx = ADXL362_ReadReg(ADXL362_XDATA) & 0xFF;

	return(rx);
}

/******************************************************************************
* @brief Read Y Axis 8 MSB.
*
* @param None.
*
* @return rx - read value.
******************************************************************************/
char ADXL362_ReadYSmall(void)
{
	char rx;

	while(!ADXL362_IsDataReady());
	rx = ADXL362_ReadReg(ADXL362_YDATA) & 0xFF;

	return(rx);
}

/******************************************************************************
* @brief Read Z Axis 8 MSB.
*
* @param None.
*
* @return rx - read value.
******************************************************************************/
char ADXL362_ReadZSmall(void)
{
	char rx;

	while(!ADXL362_IsDataReady());
	rx = ADXL362_ReadReg(ADXL362_ZDATA) & 0xFF;

	return(rx);
}

/******************************************************************************
* @brief Displays Axis read value 8 MSB.
*
* @param axis - which axis to print.
*
* @return None.
******************************************************************************/
void ADXL362_PrintSmall(char axis)
{
	int 	rxG 		= 0;
	int 	sign 		= 0;
	float 	value 		= 0;
	int 	whole 		= 0;
	int 	thousands 	= 0;

	switch(axis)
	{
		case 'x':
			rxG = ADXL362_ReadXSmall();
			xil_printf("X = ");
			break;
		case 'y':
			rxG = ADXL362_ReadYSmall();
			xil_printf("Y = ");
			break;
		case 'z':
			xil_printf("Z = ");
			rxG = ADXL362_ReadZSmall();
			break;
		default:
			break;
	}

	// Sign is MSB. If 1 -> Negative Number, else Positive Number
	sign = (rxG & 0x80) >> 7;

	if(sign == 1)
	{
		// If negative number, padding with FFFFF MSB's
		rxG = (rxG << 4) | 0xFFFFF000;
	}
	else
	{
		rxG = (rxG << 4);
	}

	value = ((float)rxG / 1000);

	if(rxG >= 0)
	{
		whole = value;
		thousands = (value - whole) * 1000;
	}
	else
	{
		value = value * (-1);
		whole = value;
		thousands = (value - whole) * 1000;
	}

	ADXL362_Display(whole, thousands, sign, 0);
}

/******************************************************************************
* @brief Displays data read from all 3 axes, 8 MSB.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL362_DisplayAllSmall(void)
{
	ADXL362_PrintSmall('x');
	ADXL362_PrintSmall('y');
	ADXL362_PrintSmall('z');
}

/******************************************************************************
* @brief Read X Axis 12 MSB.
*
* @param None.
*
* @return rx - read value.
******************************************************************************/
int ADXL362_ReadX(void)
{
	int rx;

	while(!ADXL362_IsDataReady());
	rx = ((ADXL362_ReadReg(ADXL362_XDATA_H) & 0xFF) << 8) | (ADXL362_ReadReg(ADXL362_XDATA_L) & 0xFF);


	return(rx);
}

/******************************************************************************
* @brief Read Y Axis 12 MSB.
*
* @param None.
*
* @return rx - read value.
******************************************************************************/
int ADXL362_ReadY(void)
{
	int rx;

	while(!ADXL362_IsDataReady());
	rx = ((ADXL362_ReadReg(ADXL362_YDATA_H) & 0xFF) << 8) | (ADXL362_ReadReg(ADXL362_YDATA_L) & 0xFF);


	return(rx);
}

/******************************************************************************
* @brief Read Z Axis 12 MSB.
*
* @param None.
*
* @return rx - read value.
******************************************************************************/
int ADXL362_ReadZ(void)
{
	int rx;

	while(!ADXL362_IsDataReady());
	rx = ((ADXL362_ReadReg(ADXL362_ZDATA_H) & 0xFF) << 8) | (ADXL362_ReadReg(ADXL362_ZDATA_L) & 0xFF);


	return(rx);
	}

/******************************************************************************
* @brief Displays read axis data 12 MSB.
*
* @param axis - which data to display.
*
* @return None.
******************************************************************************/
void ADXL362_Print(char axis)
{
	int 	rxG 		= 0;
	char 	sign 		= 0;
	float 	value 		= 0;
	int 	whole 		= 0;
	int 	thousands 	= 0;

	switch(axis)
	{
		case 'x':
			rxG = ADXL362_ReadX();
			xil_printf("X = ");
			break;
		case 'y':
			rxG = ADXL362_ReadY();
			xil_printf("Y = ");
			break;
		case 'z':
			rxG = ADXL362_ReadZ();
			xil_printf("Z = ");
			break;
		default:
			break;
	}

	// Sign is MSB. If 1 -> Negative Number, else Positive Number
	sign = (rxG & 0x800) >> 11;

	if (sign == 1)
	{
		// If negative number, padding with FFFFF MSB's
		rxG = rxG | 0xFFFFF000;
	}
	else
	{
		rxG = rxG | 0x00000000;
	}

	value = ((float)rxG / 1000);

	if(rxG >= 0)
	{
		whole = value;
		thousands = (value - whole) * 1000;
	}
	else
	{
		value = value * (-1);
		whole = value;
		thousands = (value - whole) * 1000;
	}

	ADXL362_Display(whole, thousands, sign, 0);
}

/******************************************************************************
* @brief Displays data from all 3 axes, 12 MSB.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL362_DisplayAll(void)
{
	ADXL362_Print('x');
	ADXL362_Print('y');
	ADXL362_Print('z');
}

/******************************************************************************
* @brief Switches between 8 bit and 12 bit resolution.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL362_SwitchRes(void)
{
	mode = !mode;

	if(mode == 0)
	{
		xil_printf("\n\r>Full 12-Bit resolution selected.\n\r");
	}
	else
	{
		xil_printf("\n\r>8-Bit resolution selected.\n\r");
	}
	rxData = 0;
	xil_printf("\n\r");
	ADXL362_DisplayMenu();
}

/******************************************************************************
* @brief Sets measurement range.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL362_SetRange(void)
{
	char rx 	= 0;
	char rxReg 	= 0;
	xil_printf("\n\r>Select measurement range: \n\r");
	xil_printf("    [1] +/- 2g\n\r");
	xil_printf("    [2] +/- 4g\n\r");
	xil_printf("    [3] +/- 8g\n\r");
	xil_printf("Press [1] to [3] to select desired range\n\r");

	DisableInterrupts(UART_INTERRUPT);

	// Check if data is available on the UART
	rx = UartReadChar();

	rxReg = (ADXL362_ReadReg(ADXL362_FILTER_CTL) & 0x3F);

	switch(rx)
	{
		case '1':
			rxReg = 0x00 | rxReg;
			xil_printf("> +/- 2g measurement range selected\n\r");
			break;
		case '2':
			rxReg = 0x40 | rxReg;
			xil_printf("> +/- 4g measurement range selected\n\r");
			break;
		case '3':
			rxReg = 0x80 | rxReg;
			xil_printf("> +/- 8g measurement range selected\n\r");
			break;
		default:
			xil_printf("> wrong measurement range\n\r");
			break;
	}

	ADXL362_WriteReg(ADXL362_FILTER_CTL, rxReg);

	EnableInterrupts(UART_INTERRUPT);
	rxData = 0;
	xil_printf("\n\r");
	ADXL362_DisplayMenu();

}

/******************************************************************************
* @brief Initialize ADXL362.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL362_Init(void)
{
	   // Software Reset
	   ADXL362_WriteReg(ADXL362_SOFT_RESET, ADXL362_RESET_CMD);
	   delay_ms(10);
	   ADXL362_WriteReg(ADXL362_SOFT_RESET, 0x00);

	   // Enable Measurement
	   ADXL362_WriteReg(ADXL362_POWER_CTL, (2 << ADXL362_MEASURE));
}

/******************************************************************************
* @brief Demo application for AD5541A
*
* @param None.
*
* @return None.
******************************************************************************/
void DemoProgram(void)
{
	   ADXL362_DisplayMainMenu();

	   while(1)
	   {
		   switch(rxData)
		   {
		   	   case 'a':
		   		   (mode == 0) ? ADXL362_DisplayAll() : ADXL362_DisplayAllSmall();
		   		   delay_ms(100);
		   		   break;
		   	   case 'x':
		   		   (mode == 0) ? ADXL362_Print('x') : ADXL362_PrintSmall('x');
		   		   delay_ms(100);
		   		   break;
		   	   case 'y':
		   		   (mode == 0) ? ADXL362_Print('y') : ADXL362_PrintSmall('y');
		   		   delay_ms(100);
		   		   break;
		   	   case 'z':
		   		   (mode == 0) ? ADXL362_Print('z') : ADXL362_PrintSmall('z');
		   		   delay_ms(100);
		   		   break;
		   	   case 't':
		   		   ADXL362_PrintTemp();
		   		   break;
		   	   case 'r':
		   		   ADXL362_SetRange();
		   		   break;
		   	   case 's':
		   		   ADXL362_SwitchRes();
		   		   break;
		   	   case 'm':
		   		   ADXL362_DisplayMainMenu();
		   		   break;
		   	   case 'i':
		   		   ADXL362_PrintID();
		   		   break;
		   	   case 0:
		   		   break;
		   	   default:
		   		   xil_printf("\n\r> Wrong option! Please select one of the options below");
		   		   ADXL362_DisplayMenu();
		   		   break;
		   }
	   }
}

