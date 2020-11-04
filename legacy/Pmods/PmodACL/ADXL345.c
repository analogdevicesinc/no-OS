/***************************************************************************//**
 *   @file   ADXL345.c
 *   @brief  Implementation of ADXL345 Driver.
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
#include <stdio.h>
#include "adxl345.h"
#include "system_config.h"

volatile char rxData;
volatile char isRunning = 0;
volatile int timeStepValue = 0;

#ifndef XPS_FPGA11_INT_ID
	#define XPS_FPGA11_INT_ID 	0
#endif

/******************************************************************************
* @brief Displays Main Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_DisplayMainMenu(void)
{
	xil_printf("\n\r\n\rPmodACL Demo Program\n\r");
	xil_printf("\n\r");
	xil_printf("Options:\n\r");
	xil_printf("    [e] Enable Measurement\n\r");
	xil_printf("    [d] Disable Measurement\n\r");
	xil_printf("    [a] Display Acceleration\n\r");
	xil_printf("    [s] Select Measurement Range\n\r");
	xil_printf("    [r] Change acquisition rate\n\r");
	xil_printf("    [t] Select Tap Interrupts\n\r");
	xil_printf("    [q] Stop any on going action\n\r");
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
void ADXL345_DisplayMenu(void)
{
	xil_printf("\n\rOptions:\n\r");
	xil_printf("    [e] Enable Measurement\n\r");
	xil_printf("    [d] Disable Measurement\n\r");
	xil_printf("    [a] Display Acceleration\n\r");
	xil_printf("    [s] Select Measurement Range\n\r");
	xil_printf("    [r] Change acquisition rate\n\r");
	xil_printf("    [t] Select Tap Interrupts\n\r");
	xil_printf("    [q] Stop any on going action\n\r");
	xil_printf("Please select desired option\n\r");
	rxData = 0;
}

/******************************************************************************
* @brief Displays Range Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_DisplayRangeMenu(void)
{
	xil_printf("\n\rMeasurement Range\n\r");
	xil_printf("Options:\n\r");
	xil_printf("    [1] +-2g\n\r");
	xil_printf("    [2] +-4g\n\r");
	xil_printf("    [3] +-8g\n\r");
	xil_printf("    [4] +-16g\n\r");
	xil_printf("Press [1] to [4] to select desired range\n\r");
}

/******************************************************************************
* @brief Displays Rate Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_DisplayRateMenu(void)
{
	xil_printf("\n\rAcquisition Rate\n\r");
	xil_printf("Options:\n\r");
	xil_printf("    [1] 6.25 Hz\n\r");
	xil_printf("    [2] 12.5 Hz\n\r");
	xil_printf("    [3] 25 Hz\n\r");
	xil_printf("    [4] 50 Hz\n\r");
	xil_printf("    [5] 100 Hz\n\r");
	xil_printf("    [6] 200 Hz\n\r");
	xil_printf("    [7] 400 Hz\n\r");
	xil_printf("    [8] 800 Hz\n\r");
	xil_printf("    [9] 1600 Hz\n\r");
	xil_printf("Press [1] to [9] to select desired range\n\r");
}

/******************************************************************************
* @brief Displays Tap Menu.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_DisplayTapMenu(void)
{
	xil_printf("\n\rTap Menu\n\r");
	xil_printf("Options:\n\r");
	xil_printf("    [1] Single Tap\n\r");
	xil_printf("    [2] Double Tap\n\r");
	xil_printf("    [3] Single and Double Tap\n\r");
	xil_printf("    [4] Tap Off\n\r");
	xil_printf("Press [1] to [4] to select desired range\n\r");
}

/******************************************************************************
* @brief Writes to a ADXL345 Internal Register.
*
* @param addr 	- Register address.
* @param txData - Data to be sent.
*
* @return None.
******************************************************************************/
void ADXL345_WriteReg(unsigned char addr, unsigned char txData)
{
	char txBuffer[2] = {0x00, 0x00};
	char rxBuffer[2] = {0x00, 0x00};

	txBuffer[0] = addr & 0x3F;
	txBuffer[1] = txData;

	SPI_TransferData(SPI_BASEADDR, 2, txBuffer, 2, rxBuffer, 1);
}

/******************************************************************************
* @brief Reads from a ADXL345 Internal Register.
*
* @param addr 	- Register address.
*
* @return rxData - Data read from ADXL345.
******************************************************************************/
int ADXL345_ReadReg(unsigned char addr)
{
	char txBuffer[2] 	= {0x00, 0x00};
	char rxBuffer[2] 	= {0x00, 0x00};
	char rxData 		= 0;

	txBuffer[0] = (1 << ADXL345_RnW) | (addr & 0x3F);
	txBuffer[1] = 0x00;

	SPI_TransferData(SPI_BASEADDR, 2, txBuffer, 2, rxBuffer, 1);

	rxData = (rxBuffer[1] & 0xFF);

	return(rxData);
}

/******************************************************************************
* @brief Burst reads from a ADXL345 Internal Register.
*
* @param addr 	- First Register address.
*
* @return rxData - Data read from ADXL345.
******************************************************************************/
int ADXL345_BurstReadReg(unsigned char addr)
{
	char txBuffer[3] 	= {0x00, 0x00, 0x00};
	char rxBuffer[3] 	= {0x00, 0x00, 0x00};
	int  rxData 	 	= 0;

	txBuffer[0] = (1 << ADXL345_RnW) | (1 << ADXL345_MB) | (addr & 0x3F);
	txBuffer[1] = 0x00;

	SPI_TransferData(SPI_BASEADDR, 3, txBuffer, 3, rxBuffer, 1);

	rxData = ((rxBuffer[2] & 0xFF) << 8) | (rxBuffer[1] & 0xFF);

	return(rxData);
}

/******************************************************************************
* @brief Initialize ADXL345.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_Init(void)
{
	ADXL345_WriteReg(ADXL345_BW_RATE, (0x07 << ADXL345_RATE)); 			// Data update rate = 100 Hz
	ADXL345_WriteReg(ADXL345_DATA_FORMAT, ((1 << ADXL345_FULL_RES)	|	// Data format = +- 16g, Full Resolution
										   (3 << ADXL345_Range)));
	ADXL345_WriteReg(ADXL345_INT_ENABLE, ((1 << ADXL345_SINGLE_TAP)	|	// Enable Single, Double Tap
										  (1 << ADXL345_DOUBLE_TAP)	|
										  (0 << ADXL345_Activity)	|
										  (0 << ADXL345_FREE_FALL)	|
										  (0 << ADXL345_DATA_READY)));
	ADXL345_WriteReg(ADXL345_THRESH_TAP, 0x20); 						// Tap threshold
	ADXL345_WriteReg(ADXL345_DUR, 0x0D); 								// Tap duration
	ADXL345_WriteReg(ADXL345_Latent, 0x50); 							// Tap latency
	ADXL345_WriteReg(ADXL345_Window, 0xF0); 							// Tap Window
	ADXL345_WriteReg(ADXL345_TAP_AXES, ((1 << ADXL345_TAP_X_en)		|	// Enable tap on all axes
										(1 << ADXL345_TAP_Y_en)		|
										(1 << ADXL345_TAP_Z_en)));
	ADXL345_WriteReg(ADXL345_THRESH_ACT, 0x08); 						// Activity threshold
	ADXL345_WriteReg(ADXL345_ACT_INACT_CTL, ((1 << ADXL345_ACT_X_en)|	// Activity axes
											(1 << ADXL345_ACT_Y_en) |
											(1 << ADXL345_ACT_Z_en)));
	ADXL345_WriteReg(ADXL345_THRESH_FF, 0x08); 							// Free fall threshold
	ADXL345_WriteReg(ADXL345_TIME_FF, 0x0A); 							// Free fall window
	ADXL345_WriteReg(ADXL345_FIFO_CTL, 0x01);

	ADXL345_ReadAllAxes();

    if(USE_PS7 == 1)
    {
    	timeStepValue = 0x1A7D;
        Xil_Out32(LED_BASEADDR + 0x288, (1 << 19) | (1 << 20));
        Xil_Out32(LED_BASEADDR + 0x284, (1 << 19) | (1 << 20));
    }
    else
    {
    	timeStepValue = 0xFA0A1F01;
    }
}

/******************************************************************************
* @brief Display g force for desired axis.
*
* @param axis - Which axis to read.
*
* @return None.
******************************************************************************/
void ADXL345_Display_G_Force(char axis)
{
	int 	gForce 		= 0;
	int 	gForceCalc 	= 0;
	float 	value 		= 0;
	int 	whole 		= 0;
	int 	thousands	= 0;

	// Select which axis to read
	switch(axis)
	{
		case 'x':
			gForce = ADXL345_BurstReadReg(ADXL345_DATA_X0);
			xil_printf("[X = ");
			break;
		case 'y':
			gForce = ADXL345_BurstReadReg(ADXL345_DATA_Y0);
			xil_printf("[Y = ");
			break;
		case 'z':
			gForce = ADXL345_BurstReadReg(ADXL345_DATA_Z0);
			xil_printf("[Z = ");
			break;
		default:
			break;
	}

	// If read result is negative, apply padding with 0xFFFFF000
	if((gForce & 0x1000) == 0x1000)
	{
		gForceCalc = 0xFFFFF000 | gForce;
	}
	// If read result is positive, do nothing to modify it
	else
	{
		gForceCalc = gForce & 0x0FFF;
	}

	// Apply HEX to Accel data processing
	value = ((float)gForceCalc * 0.004);

	// Prepare and display data
	// If positive, leave as is
	if(gForceCalc >= 0)
	{
		whole = value;
		thousands = (value - whole) * 1000;

		if(thousands > 99)
		{
			xil_printf("+%d.%3d] ", whole, thousands);
		}
		else if(thousands > 9)
		{
			xil_printf("+%d.0%2d] ", whole, thousands);
		}
		else
		{
			xil_printf("+%d.00%1d] ", whole, thousands);
		}
	}
	// If negative, multiply by (-1) for proper display
	else
	{
		value = value * (-1);
		whole = value;
		thousands = (value - whole) * 1000;

		if(thousands > 99)
		{
			xil_printf("-%d.%3d] ", whole, thousands);
		}
		else if(thousands > 9)
		{
			xil_printf("-%d.0%2d] ", whole, thousands);
		}
		else
		{
			xil_printf("-%d.00%1d] ", whole, thousands);
		}
	}
}

/******************************************************************************
* @brief Display all axes.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_DisplayAllAxes(void)
{
	ADXL345_Display_G_Force('x');
	ADXL345_Display_G_Force('y');
	ADXL345_Display_G_Force('z');
	xil_printf("\n\r");
}

/******************************************************************************
* @brief Read all axes.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_ReadAllAxes(void)
{
	ADXL345_BurstReadReg(ADXL345_DATA_X0);
	ADXL345_BurstReadReg(ADXL345_DATA_Y0);
	ADXL345_BurstReadReg(ADXL345_DATA_Z0);
}

/******************************************************************************
* @brief Run measurements.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_Run(void)
{
	if(isRunning == 0)
	{
		ADXL345_WriteReg(ADXL345_POWER_CTL, (1 << ADXL345_Measure));
		isRunning = 1;
	}
	xil_printf("\n\r>ADXL345 Measurement Enabled!\n\r\n\r");
	ADXL345_DisplayMenu();
}

/******************************************************************************
* @brief Stop measurements.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_Stop(void)
{
	if(isRunning == 1)
	{
		ADXL345_WriteReg(ADXL345_POWER_CTL, (0 << ADXL345_Measure));
		isRunning = 0;
	}
	xil_printf("\n\r>ADXL345 Measurement Disabled!\n\r\n\r");
	ADXL345_DisplayMenu();
}

/******************************************************************************
* @brief Displays acceleration on all 3 axes.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_DisplayAccel(void)
{
	char rxValue = 0x00;
	if(isRunning == 1)
	{
		DisableInterrupts(XPS_FPGA11_INT_ID);

		rxValue = ADXL345_ReadReg(ADXL345_INT_SOURCE);
		if(rxValue & 0x80)
		{
			ADXL345_DisplayAllAxes();
		}
		if(rxValue & 0x40)
		{
			if(USE_PS7 == 1)
			{
	            // Turn ON LED 1
				GpioOut(LED_BASEADDR, (1 << 19));
			}
			else
			{
	            // Turn ON LED 1
				GpioOut(LED_BASEADDR, 0x01);
			}
	        // Load Timer with 1 second value
			LoadTimer(timeStepValue);
			// Start Timer
			StartTimer();
		}
		else if(rxValue & 0x20)
		{
			if(USE_PS7 == 1)
			{
	            // Turn ON LED 1 and LED2
				GpioOut(LED_BASEADDR, (3 << 19));
			}
			else
			{
	            // Turn ON LED 1 and LED2
				GpioOut(LED_BASEADDR, 0x03);
			}
	        // Load Timer with 1 second value
			LoadTimer(timeStepValue);
			// Start Timer
			StartTimer();
		}
		EnableInterrupts(XPS_FPGA11_INT_ID);
	}
	else if(isRunning == 0)
	{
		xil_printf("\n\r!!! Measurement not enabled. Please enable measurement before performing current action !!!\n\r");
		ADXL345_DisplayMenu();
	}

}

/******************************************************************************
* @brief Select measurement range.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_SelectRange(void)
{
	char rx 	= 0;
	char rxReg 	= 0;

	ADXL345_DisplayRangeMenu();

	// Disable Interrupts in order to use polling mode
	DisableInterrupts(UART_INTERRUPT);

	// Check if data is available on the UART
	rx = UartReadChar();

	// Convert ASCII format to desired one
	rx = rx - 0x31;

	if ((rx >= 0x00) && (rx <= 0x03))
		{
		rxReg = (ADXL345_ReadReg(ADXL345_DATA_FORMAT) & 0xFC) | rx;

		// Change DATA_FORMAT Register value
		ADXL345_WriteReg(ADXL345_DATA_FORMAT, rxReg);
		}

	// Display new settings
	switch(rx)
	{
		case 0:
			xil_printf("> +-2g range selected\n\r");
			break;
		case 1:
			xil_printf("> +-4g range selected\n\r");
			break;
		case 2:
			xil_printf("> +-8g range selected\n\r");
			break;
		case 3:
			xil_printf("> +-16g range selected\n\r");
			break;
		default:
			xil_printf("> wrong measurement range\n\r");
			break;
	}

	// Await new command after exiting function
	rxData = 'q';

	// Enable Interrupts
	EnableInterrupts(UART_INTERRUPT);

	ADXL345_DisplayMenu();
}

/******************************************************************************
* @brief Select Acquisition Rate.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_AcquisitionRate(void)
{
	char rx 	= 0;
	char rxReg 	= 0;

	ADXL345_DisplayRateMenu();

	// Disable Interrupts in order to use polling mode
	DisableInterrupts(UART_INTERRUPT);

	// Check if data is available on the UART
	rx = UartReadChar();

	// Convert ASCII format to desired one
	rx = rx - 0x30 + 0x05;

	if ((rx >= 0x06) && (rx <= 0x0E))
	{
		rxReg = (ADXL345_ReadReg(ADXL345_BW_RATE) & 0xF0) | rx;

		// Change BW_RATE Register Value
		ADXL345_WriteReg(ADXL345_BW_RATE, rxReg);
	}
	// Display new settings
	switch(rx)
	{
		case 0x06:
			xil_printf("> 6.25 Hz selected\n\r");
			break;
		case 0x07:
			xil_printf("> 12.5 Hz selected\n\r");
			break;
		case 0x08:
			xil_printf("> 25 Hz selected\n\r");
			break;
		case 0x09:
			xil_printf("> 50 Hz selected\n\r");
			break;
		case 0x0A:
			xil_printf("> 100 Hz selected\n\r");
			break;
		case 0x0B:
			xil_printf("> 200 Hz selected\n\r");
			break;
		case 0x0C:
			xil_printf("> 400 Hz selected\n\r");
			break;
		case 0x0D:
			xil_printf("> 800 Hz selected\n\r");
			break;
		case 0x0E:
			xil_printf("> 1600 Hz selected\n\r");
			break;
		default:
			xil_printf("> wrong acquisition rate\n\r");
			break;
	}

	// Await new command after exiting function
	rxData = 'q';

	// Enable Interrupts
	EnableInterrupts(UART_INTERRUPT);

	ADXL345_DisplayMenu();
}

/******************************************************************************
* @brief Select tap type.
*
* @param None.
*
* @return None.
******************************************************************************/
void ADXL345_SelectTap(void)
{
	char rx 	= 0;
	char rxReg 	= 0;

	ADXL345_DisplayTapMenu();

	// Disable Interrupts in order to use polling mode
	DisableInterrupts(UART_INTERRUPT);

	// Check if data is available on the UART
	rx = UartReadChar();

	// Enable and display desired options
	switch(rx)
	{
		case '1':
			rxReg = (ADXL345_ReadReg(ADXL345_INT_ENABLE) & 0x9F) | (1 << ADXL345_SINGLE_TAP);
			ADXL345_WriteReg(ADXL345_INT_ENABLE, rxReg);
			xil_printf("> Single Tap selected\n\r");
			break;
		case '2':
			rxReg = (ADXL345_ReadReg(ADXL345_INT_ENABLE) & 0x9F) | (1 << ADXL345_DOUBLE_TAP);
			ADXL345_WriteReg(ADXL345_INT_ENABLE, rxReg);
			xil_printf("> Double Tap selected\n\r");
			break;
		case '3':
			rxReg = (ADXL345_ReadReg(ADXL345_INT_ENABLE) & 0x9F) | (1 << ADXL345_SINGLE_TAP) | (1 << ADXL345_DOUBLE_TAP);
			ADXL345_WriteReg(ADXL345_INT_ENABLE, rxReg);
			xil_printf("> Single and Double Tap selected\n\r");
			break;
		case '4':
			rxReg = ADXL345_ReadReg(ADXL345_INT_ENABLE) & 0x9F;
			ADXL345_WriteReg(ADXL345_INT_ENABLE, rxReg);
			xil_printf("> Tap Off\n\r");
			break;
		default:
			xil_printf("> wrong tap option\n\r");
			break;
	}

	// Await new command after exiting function
	rxData = 'q';

	// Enable Interrupts
	EnableInterrupts(UART_INTERRUPT);

	ADXL345_DisplayMenu();
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
	unsigned char intSource  = 0;
	unsigned char intEnabled = 0;

	// Read ADXL345 Interrupt Source
	intSource = ADXL345_ReadReg(ADXL345_INT_SOURCE);
	// Read ADXL345 Enabled Interrupts
	intEnabled = ADXL345_ReadReg(ADXL345_INT_ENABLE);

	// If Interrupt Enabled and Triggered is Single Tap
	if((intSource & 0x40) && (intEnabled & 0x40))
	{
		if(USE_PS7 == 1)
		{
            // Turn ON LED 1
			GpioOut(LED_BASEADDR, (1 << 19));
		}
		else
		{
            // Turn ON LED 1
			GpioOut(LED_BASEADDR, 0x01);
		}
        // Load Timer with 1 second value
		LoadTimer(timeStepValue);
		// Start Timer
		StartTimer();
	}
	// If Interrupt Enabled and Triggered is Double Tap
	if((intSource & 0x20) && (intEnabled & 0x20))
	{
		if(USE_PS7 == 1)
		{
            // Turn ON LED 1 and LED2
			GpioOut(LED_BASEADDR, (3 << 19));
		}
		else
		{
            // Turn ON LED 1 and LED2
			GpioOut(LED_BASEADDR, 0x03);
		}
        // Load Timer with 1 second value
		LoadTimer(timeStepValue);
		// Start Timer
		StartTimer();
	}

    // Clear interrupts by reading all axes registers
	ADXL345_ReadAllAxes();
	if(USE_PS7 == 0)
	{
        // Clear External Interrupts
		Xil_Out32(EXT_BASEADDR + 0x120, 0x01);
	}
}

/******************************************************************************
* @brief Demo Program for ADXL345.
*
* @param None.
*
* @return None.
******************************************************************************/
void DemoProgram(void)
{
	// Display Main Menu
	ADXL345_DisplayMainMenu();

	while(1)
	{
		switch(rxData)
		{
			case 'e':
				ADXL345_Run();
				break;
			case 'd':
				ADXL345_Stop();
				break;
			case 'a':
				ADXL345_DisplayAccel();
				break;
			case 's':
				ADXL345_SelectRange();
				break;
			case 'r':
				ADXL345_AcquisitionRate();
				break;
			case 't':
				ADXL345_SelectTap();
				break;
			case 'q':
				ADXL345_DisplayMainMenu();
				break;
			case 0:
				break;
			default:
				xil_printf("\n> Wrong option! Please select one of the options below\r");
				ADXL345_DisplayMenu();
				break;
		}
	}
}
