/**************************************************************************//**
*   @file   ad7685.c
*   @brief  Implementation of ad7685 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ad7685.h"

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/

/**************************************************************************//**
 * @brief Initializes the communication with the device.
 *
 * @param None.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
******************************************************************************/
char ad7685_Init(void)
{
	char status = -1;

	/* Setup SPI Interface */
	status = SPI_Init(0, 1000000, 1, 0);

	return status;
}

/**************************************************************************//**
 * @brief Makes a conversion for a number of devices daisy-chained (only works
 *        for Chain Mode, no busy indicator).
 *
 * @param partsNo  - number of devices daisy-chained.
 *        *dataDac - pointer to the output values of each device.
 *
 * @return rxData - Received data in case of success.
******************************************************************************/
char ad7685_GetConversion(char partsNo, unsigned short *dataDac)
{
	unsigned char bytesRead = 0;
	unsigned char bytesNo   = 0;
	unsigned char *data        ;
	unsigned char count     = 0;

	bytesNo = partsNo * 2;
	data = calloc(bytesNo, sizeof (unsigned char));
	bytesRead = SPI_Read(ad7685_SLAVE_ID, data, bytesNo);
	if (bytesRead != bytesNo) {
		return -1;
	}
	for (count = 0; count < bytesNo; count++) {
		dataDac[count] = (data[2 * count] << 8) + (data[2 * count + 1] << 0);
	}

	free(data);

	return 0;
}

/***************************************************************************//**
 * @brief Converts a 16-bit raw sample to volts.
 *
 * @param rawSample - 16-bit data sample.
 * @param vRef      - The value of the voltage reference used by the device.
 *
 * @return voltage  - The result of the conversion expressed as volts.
*******************************************************************************/
float ad7685_ConvertToVolts(unsigned short rawSample, float vRef)
{
	float voltage = 0;

	voltage = vRef * (float)rawSample / (1ul << 16);

	return voltage;
}
