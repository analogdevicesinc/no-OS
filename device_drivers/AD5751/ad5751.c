/**************************************************************************//**
*   @file   ad5751.c
*   @brief  Implementation of ad5751 Driver for Microblaze processor.
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
#include "ad5751.h"

const unsigned char RANGE_CONFIG[15] = {0x05, // 0-5V
                                        0x06, // 0-10V
                                        0x09, // 0-6V  (20% overrange)
                                        0x0A, // 0-12V (20% overrange)
                                        0x0E, // 0-40V
                                        0x0F, // 0-44V
                                        0x00, // 4-20mA (External 15 k current sense resistor)
                                        0x10, // 4-20mA (Internal current sense resistor)
                                        0x01, // 0-20mA (External 15 k current sense resistor)
                                        0x11, // 0-20mA (Internal current sense resistor)
                                        0x02, // 0-24mA (External 15 k current sense resistor)
                                        0x12, // 0-24mA (Internal current sense resistor)
                                        0x1D, // 3.92-20.4mA (Internal current sense resistor)
                                        0x1E, // 0-20.4mA (Internal current sense resistor)
                                        0x1F};// 0-24.5mA (Internal current sense resistor)

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
char ad5751_Init(void)
{
    char status = -1;

    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 1, 1);
    ad5751_CLR_OUT;
    ad5751_CLR_LOW;
    ad5751_FAULT_IN;

    return status;
}

/***************************************************************************//**
 * @brief Writes 16 bits to device through SPI.
 *
 * @param data - Data which will be transmitted.
 *
 * @return status - Result of the write operation.
 *                  Example:  0 - if operation was successful;
 *                           -1 - if operation was unsuccessful.
*******************************************************************************/
char ad5751_Write16Bits(unsigned short data)
{
    unsigned char bytesWritten = 0;
    char          status       = 0;
    unsigned char dataBuf[2]   = {(data & 0xFF00) >> 8,
                                  (data & 0x00FF) >> 0};

    bytesWritten = SPI_Write(ad5751_SLAVE_ID, dataBuf, 2);
    if (bytesWritten != 2)
    {
        status = -1;
    }

    return status;
}

/***************************************************************************//**
 * @brief Reads 16 bits from the device through SPI.
 *
 * @param data - Data which will be transmitted (MOSI pin).
 *
 * @return bytesRead - received data, in case of success.
*******************************************************************************/
unsigned short ad5751_Read16Bits(unsigned short data)
{
    unsigned char  bytesRead  = 0;
    unsigned short receivedData;
    unsigned char  dataBuf[2] = {(data & 0xFF00) >> 8,
                                 (data & 0x00FF) >> 0};

    bytesRead = SPI_Read(ad5751_SLAVE_ID, dataBuf, 2);
    if(bytesRead != 2)
    {
        return -1;
    }
    receivedData = ((unsigned short)dataBuf[0] << 8) + ((unsigned short)dataBuf[1] << 0);

    return receivedData;
}
