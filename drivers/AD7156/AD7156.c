/***************************************************************************//**
 *   @file   AD7156.c
 *   @brief  Implementation of AD7156 Driver.
 *   @author DNechita(Dan.Nechita@analog.com)
********************************************************************************
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "AD7156.h"
#include "Communication.h"

/******************************************************************************/
/************************ Variables Declarations ******************************/
/******************************************************************************/
float ad7156Channel1Range = 2;
float ad7156Channel2Range = 2;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Performs a burst read of a specified number of registers.
 *
 * @param pReadData       - The read values are stored in this buffer.
 * @param registerAddress - The start address of the burst read.
 * @param bytesNumber     - Number of bytes to read.
 *
 * @return None.
*******************************************************************************/
void AD7156_GetRegisterValue(unsigned char* pReadData,
                             unsigned char registerAddress,
                             unsigned char bytesNumber)
{
    I2C_Write(AD7156_ADDRESS, &registerAddress, 1, 0);
    I2C_Read(AD7156_ADDRESS, pReadData, bytesNumber, 1);
}

/***************************************************************************//**
 * @brief Writes data into one or two registers.
 *
 * @param registerValue   - Data value to write.
 * @param registerAddress - Address of the register.
 * @param bytesNumber     - Number of bytes. Accepted values: 0 - 1.
 *
 * @return None.
*******************************************************************************/
void AD7156_SetRegisterValue(unsigned short registerValue,
                             unsigned char  registerAddress,
                             unsigned char  bytesNumber)
{
    unsigned char dataBuffer[3] = {0, 0, 0};
    
    dataBuffer[0] = registerAddress;
    dataBuffer[1] = (registerValue >> 8);
    dataBuffer[bytesNumber] = (registerValue & 0x00FF);
    I2C_Write(AD7156_ADDRESS, dataBuffer, bytesNumber + 1, 1);
}

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the device is
 *        present.
 *
 * @return status - The result of the initialization procedure.
 *                  Example: -1 - I2C peripheral was not initialized or the
 *                                 device is not present.
 *                            0 - I2C peripheral was initialized and the
 *                                 device is present.
*******************************************************************************/
char AD7156_Init(void)
{
    char          status = -1;
    unsigned char test   = 0;
    
    status = I2C_Init(100000);
    AD7156_GetRegisterValue(&test, AD7156_REG_CHIP_ID, 1);
    if(test != AD7156_DEFAULT_ID)
    {
        status = -1;
    }
    return status;
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return None.
*******************************************************************************/
void AD7156_Reset(void)
{
    unsigned char addressPointer = 0;

    addressPointer = AD7156_RESET_CMD;
    I2C_Write(AD7156_ADDRESS, &addressPointer, 1, 1);
}

/***************************************************************************//**
 * @brief Sets the converter mode of operation.
 *
 * @oaram pwrMode - Mode of operation option.
 *		    Example: AD7156_CONV_MODE_IDLE - Idle
 *                           AD7156_CONV_MODE_CONT_CONV  - Continuous conversion
 *                           AD7156_CONV_MODE_SINGLE_CONV - Single conversion
 *                           AD7156_CONV_MODE_PWR_DWN - Power-down
 *
 * @return None.
*******************************************************************************/
void AD7156_SetPowerMode(unsigned char pwrMode)
{
    unsigned char oldConfigReg = 0;
    unsigned char newConfigReg = 0;

    AD7156_GetRegisterValue(&oldConfigReg, AD7156_REG_CONFIG, 1);
    oldConfigReg &= ~AD7156_CONFIG_MD(0x3);
    newConfigReg = oldConfigReg| AD7156_CONFIG_MD(pwrMode);
    AD7156_SetRegisterValue(newConfigReg, AD7156_REG_CONFIG, 1);
}

/***************************************************************************//**
 * @brief Enables or disables conversion on the selected channel.
 *
 * @param channel    - Channel option.
 *                      Example: AD7156_CHANNEL1
 *                               AD7156_CHANNEL2
 * @param enableConv - The state of channel activity.
 *                      Example: 0 - disable conversion on selected channel.
 *                               1 - enable conversion on selected channel.
 *
 * @return None.
*******************************************************************************/
void AD7156_ChannelState(unsigned char channel, unsigned char enableConv)
{
    unsigned char oldConfigReg = 0;
    unsigned char newConfigReg = 0;
    unsigned char channelMask  = 0;
    
    channelMask = (channel == 1) ? AD7156_CONFIG_EN_CH1 : AD7156_CONFIG_EN_CH2;

    AD7156_GetRegisterValue(&oldConfigReg, AD7156_REG_CONFIG, 1);
    oldConfigReg &= ~channelMask;
    newConfigReg = oldConfigReg | (channelMask * enableConv);
    AD7156_SetRegisterValue(newConfigReg, AD7156_REG_CONFIG, 1);
}

/***************************************************************************//**
 * @brief Sets the input range of the specified channel.
 *
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2 
 * @param range   - Input range option.
 *                  Example: AD7156_CDC_RANGE_2_PF   - 2pF input range.
 *                           AD7156_CDC_RANGE_0_5_PF - 0.5pF input range.
 *                           AD7156_CDC_RANGE_1_PF   - 1pF input range.
 *                           AD7156_CDC_RANGE_4_PF   - 4pF input range.
 *
 * @return None.
*******************************************************************************/
void AD7156_SetRange(unsigned channel, unsigned char range)
{
    unsigned char oldSetupReg = 0;
    unsigned char newSetupReg = 0;
    unsigned char regAddress  = 0;

    regAddress = (channel == 1) ? AD7156_REG_CH1_SETUP : AD7156_REG_CH2_SETUP;
    AD7156_GetRegisterValue(&oldSetupReg, regAddress, 1);
    oldSetupReg &= ~AD7156_CH1_SETUP_RANGE(0x3);
    newSetupReg = oldSetupReg | AD7156_CH1_SETUP_RANGE(range);
    AD7156_SetRegisterValue(newSetupReg, regAddress, 1);
    /* Update global variables that hold range information. */
    if(channel == 1)
    {
        ad7156Channel1Range = AD7156_GetRange(channel);
    }
    else
    {
        ad7156Channel2Range = AD7156_GetRange(channel);
    }
}

/***************************************************************************//**
 * @brief Reads the range bits from the device and returns the range in pF.
 *
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2 
 *
 * @return The capacitive input range(pF).
*******************************************************************************/
float AD7156_GetRange(unsigned channel)
{
    unsigned char setupReg    = 0;
    unsigned char regAddress  = 0;
    float range = 0;
    
    regAddress = (channel == 1) ? AD7156_REG_CH1_SETUP : AD7156_REG_CH2_SETUP;
    AD7156_GetRegisterValue(&setupReg, regAddress, 1);
    setupReg = (setupReg & AD7156_CH1_SETUP_RANGE(0x3)) >> 6;
    switch(setupReg)
    {
        case AD7156_CDC_RANGE_2_PF:
            range =  2.0;
            break;
        case AD7156_CDC_RANGE_0_5_PF:
            range = 0.5;
            break;
        case AD7156_CDC_RANGE_1_PF:
            range =  1.0;
            break;
        case AD7156_CDC_RANGE_4_PF:
            range =  4.0;
            break;
    }
    /* Update global variables that hold range information. */
    if(channel == 1)
    {
        ad7156Channel1Range = range;
    }
    else
    {
        ad7156Channel2Range = range;
    }

    return range;
}

/***************************************************************************//**
 * @brief Selects the threshold mode of operation.
 *
 * @param thrMode  - Output comparator mode.
 *                   Example: AD7156_THR_MODE_NEGATIVE
 *                            AD7156_THR_MODE_POSITIVE
 *                            AD7156_THR_MODE_IN_WINDOW
 *                            AD7156_THR_MODE_OU_WINDOW
 * @param thrFixed - Selects the threshold mode.
 *                   Example: AD7156_ADAPTIVE_THRESHOLD
 *                            AD7156_FIXED_THRESHOLD
 *
 * @return None.
*******************************************************************************/
void AD7156_SetThresholdMode(unsigned char thrMode, unsigned char thrFixed)
{
    unsigned char oldConfigReg = 0;
    unsigned char newConfigReg = 0;

    AD7156_GetRegisterValue(&oldConfigReg, AD7156_REG_CONFIG, 1);
    oldConfigReg &= ~(AD7156_CONFIG_THR_FIXED | AD7156_CONFIG_THR_MD(0x3));
    newConfigReg = oldConfigReg |
                   (AD7156_CONFIG_THR_FIXED * thrFixed) |
                   (AD7156_CONFIG_THR_MD(thrMode));
    AD7156_SetRegisterValue(newConfigReg, AD7156_REG_CONFIG, 1);
}

/***************************************************************************//**
 * @brief Writes to the threshold register when threshold fixed mode is enabled.
 *
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @param pFthr   - The threshold value in picofarads(pF). The value must not be
 *                  out of the selected input range.
 *
 * @return None.
*******************************************************************************/
void AD7156_SetThreshold(unsigned char channel, float pFthr)
{
    unsigned char  thrRegAddress  = 0;
    unsigned short rawThr         = 0;
    float  range                  = 0;

    thrRegAddress = (channel == 1) ? AD7156_REG_CH1_SENS_THRSH_H :
                                     AD7156_REG_CH2_SENS_THRSH_H;
    range = AD7156_GetRange(channel);
    rawThr = (unsigned short)((pFthr * 0xA000 / range) + 0x3000);
    if(rawThr > 0xD000)
    {
        rawThr = 0xD000;
    }
    else if(rawThr < 0x3000)
    {
        rawThr = 0x3000;
    }
    AD7156_SetRegisterValue(rawThr, thrRegAddress, 2);
}

/***************************************************************************//**
 * @brief Writes a value(pF) to the sensitivity register. This functions 
 * should be used when adaptive threshold mode is selected.
 *
 * @param channel       - Channel option.
 *                        Example: AD7156_CHANNEL1
 *                                 AD7156_CHANNEL2
 * @param pFsensitivity - The sensitivity value in picofarads(pF).
 *
 * @return None.
*******************************************************************************/
void AD7156_SetSensitivity(unsigned char channel, float pFsensitivity)
{
    unsigned char  sensitivityRegAddr = 0;
    unsigned short rawSensitivity     = 0;
    float range = 0;

    sensitivityRegAddr = (channel == 1) ? AD7156_REG_CH1_SENS_THRSH_H :
                                          AD7156_REG_CH2_SENS_THRSH_H;
    range = (channel == 1) ? ad7156Channel1Range : ad7156Channel2Range;
    rawSensitivity = (unsigned short)(pFsensitivity * 0xA00 / range);
    rawSensitivity = (rawSensitivity << 4) & 0x0FF0;
    AD7156_SetRegisterValue(rawSensitivity, sensitivityRegAddr, 2);
}

/***************************************************************************//**
 * @brief Reads a 12-bit sample from the selected channel.
 *
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @return Conversion result form the selected channel.
*******************************************************************************/
unsigned short AD7156_ReadChannelData(unsigned char channel)
{
    unsigned short chResult   = 0;
    unsigned char  regData[2] = {0, 0};
    unsigned char  chAddress  = 0;

    if(channel == 1)
    {
        chAddress = AD7156_REG_CH1_DATA_H;
    }
    else
    {
        chAddress = AD7156_REG_CH2_DATA_H;
    }
    AD7156_GetRegisterValue(regData, chAddress, 2);
    chResult = (regData[0] << 8) + regData[1];

    return chResult;
}

/***************************************************************************//**
 * @brief Waits for a finished CDC conversion and reads a 12-bit sample from
 *        the selected channel.
 *
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @return Conversion result form the selected channel.
*******************************************************************************/
unsigned short AD7156_WaitReadChannelData(unsigned char channel)
{
    unsigned short chResult   = 0;
    unsigned char  regData[2] = {0, 0};
    unsigned char  status     = 0;
    unsigned char  chRdyMask  = 0;
    unsigned char  chAddress  = 0;

    if(channel == 1)
    {
        chRdyMask = AD7156_STATUS_RDY1;
        chAddress = AD7156_REG_CH1_DATA_H;
    }
    else
    {
        chRdyMask = AD7156_STATUS_RDY2;
        chAddress = AD7156_REG_CH2_DATA_H;
    }
    do
    {
        AD7156_GetRegisterValue(&status, AD7156_REG_STATUS, 1);
    }while((status & chRdyMask) != 0);
    AD7156_GetRegisterValue(regData, chAddress, 2);
    chResult = (regData[0] << 8) + regData[1];

    return chResult;
}

/***************************************************************************//**
 * @brief Reads a sample the selected channel and converts the data to
 *        picofarads(pF).
 *
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @return Conversion result form the selected channel as picofarads(pF).
*******************************************************************************/
float AD7156_ReadChannelCapacitance(unsigned char channel)
{
    unsigned short rawCh = 0;
    float chRange = 0;
    float pFdata = 0;

    chRange = (channel == 1) ? ad7156Channel1Range : ad7156Channel2Range;
    rawCh = AD7156_ReadChannelData(channel);
    if(rawCh < 0x3000)
    {
        rawCh= 0x3000;
    }
    else if(rawCh > 0xD000)
    {
        rawCh = 0xD000;
    }
    pFdata = (((rawCh) - 0x3000) * chRange) / 0xA000;

    return pFdata;
}

/***************************************************************************//**
 * @brief Waits for a finished CDC conversion the selected channel, reads a
 *        sample and converts the data to picofarads(pF).
 *
 * @param channel - Channel option.
 *                  Example: AD7156_CHANNEL1
 *                           AD7156_CHANNEL2
 * @return Conversion result form the selected channel as picofarads(pF).
*******************************************************************************/
float AD7156_WaitReadChannelCapacitance(unsigned char channel)
{
    unsigned short rawCh = 0;
    float chRange = 0;
    float pFdata = 0;

    chRange = (channel == 1) ? ad7156Channel1Range : ad7156Channel2Range;
    rawCh = AD7156_WaitReadChannelData(channel);
    if(rawCh < 0x3000)
    {
        rawCh= 0x3000;
    }
    else if(rawCh > 0xD000)
    {
        rawCh = 0xD000;
    }
    pFdata = (((rawCh) - 0x3000) * chRange) / 0xA000;

    return pFdata;
}
