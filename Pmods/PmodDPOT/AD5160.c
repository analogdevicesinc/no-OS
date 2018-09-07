/**************************************************************************//**
*   @file   ad5160.c
*   @brief  Implementation of AD5160 Driver for Microblaze processor.
*   @author ATofan (alexandru.tofan@analog.com)
*
*******************************************************************************
* Copyright 2012(c) Analog Devices, Inc.
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
*   SVN Revision: 468
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "xparameters.h"
#include "system_config.h"
#include "xil_io.h"
#include "ad5160.h"

volatile int rxData;

#if(USE_PS7 == 1)
	extern XScuGic IntcInstance;
#endif

extern u32 (*SPI_Init)(u32, char, char, char);
extern u32 (*SPI_TransferData)(u32, char, char*, char, char*, char);

static int rab = 10000; // Resistance between A and B on the PmodDPOT => Change with measured value
static int rwp = 60;	// Wiper resistance (measured between B and W when D = 0) => Change with measured value

/******************************************************************************
* @brief Displays Main Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5160_DisplayMainMenu(void)
{
	xil_printf("\n\rPmod-DPOT Demo Program\n\r");
	xil_printf("The software is calibrated for the following values: \n\r");
	xil_printf("    Rab = %d ohms\n\r", rab);
	xil_printf("    Rwp = %d ohms\n\r", rwp);
	xil_printf("These value may differ for your PmodDPOT\n\r");
	xil_printf("\n\r");
	xil_printf("Available options: \n\r");
	xil_printf("    [d] Set PmodDPOT division factor\n\r");
	xil_printf("    [c] Calibrate PmodDPOT\n\r");
	xil_printf("    [m] Display this menu\n\r");
	xil_printf("Press key to select the desired option\n\r");
}

/******************************************************************************
* @brief Displays Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5160_DisplayMenu(void)
{
	xil_printf("\n\rAvailable options: \n\r");
	xil_printf("    [d] Set PmodDPOT division factor\n\r");
	xil_printf("    [c] Calibrate PmodDPOT\n\r");
	xil_printf("    [m] Display this menu\n\r");
	rxData = 0;
}

/******************************************************************************
* @brief Sets AD5160 resistors value.
*
* @param None.
*
* @return retValue - Value of programmed resistor value.
******************************************************************************/
int AD5160_Set(void)
{
	char  rx    = 0;	            // Character received from UART
	char  c[]         = "000";      // Variable in which we store received data
	char  *c_ptr;                   // Pointer to c[]
	int   i           = 0;			// Used to keep track of how many characters we have received from UART
	int   cnt         = 0;		    // Number of received characters from UART
	char  valid       = 0;		    // Validates correct input
	int   value       = 0;		    // Value between 0 and 255 to be sent to the Pmod-DPOT
	float rwb;			            // Resistance between W and B on the Pmod-DPOT
	float rwa;			            // Resistance between W and A on the Pmod-DPOT
	int   whole;			        // Used to send float Values on the UART
    int   retValue    = 0;          // Value returned by function
    char  txBuffer[1] = {0x00};
    char  rxBuffer[1] = {0x00};

	c_ptr = c;

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	// Maximum number of received characters will be 4 (e.g. F F 0x0D 0x0A)
	while(rx != 'q')
	{
		cnt   = 0;
		i     = 0;
		valid = 0;
		value = 0;
		c_ptr = c;

		if(i == 0)
		{
			xil_printf("\n\rPlease enter a value between 0x00 and 0xFF or [q] to return to menu: 0x");
		}
		while(i < 4)
		{
			rx = UartReadChar();
			xil_printf("%c",rx);

			// Check if key pressed is [Enter]
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
			cnt = cnt + 1;
			valid = 1;
			}
			if(cnt == 2)
			{
				i = 4;
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
			// Calculated according to Datasheet. rab and rwp measured on the Pmod using multimeter
			// Please replace rab and rwp with your own measured values (see beginning of AD5160.c)
			rwb   = (float)((value * rab) / 256) + rwp;
			whole = (int)rwb;
			xil_printf("\n\r  >>Rwb = %d ohms\n\r",whole);

			rwa   = (float)(((256 - value) * rab)/256) + rwp;
			whole = (int)rwa;
			xil_printf("  >>Rwa = %d ohms\n\r",whole);

			retValue = value;

			txBuffer[0] = value;
			SPI_TransferData(SPI_BASEADDR, 1, txBuffer, 1, rxBuffer, 1);
		}
	}
	rxData = 'm';
	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
	return(retValue);
}

/******************************************************************************
* @brief Calibrate value for Rab.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5160_CalibrateRab(void)
{
	char rx    = 0;			    // Character received from UART
	char c[]   = "00000";		// Variable in which we store received data
	char *c_ptr;			    // Pointer to c[]
	int  i     = 0;				// Used to keep track of how many characters we have received from UART
	char cnt   = 0;			    // Number of received characters from UART
	char valid = 0;			    // Validates correct input
	int  value = 0;			    // Value between 0 and 255 to be sent to the Pmod-DPOT

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	while(!valid)
	{
		xil_printf("\n\rPlease measure the resistance between A and B. Enter the value in ohms: ");
		cnt   = 0;
		i     = 0;
		value = 0;
		c_ptr = c;
		// Maximum number of received characters will be 7 (e.g. 1 0 2 7 0 0x0A 0x0D)
		while(i < 7)
		{
			// Check if data is available on the UART
			rx = UartReadChar();
			xil_printf("%c",rx);
			// Check if pressed key is [Enter]
			if(rx == 0x0D)
			{
				i = 6;
			}
			else if(rx == 0x0A)
			{
				i = 6;
			}
			else if(((rx > 0x00)&&(rx < 0x30)) || ((rx > 0x39))) // Not 0 - 9
			{
				xil_printf("\n\rCharacters entered must be valid numbers (0 to 9)");
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
				i = 7;
			}
			i++;
		}

		// Translate from ASCII to decimal
		for(i = 0; i < cnt; i++)
		{
			value = value * 10 + (c[i] - 0x30);
		}

		if(valid == 1)
		{
			rab = value;
			xil_printf("\n\rNew value stored for Rab: %d\n\r", rab);
		}
		else
		{
			xil_printf("\n\rValue entered is not valid.\n\r");
		}
	}
	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
}

/******************************************************************************
* @brief Calibrate value for Rwiper.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5160_CalibrateRwp(void)
{
	char rx          = 0;			    // Character received from UART
	char c[]         = "000";		    // Variable in which we store received data
	char *c_ptr;			            // Pointer to c[]
	int  i           = 0;				// Used to keep track of how many characters we have received from UART
	char cnt         = 0;			    // Number of received characters from UART
	char valid       = 0;			    // Validates correct input
	int  value       = 0;			    // Value between 0 and 255 to be sent to the Pmod-DPOT
	char txBuffer[1] = {0x00};
	char rxBuffer[1] = {0x00};

	// Disable interrupts in order to use normal polling
	DisableInterrupts(UART_INTERRUPT);

	// Send 0x00 to PmodDPOT in order to measure Rwp between W and B
	SPI_TransferData(SPI_BASEADDR, 1, txBuffer, 1, rxBuffer, 1);

	while(!valid)
	{
		xil_printf("\n\rPlease measure the resistance between W and B. Enter the value in ohms: ");
		c_ptr = c;
		cnt   = 0;
		i     = 0;
		value = 0;

		// Maximum number of received characters will be 5 (e.g. 1 1 7 0x0A 0x0D)
		while(i < 5)
		{
			// Check if data is available on the UART
			rx = UartReadChar();
			xil_printf("%c",rx);
			// Check if pressed key is [Enter]
			if(rx == 0x0D)
			{
				i = 4;
			}
			else if(rx == 0x0A)
			{
				i = 4;
			}
			else if(((rx > 0x00)&&(rx < 0x30)) || ((rx > 0x39))) // Not 0 - 9
			{
				xil_printf("\n\rCharacters entered must be valid numbers (0 to 9)");
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

		// Translate from ASCII to decimal
		for(i = 0; i < cnt; i++)
		{
			value = value * 10 + (c[i] - 0x30);
		}

		if(valid == 1)
		{
			rwp = value;
			xil_printf("\n\rNew value stored for Rwp: %d\n\r", rwp);
		}
		else
		{
			xil_printf("\n\rValue entered is not valid.\n\r");
		}
	}
	rxData = 'm';
	// Enable interrupts
	EnableInterrupts(UART_INTERRUPT);
	xil_printf("\n\r>Returning to Menu...\n\r");
}

/******************************************************************************
* @brief Launch demo application for AD5160
*
* @param None.
*
* @return None.
******************************************************************************/
void DemoProgram(void)
{
	AD5160_DisplayMainMenu();

	while(1)
	{
		switch(rxData)
		{
			case 'd':
				AD5160_Set();
				break;
			case 'c':
				AD5160_CalibrateRab();
				AD5160_CalibrateRwp();
				break;
			case 'm':
				AD5160_DisplayMenu();
				break;
			case 0:
				break;
			default:
				xil_printf("\n\rWrong option! Please select one of the options below.");
				AD5160_DisplayMenu();
				break;
		}
	}
}
