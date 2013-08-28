/***************************************************************************//**
*   @file   AD5933.c
*   @brief  AD5933 Driver.
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
#include <math.h>
#include "system_config.h"
#include "AD5933.h"

volatile int   rxData;
int            temperature     = 0;
unsigned long  impedanceKohms  = 0;
unsigned long  impedanceOhms   = 0;
float          impedance       = 0.0f;
float          gainFactor      = 0.0f;


/******************************************************************************
* @brief Set an AD5933 internal register value.
*
* @param registerAddress - Address of AD5933 register.
*
* @param registerValue - Value of data to be written in the register.
*
* @param numberOfBytes - Number of bytes to be written in the register
*
* @return None.
******************************************************************************/
void AD5933_SetRegisterValue(int registerAddress,
							 int registerValue,
							 char numberOfBytes)
{
	char 			byte         = 0;
	unsigned char 	writeData[4] = {0, 0, 0, 0};
	// Write numberOfBytes from starting address to starting address + numberOfBytes
	for(byte = 0; byte < numberOfBytes; byte++)
	{
		writeData[0] = registerAddress + numberOfBytes - byte - 1;
		writeData[1] = ((registerValue >> (8*byte)) & 0xFF);
		I2C_Write_axi(I2C_BASEADDR, AD5933_I2C_ADDR, -1, 2, writeData);
	}
}

/******************************************************************************
* @brief Read an AD5933 internal register value.
*
* @param registerAddress - Address of AD5933 register.
*
* @param numberOfBytes - Number of bytes to be read from the register.
*
* @return Register value.
******************************************************************************/
int AD5933_GetRegisterValue(int registerAddress,
							char numberOfBytes)
{
	int  registerValue 	= 0;
	char byte 			= 0;
	unsigned char  writeData[4] 	= {0, 0, 0, 0};
	unsigned char  readData[4] 	    = {0, 0, 0, 0};

	for(byte = 0; byte < numberOfBytes; byte++)
	{
		// Set pointer to desired register
		writeData[0] = AD5933_ADDR_POINTER;
		writeData[1] = registerAddress + byte;
		I2C_Write_axi(I2C_BASEADDR, AD5933_I2C_ADDR, -1, 2, writeData);

		// Read register data
		I2C_Read_axi(I2C_BASEADDR, AD5933_I2C_ADDR, -1, 1, readData);
		registerValue |= readData[0] << (8 * (numberOfBytes - byte - 1));
	}
	return(registerValue);
}

/******************************************************************************
* @brief Read the AD5933 temperature.
*
* @param None.
*
* @return Temperature value.
******************************************************************************/
int AD5933_GetTemperature(void)
{
	int temperature = 0;
	int status 		= 0;

	// Enable temperature measurement
	AD5933_SetRegisterValue(AD5933_CONTROL_REG_HB,
							AD5933_CONTROL_FUNCTION(AD5933_MEAS_TEMP),
							1);
	// Wait for read temperature to be valid
	while((status & 1) == 0)
	{
		status = AD5933_GetRegisterValue(AD5933_STATUS_REG, 1);
	}

	// Read correct temperature value
	temperature = AD5933_GetRegisterValue(AD5933_TEMP_REG_HB, 2);

	// Calculate temperature according to datasheet specifications
	if(temperature < 8192)
	{
		temperature /= 32;
	}
	else
	{
		temperature -= 16384;
		temperature /= 32;
	}

	return(temperature);
}

/******************************************************************************
* @brief Configure the AD5933 frequency sweep parameters.
*
* @param startFreq - Starting frequency value.
*
* @param incSteps - Number of increment steps.
*
* @param incFreq - Frequency step value.
*
* @return None.
******************************************************************************/
void AD5933_ConfigSweep(int startFreq,
						int incSteps,
						int incFreq)
{
	// Configure starting frequency
	AD5933_SetRegisterValue(AD5933_START_FREQ_REG_HB,
							startFreq,
							3);
	// Configure number of steps
	AD5933_SetRegisterValue(AD5933_NR_INCR_REG_HB,
							incSteps,
							2);
	// Configure frequency increment step
	AD5933_SetRegisterValue(AD5933_FREQ_INCR_REG_HB,
							incFreq,
							3);
}

/******************************************************************************
* @brief Start AD5933 frequency sweep.
*
* @param None.
*
* @return None.
******************************************************************************/
void AD5933_StartSweep(void)
{
	int status = 0;

	// Place AD5933 in standby
	AD5933_SetRegisterValue(AD5933_CONTROL_REG_HB,
							AD5933_CONTROL_FUNCTION(AD5933_STANDBY),
							1);

	// Select internal system clock
	AD5933_SetRegisterValue(AD5933_CONTROL_REG_LB,
							0x00,
							1);

	// Initialize starting frequency
	AD5933_SetRegisterValue(AD5933_CONTROL_REG_HB,
							AD5933_CONTROL_FUNCTION(AD5933_INIT_START_FREQ),
							1);

	// Start frequency sweep
	AD5933_SetRegisterValue(AD5933_CONTROL_REG_HB,
							AD5933_CONTROL_FUNCTION(AD5933_START_FREQ_SWEEP),
							1);

	// Wait for data to be valid
	while((status & AD5933_STATUS_DATA_VALID) == 0)
	{
		status = AD5933_GetRegisterValue(AD5933_STATUS_REG,1);
	}
}

/******************************************************************************
* @brief Calculate gain factor
*
* @param calibrationImpedance - Known value of connected impedance for calibration.
*
* @param freqFunction - Select Repeat Frequency Sweep.
*
* @return gainFactor.
******************************************************************************/
double AD5933_CalculateGainFactor(unsigned long calibrationImpedance,
							   char freqFunction)
{
	double       gainFactor = 0;
	double       magnitude  = 0;
	int          status     = 0;
	signed short realData   = 0;
	signed short imgData    = 0;

	// Repeat frequency sweep with last set parameters
	AD5933_SetRegisterValue(AD5933_CONTROL_REG_HB,
							AD5933_CONTROL_FUNCTION(freqFunction),
							1);

	// Wait for data received to be valid
	while((status & AD5933_STATUS_DATA_VALID) == 0)
	{
		status = AD5933_GetRegisterValue(AD5933_STATUS_REG, 1);
	}

	// Read real and imaginary data
	realData = AD5933_GetRegisterValue(AD5933_REAL_REG_HB,2);
	imgData  = AD5933_GetRegisterValue(AD5933_IMG_REG_HB,2);

	// Calculate magnitude
	magnitude = sqrtf((realData * realData) + (imgData * imgData));

	// Calculate gain factor
	gainFactor = 1000000000 / (magnitude * calibrationImpedance);

	return(gainFactor);
}

/******************************************************************************
* @brief Calculate impedance.
*
* @param gainFactor - Gain factor calculated using a known impedance.
*
* @param freqFunction - Select Repeat Frequency Sweep.
*
* @return impedance.
******************************************************************************/
double AD5933_CalculateImpedance(double gainFactor,
								 char freqFunction)
{
	signed short realData   = 0;
	signed short imgData    = 0;
	double       magnitude  = 0;
	double       impedance  = 0;
	int          status     = 0;

	// Repeat frequency sweep with last set parameters
	AD5933_SetRegisterValue(AD5933_CONTROL_REG_HB,
							AD5933_CONTROL_FUNCTION(freqFunction),
							1);

	// Wait for data received to be valid
	while((status & AD5933_STATUS_DATA_VALID) == 0)
	{
		status = AD5933_GetRegisterValue(AD5933_STATUS_REG,1);
	}

	// Read real and imaginary data
	realData = AD5933_GetRegisterValue(AD5933_REAL_REG_HB,2);
	imgData  = AD5933_GetRegisterValue(AD5933_IMG_REG_HB,2);

	// Calculate magnitude
	magnitude = sqrtf((realData * realData) + (imgData * imgData));

	// Calculate impedance
	impedance = 1 / (magnitude * gainFactor / 1000000000);

	return(impedance);
}

/******************************************************************************
* @brief Demo program for AD5933
*
* @param none.
*
* @return none.
******************************************************************************/
void DemoProgram(void)
{
	// Read temperature from device
	temperature = AD5933_GetTemperature();
	xil_printf("\n\rTemperature: %d C\n\r",temperature);

	// Set sweep parameters
	AD5933_ConfigSweep(0x0F5C28,
					   0x01F4,
					   0x000D16);

	// Start the sweep
	AD5933_StartSweep();

	// Calculate gain factor for an impedance of 47kohms
	gainFactor = AD5933_CalculateGainFactor(47000,
											AD5933_REPEAT_FREQ);

	// Change the resistor used for calibration with the one you wish to measure
	xil_printf("Replace calibration component with desired one for measurement and press any key\n\r");

	UartReadChar();

	// Calculate impedance between Vout and Vin
	impedance = AD5933_CalculateImpedance(gainFactor,
										  AD5933_REPEAT_FREQ);
	impedanceOhms = (unsigned long)impedance;

	// Print impedance
	xil_printf("Impedance read: %d ohms\n\r", impedanceOhms);

	// Calculate impedance in kohm
	impedanceKohms = impedanceOhms / 1000;
	impedanceOhms %= 1000;

	// Print impedance in kohm
	if (impedanceOhms < 10)
	{
		xil_printf("Impedance read: %d.00%d Kohms\n\r", impedanceKohms, impedanceOhms);
	}
	else if (impedanceOhms < 100)
	{
		xil_printf("Impedance read: %d.0%d Kohms\n\r", impedanceKohms, impedanceOhms);
	}
	else
	{
		xil_printf("Impedance read: %d.%d Kohms\n\r", impedanceKohms, impedanceOhms);
	}

	xil_printf("Exiting application.\n\r");
}
