/***************************************************************************//**
 *   @file   AD5781.c
 *   @brief  Implementation of AD5781 Driver.
 *   @author DNechita (Dan.Nechita@analog.com)
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
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "AD5781.h"    // AD5781 definitions.
#include "TIME.h"      // TIME definitions.

/***************************************************************************//**
 * @brief Initializes the communication with the device.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
long AD5781_Init(void)
{  
    long status = -1;

    AD5781_RESET_OUT;
    AD5781_RESET_HIGH;
    AD5781_LDAC_OUT;
    AD5781_LDAC_HIGH;
    AD5781_CLR_OUT;
    AD5781_CLR_HIGH;

    status = (long)SPI_Init(0,        // transfer format(MSB first)
                            1000000,  // SPI clock frequency(1 MHz)
                            1,        // SPI clock polarity(Idle High)
                            1);       // SPI clock edge(active->idle edge)
    
    return status;
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param registerAddress - Address of the register.
 *                          Example:
 *                          AD5781_REG_DAC          - DAC register
 *                          AD5781_REG_CTRL         - Control register
 *                          AD5781_REG_CLR_CODE     - Clearcode register
 *                          AD5781_CMD_WR_SOFT_CTRL - Software control register
 * @param registerValue   - Value of the register.
 *
 * @return Returns 0 in case of success or negative error code.
*******************************************************************************/
long AD5781_SetRegisterValue(unsigned char registerAddress,
                             unsigned long registerValue)
{
    unsigned char writeCommand[3] = {0, 0, 0};
    unsigned long spiWord         = 0;
    char          status          = 0;

    spiWord = AD5781_WRITE |
              AD5781_ADDR_REG(registerAddress) |
              (registerValue & 0xFFFFF);
    writeCommand[0] = (spiWord >> 16) & 0x0000FF;
    writeCommand[1] = (spiWord >>  8) & 0x0000FF;
    writeCommand[2] = (spiWord >>  0) & 0x0000FF;
    status = SPI_Write(AD5781_SLAVE_ID, writeCommand, 3);
    if(status != 3)
    {
        return -1;
    }

    return 0;
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param registerAddress - Address of the register.
 *                          Example:
 *                          AD5781_REG_DAC          - DAC register
 *                          AD5781_REG_CTRL         - Control register
 *                          AD5781_REG_CLR_CODE     - Clearcode register
 *                          AD5781_CMD_WR_SOFT_CTRL - Software control register
 *
 * @return dataRead       - The register's value or negative error code.
*******************************************************************************/
long AD5781_GetRegisterValue(unsigned char registerAddress)
{
    unsigned char registerWord[3] = {0, 0, 0};
    unsigned long dataRead        = 0x0;
    char          status          = 0;

    registerWord[0] = (AD5781_READ | AD5781_ADDR_REG(registerAddress)) >> 16;
    status = SPI_Write(AD5781_SLAVE_ID, registerWord, 3);
    if(status != 3)
    {
        return -1;
    }
    registerWord[0] = 0x00;
    registerWord[1] = 0x00;
    registerWord[2] = 0x00;
    status = SPI_Read(AD5781_SLAVE_ID, registerWord, 3);
    if(status != 3)
    {
        return -1;
    }
    dataRead = ((long)registerWord[0] << 16) |
               ((long)registerWord[1] <<  8) |
               ((long)registerWord[2] <<  0);

    return dataRead;
}

/***************************************************************************//**
 * @brief Sets the DAC output in one of the three states.
 *
 * @param state - The output state.
 *                Example:
 *                AD5781_OUT_NORMAL     - normal operation mode
 *                AD5781_OUT_CLAMPED_6K - output is clamped via ~6KOhm to AGND
 *                AD5781_OUT_TRISTATE   - output is in tristate
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
long AD5781_DacOuputState(unsigned char state)
{
    unsigned long oldCtrl = 0;
    unsigned long newCtrl = 0;
    long          status  = 0;
    
    status = AD5781_GetRegisterValue(AD5781_REG_CTRL);
    if(status < 0)
    {
        return status;
    }
    oldCtrl = status;
    /* Clear DACTRI and OPGND bits. */
    oldCtrl = oldCtrl & ~(AD5781_CTRL_DACTRI | AD5781_CTRL_OPGND);
    /* Sets the new state provided by the user. */
    newCtrl = oldCtrl |
              ((state << 1) & (AD5781_CTRL_DACTRI | AD5781_CTRL_OPGND));
    status = AD5781_SetRegisterValue(AD5781_REG_CTRL, newCtrl);

    return status;
}

/***************************************************************************//**
 * @brief Writes to the DAC register.
 *
 * @param value - The value to be written to DAC.
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
long AD5781_SetDacValue(unsigned long value)
{
    long status = 0;

    AD5781_LDAC_LOW;
    status = AD5781_SetRegisterValue(AD5781_REG_DAC, AD5781_DAC_DATA(value));
    AD5781_LDAC_HIGH;

    return status;
}

/***************************************************************************//**
 * @brief Asserts RESET, CLR or LDAC in a software manner.
 *
 * @param instructionBit - A Software Control Register bit.
 *                         Example:
 *                         AD5781_SOFT_CTRL_LDAC  - Load DAC
 *                         AD5781_SOFT_CTRL_CLR   - Clear
 *                         AD5781_SOFT_CTRL_RESET - Reset
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
long AD5781_SoftInstruction(unsigned char instructionBit)
{
    long status = 0;

    status = AD5781_SetRegisterValue(AD5781_CMD_WR_SOFT_CTRL, instructionBit);
    if(status < 0)
    {
        return status;
    }
    TIME_DelayMs(1);    // Wait for the instruction to take effect.

    return status;
}

/***************************************************************************//**
 * @brief Configures the output amplifier, DAC coding, SDO state and the
 *        linearity error compensation.
 *
 * @param setupWord - Is a 24-bit value that sets or clears the Control Register
 *                    bits : RBUF bit(AD5781_CTRL_RBUF),
 *                           BIN/2sC bit(AD5781_CTRL_BIN2SC),
 *                           SDODIS bit(AD5781_CTRL_SDODIS) and
 *                           LINCOMP bits(AD5781_CTRL_LINCOMP(x)).
 *                    Example: AD5781_CTRL_BIN2SC | AD5781_CTRL_RBUF - sets
 *                             the DAC register to use offset binary coding and 
 *                             powers down the internal output amplifier.
 *
 * @return Negative error code or 0 in case of success.
*******************************************************************************/
long AD5781_Setup(unsigned long setupWord)
{
    unsigned long oldCtrl = 0;
    unsigned long newCtrl = 0;
    long          status  = 0;

    /* Reads the control register in order to save the options related to the
       DAC output state that may have been configured previously. */
     status = AD5781_GetRegisterValue(AD5781_REG_CTRL);
     if(status < 0)
     {
         return status;
     }
     oldCtrl = status;
    /* Clear LINCOMP, SDODIS, BIN2SC and RBUF bits. */
    oldCtrl = oldCtrl & ~(AD5781_CTRL_LINCOMP(-1) |
                          AD5781_CTRL_SDODIS |
                          AD5781_CTRL_BIN2SC |
                          AD5781_CTRL_RBUF);
    /* Sets the new state provided by the user. */
    newCtrl = oldCtrl | setupWord;
    status = AD5781_SetRegisterValue(AD5781_REG_CTRL, newCtrl);

    return status;
}
