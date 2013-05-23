/***************************************************************************//**
 *   @file   AD7091R.c
 *   @brief  Implementation of AD7091R Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "AD7091R.h"
#include "Communication.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the SPI communication peripheral.
 *
 * @return Result of the initialization procedure.
 *                   Example: -1 - SPI peripheral was not initialized.
 *                             0 - SPI peripheral is initialized.
*******************************************************************************/
char AD7091R_Init(void)
{
    unsigned char status = 0;
    unsigned char tmpVal = 0xFF;
    
    status = SPI_Init(0, 8000000, 0, 1);
    /* Ensures that last state of SDO is high. */
    SPI_Write(0, &tmpVal, 1);
    AD7091R_SoftwareReset();

    return status;
}

/***************************************************************************//**
 * @brief Initiate a software reset of the device. The AD7091R requires the user
 *        to initiate a software reset when power is first applied.
 *
 * @return None.
*******************************************************************************/
void AD7091R_SoftwareReset(void)
{
    unsigned char writeByte   = 0xBF;

    /* Initiate a conversion. */
    SPI_Write(0, &writeByte, 1);
    /* Short cycle the read operation. */
    writeByte = 0xFF;
    SPI_Read(AD7091R_SLAVE_ID, &writeByte, 1);
    /* Perform another conversion in order to reset the device. */
    writeByte = 0xBF;
    SPI_Write(0, &writeByte, 1);
}

/***************************************************************************//**
 * @brief Initiates one conversion and reads back the result. During this
 *        process the device runs in normal mode and operates without the busy
 *        indicator.
 *
 * @return conversionResult - 12bit conversion result.
*******************************************************************************/
unsigned short AD7091R_ReadSample(void)
{
    unsigned short conversionResult = 0;
    unsigned char  writeByte        = 0xBF;
    unsigned char  buffer[2]        = {0xFF, 0xFF};

    /* Initiate a conversion. */
    SPI_Write(0, &writeByte, 1);
    /* Read conversion data. */
    SPI_Read(AD7091R_SLAVE_ID, buffer, 2);
    conversionResult = (buffer[0] << 8) + buffer[1];
    conversionResult >>= 4;

    return conversionResult;
}

/***************************************************************************//**
 * @brief Puts the device in power-down mode.
 *
 * @return None.
*******************************************************************************/
void AD7091R_PowerDown(void)
{
    unsigned char buffer[2]   = {0, 0};
    unsigned char writeValue  = 0x00;

    /* Initiate a conversion. */
    SPI_Write(0, &writeValue, 1);
    /* Perform a dummy read. */
    SPI_Read(AD7091R_SLAVE_ID, buffer, 2);
}

/***************************************************************************//**
 * @brief Powers up the device by pulling CONVST high. After calling this
 *        function, a time delay is required before initiating another
 *        conversion:
 *         - 50 ms delay, when internal reference is used.(with 2.2uF capacitor)
 *         - 100 us delay, when external reference is used.
 *
 * @return  None.
*******************************************************************************/
void AD7091R_PowerUp(void)
{
    unsigned char writeValue = 0xFF;

    /* Pull CONVST signal high. */
    SPI_Write(0, &writeValue, 1);
}


/***************************************************************************//**
 * @brief Converts a 12-bit raw sample to volts.
 *
 * @param rawSample - 12-bit data sample.
 * @param vRef      - The value of the voltage reference used by the device.
 *
 * @return voltage  - The result of the conversion expressed as volts.
*******************************************************************************/
float AD7091R_ConvertToVolts(short rawSample, float vRef)
{
    float voltage = 0;
    
    if(vRef == 0)
    {
        vRef = 2.5;
    }
    voltage = vRef * (float)rawSample / (1 << 12);
    
    return voltage;
}
