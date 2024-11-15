/**************************************************************************//**
*   @file   ad5662.c
*   @brief  Implementation of ad5662 Driver for Microblaze processor.
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
#include "ad5662.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
unsigned short fullScale = 0xFFFF;

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
char ad5662_Init(void)
{

    char status = -1;

    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 1, 1);

    return status;
}

/***************************************************************************//**
 * @brief Writes 24 bits to device through SPI.
 *
 * @param data - Data which will be transmitted.
 *
 * @return status - Result of the write operation.
 *                  Example:  0 - if operation was successful;
 *                           -1 - if operation was unsuccessful.
*******************************************************************************/
char ad5662_Write24Bits(unsigned long data)
{
    unsigned char bytesWritten = 0;
    char          status       = 0;
    unsigned char dataBuf[3]   = {(data & 0x00FF0000) >> 16,
                                  (data & 0x0000FF00) >> 8,
                                  (data & 0x000000FF) >> 0};

    bytesWritten = SPI_Write(ad5662_SLAVE_ID, dataBuf, 3);
    if (bytesWritten != 3)
    {
        return status = -1;
    }

    return status;
}

/***************************************************************************//**
 * @brief Sets the output DAC voltage to a desired value.
 *
 * @param vOut - the desired output voltage of the DAC.
 *        vRef - reference voltage of the DAC.
 *
 * @return None.
*******************************************************************************/
void ad5662_SetVoltage(float vOut, float vRef)
{
    unsigned long dacCode = 0;

    dacCode  = (unsigned long)((float)(vOut / vRef) * fullScale);
    dacCode |= AD5662_LOAD;
    ad5662_Write24Bits(dacCode);
}
