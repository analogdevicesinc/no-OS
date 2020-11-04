/**************************************************************************//**
*   @file   AD7156.c
*   @brief  AD7156 Driver file.
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

#include <stdio.h>
#include "system_config.h"
#include "AD7156.h"

volatile char 			rxData 		 = 0;
volatile float			rangeCh1	 = 2;
volatile float			rangeCh2	 = 2;
volatile char 			selChan 	 = 0x03;
volatile char 			thrFixed 	 = 0x01;
volatile char 			thrType 	 = 0x00;
volatile int 			threshChan1  = 0x8000;
volatile int 			threshChan2  = 0x8000;
volatile unsigned char 	sensChan1 	 = 0x08;
volatile unsigned char 	sensChan2 	 = 0x08;
volatile unsigned char 	timeOutChan1 = 0x86;
volatile unsigned char 	timeOutChan2 = 0x86;
volatile int 			timeStepValue = 0;

/******************************************************************************
* @brief Display Main Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7156_DisplayMainMenu(void)
{
	xil_printf("\n\rPmodCDC1 Demo Program");
	xil_printf("\n\r");
	xil_printf("Available options:\n\r");
	xil_printf("    [d] Select Channels for display\n\r");
	xil_printf("    [c] Display Capacity\n\r");
	xil_printf("    [a] Display Average Capacity\n\r");
	xil_printf("    [t] Set Threshold / Sensitivity / Timeout\n\r");
	xil_printf("    [r] Set Input Range\n\r");
	xil_printf("    [m] Threshold Menu\n\r");
	xil_printf("    [s] Display current Settings\n\r");
	xil_printf("    [q] Stop and return to menu\n\r");
	xil_printf("Press key for desired option\n\r");
}

/******************************************************************************
* @brief Display Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7156_DisplayMenu(void)
{
	xil_printf("\n\r");
	xil_printf("Available options:\n\r");
	xil_printf("    [d] Select Channels for display\n\r");
	xil_printf("    [c] Display Capacity\n\r");
	xil_printf("    [a] Display Average Capacity\n\r");
	xil_printf("    [t] Set Threshold / Sensitivity / Timeout\n\r");
	xil_printf("    [r] Set Input Range\n\r");
	xil_printf("    [m] Threshold Menu\n\r");
	xil_printf("    [s] Display current Settings\n\r");
	xil_printf("    [q] Stop and return to menu\n\r");
	xil_printf("Press key for desired option\n\r");
	rxData = 0x00;
}

/******************************************************************************
* @brief Single Read a Register.
*
* @param reg - Register Address.
*
* @return rxBuffer[0] - Read Data.
******************************************************************************/
int AD7156_ReadReg(int reg)
{
	unsigned char rxBuffer[1] = {0x00};

	I2C_Read(I2C_BASEADDR, AD7156_I2C_ADDR, reg, 1, rxBuffer);

	return(rxBuffer[0]);
}

/******************************************************************************
* @brief Burst Read a Register.
*
* @param reg - Register Address.
* @param size - Number of bytes to read.
*
* @return rxValue - Read Data.
******************************************************************************/
int AD7156_BurstReadReg(int reg, char size)
{
	int 			rxValue 	= 0;
	int 			i 			= 0;
	unsigned char 	rxBuffer[4] = {0x00, 0x00, 0x00, 0x00};

	I2C_Read(I2C_BASEADDR, AD7156_I2C_ADDR, reg, size, rxBuffer);

	for(i = 0; i < size; i++)
	{
		rxValue = ((rxValue << 8) | rxBuffer[i]);
	}

	return(rxValue);
}

/******************************************************************************
* @brief Single Write a Register.
*
* @param reg - Register Address.
* @param txData - Data to be sent
*
* @return None.
******************************************************************************/
void AD7156_WriteReg(int reg, unsigned char txData)
{
	unsigned char txBuffer[1] = {0x00};

	txBuffer[0] = txData;
	I2C_Write(I2C_BASEADDR, AD7156_I2C_ADDR, reg, 1, txBuffer);
}

/******************************************************************************
* @brief Burst Write a Register.
*
* @param reg - Register Address.
* @param size - Number of bytes to write
* @param txData - Data to be sent
*
* @return None.
******************************************************************************/
void AD7156_BurstWriteReg(int reg, char size, int txData)
{
	unsigned char txBuffer[4] = {0x00, 0x00, 0x00, 0x00};
	int i;

	if (size < 1 || size > (sizeof(txBuffer)/sizeof(txBuffer[0])))
		return;
	for(i = 0; i < size; i++)
		txBuffer[i] = (txData >> (8 * (size - i - 1))) & 0xff;
	I2C_Write(I2C_BASEADDR, AD7156_I2C_ADDR, reg, size, txBuffer);
}

/******************************************************************************
* @brief Display current settings.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7156_DisplayCurrentSettings(void)
{
	char  rangeValueCh1;
	char  rangeValueCh2;

	xil_printf("\n\r");
	xil_printf("Current PmodCDC1 Settings are:\n\r");
	xil_printf("\n\r");
	
	// Depending on Threshold Mode
	// If Fixed Threshold
	if(thrFixed == 0x01)
	{
		xil_printf("Threshold Mode: Fixed\n\r");
		xil_printf("Threshold Type: ");
		switch(thrType)
		{
			case 0:
				xil_printf("Negative\n\r");
				break;
			case 1:
				xil_printf("Positive\n\r");
				break;
			default:
				break;
		}
		xil_printf("Channel 1\n\r");
		xil_printf("    Threshold: 0x%x\n\r", threshChan1);
		xil_printf("    Input Range: ");
		rangeValueCh1 = (AD7156_ReadReg(AD7156_CH1_SETUP)) >> 6;
		switch(rangeValueCh1)
		{
			case 0x00:
				xil_printf("2pF");
		    	rangeCh1 = 2;
				break;
			case 0x01:
				xil_printf("0.5pF");
				rangeCh1 = 0.5;
				break;
			case 0x02:
				xil_printf("1pF");
				rangeCh1 = 1;
				break;
			case 0x03:
				xil_printf("4pF");
				rangeCh1 = 4;
				break;
			default:
				break;
		}
		xil_printf("\n\rChannel 2\n\r");
		xil_printf("    Threshold: 0x%x\n\r", threshChan2);
		xil_printf("    Input Range: ");
		rangeValueCh2 = (AD7156_ReadReg(AD7156_CH2_SETUP)) >> 6;
		switch(rangeValueCh2)
		{
			case 0x00:
				xil_printf("2pF");
		    	rangeCh2 = 2;
		    	break;
			case 0x01:
				xil_printf("0.5pF");
				rangeCh2 = 0.5;
				break;
			case 0x02:
				xil_printf("1pF");
				rangeCh2 = 1;
				break;
			case 0x03:
				xil_printf("4pF");
				rangeCh2 = 4;
				break;
			default:
				break;
		}
		xil_printf("\n\r");
	}
	// If Adaptive Threshold
	else if(thrFixed == 0x00)
	{
		xil_printf("Threshold Mode: Adaptive\n\r");
		xil_printf("Threshold Type: ");
		switch(thrType)
		{
			case 0:
				xil_printf("Negative\n\r");
				break;
			case 1:
				xil_printf("Positive\n\r");
				break;
			case 2:
				xil_printf("In-Window\n\r");
				break;
			case 3:
				xil_printf("Out-Window\n\r");
				break;
			default:
				break;
		}
		xil_printf("Channel 1\n\r");
		xil_printf("    Sensitivity: 0x%x\n\r", sensChan1);
		xil_printf("    TimeOutApr:  0x%x\n\r", ((timeOutChan1 & 0xF0) >> 4));
		xil_printf("    TimeOutRec:  0x%x\n\r", (timeOutChan1 & 0x0F));
		xil_printf("    Input Range: ");
		rangeValueCh1 = (AD7156_ReadReg(AD7156_CH1_SETUP)) >> 6;
		switch(rangeValueCh1)
		{
			case 0x00:
				xil_printf("2pF");
				rangeCh1 = 2;
				break;
			case 0x01:
				xil_printf("0.5pF");
				rangeCh1 = 0.5;
				break;
			case 0x02:
				xil_printf("1pF");
				rangeCh1 = 1;
				break;
			case 0x03:
				xil_printf("4pF");
				rangeCh1 = 4;
				break;
			default:
				break;
		}
		xil_printf("\n\rChannel 2\n\r");
		xil_printf("    Sensitivity: 0x%x\n\r", sensChan2);
		xil_printf("    TimeOutApr:  0x%x\n\r", ((timeOutChan2 & 0xF0) >> 4));
		xil_printf("    TimeOutRec:  0x%x\n\r", (timeOutChan2 & 0x0F));
		xil_printf("    Input Range: ");
		rangeValueCh2 = (AD7156_ReadReg(AD7156_CH2_SETUP)) >> 6;
		switch(rangeValueCh2)
		{
			case 0x00:
				xil_printf("2pF");
				rangeCh2 = 2;
				break;
			case 0x01:
				xil_printf("0.5pF");
				rangeCh2 = 0.5;
				break;
			case 0x02:
				xil_printf("1pF");
				rangeCh2 = 1;
				break;
			case 0x03:
				xil_printf("4pF");
				rangeCh2 = 4;
				break;
			default:
				break;
		}
		xil_printf("\n\r");

	}


	// Display Menu
	rxData = 'q';

}

/******************************************************************************
* @brief Initialize AD7156.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7156_Init(void)
{
	int cfgValue = 0x00;

	// Enable Fixed Threshold Mode
	cfgValue = (1 << AD7156_ThrFixed) 	|
			   (1 << AD7156_EnCh1) 		|
			   (1 << AD7156_EnCh2) 		|
			   (1 << AD7156_MD0);
	AD7156_WriteReg(AD7156_CONFIGURATION, cfgValue);

	// Set Channel 1 Default Setup
	AD7156_WriteReg(AD7156_CH1_SETUP, 0x0B);

	// Set Channel 2 Default Setup
	AD7156_WriteReg(AD7156_CH2_SETUP, 0x0B);

	// Set Threshold Registers
	cfgValue = 0x8000;
	AD7156_BurstWriteReg(AD7156_CH1_THRESH_HIGH, 2, cfgValue);
	AD7156_BurstWriteReg(AD7156_CH2_THRESH_HIGH, 2, cfgValue);

    if(USE_PS7 == 1)
    {
    	timeStepValue = 0x1A7D;
        Xil_Out32(LED_BASEADDR + 0x288, (1 << 19) | (1 << 20));
        Xil_Out32(LED_BASEADDR + 0x284, (1 << 19) | (1 << 20));
    }
}

/******************************************************************************
* @brief Change Range.
*
* @param range - range value.
* @param chan - Channel to change range for.
*
* @return None.
******************************************************************************/
void AD7156_ChangeRange(char range, char chan)
{
	unsigned char cfgValue = 0x00;

	cfgValue = (AD7156_ReadReg(AD7156_CH1_SETUP) & 0x3F);
	cfgValue |= (range << 6);

	// Depending on Channel
	switch(chan)
	{
		case 1:
			AD7156_WriteReg(AD7156_CH1_SETUP, cfgValue);
			break;
		case 2:
			AD7156_WriteReg(AD7156_CH2_SETUP, cfgValue);
			break;
		default:
			break;
	}

}

/******************************************************************************
* @brief Print Capacitance.
*
* @param channel - Desired Channel.
*
* @return None.
******************************************************************************/
void AD7156_PrintCap(char channel)
{
	int 	rxValue 	= 0x00;
	float 	value 		= 0x00;
	int 	whole 		= 0x00;
	int 	thousands 	= 0x00;

	// Depending on Channel
	switch(channel)
	{
		case 1:
			// Read data
			rxValue = AD7156_BurstReadReg(AD7156_CH1_DATA_HIGH, 2);
			// Process data
			value = (float)((rxValue - 0x3000) * rangeCh1) / 0xA000;
			break;
		case 2:
			// Read data
			rxValue = AD7156_BurstReadReg(AD7156_CH2_DATA_HIGH, 2);
			// Process data
			value = (float)((rxValue - 0x3000) * rangeCh2) / 0xA000;
			break;
		default:
			break;
	}


	//xil_printf("%x\n\r",rxValue);

	// Print data
	if(rxValue < 0x3000)
	{
		xil_printf("Under Range!\n\r");
	}
	else if(rxValue > 0xD000)
	{
		xil_printf("Over Range!\n\r");
	}
	else if(value >= 0)
	{
		whole = (int)value;
		thousands = (value - whole) * 1000;

		if(thousands > 99)
		{
			xil_printf("Channel %d = +%d.%3dpF\n\r", channel, whole, thousands);
		}
		else if(thousands > 9)
		{
			xil_printf("Channel %d = +%d.0%2dpF\n\r", channel, whole, thousands);
		}
		else
		{
			xil_printf("Channel %d = +%d.00%1dpF\n\r", channel, whole, thousands);
		}
	}
}

/******************************************************************************
* @brief Print Average Capacitance.
*
* @param channel - Desired channel.
*
* @return None.
******************************************************************************/
void AD7156_PrintAverageCap(char channel)
{
	int 	rxValue 	= 0x00;
	float 	value 		= 0x00;
	int 	whole 		= 0x00;
	int 	thousands 	= 0x00;


	// Depending on Channel
	switch(channel)
	{
		case 1:
			// Read data
			rxValue = AD7156_BurstReadReg(AD7156_CH1_AVERAGE_HIGH, 2);
			// Process data
			value = (float)((rxValue - 0x3000) * rangeCh1) / 0xA000;
			break;
		case 2:
			// Read data
			rxValue = AD7156_BurstReadReg(AD7156_CH2_AVERAGE_HIGH, 2);
			// Process data
			value = (float)((rxValue - 0x3000) * rangeCh2) / 0xA000;
			break;
		default:
			break;
	}


	// Print data
	if(rxValue < 0x3000)
	{
		xil_printf("Under Range!\n\r");
	}
	else if(rxValue > 0xD000)
	{
		xil_printf("Over Range!\n\r");
	}
	else if(value >= 0)
	{
		whole = (int)value;
		thousands = (value - whole) * 1000;

		if(thousands > 99)
		{
			xil_printf("Channel %d = +%d.%3dpF\n\r", channel, whole, thousands);
		}
		else if(thousands > 9)
		{
			xil_printf("Channel %d = +%d.0%2dpF\n\r", channel, whole, thousands);
		}
		else
		{
			xil_printf("Channel %d = +%d.00%1dpF\n\r", channel, whole, thousands);
		}
	}
}

/******************************************************************************
* @brief Print data.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7156_Print(char channel)
{
	// Depending on channel
	switch(channel)
	{
		case 1:
			AD7156_PrintCap(2);
			break;
		case 2:
			AD7156_PrintCap(1);
			break;
		case 3:
			AD7156_PrintCap(2);
			AD7156_PrintCap(1);
			break;
		default:
			break;
	}
}

/******************************************************************************
* @brief Print Average data.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7156_PrintAverage(char channel)
{
	// Depending on channel
	switch(channel)
	{
		case 1:
			AD7156_PrintAverageCap(2);
			break;
		case 2:
			AD7156_PrintAverageCap(1);
			break;
		case 3:
			AD7156_PrintAverageCap(2);
			AD7156_PrintAverageCap(1);
			break;
		default:
			break;
	}
}

/******************************************************************************
* @brief Select channels.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7156_SelChannels(void)
{
	char 		  	rx 	 	= 0x00;
	unsigned char 	cfgReg 	= 0x00;

	xil_printf("\n\r");
	xil_printf("Select Channel for display:\n\r");
	xil_printf("    1. Channel 2\n\r");
	xil_printf("    2. Channel 1\n\r");
	xil_printf("    3. Both Channels\n\r");
	xil_printf("Press key for desired option.\n\r");
	xil_printf("\n\r");

	// Disable Interrupts in order to use polling mode
	DisableInterrupts(UART_INTERRUPT);

	// Check if data is available on the UART
	rx = UartReadChar();

	selChan = rx - 0x30;

	switch(selChan)
	{
		case 1:
			xil_printf("> Channel 2 selected\n\r");
			break;
		case 2:
			xil_printf("> Channel 1 selected\n\r");
			break;
		case 3:
			xil_printf("> Both Channels selected\n\r");
			break;
		default:
			break;
	}

	cfgReg = AD7156_ReadReg(AD7156_CONFIGURATION) & 0xE7;
	cfgReg |= (selChan << AD7156_EnCh2);
	AD7156_WriteReg(AD7156_CONFIGURATION, cfgReg);

	// Await new command after exiting function
	rxData = 'q';

	// Enable Interrupts
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Select Threshold.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7156_SelThreshold(void)
{
	char 			rx 		= 0x00;
	unsigned char 	cfgReg  = 0x00;

	xil_printf("\n\r");
	xil_printf("Select threshold type:\n\r");
	xil_printf("    1. Adaptive\n\r");
	xil_printf("    2. Fixed\n\r");
	xil_printf("Press key for desired option.\n\r");
	xil_printf("\n\r");

	// Disable Interrupts in order to use polling mode
	DisableInterrupts(UART_INTERRUPT);

	// Check if data is available on the UART
	rx = UartReadChar();

	rx = rx - 0x30 - 0x01;

	switch(rx)
	{
		case 0:
			xil_printf("> Adaptive Threshold selected\n\r");
			break;
		case 1:
			xil_printf("> Fixed Threshold selected\n\r");
			break;
		default:
			break;
	}

	thrFixed = rx;
	cfgReg = AD7156_ReadReg(AD7156_CONFIGURATION) & 0x7F;
	cfgReg |= (rx << AD7156_ThrFixed);
	AD7156_WriteReg(AD7156_CONFIGURATION, cfgReg);

	xil_printf("\n\r");
	xil_printf("Select threshold mode:\n\r");
	if(thrFixed == 0x01)
	{
		xil_printf("    1. Negative\n\r");
		xil_printf("    2. Positive\n\r");
	}
	else
	{
		xil_printf("    1. Negative\n\r");
		xil_printf("    2. Positive\n\r");
		xil_printf("    3. In-Window\n\r");
		xil_printf("    4. Out-Window\n\r");
	}
	xil_printf("Press key for desired option.\n\r");
	xil_printf("\n\r");

	// Check if data is available on the UART
	rx = UartReadChar();

	rx = rx - 0x30 - 0x01;

	thrType = rx;

	switch(rx)
	{
		case 0:
			xil_printf("> Negative Threshold selected\n\r");
			break;
		case 1:
			xil_printf("> Positive Threshold selected\n\r");
			break;
		case 2:
			if(thrFixed == 0x00)
			{
				xil_printf("> In-Window Threshold selected\n\r");
			}
			break;
		case 3:
			if(thrFixed == 0x00)
			{
				xil_printf("> Out-Window Threshold selected\n\r");
			}
		default:
			break;
	}

	cfgReg = AD7156_ReadReg(AD7156_CONFIGURATION) & 0x9F;
	cfgReg |= (rx << AD7156_ThrMD0);
	AD7156_WriteReg(AD7156_CONFIGURATION, cfgReg);

	xil_printf("> Saving previous threshold/sensitivity/timeout values...\n\r");

	if(thrFixed == 0x01)
	{
		sensChan1 = AD7156_ReadReg(AD7156_CH1_THRESH_HIGH);
		sensChan2 = AD7156_ReadReg(AD7156_CH2_THRESH_HIGH);
		timeOutChan1 = AD7156_ReadReg(AD7156_CH1_THRESH_LOW);
		timeOutChan2 = AD7156_ReadReg(AD7156_CH2_THRESH_LOW);
	}
	else if(thrFixed == 0x00)
	{
		threshChan1 = AD7156_BurstReadReg(AD7156_CH1_THRESH_HIGH, 2);
		threshChan2 = AD7156_BurstReadReg(AD7156_CH2_THRESH_HIGH, 2);
	}

	xil_printf("> Setting new mode parameters... \n\r");

	if(thrFixed == 0x01)
	{
		AD7156_BurstWriteReg(AD7156_CH1_THRESH_HIGH, 2, threshChan1);
		AD7156_BurstWriteReg(AD7156_CH2_THRESH_HIGH, 2, threshChan2);
	}
	else if(thrFixed == 0x00)
	{
		AD7156_WriteReg(AD7156_CH1_THRESH_HIGH, sensChan1);
		AD7156_WriteReg(AD7156_CH2_THRESH_HIGH, sensChan2);
		AD7156_WriteReg(AD7156_CH1_THRESH_LOW, timeOutChan1);
		AD7156_WriteReg(AD7156_CH2_THRESH_LOW, timeOutChan2);
	}

	xil_printf("> Parameters set according to selected threshold mode!\n\r");

	// Await new command after exiting function
	rxData = 'q';

	// Enable Interrupts
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Set threshold / sensitivity / timeout value.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7156_SetThreshold(void)
{
	char 	rx 		= 0x00;
	char 	channel = 0x00;
	char 	option 	= 0x00;
	char 	c[4] 	= "0000";
	char 	*c_ptr;
	int 	i 		= 0;
	char 	cnt 	= 0;
	char 	valid 	= 0;
	int 	value 	= 0;

	c_ptr = c;

	if(thrFixed == 0x01)
	{
		xil_printf("\n\r");
		xil_printf("Set Threshold for:\n\r");
		xil_printf("    1. Channel 1\n\r");
		xil_printf("    2. Channel 2\n\r");

		// Disable Interrupts in order to use polling mode
		DisableInterrupts(UART_INTERRUPT);

		// Check if data is available on the UART
		channel = UartReadChar();
		channel = channel - 0x30;

		xil_printf("Please enter threshold value for Channel %d: 0x", channel);

		// Maximum number of received characters will be 6 (e.g. F F F F 0x0D 0x0A)
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
			else if(rx == 'q')
			{
				i = 6;
				valid = 0;
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
				cnt += 1;
				valid = 1;
			}

			// If 4 characters entered, stop automatically
			if(cnt == 4)
			{
				i = 6;
			}

			// Increment counter
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

		if(valid == 0x01)
		{
			switch(channel)
			{
				case 0x01:
					AD7156_BurstWriteReg(AD7156_CH1_THRESH_HIGH, 2, value);
					threshChan1 = value;
					break;
				case 0x02:
					AD7156_BurstWriteReg(AD7156_CH2_THRESH_HIGH, 2, value);
					threshChan2 = value;
					break;
				default:
					break;
			}
			xil_printf("\n\r> Channel %d Threshold modified to 0x%x\n\r", channel, value);
		}
	}
	else if(thrFixed == 0x00)
	{
		xil_printf("\n\r");
		xil_printf("Select modify option:\n\r");
		xil_printf("    1. Sensitivity\n\r");
		xil_printf("    2. Timeout\n\r");

		// Disable Interrupts in order to use polling mode
		DisableInterrupts(UART_INTERRUPT);

		// Check if data is available on the UART
		option = UartReadChar();
		option = option - 0x30;

		// Display menu according to option
		if(option == 0x01)
		{
			xil_printf("\n\r");
			xil_printf("Set Sensitivity for:\n\r");
		}
		else if(option == 0x02)
		{
			xil_printf("\n\r");
			xil_printf("Set Timeout for:\n\r");
		}
		xil_printf("    1. Channel 1\n\r");
		xil_printf("    2. Channel 2\n\r");

		// Check if data is available on the UART
		channel = UartReadChar();
		channel = channel - 0x30;

		if(option == 0x01)
		{
			xil_printf("Please enter sensitivity value for Channel %d: 0x", channel);
		}
		else if(option == 0x02)
		{
			xil_printf("Please enter timeout value for Channel %d: 0x", channel);
		}

		// Maximum number of received characters will be 4 (e.g. F F 0x0D 0x0A)
		while(i < 4)
		{
			// Check if data is available on the UART
			rx = UartReadChar();
			xil_printf("%c", rx);

			// Check if pressed key is [Enter]
			if(rx == 0x0D)
			{
				i = 3;
			}
			else if(rx == 0x0A)
			{
				i = 3;
			}
			else if(rx == 'q')
			{
				i = 4;
				valid = 0;
			}
			else if(((rx > 0x00)&&(rx < 0x30))|| // Not 0 - 9
					((rx > 0x39)&&(rx < 0x41))|| // Not A - F
					((rx > 0x46)&&(rx < 0x61))|| // Not a - f
					(rx > 0x66))
			{
				xil_printf("\n\rCharacters entered must be HEX values (0 to 9 and A B C D E F)\n\r");
				i = 4;
				valid = 0;
			}
			else
			{
				*c_ptr++ = rx;
				cnt += 1;
				valid = 1;
			}

			// If 4 characters entered, stop automatically
			if(cnt == 2)
			{
				i = 4;
			}

			// Increment counter
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

		if(valid == 0x01)
		{
			if(option == 0x01)
			{
				switch(channel)
				{
					case 0x01:
						AD7156_WriteReg(AD7156_CH1_THRESH_HIGH, value);
						sensChan1 = value;
						break;
					case 0x02:
						AD7156_WriteReg(AD7156_CH2_THRESH_HIGH, value);
						sensChan2 = value;
						break;
					default:
						break;
				}
				xil_printf("\n\r> Channel %d Sensitivity modified to 0x%x\n\r", channel, value);
			}
			else if(option == 0x02)
			{
				switch(channel)
				{
					case 0x01:
						AD7156_WriteReg(AD7156_CH1_THRESH_LOW, value);
						timeOutChan1 = value;
						break;
					case 0x02:
						AD7156_WriteReg(AD7156_CH2_THRESH_LOW, value);
						timeOutChan2 = value;
						break;
					default:
						break;
				}
				xil_printf("\n\r> Channel %d TimeOutApr modified to 0x0%x\n\r", channel, ((value & 0xF0) >> 4));
				xil_printf("\n\r> Channel %d TimeOutRec modified to 0x0%x\n\r", channel, (value & 0x0F));
			}
		}
	}

	// Await new command after exiting function
	rxData = 'q';

	// Enable Interrupts
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Set input range value.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD7156_SetRange(void)
{
	char channel = 0x00;
	char option  = 0x00;


	xil_printf("\n\r");
	xil_printf("Select channel for changing its range:\n\r");
	xil_printf("    1. Channel 1\n\r");
	xil_printf("    2. Channel 2\n\r");

	// Disable Interrupts in order to use polling mode
	DisableInterrupts(UART_INTERRUPT);

	// Check if data is available on the UART
	channel = UartReadChar();
	channel = channel - 0x30;

	xil_printf("\n\r");
	xil_printf("Select range for channel %d:\n\r",channel);
	xil_printf("    1. 2pF\n\r");
	xil_printf("    2. 0.5pF\n\r");
	xil_printf("    3. 1pF\n\r");
	xil_printf("    4. 4pF\n\r");


	// Check if data is available on the UART
	option = UartReadChar();
	option = option - 0x30;


	// Modify range for selected channel
	switch(option)
	{
		case 0x01:
			AD7156_ChangeRange(0,channel);
			if (channel == 0x01)
			{
				rangeCh1 = 2;
			}
			else
			{
				rangeCh2 = 2;
			}

			xil_printf("\n\r> Channel %d Range modified to 2pF\n\r", channel);
			break;
		case 0x02:
			AD7156_ChangeRange(1,channel);
			if (channel == 0x01)
			{
				rangeCh1 = 0.5;
			}
			else
			{
				rangeCh2 = 0.5;
			}
			xil_printf("\n\r> Channel %d Range modified to 0.5pF\n\r", channel);
			break;
		case 0x03:
			AD7156_ChangeRange(2,channel);
			if (channel == 0x01)
			{
				rangeCh1 = 1;
			}
			else
			{
				rangeCh2 = 1;
			}
			xil_printf("\n\r> Channel %d Range modified to 1pF\n\r", channel);
			break;
		case 0x04:
			AD7156_ChangeRange(3,channel);
			if (channel == 0x01)
			{
				rangeCh1 = 4;
			}
			else
			{
				rangeCh2 = 4;
			}
			xil_printf("\n\r> Channel %d Range modified to 4pF\n\r", channel);
			break;
		default:
			break;
	}


	// Await new command after exiting function
	rxData = 'q';

	// Enable Interrupts
	EnableInterrupts(UART_INTERRUPT);

}

/******************************************************************************
* @brief Timer interrupt function.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADITimerIntrFunction(void)
{
	if(USE_PS7 == 1)
	{
        // Turn OFF LEDs
		GpioOut(LED_BASEADDR, 0);
        // Stop Timer
		StopTimer();
	}
	else
	{
        // Turn OFF LEDs
		GpioOut(LED_BASEADDR, 0);
        // Stop Timer
		StopTimer();
        // Clear Timer Interrupts
		Xil_Out32(TIMER_BASEADDR + 0x00, 0x00000100);
	}
}

/******************************************************************************
* @brief External interrupt function.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADIExtIntrFunction(void)
{
	int intSource = 0;

	intSource = Xil_In32(XPS_SCU_PERIPH_BASE + 0x1D08);

	if((intSource & 0x1800000) == 0x1800000)
	{
        // Turn ON LED 1 and LED2
		GpioOut(LED_BASEADDR, (3 << 19));
        // Load Timer with 1 second value
		LoadTimer(timeStepValue);
		// Start Timer
		StartTimer();
	}
	else if((intSource & 0x1000000) == 0x1000000)
	{
        // Turn ON LED 1
		GpioOut(LED_BASEADDR, (1 << 19));
        // Load Timer with 1 second value
		LoadTimer(timeStepValue);
		// Start Timer
		StartTimer();
	}
	else if((intSource & 0x800000) == 0x800000)
	{
        // Turn ON LED 2
		GpioOut(LED_BASEADDR, (2 << 19));
        // Load Timer with 1 second value
		LoadTimer(timeStepValue);
		// Start Timer
		StartTimer();
	}

}

/******************************************************************************
* @brief Demo Program for PmodCDC1.
*
* @param None.
*
* @return None.
******************************************************************************/
void DemoProgram(void)
{
	// Display Main Menu
	AD7156_DisplayMainMenu();

	while(1)
	{
		switch(rxData)
		{
			case 's':
				AD7156_DisplayCurrentSettings();
				break;
			case 'c':
				AD7156_Print(selChan);
				break;
			case 'a':
				AD7156_PrintAverage(selChan);
				break;
			case 't':
				AD7156_SetThreshold();
				break;
			case 'd':
				AD7156_SelChannels();
				break;
			case 'm':
				AD7156_SelThreshold();
				break;
			case 'r':
				AD7156_SetRange();
				break;
			case 'q':
				AD7156_DisplayMenu();
				break;
			case 0:
				break;
			default:
				xil_printf("\n\rWrong option! Please select one of the options below:");
				AD7156_DisplayMenu();
				break;
		}
	}
}
