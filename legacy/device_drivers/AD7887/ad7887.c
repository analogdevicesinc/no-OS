/**************************************************************************//**
*   @file   ad7887.c
*   @brief  Implementation of ad7887 Driver for Microblaze processor.
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

