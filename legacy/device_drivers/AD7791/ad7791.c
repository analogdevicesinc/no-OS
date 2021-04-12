/***************************************************************************//**
*   @file   ad7791.c
*   @brief  Implementation of AD7791 Driver for Microblaze processor.
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
******************************************************************************/

/******************************************************************************/
/******************************* Include Files ********************************/
/******************************************************************************/
#include "ad7791.h"
#include "Communication.h"
#include "TIME.h"

/***************************************************************************//**
 * @brief Initializes SPI and initial values for AD7791 device.
 *
 * @param None.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
char AD7791_Init(void)
{
    char status = -1;

    status = SPI_Init(0, 1000000, 1, 0);

    return status;
}

/***************************************************************************//**
 * @brief Reads data from AD7791.
 *
 * @param regAddress  - Address of the register.
 * @param bytesNumber - Number of bytes.
 *
 * @return readData   - Received data.
*******************************************************************************/
unsigned long AD7791_Read(unsigned char regAddress,
                          unsigned char bytesNumber)
{
    unsigned long receivedData = 0x0;
    unsigned char command      = 0x0;
    unsigned char dataBuf[4]   = {0x00, 0x00, 0x00, 0x00};

    command    = AD7791_COMM_READ + AD7791_COMM_ADDR(regAddress);
    dataBuf[0] = command;
    SPI_Read(AD7791_SLAVE_ID, (unsigned char*)dataBuf, bytesNumber + 1);
    receivedData = (dataBuf[1] << 16) + (dataBuf[2] << 8) + (dataBuf[3] << 0);

    return(receivedData);
}

/***************************************************************************//**
 * @brief Writes data to AD7791.
 *
 * @param regAddress  - Address of the register.
 * @param bytesNumber - Number of bytes.
 * @param regData     - Data that will be loaded.
 *
 * @return None.
*******************************************************************************/
void AD7791_Write(unsigned char regAddress,
                  unsigned char bytesNumber,
                  unsigned char regData)
{
    unsigned char command  = 0x0;
    unsigned char txBuf[2] = {0x00, 0x00};

    command  = AD7791_COMM_WRITE + AD7791_COMM_ADDR(regAddress);
    txBuf[0] = command;
    txBuf[1] = regData;
    bytesNumber = 1;
    SPI_Write(AD7791_SLAVE_ID, (unsigned char*)txBuf, bytesNumber + 1);

}

/***************************************************************************//**
 * @brief Converts 24-bit raw data to millivolts.
 *
 * @param rawData  - 24-bit data sample.
 * @param vRef     - The value of the voltage reference used by the device in volts.
 * @param polarity - The set polarity of the input voltage.
 *                   Example: 0 - Bipolar  mode.
 *                            1 - Unipolar mode.
 *
 * @return voltage - The result of the conversion expressed in millivolts.
*******************************************************************************/
float AD7791_ConvertToVolts(unsigned long rawData,
                            float         vRef,
                            unsigned char polarity)
{
    float voltage = 0;

    if(polarity == 0)       // Bipolar mode
    {
        voltage = 1000 * (((float)rawData / (1ul << 23)) - 1) * vRef;
    }
    else
    {
        if (polarity == 1)  // Unipolar mode
        {
            voltage = 1000 * ((float)rawData * vRef) / (1ul << 24);
        }
    }

    return voltage;
}

/***************************************************************************//**
 * @brief Resets the serial interface with the AD7791.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void AD7791_Reset(void)
{
    unsigned char bytesNumber = 5;
    unsigned char txBuf[5]    = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    SPI_Write(AD7791_SLAVE_ID, (unsigned char*)txBuf, bytesNumber);
}
