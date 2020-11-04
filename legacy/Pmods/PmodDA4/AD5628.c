/***************************************************************************//**
*   @file   AD5628.c
*   @brief  Driver File for AD5628 Driver.
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
#include "xparameters.h"
#include "xil_io.h"
#include "AD5628.h"
#include "system_config.h"

volatile int 			rxData 			= 0;
volatile int		 	timeStepValue 	= 0xFD050F81;
volatile int 			dacStepValue 	= 0x1555;
volatile int 			dacStep 		= 0x0F;
volatile char 			dacTop 			= 0;
volatile char 			dacBot 			= 1;
volatile char 			dacSelected 	= 1;

extern u32 (*SPI_Init)(u32, char, char, char);
extern u32 (*SPI_TransferData)(u32, char, char*, char, char*, char);

/******************************************************************************
* @brief Displays Main Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_DisplayMainMenu(void)
{
	xil_printf("\n\rPmod-DA4 Demo Program\n\r");
	xil_printf("Available modes: \n\r");
	xil_printf("    [f] Fixed Value\n\r");
	xil_printf("    [w] Waveform Generation\n\r");
	xil_printf("    [p] Programmable Ramp Signal Generator\n\r");
	xil_printf("    [m] Stop the program and display this menu\n\r");
	xil_printf("Press key to select the desired mode:\n\r");
}

/******************************************************************************
* @brief Displays Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_DisplayMenu(void)
{
	xil_printf("\n\rAvailable modes: \n\r");
	xil_printf("    [f] Fixed Value\n\r");
	xil_printf("    [w] Waveform Generation\n\r");
	xil_printf("    [p] Programmable Ramp Signal Generator\n\r");
	xil_printf("    [m] Stop the program and display this menu\n\r");
	xil_printf("Press key to select the desired mode:\n\r");
	rxData = 0;
}

/******************************************************************************
* @brief Displays Fixed Value Mode Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_DisplayFixedMenu(void)
{
	xil_printf("\n\r>Fixed Value Mode Selected\n\r");
	xil_printf("Press [s] to select DAC\n\r");
	xil_printf("Press [q] to return to Main Menu\n\r");
}

/******************************************************************************
* @brief Displays DAC Selection Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_DisplayDacMenu(void)
{
	xil_printf("\n\r>DAC Selection");
	xil_printf("\n\rPlease select which DAC you wish to update:\n\r");
	xil_printf("    1. DAC A\n\r");
	xil_printf("    2. DAC B\n\r");
	xil_printf("    3. DAC C\n\r");
	xil_printf("    4. DAC D\n\r");
	xil_printf("    5. DAC E\n\r");
	xil_printf("    6. DAC F\n\r");
	xil_printf("    7. DAC G\n\r");
	xil_printf("    8. DAC H\n\r");
	xil_printf("    9. ALL DACs\n\r");
}

/******************************************************************************
* @brief Displays Waveform Generation ModeMenu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_DisplayWaveMenu(void)
{

	xil_printf("Please select between the following waveforms:\n\r");
	xil_printf("    1. Square Waveform\n\r");
	xil_printf("    2. Triangle Waveform\n\r");
	xil_printf("    3. Sawtooth Waveform\n\r");
	xil_printf("    4. Sine Waveform\n\r");
	xil_printf("    s. Select DAC\n\r");
	xil_printf("    q. Return to Main Menu\n\r");
}

/******************************************************************************
* @brief Displays Programmable Ramp Mode Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_DisplayProgRampMenu(void)
{
	xil_printf("\n\rPlease select between the following options:\n\r");
	xil_printf("    [t] Enter time step\n\r");
	xil_printf("    [i] Enter increment size\n\r");
	xil_printf("    [d] Select DAC\n\r");
	xil_printf("    [r] Run Ramp Signal Generator\n\r");
	xil_printf("Press [q] to return to Main Menu\n\r");
}

/******************************************************************************
* @brief Sets the Internal Reference on.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_RefON(void)
{
	int  txData      = 0;
	char txBuffer[4] = {0, 0, 0, 0};
	char rxBuffer[4] = {0, 0, 0, 0};

	txData = (SetUpRefReg << CommandDef) |
			 (1 << 0);

	txBuffer[0] = (txData & 0xFF000000) >> 24;
	txBuffer[1] = (txData & 0x00FF0000) >> 16;
	txBuffer[2] = (txData & 0x0000FF00) >> 8;
	txBuffer[3] = (txData & 0x000000FF);

	SPI_TransferData(SPI_BASEADDR, 4, txBuffer, 4, rxBuffer, 1);
}

/******************************************************************************
* @brief Sets the internal reference off.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_RefOFF(void)
{
	int  txData      = 0;
	char txBuffer[4] = {0, 0, 0, 0};
	char rxBuffer[4] = {0, 0, 0, 0};

	txData = (SetUpRefReg << CommandDef) |
			 (0 << 0);

	txBuffer[0] = (txData & 0xFF000000) >> 24;
	txBuffer[1] = (txData & 0x00FF0000) >> 16;
	txBuffer[2] = (txData & 0x0000FF00) >> 8;
	txBuffer[3] = (txData & 0x000000FF);

	SPI_TransferData(SPI_BASEADDR, 4, txBuffer, 4, rxBuffer, 1);
}

/******************************************************************************
* @brief Updates the DAC.
*
* @param n - Select which DAC to Update.
*
* @param data - Value to be updated
*
* @return None.
******************************************************************************/
void AD5628_WriteUpdateDac(int n, int data)
{
	int  txData      = 0;
	char txBuffer[4] = {0, 0, 0, 0};
	char rxBuffer[4] = {0, 0, 0, 0};

	txData = (WriteUpdateDACn << CommandDef) |
			 (n << AddressCom) |
			 (data << DataBits);

	txBuffer[0] = (txData & 0xFF000000) >> 24;
	txBuffer[1] = (txData & 0x00FF0000) >> 16;
	txBuffer[2] = (txData & 0x0000FF00) >> 8;
	txBuffer[3] = (txData & 0x000000FF);

	SPI_TransferData(SPI_BASEADDR, 4, txBuffer, 4, rxBuffer, 1);
}

/******************************************************************************
* @brief Fixed Value Mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_FixedValue(void)
{
	char rx     = 's';
	char c[3]   = "000";
	char *c_ptr;
	int  i      = 0;
	char cnt    = 0;
	char valid  = 0;
	char dacSel = 0;
	int  value  = 0;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	AD5628_DisplayFixedMenu();

	while(rx != 'q')
	{
		cnt   = 0;
		i     = 0;
		valid = 0;
		value = 0;
		c_ptr = c;

		if(rx == 's')
		{
			xil_printf("\n\r");
			AD5628_DisplayDacMenu();
			// Check if data is available on UART
			rx = UartReadChar();
			if(rx != 'q')
			{
				if(rx == '9')
				{
					dacSel = 0x0F;
				}
				else
				{
					dacSel = (rx - 0x31);
				}
				
				switch(dacSel)
				{
					case 0:
						xil_printf("\n\r>You have chosen to update ");
						xil_printf("DAC A.\n\r");
						break;
					case 1:
						xil_printf("\n\r>You have chosen to update ");
						xil_printf("DAC B.\n\r");
						break;
					case 2:
						xil_printf("\n\r>You have chosen to update ");
						xil_printf("DAC C.\n\r");
						break;
					case 3:
						xil_printf("\n\r>You have chosen to update ");
						xil_printf("DAC D.\n\r");
						break;
					case 4:
						xil_printf("\n\r>You have chosen to update ");
						xil_printf("DAC E.\n\r");
						break;
					case 5:
						xil_printf("\n\r>You have chosen to update ");
						xil_printf("DAC F.\n\r");
						break;
					case 6:
						xil_printf("\n\r>You have chosen to update ");
						xil_printf("DAC G.\n\r");
						break;
					case 7:
						xil_printf("\n\r>You have chosen to update ");
						xil_printf("DAC H.\n\r");
						break;
					case 15:
						xil_printf("\n\r>You have chosen to update ");
						xil_printf("All DACs.\n\r");
						break;
					case ('s'-0x31):
						i=5;
						valid = 0;
						break;
					default:
						xil_printf("Wrong option!");
						rx = 's';
						i=5;
						valid = 0;
						break;
				}
			}
			else
			{
				i = 5;
				valid = 0;
			}
		}

		if(i == 0)
		{
			xil_printf("\n\rPlease enter a value between 0x000 and 0xFFF: 0x");
		}
		// Maximum number of received characters will be 5 (e.g. F F F 0x0D 0x0A)
		while(i < 5)
		{
			// Check if data is available on UART
			rx = UartReadChar();
			xil_printf("%c", rx);
			// Check if pressed key is [Enter]
			if(rx == 0x0D)
			{
				i = 4;
			}
			else if(rx == 0x0A)
			{
				i = 4;
			}
			else if(rx == 'q')
			{
				i = 5;
				valid = 0;
			}
			else if(rx == 's')
			{
				i = 5;
				valid = 0;
			}
			else if(((rx > 0x00)&&(rx < 0x30))|| // Not 0 - 9
					((rx > 0x39)&&(rx < 0x41))|| // Not A - F
					((rx > 0x46)&&(rx < 0x61))|| // Not a - f
					(rx > 0x66))
			{
				xil_printf("\n\rCharacters entered must be HEX values (0 to 9 and A B C D E F)");
				i = 5;
				valid = 0;
			}
			else
			{
				*c_ptr++ = rx;
				cnt = cnt + 1;
				valid = 1;
			}
			if(cnt == 3)
			{
				i = 5;
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
			AD5628_WriteUpdateDac(dacSel, value);
			xil_printf("\n\r>DAC Output modified\n\r");
		}
	}
	rxData = 'm';
	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
	xil_printf("\n\r\n\r>Returning to Menu\n\r");
}

/******************************************************************************
* @brief Waveform Generation Mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_WaveGen(void)
{
	int  prevRxData = 0;
	int  i          = 0;
	char valid      = 1;
	char rx         = 's';
	char dacSel     = 0;
	int  repeat     = 1;
	

	//AD5628_DisplayWaveMenu();
	rxData = 's';

	xil_printf("\n\r>Waveform Generation Mode Selected\n\r");
	while(valid)
	{
		repeat = 1;
		switch(rxData)
		{
		case '1':
			AD5628_WriteUpdateDac(dacSel, (squaretable[i] << 4));
			i+=1;
			break;
		case '2':
			AD5628_WriteUpdateDac(dacSel, (triangletable[i] << 4));
			i+=1;
			break;
		case '3':
			AD5628_WriteUpdateDac(dacSel, (sawtable[i] << 4));
			i+=1;
			break;
		case '4':
			AD5628_WriteUpdateDac(dacSel, (sinetable[i] << 4));
			i+=1;
			break;
		case 's':
			break;
		case 'q':
			xil_printf("\n\r>Returning to Menu...\n\r");
			valid = 0;
			rxData = 'm';
			break;
		case 'm':
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
			case 's':
				while (repeat == 1)
				{
				AD5628_DisplayDacMenu();
				DisableInterrupts(UART_INTERRUPT);
				// Check if data is available on UART
				rx = UartReadChar();
				if(rx != 'q')
				{
					if(rx == '9')
					{
						dacSel = 0x0F;
					}
					else
					{
						dacSel = (rx - 0x31);
					}
					repeat = 0;
					switch(dacSel)
					{
						case 0:
							xil_printf("\n\r>You have chosen to update ");
							xil_printf("DAC A.\n\r");
							break;
						case 1:
							xil_printf("\n\r>You have chosen to update ");
							xil_printf("DAC B.\n\r");
							break;
						case 2:
							xil_printf("\n\r>You have chosen to update ");
							xil_printf("DAC C.\n\r");
							break;
						case 3:
							xil_printf("\n\r>You have chosen to update ");
							xil_printf("DAC D.\n\r");
							break;
						case 4:
							xil_printf("\n\r>You have chosen to update ");
							xil_printf("DAC E.\n\r");
							break;
						case 5:
							xil_printf("\n\r>You have chosen to update ");
							xil_printf("DAC F.\n\r");
							break;
						case 6:
							xil_printf("\n\r>You have chosen to update ");
							xil_printf("DAC G.\n\r");
							break;
						case 7:
							xil_printf("\n\r>You have chosen to update ");
							xil_printf("DAC H.\n\r");
							break;
						case 15:
							xil_printf("\n\r>You have chosen to update ");
							xil_printf("All DACs.\n\r");
							break;
						default:
							xil_printf("Wrong option!\n\r");
							//xil_printf("\n\r>Returning to DAC Selection...\n\r");
							repeat = 1;
							break;
					}
				}
				else
				{
					valid = 0;
				}
				if ( (((dacSel >= 0) && (dacSel <= 7)) || (dacSel == 15)) && (valid != 0) )
				{
					AD5628_DisplayWaveMenu();
					xil_printf("Press [1] to [4] in order to select the desired output, or [q] to exit\n\r");
				}
				EnableInterrupts(UART_INTERRUPT);
				}
				break;
			case 'q':
				xil_printf("\n\r>Returning to Menu...\n\r");
				valid = 0;
				rxData = 'm';
				break;
			default:
				if (valid != 0)
				{
					xil_printf("Wrong option!\n\r");
					AD5628_DisplayWaveMenu();
					xil_printf("Press [1] to [4] in order to select the desired output, or [q] to exit\n\r");
				}
				break;
			}
		}
		if(i > 255)
		{
		   i = 0;
		}
	}

}

/******************************************************************************
* @brief Enters Time Step Value Set Mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_EnterTimeStep(void)
{
	char 				rx       = 0;
	char 				c[4]     = "0000";
	char 				*c_ptr;
	int  				i        = 0;
	char 				cnt      = 0;
	char 				valid    = 0;
	int  				value    = 0;
	int                 topValue = 4294967295;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	while(valid == 0)
	{
		c_ptr = c;
		i = 0;
		cnt = 0;
		value = 0;

		xil_printf("\n\rPlease enter a value between 100 and 5000 [ms]: ");

		// Maximum number of received characters will be 6 (e.g. 5 0 0 0 0x0D 0x0A)
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
			else if(((rx > 0x00)&&(rx < 0x30))||(rx > 0x39))
			{
				xil_printf("\n\rCharacters must be digits [0] to [9]\n\r");
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
		// Translate from ASCII to dec
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

	rxData ='p';
}

/******************************************************************************
* @brief Enters Increment Step Value Set Mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_EnterIncrStep(void)
{
	char rx    = 0;
	char c[3]  = "000";
	char *c_ptr;
	int  i     = 0;
	char cnt   = 0;
	char valid = 0;
	int  value = 0;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	xil_printf("\n\rPlease enter a value between 0x000 and 0xFFF: 0x");

	c_ptr = c;

	// Maximum number of received characters will be 5 (e.g. 5 0 F 0x0D 0x0A)
	while(i < 5)
	{
		// Check if data is available on UART
		rx = UartReadChar();
		xil_printf("%c", rx);
		// Check if pressed key is [Enter]
		if(rx == 0x0D)
		{
			i = 4;
		}
		else if(rx == 0x0A)
		{
			i = 4;
		}
		else if(((rx > 0x00)&&(rx < 0x30))|| // Not 0 - 9
				((rx > 0x39)&&(rx < 0x41))|| // Not A - F
				((rx > 0x46)&&(rx < 0x61))|| // Not a - f
				(rx > 0x66))
		{
			xil_printf("\n\rCharacters entered must be HEX values (0 to 9 and A B C D E F)");
			i = 5;
			valid = 0;
		}
		else
		{
			*c_ptr ++= rx;
			cnt++;
			valid = 1;
		}

		if(cnt == 3)
		{
			i = 5;
		}
		i++;
	}
		xil_printf("\n\r");
		// Translate from ASCII to dec
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
			xil_printf(">Incremented size modified to 0x%x", dacStepValue);
			xil_printf("\n\r");
		}


	rxData ='p';
}

/******************************************************************************
* @brief Runs Programmable Ramp Generation Mode.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_RunProgRamp(void)
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
	rxData ='m';
}

/******************************************************************************
* @brief Runs Programmable Ramp Generation Mode DAC Selection Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_RampDacSel(void)
{
	char rx = 0x00;


	do
	{
	AD5628_DisplayDacMenu();
	// Check if data is available on UART
	rx = UartReadChar();
	if(rx != 'q')
	{
		if(rx == '9')
		{
			dacSelected = 0x0F;
		}
		else
		{
			dacSelected = (rx - 0x31);
		}
	
		switch(dacSelected)
		{
			case 0:
				xil_printf("\n\r>You have chosen to update ");
				xil_printf("DAC A.\n\r");
				break;
			case 1:
				xil_printf("\n\r>You have chosen to update ");
				xil_printf("DAC B.\n\r");
				break;
			case 2:
				xil_printf("\n\r>You have chosen to update ");	
				xil_printf("DAC C.\n\r");
				break;
			case 3:
				xil_printf("\n\r>You have chosen to update ");
				xil_printf("DAC D.\n\r");
				break;
			case 4:
				xil_printf("\n\r>You have chosen to update ");
				xil_printf("DAC E.\n\r");
				break;
			case 5:
				xil_printf("\n\r>You have chosen to update ");
				xil_printf("DAC F.\n\r");
				break;
			case 6:
				xil_printf("\n\r>You have chosen to update ");
				xil_printf("DAC G.\n\r");
				break;
			case 7:
				xil_printf("\n\r>You have chosen to update ");
				xil_printf("DAC H.\n\r");
				break;
			case 15:
				xil_printf("\n\r>You have chosen to update ");
				xil_printf("All DACs.\n\r");
				break;
			default:
				xil_printf("\n\rWrong option!\n\r");

				break;
		}
	}
	}
	while (!(((dacSelected >=0) && (dacSelected <=7)) || (dacSelected == 15)));

}

/******************************************************************************
* @brief Runs Programmable Ramp Generation Mode Main Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5628_ProgRamp(void)
{
	DisableInterrupts(UART_INTERRUPT);
	AD5628_DisplayProgRampMenu();

	// Check if data is available on the UART
	rxData = UartReadChar();

	switch(rxData)
	{
		case 't':
			AD5628_EnterTimeStep();
			break;
		case 'i':
			AD5628_EnterIncrStep();
			break;
		case 'd':
			AD5628_RampDacSel();
			break;
		case 'r':
			AD5628_RunProgRamp();
			break;
		case 'q':
			xil_printf("\n\r>Returning to Menu...\n\r");
			rxData = 'm';
			break;
		default:
			xil_printf("\n\rWrong option!");
			rxData = 0;
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
	if((dacStep > 0xFFF) && (dacTop == 0))
	{
		dacTop = 1;
		dacBot = 0;
	}
	if((dacStep < 0x000) && (dacBot == 0))
	{
		dacTop = 0;
		dacBot = 1;
	}
	if(dacStep > 0xFFF)
	{
		dacStep = 0xFFF;
	}
	if(dacStep < 0x000)
	{
		dacStep = 0x000;
	}

	if(dacStep != prevDacStep)
	{
		AD5628_WriteUpdateDac(dacSelected, dacStep);
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
	AD5628_DisplayMainMenu();

	while(1)
	{
		switch(rxData)
		{
			case 'f':
				AD5628_FixedValue();
				break;
			case 'w':
				AD5628_WaveGen();
				break;
			case 'p':
				AD5628_ProgRamp();
				break;
			case 'm':
				AD5628_DisplayMenu();
				break;
			case 0:
				break;
			default:
				xil_printf("\n\rWrong option! Please select one of the options below.");
				AD5628_DisplayMenu();
				break;
		}
	}
}
