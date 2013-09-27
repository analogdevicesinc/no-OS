/***************************************************************************//**
*   @file   ADT7420.c
*   @brief  ADT7420 Driver Files for MicroBlaze Processor.
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

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdio.h>
#include "xil_io.h"
#include "system_config.h"
#include "ADT7420.h"

/*****************************************************************************/
/********************** Variable Definitions *********************************/
/*****************************************************************************/
volatile int 		rxData;
char                valid  = 0;
int                 TUpper = 0x1FFF;

/******************************************************************************
* @brief Performs Software Reset for ADT7420 and sets Alert Mode as Comparator.
*
* @param  None.
*
* @return None.
******************************************************************************/
void ADT7420_Init(void)
{
	unsigned char txBuffer[1] = { 0x00 };

	I2C_Write(I2C_BASEADDR, ADT7420_IIC_ADDR, SOFT_RST_REG, 1, txBuffer);

	SetAlertModeComparator();
}
/******************************************************************************
* @brief Sets Alert Mode as Comparator.
*
* @param None.
*
* @return None.
******************************************************************************/
void SetAlertModeComparator(void)
{
	unsigned char txBuffer[1] = {0x00};

	txBuffer[0] = 1 << INT_CT;

	I2C_Write(I2C_BASEADDR, ADT7420_IIC_ADDR, CONFIG_REG, 1, txBuffer);
}

/******************************************************************************
* @brief Returns value from Configuration Register.
*
* @param None.
*
* @return rxBuffer[0] & 0x7F - all bits in the Configuration Register, except RESOLUTION bit
******************************************************************************/
char ADT7420_ReadConfigReg(void)
{
	unsigned char rxBuffer[1] = {0x00};

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, CONFIG_REG, 1, rxBuffer);

	return(rxBuffer[0] & 0x7F);
}

/******************************************************************************
* @brief Displays Revision ID and Manufacturer ID.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_PrintID(void)
{
	unsigned char rxBuffer[1] = {0x00};

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, ID_REG, 1, rxBuffer);

	xil_printf("Revision ID = %d\n\r", rxBuffer[0] & REVISION_ID);
	xil_printf("Manufacture ID = %d\n\r", (rxBuffer[0] & MANUFACTURE_ID) >> 3);
	xil_printf("-----------------------------------------\n\r");
}

/******************************************************************************
* @brief Displays Main Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_DisplayMainMenu(void)
{
	xil_printf("\n\rPmodTMP2 Demo Program \n\r");

	ADT7420_PrintID();

	xil_printf("Available options: \n\r");
	xil_printf("	[t] Read Temperature\n\r");
	xil_printf("	[r] Set Resolution\n\r");
	xil_printf("	[h] Set THigh\n\r");
	xil_printf("	[l] Set TLow\n\r");
	xil_printf("	[c] Set TCrit\n\r");
	xil_printf("	[y] Set THyst\n\r");
	xil_printf("	[f] Set Fault Queue\n\r");
	xil_printf("	[s] Display Settings \n\r");
	xil_printf("	[m] Stop the program and display this menu\n\r");
	xil_printf("Press key to select desired option\n\r");
	xil_printf("Press [q] to exit the application\n\r");
}

/******************************************************************************
* @brief Displays Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_DisplayMenu(void)
{
	xil_printf("\n\rAvailable options: \n\r");
	xil_printf("	[t] Read Temperature\n\r");
	xil_printf("	[r] Set Resolution\n\r");
	xil_printf("	[h] Set THigh\n\r");
	xil_printf("	[l] Set TLow\n\r");
	xil_printf("	[c] Set TCrit\n\r");
	xil_printf("	[y] Set THyst\n\r");
	xil_printf("	[f] Set Fault Queue\n\r");
	xil_printf("	[s] Display Settings \n\r");
	xil_printf("	[m] Stop the program and display this menu\n\r");

	rxData = 0;
}

/******************************************************************************
* @brief Returns resolution of ADT7420 internal ADC.
*
* @param display - 0 -> resolution is displayed on UART
*				 - 1 -> resolution is not displayed on UART.
*
* @return (rxBuffer[0] & (1 << RESOLUTION)) - bit 7 of CONFIGURATION REGISTER
* 				 - 0 -> resolution is 13 bits
* 				 - 1 -> resolution is 16 bits.
******************************************************************************/
unsigned char ADT7420_GetResolution(char display)
{
	unsigned char rxBuffer[2] = {0x00, 0x00};

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, CONFIG_REG, 1, rxBuffer);

	if(display == 1)
	{
		if((rxBuffer[0] & (1 << RESOLUTION)) == 0)
		{
			xil_printf("Resolution is 13 bits (0.0625 C/LSB)\n\r");
		}
		else
		{
			xil_printf("Resolution is 16 bits (0.0078 C/LSB)\n\r");
		}
	}

	return (rxBuffer[0] & (1 << RESOLUTION));
}

/******************************************************************************
* @brief Displays Set Resolution Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_DisplayResolutionMenu(void)
{
	xil_printf("\n\r>Set Resolution Menu\n\r");
	xil_printf("-----------------------------------------\n\r");
	xil_printf("Possible resolutions are:\n\r");
	xil_printf("	1. 13 bits (0.0625 C/LSB):\n\r");
	xil_printf("	2. 16 bits (0.0078 C/LSB):\n\r");
}

/******************************************************************************
* @brief Sets and displays resolution for ADC of ADT7420
*
* @param None
*
* @return None
******************************************************************************/
void ADT7420_SetResolution(void)
{
	unsigned char txBuffer[1] = { 0x00 };
	char          rx          = 0;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	ADT7420_DisplayResolutionMenu();

	// Check if data is available on the UART
	rx = UartReadChar();

	switch (rx)
		{
		case '1' :
			txBuffer[0] = (0 << RESOLUTION) | ADT7420_ReadConfigReg() ; // so as not to change other configuration parameters
			xil_printf("Resolution is 13 bits (0.0625 C/LSB)\n\r");
			TUpper = 0x1FFF;
			rxData = 'm';
			xil_printf("\n\r>Returning to Main Menu...\n\r");
			break;
		case '2' :
			txBuffer[0] = (1 << RESOLUTION) | ADT7420_ReadConfigReg();
			xil_printf("Resolution is 16 bits (0.0078 C/LSB)\n\r");
			TUpper = 0xFFFF;
			rxData = 'm';
			xil_printf("\n\r>Returning to Main Menu...\n\r");
			break;
		default:
			xil_printf("Wrong option!\n\r");
			break;
		}

	I2C_Write(I2C_BASEADDR, ADT7420_IIC_ADDR, CONFIG_REG, 1, txBuffer);

	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Reads data from the Temperature MSB and LSB registers of ADT7420.
*
* @param None.
*
* @return data - value read from the Temperature MSB and LSB registers.
******************************************************************************/
int ADT7420_ReadTemp(void)
{
	unsigned char rxBuffer[2] = {0x00,0x00};
	int           data        = 0;

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, TEMP_REG, 2, rxBuffer);

	if(ADT7420_GetResolution(0) == 0)
	{
		data = (rxBuffer[0] << 5) | (rxBuffer[1] >> 3);
	}
	else
	{
		data = (rxBuffer[0] << 8) | (rxBuffer[1]);
	}

	return (data);
}

/******************************************************************************
* @brief Displays temperature data in degrees Celsius.
*
* @param data - value read from the Temperature MSB and LSB registers.
*
* @return None.
******************************************************************************/
void Display_Temp(short int data)
{
	float     value     = 0;
    short int whole     = 0;
	short int thousands = 0;
	float     Vlsb_13   = 0.0625;
	float     Vlsb_16   = 0.0078;

	// converting data for display
	if(ADT7420_GetResolution(0) == 0)
	{
		if(data&0x1000)
		{
			data = data	| 0xffffe000;
		}
		value = data * Vlsb_13;
	}
	else
	{
		value = data * Vlsb_16;
	}

	if(value >= 0)
	{
	    whole = (int)value;
	   	thousands = (value - whole) * 1000;

	    if(thousands < 10)
		{
			xil_printf("T = %d.00%1d C\n\r", whole,thousands);
		}
	    else if(thousands < 100)
		{
			xil_printf("T = %d.0%2d C\n\r", whole,thousands);
		}
	    else
	    {
	    	xil_printf("T = %d.%3d C\n\r", whole,thousands);
	    }
	}
	else
	{
		value = value * (-1);
		whole = (int)value;
		thousands = (value - whole) * 1000;

	    if(thousands < 10)
	    {
		    xil_printf("T = -%d.00%1d C\n\r", whole,thousands);
	    }
	    else if(thousands < 100)
	    {
		    xil_printf("T = -%d.0%2d C\n\r", whole,thousands);
        }
	    else
	    {
		    xil_printf("T = -%d.%3d C\n\r", whole,thousands);
        }
	}
}

/******************************************************************************
* @brief Reads data from the UART console.
*
* @param None.
*
* @return value -> data converted to hex value
* 			0 	-> characters read are not hex values.
******************************************************************************/
int ADT7420_ConsoleRead(void)
{
	char rx    = 0;
	char c[4]  = "0000";
	char *c_ptr;
	int  i     = 0;
	char cnt   = 0;
	int  value = 0;

	cnt   = 0;
	i     = 0;
	valid = 0;
	value = 0;
	c_ptr = c;

	while(i < 6)
	{
		// Check if data is available on the UART
		rx = UartReadChar();
		xil_printf("%c", rx);
		// Check if pressed key is [Enter]
		if(rx == 0x0D)
		{
			i = 5;
		}
		else if(rx == 0x0A)
		{
			i = 5;
		}
		else if(((rx > 0x00)&&(rx < 0x30))|| // Not 0 - 9
				((rx > 0x39)&&(rx < 0x41))|| // Not A - F
				((rx > 0x46)&&(rx < 0x61))|| // Not a - f
				(rx > 0x66))
		{
			xil_printf("\n\rCharacters entered must be HEX values (0 to 9 and A B C D E F)\n\r");
			i = 6;
			valid = 0;
		}
		else
		{
			*c_ptr++ = rx;
			cnt = cnt + 1;
			valid = 1;
		}
		if(cnt == 4)
		{
			i = 6;
		}
		i++;
	}

	// Translate from ASCII to hex
	for(i = 0; i < cnt; i++)
	{
		if(c[i] > 0x60)
		{
			value = value * 16 + (c[i] - 0x57);
		}
		else if(c[i] > 0x39)
		{
			value = value * 16 + (c[i] - 0x37);
		}
		else
		{
			value = value * 16 + (c[i] - 0x30);
		}
	}

	if(valid == 1)
	{
		return value;
	}
	else
	{
		return 0;
	}
}

/******************************************************************************
* @brief Menu for setting data in THigh register.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_DisplaySetTHighMenu(void)
{
	int THigh = 0;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	xil_printf("\n\r>Set THigh Menu\n\r");
	xil_printf("-----------------------------------------\n\r");

	xil_printf("\n\rPlease enter a value between 0x0000 and 0x%4x: 0x", TUpper);

	THigh = ADT7420_ConsoleRead();

	while(!((THigh>=0x0000)&(THigh<=TUpper)))
	{
		xil_printf("\n\rValue for THigh must be in the range 0x0000 and 0x%4x\n\r", TUpper);
		xil_printf("Please enter a valid value: 0x");
		THigh = ADT7420_ConsoleRead();
	}

	if(valid == 1)
	{
		ADT7420_SetTHigh(THigh);
		rxData = 'm';
		xil_printf("\n\r\n\r>Returning to Main Menu...\n\r");
	}

	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Sets value of THigh register.
*
* @param THigh - value to be placed in the register.
*
* @return None.
******************************************************************************/
void ADT7420_SetTHigh(int THigh)
{
	unsigned char txBuffer[2] = {0x00, 0x00};

	if(ADT7420_GetResolution(0) == 0)
	{
		txBuffer[0] = (THigh & 0x1FE0) >> 5;
		txBuffer[1] = (THigh & 0x001F) << 3;
	}
	else
	{
		txBuffer[0] = (THigh & 0xFF00) >> 8;
		txBuffer[1] = THigh & 0x00FF;
	}

	I2C_Write(I2C_BASEADDR, ADT7420_IIC_ADDR, TH_SETP_MSB, 2, txBuffer);
}

/******************************************************************************
* @brief Displays value of THigh setpoint.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_PrintTHigh(void)
{
	unsigned char rxBuffer[2] = {0x00, 0x00};
	int           val         = 0;

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, TH_SETP_MSB, 2, rxBuffer);

	if(ADT7420_GetResolution(0) == 0)
		val = ( rxBuffer[0] << 5 ) | ( rxBuffer[1] >> 3);
	else
		val = (rxBuffer[0] << 8) | rxBuffer[1];

	xil_printf("THigh Setpoint ");
	Display_Temp(val);
}

/******************************************************************************
* @brief Menu for setting data in TLow register.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_DisplaySetTLowMenu(void)
{
	int TLow = 0;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	xil_printf("\n\r>Set TLow Menu\n\r");
	xil_printf("-----------------------------------------\n\r");
	xil_printf("\n\rPlease enter a value between 0x0000 and 0x%4x: 0x", TUpper);

	TLow = ADT7420_ConsoleRead();

	while(!((TLow>=0x0000)&(TLow<=TUpper)))
	{
		xil_printf("\n\rValue for TLow must be in the range 0x0000 and 0x%4x\n\r", TUpper);
		xil_printf("Please enter a valid value: 0x");
		TLow = ADT7420_ConsoleRead();
	}

	if(valid == 1)
	{
		ADT7420_SetTLow(TLow);
		rxData = 'm';
		xil_printf("\n\r\n\r>Returning to Main Menu...\n\r");
	}

	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Sets value of  TLow register.
*
* @param TLow - value to be placed in the register.
*
* @return None.
******************************************************************************/
void ADT7420_SetTLow(int TLow)
{
	unsigned char txBuffer[2] = {0x00, 0x00};

	if(ADT7420_GetResolution(0) == 0)
	{
		txBuffer[0] = (TLow & 0x1FE0) >> 5;
		txBuffer[1] = (TLow & 0x001F) << 3;
	}
	else
	{
		txBuffer[0] = (TLow & 0xFF00) >> 8;
		txBuffer[1] = TLow & 0x00FF;
	}

	I2C_Write(I2C_BASEADDR, ADT7420_IIC_ADDR, TL_SETP_MSB, 2, txBuffer);
}

/******************************************************************************
* @brief Displays value of TLow setpoint.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_PrintTLow(void)
{
	unsigned char rxBuffer[2] = {0x00, 0x00};
	int           val         = 0;

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, TL_SETP_MSB, 2, rxBuffer);

	if(ADT7420_GetResolution(0) == 0)
	{
		val = ( rxBuffer[0] << 5 ) | ( rxBuffer[1] >> 3);
	}
	else
	{
		val = (rxBuffer[0] << 8) | rxBuffer[1];
	}

	xil_printf("TLow Setpoint ");
	Display_Temp(val);
}

/******************************************************************************
* @brief Sets value of  TCrit register.
*
* @param TCrit - value to be placed in the register.
*
* @return None.
******************************************************************************/
void ADT7420_SetTCrit(int TCrit)
{
	unsigned char txBuffer[2] = {0x00, 0x00};

	if(ADT7420_GetResolution(0) == 0)
	{
		txBuffer[0] = (TCrit & 0x1FE0) >> 5;
		txBuffer[1] = (TCrit & 0x001F) << 3;
	}
	else
	{
		txBuffer[0] = (TCrit & 0xFF00) >> 8;
		txBuffer[1] = TCrit & 0x00FF;
	}

	I2C_Write(I2C_BASEADDR, ADT7420_IIC_ADDR, TCRIT_SETP_MSB, 2, txBuffer);
}

/******************************************************************************
* @brief Menu for setting data in TCrit register.
*
* @param None.
*
* @return None.
******************************************************************************/

void ADT7420_DisplaySetTCritMenu(void)
{
	int TCrit = 0;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	xil_printf("\n\r>Set TCrit Menu\n\r");
	xil_printf("-----------------------------------------\n\r");
	xil_printf("\n\rPlease enter a value between 0x0000 and 0x%4x: 0x", TUpper);

	TCrit = ADT7420_ConsoleRead();

	while(!((TCrit>=0x0000)&(TCrit<=TUpper)))
	{
		xil_printf("\n\rValue for TCrit must be in the range 0x0000 and 0x%4x\n\r", TUpper);
		xil_printf("Please enter a valid value: 0x");
		TCrit = ADT7420_ConsoleRead();
	}

	if(valid == 1)
	{
		ADT7420_SetTCrit(TCrit);
		rxData = 'm';
		xil_printf("\n\r\n\r>Returning to Main Menu...\n\r");
	}

	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Displays value of TCrit setpoint.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_PrintTCrit(void)
{
	unsigned char rxBuffer[2] = {0x00, 0x00};
	int           val         = 0;

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, TCRIT_SETP_MSB, 2, rxBuffer);

	if(ADT7420_GetResolution(0) == 0)
	{
		val = ( rxBuffer[0] << 5 ) | ( rxBuffer[1] >> 3);
	}
	else
	{
		val = (rxBuffer[0] << 8) | rxBuffer[1];
	}

	xil_printf("TCrit Setpoint ");
	Display_Temp(val);
}

/******************************************************************************
* @brief Menu for setting data in THyst register.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_DisplaySetTHystMenu(void)
{
	int THyst = 0;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	xil_printf("\n\r>Set THyst Menu\n\r");
	xil_printf("-----------------------------------------\n\r");
	xil_printf("Enter a value from 0x0000 to 0x000F: 0x");

	THyst = ADT7420_ConsoleRead();

	while(!((THyst>=0)&(THyst<16)))
	{
		xil_printf("\n\rValue for THyst must be in the range 0 C to 15 C\n\r");
		xil_printf("Please enter a valid value: 0x");
		THyst = ADT7420_ConsoleRead();
	}

	if(valid == 1)
	{
		ADT7420_SetHysteresis(THyst);
		rxData = 'm';
		xil_printf("\n\r\n\r>Returning to Main Menu...\n\r");
	}

	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Sets value of THyst register.
*
* @param THyst - value from 0x0000 to 0x000F to be placed in the register.
*
* @return None.
******************************************************************************/
void ADT7420_SetHysteresis(int THyst)
{
	unsigned char txBuffer[1] = {0x00};

	txBuffer[0] = THyst & 0x0F;
	I2C_Write(I2C_BASEADDR, ADT7420_IIC_ADDR, T_HYST_SETP, 1, txBuffer);
}

/******************************************************************************
* @brief Displays value of THyst.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_PrintHysteresis(void)
{
	unsigned char rxBuffer[2] = { 0x00 };

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, T_HYST_SETP, 1, rxBuffer);
	xil_printf("THyst Setpoint T = %d C\n\r", rxBuffer[0]);
}

/******************************************************************************
* @brief Menu for setting Fault Queue.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_DisplaySetFaultQueueMenu(void)
{
	unsigned char txBuffer[1] = { 0x00 };
	char          rx          = 0;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	xil_printf("\n\r>Fault Queue Menu\n\r");
	xil_printf("-----------------------------------------\n\r");
	xil_printf("Number of fault queues:\n\r");
	xil_printf("	1. 1 fault\n\r");
	xil_printf("	2. 2 faults\n\r");
	xil_printf("	3. 3 faults\n\r");
	xil_printf("	4. 4 faults\n\r");

	// Check if data is available on the UART
	rx = UartReadChar();

	switch (rx)
	{
		case '1' :
			txBuffer[0] = 0x00 << FAULT_QUEUE;
			xil_printf("1 fault queue\n\r");
			rxData = 'm';
			xil_printf("\n\r>Returning to Main Menu...\n\r");
			break;
		case '2' :
			txBuffer[0] = 0x01 << FAULT_QUEUE;
			xil_printf("2 fault queues\n\r");
			rxData = 'm';
			xil_printf("\n\r>Returning to Main Menu...\n\r");
			break;
		case '3' :
			txBuffer[0] = 0x02 << FAULT_QUEUE;
			xil_printf("3 fault queues\n\r");
			rxData = 'm';
			xil_printf("\n\r>Returning to Main Menu...\n\r");
			break;
		case '4' :
			txBuffer[0] = 0x03 << FAULT_QUEUE;
			xil_printf("4 fault queues\n\r");
			rxData = 'm';
			xil_printf("\n\r>Returning to Main Menu...\n\r");
			break;
		default:
			xil_printf("Wrong option!\n\r");
			break;
	}
	I2C_Write(I2C_BASEADDR, ADT7420_IIC_ADDR, CONFIG_REG, 1, txBuffer);

	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Displays value of Fault Queues.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_PrintFaultQueue(void)
{
	unsigned char rxBuffer[1] = { 0x00 };
	char          rx          = 0;

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, CONFIG_REG, 1, rxBuffer);

	rx = rxBuffer[0] & (0x03 << FAULT_QUEUE);

	switch (rx)
	{
		case 0x00 :
			xil_printf("1 fault queue\n\r");
			break;
		case 0x01 :
			xil_printf("2 fault queues\n\r");
			break;
		case 0x02 :
			xil_printf("3 fault queues\n\r");
			break;
		case 0x03 :
			xil_printf("4 fault queues\n\r");
			break;
		default:
			break;
	}
}

/******************************************************************************
* @brief Displays Alert Mode setting.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_PrintAlertMode(void)
{
	unsigned char rxBuffer[2] = { 0x00 };

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, CONFIG_REG, 1, rxBuffer);

	if (rxBuffer[0] & (1 << INT_CT))
	{
		xil_printf("Alert Mode: Comparator\n\r");
	}
	else
	{
		xil_printf("Alert Mode: Interrupt\n\r");
	}
}

/******************************************************************************
* @brief Displays output polarity setting for CT pin.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_PrintCTPolarity(void)
{
	unsigned char rxBuffer[1] = { 0x00 };

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, CONFIG_REG, 1, rxBuffer);

	if (rxBuffer[0] & (1 << CT_POL))
	{
		xil_printf("CT pin is Active High\n\r");
	}
	else
	{
		xil_printf("CT pin is Active Low\n\r");
	}
}


/******************************************************************************
* @brief Displays output polarity setting for INT pin.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_PrintINTPolarity(void)
{
	unsigned char rxBuffer[1] = { 0x00 };

	I2C_Read(I2C_BASEADDR, ADT7420_IIC_ADDR, CONFIG_REG, 1, rxBuffer);

	if (rxBuffer[0] & (1 << INT_POL))
	{
		xil_printf("INT pin is Active High\n\r");
	}
	else
	{
		xil_printf("INT pin is Active Low\n\r");
	}
}

/******************************************************************************
* @brief Displays current settings of the ADT7420.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADT7420_DisplaySettings(void)
{
	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	xil_printf("\n\r ADT7420 Settings \n\r");
	xil_printf("-----------------------------------------\n\r");
	ADT7420_GetResolution(1);

	ADT7420_PrintTHigh();
	ADT7420_PrintTLow();
	ADT7420_PrintTCrit();
	ADT7420_PrintHysteresis();

	ADT7420_PrintFaultQueue();

	ADT7420_PrintAlertMode();
	ADT7420_PrintCTPolarity();
	ADT7420_PrintINTPolarity();

	rxData = 'm';
	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
	xil_printf("\n\r>Returning to Main Menu...\n\r");
}

/******************************************************************************
* @brief Demo program for the ADT7420.
*
* @param None.
*
* @return None.
******************************************************************************/
void DemoProgram(void)
{
	// Display Main Menu on UART
	ADT7420_DisplayMainMenu();

    while(rxData != 'q')
    {
    	switch(rxData)
    	{
    	case 't':
    		Display_Temp(ADT7420_ReadTemp());
    		break;
    	case 'r':
    		ADT7420_SetResolution();
    		break;
    	case 'h':
    		ADT7420_DisplaySetTHighMenu();
    		break;
    	case 'l':
    		ADT7420_DisplaySetTLowMenu();
    		break;
    	case 'c':
    		ADT7420_DisplaySetTCritMenu();
    		break;
        case 'y':
        	ADT7420_DisplaySetTHystMenu();
    		break;
    	case 'f':
    		ADT7420_DisplaySetFaultQueueMenu();
    		break;
    	case 's':
			ADT7420_DisplaySettings();
			break;
    	case 'm':
    		ADT7420_DisplayMenu();
    		break;
    	case 0:
    		break;
    	default:
    		xil_printf("\n\rWrong option! Please select one of the options below.");
    		ADT7420_DisplayMenu();
    		break;
    	}
    }

    xil_printf("Exiting application\n\r");

}
