/**************************************************************************//**
*   @file   ad7887.c
*   @brief  Implementation of ad7887 Driver for Microblaze processor.
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
#include "ad7887.h"

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
char AD7887_Init(void)
{

    char status = -1;

    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 1, 0);

    return status;
}

/**************************************************************************//**
 * @brief Receives 16 bits from SPI and transmits 2 control bytes.
 *
 * @param data[2] - 2 control bytes.
 * 		  			The first byte is the control register on the AD7887.
 * 		  			The second byte is "don't care".
 *
 * @return rxData - Received data in case of success.
******************************************************************************/
unsigned short AD7887_Read16Bits(unsigned char ctrl[2])
{
    unsigned short rxData;
    char           bytesRead = 0;
    unsigned char  data[2] = {0, 0};

    data[0] = ctrl[0];
    data[1] = ctrl[1];

    bytesRead = SPI_Read(AD7887_SLAVE_ID, data, 2);
    if (bytesRead != 2)
    {
        return -1;
    }

    rxData = ((unsigned short)data[0] << 8) + data[1];

    return(rxData);
}

