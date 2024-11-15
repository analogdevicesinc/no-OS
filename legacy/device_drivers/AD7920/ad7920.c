/**************************************************************************//**
*   @file   ad7920.c
*   @brief  Implementation of AD7920 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ad7920.h"

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
char AD7920_Init(void)
{
    char status = -1;

    AD7920_SD_PIN_OUT;
    AD7920_SD_HIGH;

    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 0, 0);

    return status;
}

/**************************************************************************//**
 * @brief Reads the output ADC code.
 *
 * @param none.
 *
 * @return rxData - Data read in case of success.
******************************************************************************/
unsigned short ad7920_GetAdcCode(void)
{
    unsigned short rxData;
    char           bytesRead = 0;
    unsigned char  data[2] = {0x0, 0x0};

    /* Receives 16 bits from SPI. */
    bytesRead = SPI_Read(AD7920_SLAVE_ID, data, 2);
    if (bytesRead != 2)
    {
        return -1;
    }
    rxData = ((unsigned short)data[0] << 8) + data[1];
    /* Extract the ADC code from the 16 bits word. */
    rxData &= 0x0FFF;

    return(rxData);
}

/**************************************************************************//**
 * @brief  Calculates the input voltage.
 *
 * @param  none.
 *
 * @return vin - input voltage.
******************************************************************************/
float ad7920_GetVoltage(float vRef)
{
    unsigned short data = 0;
    float          vin  = 0;

    data = ad7920_GetAdcCode();
    /* Calculate the input voltage corresponding to the current ADC code. */
    vin = ((float)data / 4096) * vRef;

    return vin;
}
