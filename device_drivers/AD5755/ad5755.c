/***************************************************************************//**
 *   @file   AD5755.c
 *   @brief  Implementation of AD5755 Driver.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "AD5755.h"         // AD5755 definitions.
#include "AD5755_cfg.h"     // AD5755_cfg definitions.

AD5755_type_t this_device;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the device and powers-up all channels. The device is
 *        initialized with the values held by AD5755_InitialSettings structure.
 *
 * @return status - Result of the initialization procedure.
 *          Example: -1 - SPI peripheral was not initialized.
 *                    0 - SPI peripheral is initialized.
*******************************************************************************/
 char AD5755_Init(AD5755_type_t device)
 {
    unsigned char status  = -1;
    unsigned char channel = 0;
    AD5755_Setup *pAD5755_st = &AD5755_st;
    unsigned short dacControlBuff[4] = {0, 0, 0, 0};

    this_device = device;

    /* GPIO configuration. */
    AD5755_LDAC_OUT;
    AD5755_LDAC_LOW;
    AD5755_RESET_OUT;
    AD5755_RESET_HIGH;
    AD5755_CLEAR_OUT;
    AD5755_CLEAR_LOW;
    AD5755_POC_OUT;
    AD5755_POC_LOW;

    status = SPI_Init(0,        // Transfer format.
                      8000000,  // SPI clock frequency.
                      1,        // SPI clock polarity.
                      1);       // SPI clock edge.
    /* Device Setup. */
    /* Configure the POC bit, STATREAD bit and ShtCcLim bit. */
    AD5755_SetControlRegisters(AD5755_CREG_MAIN,
                               0,
                               (pAD5755_st->pocBit * AD5755_MAIN_POC) |
                               (pAD5755_st->statReadBit * AD5755_MAIN_STATREAD) |
                               AD5755_MAIN_SHTCCTLIM(pAD5755_st->shtCcLimBit));

    AD5755_Software_Reset();
    TIME_DelayUs(100);
    /* DC-to-DC configuration. */
    AD5755_SetControlRegisters(AD5755_CREG_DC_DC,
                               0,
                               (pAD5755_st->dcDcCompBit * AD5755_DC_DC_COMP) |
                               (AD5755_DC_DC_FREQ(pAD5755_st->dcDcFreqBit)) |
                               (AD5755_DC_DC_PHASE(pAD5755_st->dcDcPhaseBit)) |
                               (AD5755_DC_DC_MAX_V(pAD5755_st->dcDcMaxVBit)));
    /* Configure the DAC control register on a per channel basis. */
    for(channel = AD5755_DAC_A; channel <= AD5755_DAC_D; channel++)
    {
        if((this_device == ID_AD5755) || (this_device == ID_AD5755_1))
        {
            dacControlBuff[channel] = AD5755_DAC_INT_ENABLE |
                                      AD5755_DAC_CLR_EN |
                                      pAD5755_st->rsetBits[channel] |
                                      AD5755_DAC_DC_DC |
                                      pAD5755_st->ovrngBits[channel] |
                                      AD5755_DAC_R(AD5755_R_0_5_V);
        }
        else
        {
            dacControlBuff[channel] = AD5755_DAC_INT_ENABLE |
                                      AD5755_DAC_CLR_EN |
                                      pAD5755_st->rsetBits[channel] |
                                      AD5755_DAC_DC_DC |
                                      pAD5755_st->ovrngBits[channel] |
                                      AD5755_DAC_R(AD5755_R_4_20_MA);
        }
        AD5755_SetControlRegisters(AD5755_CREG_DAC,
                                  channel,
                                  dacControlBuff[channel]);
    }
    /* Allow at least 200us before enabling the channel output. */
   TIME_DelayUs(200);
    /* Enable the channel output. */
   for(channel = AD5755_DAC_A; channel <= AD5755_DAC_D; channel++)
   {
        /* Write to each DAC data register*/
        AD5755_SetRegisterValue(AD5755_DREG_WR_DAC,
                                channel,
                                0x0000);
        AD5755_SetControlRegisters(AD5755_CREG_DAC,
                                   channel,
                                   dacControlBuff[channel] | AD5755_DAC_OUTEN);
    }
    return status;
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param registerAddress - Address of the register.
 *                          Example:
 *                           AD5755_RD_DATA_REG(x)
 *                           AD5755_RD_CTRL_REG(x)
 *                           AD5755_RD_GAIN_REG(x)
 *                           AD5755_RD_OFFSET_REG(x)
 *                           AD5755_RD_CODE_REG(x)
 *                           AD5755_RD_SR_CTRL_REG(x)
 *                           AD5755_RD_STATUS_REG
 *                           AD5755_RD_MAIN_CTRL_REG
 *                           AD5755_RD_Dc_DC_CTRL_REG
 *                           x = any of AD5755_DAC_A, .. AD5755_DAC_D
 *
 * @return regValue - Value of the register.
*******************************************************************************/
 long AD5755_GetRegisterValue(unsigned char registerAddress)
 {
    unsigned char buffer[4] = {0, 0, 0, 0};
    unsigned long command = 0;
    long regValue = 0;
    unsigned char crc = 0;

    command = AD5755_ISR_READ |
              AD5755_ISR_DUT_AD1(AD5755_st.pinAD1state) |
              AD5755_ISR_DUT_AD0(AD5755_st.pinAD0state) |
              AD5755_ISR_RD(registerAddress);
    buffer[0] = (command & 0xFF0000) >> 16;
    buffer[1] = (command & 0x00FF00) >> 8;
    buffer[2] = (command & 0x0000FF) >> 0;
    if(AD5755_st.enablePacketErrorCheck)
    {
        buffer[3] = AD5755_CheckCrc(buffer, 3);
    }
    SPI_Write(AD5755_SLAVE_ID,
              buffer,
              3 + AD5755_st.enablePacketErrorCheck);
    command = AD5755_ISR_WRITE |
              AD5755_ISR_DUT_AD1(AD5755_st.pinAD1state) |
              AD5755_ISR_DUT_AD0(AD5755_st.pinAD0state) |
              AD5755_ISR_NOP;
    buffer[0] = (command & 0xFF0000) >> 16;
    buffer[1] = (command & 0x00FF00) >> 8;
    buffer[2] = (command & 0x0000FF) >> 0;
    if(AD5755_st.enablePacketErrorCheck)
    {
        buffer[3] = AD5755_CheckCrc(buffer, 3);
    }
    SPI_Read(AD5755_SLAVE_ID,
             buffer,
             3 + AD5755_st.enablePacketErrorCheck);
    regValue = ((unsigned short)buffer[1] << 8) + buffer[2];
    /* Check the CRC. */
    if(AD5755_st.enablePacketErrorCheck)
    {
        crc = AD5755_CheckCrc(&buffer[1], 3);
        if(crc != AD5755_CRC_CHECK_CODE)
        {
            regValue = -1;
        }
    }
return regValue;
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param registerAddress - Address of the register.
 *                          Example:
 *                          AD5755_DREG_WR_DAC
 *                          AD5755_DREG_WR_GAIN
 *                          AD5755_DREG_WR_GAIN_ALL
 *                          AD5755_DREG_WR_OFFSET
 *                          AD5755_DREG_WR_OFFSET_ALL
 *                          AD5755_DREG_WR_CLR_CODE
 *                          AD5755_DREG_WR_CTRL_REG
 *
 * @param channel -  Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param registerValue - Data value to write.
 *
 * @return None.
*******************************************************************************/
 unsigned short AD5755_SetRegisterValue(unsigned char registerAddress,
                                        unsigned char channel,
                                        unsigned short registerValue)
 {
    unsigned char buff[4] = {0, 0, 0, 0};
    unsigned long command = 0;
    unsigned short statusReg = 0;

    command = AD5755_ISR_WRITE |
              AD5755_ISR_DUT_AD1(AD5755_st.pinAD1state) |
              AD5755_ISR_DUT_AD0(AD5755_st.pinAD0state) |
              AD5755_ISR_DREG(registerAddress) |
              AD5755_ISR_DAC_AD(channel)|
              AD5755_ISR_DATA(registerValue);
    buff[0] = (command & 0xFF0000) >> 16;
    buff[1] = (command & 0x00FF00) >> 8;
    buff[2] = (command & 0x0000FF) >> 0;
    if(AD5755_st.enablePacketErrorCheck)
    {
        buff[3] = AD5755_CheckCrc(buff, 3);
    }
    if(AD5755_st.statReadBit == 0)
    {
        SPI_Write(AD5755_SLAVE_ID,
                  buff,
                  3 + AD5755_st.enablePacketErrorCheck);
    }
    else
    {
        SPI_Read(AD5755_SLAVE_ID,
                 buff,
                 3 + AD5755_st.enablePacketErrorCheck);
        statusReg = (buff[1] << 8) + buff[2];
    }

return statusReg;
}


/***************************************************************************//**
 * @brief Performs a software reset to the device.
 *
 * @return None.
*******************************************************************************/
 void AD5755_Software_Reset(void)
 {
    AD5755_SetControlRegisters(AD5755_CREG_SOFT, 0, AD5755_RESET_CODE);
 }

/***************************************************************************//**
 * @brief Enables/Disables watchdog timer and sets the timeout period.
 *
 * @param wtdEnable - Watchdog enable option.
 *                    Example: 0 - disables watchdog timer
 *                             1 - enabled watchdog timer
 * @param timeout - Timeout period for the watchdog timer.
 *                  Example: AD5755_WD_5MS
 *                           AD5755_WD_10MS
 *                           AD5755_WD_100MS
 *                           AD5755_WD_200MS
 *
 * @return None.
*******************************************************************************/
 void AD5755_WatchDogSetup(unsigned char wtdEnable, unsigned char timeout)
 {
    unsigned long oldMainCtrlReg = 0;
    unsigned long newMainCtrlReg = 0;

    oldMainCtrlReg = AD5755_GetRegisterValue(AD5755_RD_MAIN_CTRL_REG);
    oldMainCtrlReg &= ~(AD5755_MAIN_EWD | AD5755_MAIN_WD(3));
    newMainCtrlReg = oldMainCtrlReg |
                    (AD5755_MAIN_EWD * wtdEnable) |
                    AD5755_MAIN_WD(timeout);
    AD5755_SetControlRegisters(AD5755_CREG_MAIN,
                               0,
                               newMainCtrlReg);
}

/***************************************************************************//**
 * @brief Write a "service pulse" to the AD5755 watchdog timer when enabled.
 *
 * @return None.
*******************************************************************************/
void AD5755_FeedWatchDogTimer(void)
{
    AD5755_SetControlRegisters(AD5755_CREG_SOFT, 0, AD5755_SPI_CODE);
}

/***************************************************************************//**
 * @brief Configures one of the control registers.
 *
 * @param ctrlRegAddress - Control Register Address.
 *                         Example:
 *                         AD5755_CREG_SLEW
 *                         AD5755_CREG_MAIN
 *                         AD5755_CREG_DAC
 *                         AD5755_CREG_DC_DC
 *                         AD5755_CREG_SOFT
 *
 * @param channel - Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param regValue - Value to be written to the selected Control Register.
 *
 * @return None.
*******************************************************************************/
void AD5755_SetControlRegisters(unsigned char  ctrlRegAddress,
                                unsigned char  channel,
                                unsigned short regValue)
{
    AD5755_SetRegisterValue(AD5755_DREG_WR_CTRL_REG,
                            channel,
                            AD5755_CTRL_CREG(ctrlRegAddress) | regValue);
}

/***************************************************************************//**
 * @brief Computes the CRC for a data buffer.
 *
 * @param data - Data buffer.
 * @param bytesNumber - Data buffer size in bytes.
 *
 * @return The computed CRC.
*******************************************************************************/
unsigned char AD5755_CheckCrc(unsigned char* data, unsigned char bytesNumber)
{
  unsigned char crc  = 0x00;
  unsigned char byte = 0;
  unsigned char bit  = 0;

  /* Calculates 8-Bit checksum with given polynomial. */
  for(byte = 0; byte < bytesNumber; byte++)
  {
    crc ^= (data[byte]);
    for(bit = 8; bit > 0; bit--)
    {
        if (crc & 0x80)
        {
            crc = (crc << 1) ^ AD5755_CRC_POLYNOMIAL;
        }
        else
        {
            crc = (crc << 1);
        }
    }
}

return crc;
}

/***************************************************************************//**
 * @brief Allows power-up/down of the dc-to-dc converter, DAC and internal
 *        amplifiers for the selected channel.
 *
 * @param channel - Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param pwrStatus - Power mode.
 *                    Example: 0 - power-down the channel;
 *                             1 - power-up the channel.
 *
 * @return None.
*******************************************************************************/
 void AD5755_SetChannelPower(unsigned char channel, unsigned char pwrStatus)
 {
    unsigned long  oldDacCtrlReg = 0;
    unsigned long  newDacCtrlReg = 0;

    /* Read the content of the DAC Control Register of the selected channel. */
    oldDacCtrlReg = AD5755_GetRegisterValue(AD5755_RD_CTRL_REG(channel));
    oldDacCtrlReg &= ~(AD5755_DAC_INT_ENABLE |
                       AD5755_DAC_DC_DC |
                       AD5755_DAC_OUTEN);
    newDacCtrlReg = oldDacCtrlReg | (pwrStatus * (AD5755_DAC_INT_ENABLE |
                                                  AD5755_DAC_DC_DC |
                                                  AD5755_DAC_OUTEN));
    AD5755_SetControlRegisters(AD5755_CREG_DAC,
                               AD5755_DAC_A,
                               newDacCtrlReg);
}

/***************************************************************************//**
 * @brief Sets the range of a channel.
 *
 * @param channel - Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param range - Range option.
 *                Example:
 *                AD5755_R_0_5_V      - 0 V to 5 V voltage range (default)
 *                AD5755_R_0_10_V     - 0 V to 10 V voltage range
 *                AD5755_R_M5_P5_V    - -5 V to +5 V voltage range
 *                AD5755_R_M10_P10_V  - -10 V to 10 V voltage range
 *                AD5755_R_4_20_MA    - 4 mA to 20 mA current range
 *                AD5755_R_0_20_MA    - 0 mA to 20 mA current range
 *                AD5755_R_0_24_MA    - 0 mA to 24 mA current range
 *
 * @return None.
*******************************************************************************/
void AD5755_SetChannelRange(unsigned char channel, unsigned char range)
{
    unsigned short outputCode = 0x0000;
    unsigned long  oldDacCtrlReg = 0;
    unsigned long  newDacCtrlReg = 0;

    /* Read the content of the DAC Control Register of the selected channel. */
    oldDacCtrlReg = AD5755_GetRegisterValue(AD5755_RD_CTRL_REG(channel));
    /* Clear the bits that will be modified by this function. */
    oldDacCtrlReg &= ~(AD5755_DAC_INT_ENABLE |
                       AD5755_DAC_OUTEN |
                       AD5755_DAC_DC_DC |
                       AD5755_DAC_R(7));
    /* Select the output code before changing the range. */
    if((range == AD5755_R_M5_P5_V) || (range == AD5755_R_M10_P10_V))
    {
        outputCode = 0x8000;
    }
    /* Set the output code to zero or midscale. */
    AD5755_SetRegisterValue(AD5755_DREG_WR_DAC, channel, outputCode);
    /* Set range. */
    newDacCtrlReg = oldDacCtrlReg |
                    AD5755_DAC_INT_ENABLE |
                    AD5755_DAC_DC_DC |
                    AD5755_DAC_R(range);
    AD5755_SetControlRegisters(AD5755_CREG_DAC, channel, newDacCtrlReg);
    /* Set the output code to zero or midscale. */
    AD5755_SetRegisterValue(AD5755_DREG_WR_DAC, channel, outputCode);
    TIME_DelayUs(200);
    /* Enable the output of the channel. */
    newDacCtrlReg |= AD5755_DAC_OUTEN;
    AD5755_SetControlRegisters(AD5755_CREG_DAC, channel, newDacCtrlReg);
}

/***************************************************************************//**
 * @brief Selects if the channel clears when CLEAR pin is activated.
 *
 * @param channel - Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param clearEn - Clear Enable option.
 *                  Example:
 *                   1 - channel clears when the part is cleared;
 *                   0 - channel does not clear when the part is cleared.
 *
 * @return None.
*******************************************************************************/
void AD5755_ChannelClearEnable(unsigned char channel, unsigned char clearEn)
{
    unsigned long  oldDacCtrlReg = 0;
    unsigned long  newDacCtrlReg = 0;

    /* Read the content of the DAC Control Register of the selected channel. */
    oldDacCtrlReg = AD5755_GetRegisterValue(AD5755_RD_CTRL_REG(channel));
    /* Clear the CLR_EN bit. */
    oldDacCtrlReg &= ~(AD5755_DAC_CLR_EN);
    newDacCtrlReg |= oldDacCtrlReg | (clearEn * AD5755_DAC_CLR_EN);
    AD5755_SetControlRegisters(AD5755_CREG_DAC, channel, newDacCtrlReg);
}

/***************************************************************************//**
 * @brief Configures the Digital Slew Rate Control.
 *
 * @param channel - Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param srEn - Enable/Disable the Slew Rate Control.
 *               Example: 0 - disable feature;
 *                        1 - enable feature.
 * @param updtFreq - Update Clock Frequency(Hz).
 *                   Example: AD5755_SR_CLK_64K
 *                            AD5755_SR_CLK_32k
 *                            ...
 *                            AD5755_SR_CLK_8
 *                            AD5755_SR_CLK_4
 *                            AD5755_SR_CLK_0_5
 * @param stepSize - Step Size (LSBs).
 *                   Example: AD5755_STEP_1
 *                            AD5755_STEP_2
 *                            ...
 *                            AD5755_STEP_128
 *                            AD5755_STEP_256
 *
 * @return None.
*******************************************************************************/
void AD5755_SlewRateCtrl(char channel, char srEn, char updtFreq, char stepSize)
{
    AD5755_SetControlRegisters(AD5755_CREG_SLEW,
                               channel,
                               srEn * (AD5755_SLEW_SREN) |
                               AD5755_SLEW_SR_CLOCK(updtFreq) |
                               AD5755_SLEW_SR_STEP(stepSize));
}

/***************************************************************************//**
 * @brief Sets the output voltage of a channel.
 *
 * @param channel - Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param voltage - Value to be outputted by the DAC(Volts).
 *
 * @return The actual voltage value that can be outputted by the channel.
*******************************************************************************/
float AD5755_SetVoltage(unsigned char channel, float voltage)
{
    unsigned long offset      = 0;
    unsigned long gain        = 0;
    unsigned long dacVal      = 0;
    unsigned long code        = 0;
    unsigned char range       = 0;
    unsigned char resolution  = 0;
    unsigned long rangeOffset = 0;
    float         vRef        = 0;
    float         realVoltage = 0;

    if((this_device == ID_AD5755) || (this_device == ID_AD5755_1))
    {
        /* Get the offset, gain and range of the selected channel. */
        offset = AD5755_GetRegisterValue(AD5755_RD_OFFSET_REG(channel));
        gain = AD5755_GetRegisterValue(AD5755_RD_GAIN_REG(channel));
        range = AD5755_GetRegisterValue(AD5755_RD_CTRL_REG(channel)) & 0x7;
        switch(range)
        {
            case AD5755_R_0_5_V :
            {
                rangeOffset = 0;
                vRef = 5.0;
                resolution = 16;
                break;
            }
            case AD5755_R_0_10_V :
            {
                rangeOffset = 0;
                vRef = 10.0;
                resolution = 16;
                break;
            }
            case AD5755_R_M5_P5_V :
            {
                rangeOffset = 0x8000;
                vRef = 5.0;
                resolution = 15;
                break;
            }
            case AD5755_R_M10_P10_V :
            {
                rangeOffset = 0x8000;
                vRef = 10.0;
                resolution = 15;
                break;
            }
            default :
            {
                rangeOffset = 0;
                vRef = 0;
                resolution = 0;
                break;
            }
        }
        /* Compute the binary code from the users voltage value. */
        code = (long)(voltage * (1l << resolution) / vRef) + rangeOffset;
        if(code > 0xFFFF)
        {
            code = 0xFFFF;
        }
        /* Offset and Gain are used to obtain the correct value to be written
         to the DAC register in order to output the voltage desired by the user.
        */
        if((code + (1l << 15) - offset) > 0)    // Avoid negative values
        {
            dacVal = (code + (1l << 15) - offset) * (1l << 16) / (gain + 1);
        }
        else
        {
            dacVal = 0;
        }

        /* Write to the Data Register of the DAC. */
        AD5755_SetRegisterValue(AD5755_DREG_WR_DAC,
                                channel,
                                dacVal);
        realVoltage = ((long)(code - rangeOffset) * vRef) / (1l << resolution);
    }
return realVoltage;
}

/***************************************************************************//**
 * @brief Sets the output current of a channel.
 *
 * @param channel - Channel option.
 *                  Example: AD5755_DAC_A
 *                           AD5755_DAC_B
 *                           AD5755_DAC_C
 *                           AD5755_DAC_D
 * @param mACurrent - Value to be outputted by the DAC(milliampere).
 *
 * @return The actual current value that can be outputted by the channel.
*******************************************************************************/
float AD5755_SetCurrent(unsigned char channel, float mACurrent)
{
    long  offset      = 0;
    long  gain        = 0;
    long  range       = 0;
    long  dacVal      = 0;
    long  code        = 0;
    char  rangeOffset = 0;
    float iRef        = 0;
    float realCurrent = 0;

    /* Get the offset, gain and range of the selected channel. */
    offset = AD5755_GetRegisterValue(AD5755_RD_OFFSET_REG(channel));
    gain = AD5755_GetRegisterValue(AD5755_RD_GAIN_REG(channel));
    range = AD5755_GetRegisterValue(AD5755_RD_CTRL_REG(channel)) & 0x7;
    switch(range)
    {
        case AD5755_R_4_20_MA :
        {
            iRef = 16.0;        // mA
            rangeOffset = 4;    // mA
            break;
        }
        case AD5755_R_0_20_MA :
        {
            iRef = 20.0;        // mA
            rangeOffset = 0;    // mA
            break;
        }
        case AD5755_R_0_24_MA :
        {
            iRef = 24.0;        // mA
            rangeOffset = 0;    // mA
            break;
        }
        default :
        {
            iRef = 1;
            rangeOffset = 0;
            break;
        }
    }
    /* Compute the binary code from the value(mA) provided by user. */
        code = (long)((mACurrent - rangeOffset) * (1l << 16) / iRef);
        if(code > 0xFFFF)
        {
            code = 0xFFFF;
        }
    /* Offset and Gain are used to obtain the correct value to be written to the
       DAC register in order to output the current desired by the user. */
    if((code + (1l << 15) - offset) > 0)    // Avoid negative values
    {
        dacVal = (code + (1l << 15) - offset) * (1l << 16) / (gain + 1);
    }
    else
    {
        dacVal = 0;
    }
    /* Write to the Data Register of the DAC. */
    AD5755_SetRegisterValue(AD5755_DREG_WR_DAC,
                            channel,
                            dacVal);

    realCurrent = (code * iRef / (float)(1l << 16)) + rangeOffset;

    return realCurrent;
}
