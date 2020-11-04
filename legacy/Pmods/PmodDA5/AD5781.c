/**************************************************************************//**
*   @file   AD5781.c
*   @brief  Source file for AD5781 driver.
*   @author ATofan (alexandru.tofan@analog.com)
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
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdio.h>
#include "xparameters.h"
#include "xil_io.h"
#include "system_config.h"
#include "AD5781.h"

/*****************************************************************************/
/***************************** Variables Definitions *************************/
/*****************************************************************************/
volatile int  rxData			= 0;
volatile int  timeStepValue		= 0xFD050F81;
volatile int  dacStepValue		= 0x1555;
volatile int  dacStep			= 0x0f;
volatile char dacTop			= 0;
volatile char dacBot			= 1;

extern u32 (*SPI_Init)(u32, char, char, char);
extern u32 (*SPI_TransferData)(u32, char, char*, char, char*, char);

/*****************************************************************************/
/***************************** Functions Definitions *************************/
/*****************************************************************************/

/******************************************************************************
* @brief Displays Main Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_DisplayMainMenu(void)
{
	xil_printf("\n\rPmod-DA5 Demo Program\n\r");
	xil_printf("Available modes: \n\r");
	xil_printf("   [f] Fixed Value\n\r");
	xil_printf("   [w] Waveform Generation\n\r");
	xil_printf("   [p] Programmable Ramp Signal Generator\n\r");
	xil_printf("   [m] Stop the program and display this menu\n\r");
	xil_printf("Press key to select the desired mode:\n\r");

}

/******************************************************************************
* @brief Displays Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_DisplayMenu(void)
{
	xil_printf("\n\rAvailable modes: \n\r");
	xil_printf("   [f] Fixed Value\n\r");
	xil_printf("   [w] Waveform Generation\n\r");
	xil_printf("   [p] Programmable Ramp Signal Generator\n\r");
	xil_printf("   [m] Stop the program and display this menu\n\r");
	xil_printf("Press key to select the desired mode:\n\r");
	rxData = 0;
}

/******************************************************************************
* @brief Displays Waveform Generation Mode Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_DisplayWaveMenu(void)
{
	xil_printf("\n\rPlease select between the following waveforms:\n\r");
	xil_printf("    1. Square Waveform\n\r");
	xil_printf("    2. Triangle Waveform\n\r");
	xil_printf("    3. Sawtooth Waveform\n\r");
	xil_printf("    4. Sine Waveform\n\r");
	xil_printf("    q. Return to Main Menu\n\r");
}

/******************************************************************************
* @brief Displays Fixed Value Mode Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_DisplayFixedMenu(void)
{
	xil_printf("\n\r>Fixed Value Mode Selected\n\r");
	xil_printf("Press [q] to return to Main Menu\n\r");
}

/******************************************************************************
* @brief Displays Programmable Ramp Mode Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_DisplayProgRampMenu(void)
{
	xil_printf("\n\rPlease select between the following options:\n\r");
	xil_printf("    [t] Enter time step\n\r");
	xil_printf("    [i] Enter increment size\n\r");
	xil_printf("    [r] Run Ramp Signal Generator\n\r");
	xil_printf("Press [q] to return to Main Menu\n\r");
}

/******************************************************************************
* @brief Sends data to the AD5781
*
* @param txData - Data to be sent.
*
* @return None.
******************************************************************************/
void AD5781_SendData(int txData)
{
	char txBuffer[3] = {0, 0, 0};
	char rxBuffer[3] = {0, 0, 0};
	int cfgValue = txData;

	txBuffer[0] = ((cfgValue & 0xFF0000) >> 16);
	txBuffer[1] = ((cfgValue & 0xFF00) >> 8);
	txBuffer[2] = (cfgValue & 0xFF);
	SPI_TransferData(SPI_BASEADDR, 3, txBuffer, 3, rxBuffer, 1);

}

/******************************************************************************
* @brief Configure the AD5871 to put it in normal operating mode
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_InitDAC(void)
{
	int configValue = 0;

	configValue = ((configValue | DAC_CNT) << 20);  /* Set up Control Register address to write */
	configValue = (configValue | 0x12);             /* Clear the DACTRI and OPGND bits, DAC register uses offset binary coding */
	AD5781_SendData(configValue);
}

/******************************************************************************
* @brief Enters Fixed Generation Mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_FixedValue(void)
{
	char rx    = 0;
	char c[5]  = "00000";
	char *c_ptr;
	int  i     = 0;
	char cnt   = 0;
	char valid = 0;
	int  value = 0;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	AD5781_DisplayFixedMenu();

	// Maximum number of received characters will be 6 (e.g. F F F F 0x0D 0x0A)
	while(rx != 'q')
	{
		cnt   = 0;
		i     = 0;
		valid = 0;
		value = 0;
		c_ptr = c;

		xil_printf("\n\rPlease enter a value between 0x00000 and 0x3ffff: 0x");
		while(i < 7)
		{
			// Check if data is available on the UART
			rx = UartReadChar();
			xil_printf("%c", rx);
			// Check if pressed key is [Enter] : CR or NL
			if((rx == 0x0D) || (rx == 0x0A))
			{
					i = 6;
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
				xil_printf("\n\rCharacters entered must be HEX values (0 to 9 and A B C D E F)");
				i = 7;
				valid = 0;
			}
			else
			{
				*c_ptr++ = rx;
				cnt = cnt + 1;
				valid = 1;
			}

		    if(cnt == 5)
			{
				if(((c[0] > 0x33) || (c[0] < 0x30)) && (cnt == 5))
				{
					xil_printf("\n\rThe value entered must be between 0x00000 and 0x3ffff\n\r");
					valid = 0;
					i = 7;
				}
			    i = 7;
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
			AD5781_SendData(REGWR(value));
			AD5781_SendData(UPDATE_DAC_REG); //The output is set according the DAC register value
			xil_printf("\n\r>DAC Output modified\n\r");
		}
	}

	rxData = 'm';
	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
	xil_printf("\n\r>Returning to Menu...\n\r");
}

/******************************************************************************
* @brief Enters Waveform Generation Mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_WaveGen(void)
{
	int  prevRxData = 0;
	int  i          = 0;
	char valid      = 1;
	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	AD5781_DisplayWaveMenu();

	// Check if data is available on the UART
	rxData = UartReadChar();

	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);

	while(valid)
	{
		switch(rxData)
		{
		case '1':
			AD5781_SendData(REGWR(squaretable[i] << 8));
			AD5781_SendData(UPDATE_DAC_REG);
			i+=1;
			break;
		case '2':
			AD5781_SendData(REGWR(triangletable[i] << 8));
			AD5781_SendData(UPDATE_DAC_REG);
			i+=1;
			break;
		case '3':
			AD5781_SendData(REGWR(sawtable[i] << 8));
			AD5781_SendData(UPDATE_DAC_REG);
			i+=1;
			break;
		case '4':
			AD5781_SendData(REGWR(sinetable[i] << 8));
			AD5781_SendData(UPDATE_DAC_REG);
			i+=1;
			break;
		case 'q':
			valid = 0;
			break;
		default:
			break;
		}
		if(rxData != prevRxData)
		{
			prevRxData = rxData;
			switch(rxData)
			{
			case '1':
				xil_printf("\n\r>Square waveform selected\n\r");
				break;
			case '2':
				xil_printf("\n\r>Triangle waveform selected\n\r");
				break;
			case '3':
				xil_printf("\n\r>Sawtooth waveform selected\n\r");
				break;
			case '4':
				xil_printf("\n\r>Sine waveform selected\n\r");
				break;
			case 'q':
				xil_printf("\n\r>Returning to Menu\n\r");
				break;
			default:
				xil_printf("Press [1] to [4] in order to select the desired output, or [q] to exit\n\r");
				break;
			}
		}
		if(i > 255)
		{
		   i = 0;
		}
	}
	rxData = 'm';
}

/******************************************************************************
* @brief Enters Time Step Value Set Mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_EnterTimeStep(void)
{
	char 				  rx    = 0;
	char 				  c[5]  = "00000";
	char 				  *c_ptr;
	int 				  i     = 0;
	char 				  cnt   = 0;
	char 				  valid = 0;
	int 				  value = 0;
	int 		          topValue = 0xFFFFFFFF;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	while(valid == 0)
	{
		c_ptr = c;
		i     = 0;
		cnt   = 0;
		value = 0;

		xil_printf("\n\rPlease enter a value between 100 and 5000 [ms]: ");

		// Maximum number of received characters will be 6 (e.g. 5 0 0 0 0x0D 0x0A)
		while(i < 7)
		{
			// Check if data is available on UART
			rx = UartReadChar();
			xil_printf("%c", rx);
			// Check if pressed key is [Enter]
			if((rx == 0x0D) || (rx == 0x0A))
			{
				i = 6;
			}
			else if(((rx > 0x00) && (rx < 0x30)) || (rx > 0x39))
			{
				xil_printf("\n\rCharacters must be digits [0] to [9]\n\r");
				i = 7;
				valid = 0;
			}
			else
			{
				*c_ptr ++= rx;
				cnt++;
				valid = 1;
			}

			if(cnt == 4)
			{
				i = 6;
			}
			i++;
		}
		xil_printf("\n\r");
		// Translate from ASCII to decimal
		for(i = 0; i < cnt; i++)
		{
			value = value * 10 + (c[i] - 0x30);
		}

		if((value > 5000)||(value < 100))
		{
			xil_printf("\n\r> Incorrect value entered. Value must be between 100 and 5000.\n\r");
			valid = 0;
		}
	}

	if(valid == 1)
	{
		if(USE_PS7 == 1){
			timeStepValue = ((float)6781 / 1000) * value + 1;
		}
		else{
			timeStepValue = (topValue - (value * 100000)) + 2;
		}
	}

}

/******************************************************************************
* @brief Runs Programmable Ramp Generation Mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_RunProgRamp(void)
{
	xil_printf("\n\r>Running. Press [q] to return to the Main Menu\n\r");

	EnableInterrupts(UART_INTERRUPT);

	// Start Timer
	StartTimer();

	while(rxData != 'q')
	{
	}

    // Stop Timer
	StopTimer();

	DisableInterrupts(UART_INTERRUPT);

	rxData = 'm';
}

/******************************************************************************
* @brief Enters Increment Step Value Set Mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_EnterIncrStep(void)
{
	char rx    = 0;
	char c[4]  = "0000";
	char *c_ptr;
	int  i     = 0;
	char cnt   = 0;
	char valid = 0;
	int  value = 0;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	xil_printf("\n\rPlease enter a value between 0x0000 and 0xFFFF: 0x");

	c_ptr = c;

	// Maximum number of received characters will be 6 (e.g. 5 0 0 F 0x0D 0x0A)
	while(i < 6)
	{
		// Check if data is available on UART
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
			xil_printf("\n\rCharacters entered must be HEX values (0 to 9 and A B C D E F)");
			i = 6;
			valid = 0;
		}
		else
		{
			*c_ptr ++= rx;
			cnt++;
			valid = 1;
		}

		if(cnt == 4)
		{
			i = 6;
		}
		i++;
	}
		xil_printf("\n\r");
		// Translate from ASCII to decimal
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
			dacStepValue = value;
			xil_printf("Increment size value modified to 0x%x\n\r", dacStepValue);
		}

}

/******************************************************************************
* @brief Runs Programmable Ramp Generation Mode Main Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5781_ProgRamp(void)
{
	DisableInterrupts(UART_INTERRUPT);
	AD5781_DisplayProgRampMenu();

	// Check if data is available on the UART
	rxData = UartReadChar();

	switch(rxData)
	{
		case 't':
			AD5781_EnterTimeStep();
			break;
		case 'i':
			AD5781_EnterIncrStep();
			break;
		case 'r':
			AD5781_RunProgRamp();
			break;
		case 'q':
			rxData = 'm';
		default:
			xil_printf("\n\rWrong option! Please select one of the options below.");
			break;
	}

	if(rxData != 'm')
	{
		rxData = 'p';
	}
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Timer interrupt function. Loads new data into DAC at desired time step
*
* @param None.
*
* @return None.
******************************************************************************/
void ADITimerIntrFunction(void)
{
	int prevDacStep = dacStep;
	LoadTimer(timeStepValue);
	// Increase output data value
	if(dacBot == 1)
	{
		dacStep = dacStep + dacStepValue;
	}
	else if(dacTop == 1)
	{
		dacStep = dacStep - dacStepValue;
	}
	if((dacStep > 0xFFFF) && (dacTop == 0))
	{
		dacTop = 1;
		dacBot = 0;
	}
	if((dacStep < 0x0000) && (dacBot == 0))
	{
		dacTop = 0;
		dacBot = 1;
	}
	if(dacStep > 0xFFFF)
	{
		dacStep = 0xFFFF;
	}
	if(dacStep < 0x0000)
	{
		dacStep = 0x0000;
	}

	if(dacStep != prevDacStep)
	{
		AD5781_SendData(REGWR(dacStep));
		AD5781_SendData(UPDATE_DAC_REG);
	}

	if(USE_PS7 == 0)
	{
		// Clear Interrupts
		Xil_Out32(TIMER_BASEADDR + 0x00, 0x00000100);
		// Start Timer
		StartTimer();
	}
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
	AD5781_DisplayMainMenu();

	while(1)
	{
		switch(rxData)
		{
			case 'f':
				AD5781_FixedValue();
				break;
			case 'w':
				AD5781_WaveGen();
				break;
			case 'p':
				AD5781_ProgRamp();
				break;
			case 'm':
				AD5781_DisplayMenu();
				break;
			case 0:
				break;
			default:
				xil_printf("\n\rWrong option! Please select one of the options below.");
				AD5781_DisplayMenu();
				break;
		}
	}
}
