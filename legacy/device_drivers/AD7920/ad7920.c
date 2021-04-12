/**************************************************************************//**
*   @file   ad7920.c
*   @brief  Implementation of AD7920 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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
