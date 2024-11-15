/**************************************************************************//**
*   @file   ad7266.c
*   @brief  Implementation of ad7266 Driver for Microblaze processor.
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
#include "ad7266.h"

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
char ad7266_Init(void)
{

    char status = -1;

    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 1, 0);

    return status;
}

/**************************************************************************//**
 * @brief  Receives 32 bits from SPI and transmits 0x0.
 *
 * @param  ctrl   - 1 control dword
 *                  (0x0 - (VA1,VA2) and (VB1,VB2) - fully differential).
 *
 * @return rxData - Received data in case of success.
******************************************************************************/
unsigned long ad7266_Read32Bits(unsigned long ctrl)
{
    unsigned long rxData;
    char          bytesRead = 0;
    unsigned char data[4]   = {0, 0, 0, 0};

    data[0] = (ctrl & 0xFF000000) >> 24;
    data[1] = (ctrl & 0x00FF0000) >> 16;
    data[2] = (ctrl & 0x0000FF00) >> 8;
    data[3] = (ctrl & 0x000000FF) >> 0;

    bytesRead = SPI_Read(ad7266_SLAVE_ID, data, 4);
    if (bytesRead != 4)
    {
        return -1;
    }

    rxData = ((unsigned long)data[0] << 24) + ((unsigned long)data[1] << 16) +
             ((unsigned long)data[2] << 8) + ((unsigned long)data[3] << 0);

    return(rxData);
}
