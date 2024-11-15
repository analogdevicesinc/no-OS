/**************************************************************************//**
*   @file   ad7685.c
*   @brief  Implementation of ad7685 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
