/**************************************************************************//**
*   @file   ad7466.c
*   @brief  Implementation of ad7466 Driver for Microblaze processor.
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
#include "ad7466.h"

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
char AD7466_Init(void)
{
    char status = -1;

    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 1, 1);

    return status;
}

/**************************************************************************//**
 * @brief Receives 16 bits from SPI.
 *
 * @return rxData - Received data in case of success.
******************************************************************************/
unsigned short AD7466_Read16Bits(void)
{
    unsigned short rxData;
    char           bytesRead = 0;
    unsigned char  data[2]   = {0, 0};

    bytesRead = SPI_Read(AD7466_SLAVE_ID, data, 2);
    if (bytesRead != 2)
    {
        return -1;
    }
    rxData = ((unsigned short)data[0] << 8) + data[1];

    return(rxData);
}
