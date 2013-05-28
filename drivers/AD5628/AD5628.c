/***************************************************************************//**
 *   @file   AD5628.c
 *   @brief  Implementation of AD5628 Driver.
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
#include "AD5628.h"    // AD5628 definitions.

/***************************************************************************//**
 * @brief Resets the device and performs several initializations.
 *
 * @return Result of the initialization procedure.
 *			Example: -1- SPI peripheral was not initialized.
 *				  0- SPI peripheral is initialized.
*******************************************************************************/
char AD5628_Init(void)
{
    char status = -1;
    
    /* Initializes communication. */
    status = SPI_Init(0, 1000000, 1, 1);
    /* Behaves as a power-on reset. */
    AD5628_Reset();
    /* Turns on the on-board reference. */
    AD5628_SetInputRegister(AD5628_CMD(AD5628_CMD_SET_INT_REF)|
                            AD5628_INT_REF_ON);
    /* Clear code is set to 0x0000. */
    AD5628_SetInputRegister(AD5628_CMD(AD5628_CMD_LOAD_CLEAR_CODE)|
                            AD5628_CODE_0X0000);

    return status; 
}

/***************************************************************************//**
 * @brief Sets the device in a specific power mode.
 *
 * @param pwrMode - power mode of the device.
 *                  Example: AD5628_PWRDN_NONE
 *                           AD5628_PWRDN_1K
 *                           AD5628_PWRDN_100K
 *                           AD5628_PWRDN_3STATE
 *
 * @param channel - The channel or channels that are being configured.
 *                  Example:  AD5628_ADDR_DAC_A
 *                            AD5628_ADDR_DAC_B
 *                            ...
 *                            AD5628_ADDR_DAC_ALL
 *
 * @return none.
*******************************************************************************/
void AD5628_PowerMode(unsigned char pwrMode, unsigned char channel)
{
    unsigned char selectedChannel = 0;

    if(channel == AD5628_ADDR_DAC_ALL)
    {
        selectedChannel = 0xFF;
    }
    else
    {
        selectedChannel = (1 << channel);
    }
    /* Selects a power mode for the selected channel. */
    AD5628_SetInputRegister(AD5628_CMD(AD5628_CMD_POWERDOWN) |
                            AD5628_POWER_MODE(pwrMode) |
                            selectedChannel);
}

/***************************************************************************//**
 * @brief Resets the device.
 *
 * @return none.
*******************************************************************************/
void AD5628_Reset(void)
{
     AD5628_SetInputRegister(AD5628_CMD(AD5628_CMD_RESET));
}

/***************************************************************************//**
 * @brief Writes a 32-bit data-word to the Input Register of the device.
 *
 * @param registerValue - Value of the register.
 *
 * @return none.
*******************************************************************************/
void AD5628_SetInputRegister(unsigned long registerValue)
{
    unsigned char registerWord[4] = {0, 0, 0, 0};
    
    registerWord[0] = (unsigned char)((registerValue & 0xFF000000) >> 24);
    registerWord[1] = (unsigned char)((registerValue & 0x00FF0000) >> 16);
    registerWord[2] = (unsigned char)((registerValue & 0x0000FF00) >> 8);
    registerWord[3] = (unsigned char)((registerValue & 0x000000FF) >> 0);
    SPI_Write(AD5628_SLAVE_ID, registerWord, 4);
}
