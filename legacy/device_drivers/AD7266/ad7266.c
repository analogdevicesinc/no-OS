/**************************************************************************//**
*   @file   ad7266.c
*   @brief  Implementation of ad7266 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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
*******************************************************************************/

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
