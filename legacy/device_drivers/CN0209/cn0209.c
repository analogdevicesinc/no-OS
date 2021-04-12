/***************************************************************************//**
*   @file   cn0209.c
*   @brief  Implementation of CN0209 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include "cn0209.h"
#include "Communication.h"
#include "TIME.h"

/***************************************************************************//**
 * @brief Reads data from ADT7310.
 *
 * @param regAddress  - Address of the register.
 * @param bytesNumber - Number of bytes.
 *
 * @return readData   - Received data.
*******************************************************************************/
unsigned short ADT7310_Read(unsigned char regAddress,
                            unsigned char bytesNumber)
{
    unsigned short receivedData = 0x0;
    unsigned char  command      = 0x0;
    unsigned char  i            = 0;
    unsigned char  dataBuf[3]   = {0x00, 0x00, 0x00};

    ADT7310_CS_LOW;
    //TIME_DelayMs(1);
    command    = ADT7310_CMD_READ + ADT7310_CMD_ADDR(regAddress);
    dataBuf[0] = command;
    SPI_Read(1, (unsigned char*)dataBuf, bytesNumber + 1);
    for(i = 0; i < bytesNumber; i++)
    {
        receivedData = receivedData << 8;
        receivedData += dataBuf[i+1];
    }
    ADT7310_CS_HIGH;

    return(receivedData);
}

/***************************************************************************//**
 * @brief Writes data to ADT7310.
 *
 * @param regAddress  - Address of the register.
 * @param bytesNumber - Number of bytes.
 * @param regData     - Data that will be loaded.
 *
 * @return None.
*******************************************************************************/
void ADT7310_Write(unsigned char  regAddress,
                   unsigned char  bytesNumber,
                   unsigned short regData)
{
    unsigned char  command     = 0x0;
    unsigned short mask        = 0x0;
    unsigned char  currentByte = 0x0;
    unsigned char  i           = 0;
    unsigned char  txBuf[3]    = {0x00, 0x00, 0x00};

    command  = ADT7310_CMD_WRITE + ADT7310_CMD_ADDR(regAddress);
    txBuf[0] = command;
    ADT7310_CS_LOW;
    for(i = 0; i < bytesNumber; i++)
    {
        mask        = 0xFF << ((bytesNumber - i) * 8);
        currentByte = (regData & mask) >> ((bytesNumber - i) * 8);
        txBuf[i+1]  = currentByte;
    }
    SPI_Write(1, (unsigned char*)txBuf, bytesNumber + 1);
    ADT7310_CS_HIGH;
}

/***************************************************************************//**
 * @brief Reads the temperature from ADT7310 device.
 *
 * @param None.
 *
 * @return adt7310TempValue - Current temperature read.
*******************************************************************************/
float ADT7310_TemperatureRead(void)
{
    float         adt7310TempValue = 0;
    unsigned long temp             = 0;

    temp = ADT7310_Read(ADT7310_REG_TEMPERATURE, 2);
    if(temp & 0x8000)
    {
        temp -= 65536;
    }
    adt7310TempValue = (float)temp / 128;

    return adt7310TempValue;
}

/***************************************************************************//**
 * @brief Resets the serial interface with the ADT7310.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void ADT7310_Reset(void)
{
    unsigned char bytesNumber = 5;
    unsigned char txBuf[5]    = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    ADT7310_CS_LOW;
    SPI_Write(1, (unsigned char*)txBuf, bytesNumber);
    ADT7310_CS_HIGH;
}

/***************************************************************************//**
 * @brief Reads data from AD7193.
 *
 * @param regAddress  - Address of the register.
 * @param bytesNumber - Number of bytes.
 *
 * @return readData   - Received data.
*******************************************************************************/
unsigned long AD7193_Read(unsigned char regAddress,
                          unsigned char bytesNumber)
{
    unsigned long receivedData = 0x0;
    unsigned char command      = 0x0;
    unsigned char i            = 0;
    unsigned char dataBuf[4]   = {0x00, 0x00, 0x00, 0x00};

    AD7193_CS_LOW;
    command    = AD7193_COMM_READ + AD7193_COMM_ADDR(regAddress);
    dataBuf[0] = command;
    SPI_Read(1, (unsigned char*)dataBuf, bytesNumber + 1);
    for(i = 0; i < bytesNumber; i++)
    {
        receivedData = receivedData << 8;
        receivedData += dataBuf[i+1];
    }
    AD7193_CS_HIGH;

    return(receivedData);
}

/***************************************************************************//**
 * @brief Writes data to AD7193.
 *
 * @param regAddress  - Address of the register.
 * @param bytesNumber - Number of bytes.
 * @param regData     - Data that will be loaded.
 *
 * @return None.
*******************************************************************************/
void AD7193_Write(unsigned char regAddress,
                  unsigned char bytesNumber,
                  unsigned long regData)
{
    unsigned char command     = 0x0;
    unsigned long mask        = 0x0;
    unsigned char currentByte = 0x0;
    unsigned char txBuf[4]    = {0x00, 0x00, 0x00, 0x00};
    unsigned char i           = 1;

    command  = AD7193_COMM_WRITE + AD7193_COMM_ADDR(regAddress);
    txBuf[0] = command;
    AD7193_CS_LOW;
    while(bytesNumber--)
    {
        mask = 0xFF << (bytesNumber * 8);
        currentByte = (regData & mask) >> (bytesNumber * 8);
        txBuf[i++] = currentByte;
    }
    bytesNumber = 3;
    SPI_Write(1, (unsigned char*)txBuf, bytesNumber + 1);
    AD7193_CS_HIGH;
}

/***************************************************************************//**
 * @brief Returns the result of a single conversion from AD7193.
 *
 * @param None.
 *
 * @return readData - Received data.
*******************************************************************************/
unsigned long AD7193_SingleConversion(void)
{
    unsigned long regData = 0x0;

    AD7193_CS_LOW;
    regData = AD7193_MODE_SEL(AD7193_MODE_CONT)       |
              AD7193_MODE_CLKSRC(AD7193_MODE_CLK_INT) |
              AD7193_MODE_REJ60;
    AD7193_Write(AD7193_REG_MODE, 3, regData);
    // Wait until data-ready signal appears.
    TIME_DelayMs(1);
    regData = AD7193_Read(AD7193_REG_DATA, 3);
    AD7193_CS_HIGH;

    return regData;
}

/***************************************************************************//**
 * @brief Reads the temperature from AD7193 device.
 *
 * @param None.
 *
 * @return ad7193TempValue - Current temperature read.
*******************************************************************************/
float AD7193_TemperatureRead(void)
{
    float         ad7193TempValue = 0;
    unsigned long temp            = 0;
    unsigned long regData         = 0x0;

    regData = AD7193_CONF_TEMP + AD7193_CONF_GAIN(AD7193_CONF_GAIN_1);
    AD7193_Write(AD7193_REG_CONF, 3, regData);
    temp = AD7193_SingleConversion();
    temp -= 0x800000;
    ad7193TempValue = (float)temp / 2815;   // Kelvin Temperature.
    ad7193TempValue -= 273;                 // Celsius Temperature.

    return ad7193TempValue;
}

/***************************************************************************//**
 * @brief Converts 24-bit raw data to millivolts.
 *
 * @param rawData  - 24-bit data sample.
 * @param vRef     - The value of the voltage reference used by the device in volts.
 * @param gain     - The set gain of the device.
 * @param polarity - The set polarity of the input voltage.
 *                   Example: 0 - Bipolar  mode.
 *                            1 - Unipolar mode.
 *
 * @return voltage - The result of the conversion expressed in millivolts.
*******************************************************************************/
float AD7193_ConvertToVolts(unsigned long rawData,
                            float         vRef,
                            unsigned char gain,
                            unsigned char polarity)
{
    float voltage = 0;

    if(polarity == 0)       // Bipolar mode
    {
        voltage = 1000 * (((float)rawData / (1ul << 23)) - 1) * vRef / gain;
    }
    else
    {
        if (polarity == 1)  // Unipolar mode
        {
            voltage = 1000 * ((float)rawData * vRef) / (1ul << 24) / gain;
        }
    }

    return voltage;
}

/***************************************************************************//**
 * @brief Reset the serial interface with the AD7193.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void AD7193_Reset(void)
{
    unsigned char bytesNumber = 6;
    unsigned char txBuf[6]    = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    AD7193_CS_LOW;
    SPI_Write(1, (unsigned char*)txBuf, bytesNumber);
    AD7193_CS_HIGH;
}

/***************************************************************************//**
 * @brief Updates the state of the ADG1414's switches.
 *
 * @param None.
 *
 * @return None.
*******************************************************************************/
void ADG1414_State(unsigned char switches)
{
    SPI_Init(0, 1000000, 0, 0);
    TIME_DelayMs(100);
    ADG1414_CS_LOW;
    SPI_Write(1, &switches, 1);
    ADG1414_CS_HIGH;
    SPI_Init(0, 1000000, 1, 0);
    TIME_DelayMs(100);
}

/***************************************************************************//**
 * @brief Initializes SPI and initial values for CN0209 board.
 *
 * @param None.
 *
 * @return Always returns success.
*******************************************************************************/
char CN0209_Init(void)
{
    char status = -1;

    AD7193_CS_OUT;
    ADT7310_CS_OUT;
    ADG1414_CS_OUT;

    AD7193_CS_HIGH;     // (Active Low)
    ADT7310_CS_HIGH;    // (Active Low)
    ADG1414_CS_HIGH;    // (Active Low)

    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 1, 0);
    AD7193_Reset();
    ADT7310_Reset();

    return status;
}

