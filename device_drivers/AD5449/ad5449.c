/***************************************************************************//**
*   @file   AD5449.c
*   @brief  Implementation of AD5449 Driver.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "AD5449.h"
#include "Communication.h"

#define MAX_RESOLUTION  12     /* Maximum resolution of the supported devices */
#define DATA_MASK       0xFFF  /* Mask for 16 data bits */
#define CMD_MASK        0xFF   /* Mask for Command bits */
#define CMD_OFFSET      12     /* Offset for Command */
#define MSB_MASK        0xFF00 /* Most significant byte of the data word */
#define MSB_OFFSET      8
#define LSB_MASK        0x00FF /* Least significant byte of the data word */
#define LSB_OFFSET      0
#define PKT_LENGTH      2      /* SPI packet length in byte */

#define TWO_BIT_MASK    0x3
/*****************************************************************************/
/***************************** Variable definition ***************************/
/*****************************************************************************/

struct ad5449_chip_info {
    unsigned char num_channels;
    unsigned char resolution;
    bool_t has_ctrl;
};

static const struct ad5449_chip_info ad5449_chip_info[] = {
    [ID_AD5415] = {
        .num_channels = 2,
        .resolution = 12,
        .has_ctrl = true,
    },
    [ID_AD5426] = {
        .num_channels = 1,
        .resolution = 8,
        .has_ctrl = false,
    },
    [ID_AD5429] = {
        .num_channels = 2,
        .resolution = 8,
        .has_ctrl = true,
    },
    [ID_AD5432] = {
        .num_channels = 1,
        .resolution = 10,
        .has_ctrl = false,
    },
    [ID_AD5439] = {
        .num_channels = 2,
        .resolution = 10,
        .has_ctrl = true,
    },
    [ID_AD5443] = {
        .num_channels = 1,
        .resolution = 12,
        .has_ctrl = false,
    },
    [ID_AD5449] = {
        .num_channels = 2,
        .resolution = 12,
        .has_ctrl = true,
    }
};

AD5449_type_t act_device;
unsigned short controlReg;

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5449 Board.
 *
 * @param device    - Type of the desired device.
 *                    Example: ID_AD5415, ID_AD5426, ID_AD5429, ID_AD5432,
 *                             ID_AD5439, ID_AD5443, ID_AD5449,
 * @return retValue - Result of the initialization.
 *                    Example: 0 - if initialization was successful;
 *                            -1 - if initialization was unsuccessful.
******************************************************************************/
char AD5449_Init(AD5449_type_t device)
{
    char status = -1;

    act_device = device;

    /* Initialize SPI communication. */
    status = SPI_Init(0, 1000000, 0, 0);

    /* Set GPIO pins. */
    AD5449_LDAC_OUT;
    AD5449_LDAC_LOW;
    AD5449_CLR_OUT;
    AD5449_CLR_HIGH;

    /* Initialize device */
    controlReg = (AD5449_SDO_FULL << AD5449_SDO_BIT) | \
                 (AD5449_DAISY_CHAIN_DIS << AD5449_DSY_BIT) | \
                 (AD5449_ZERO_SCALE << AD5449_HCLR_BIT) | \
                 (AD5449_CLOCK_NEGEDGE << AD5449_SCLK_BIT);
    if(ad5449_chip_info[act_device].has_ctrl)
    {
        AD5449_SetInputShiftReg(AD5449_CTRL_REG, controlReg);
    }
    else
    {
        AD5449_DaisyChainSetup(AD5449_DAISY_CHAIN_DIS);
    }

return status;
}

/**************************************************************************//**
 * @brief Write to input register and read from output register via SPI.
 *
 * @param   command  - command control bits.
 *          data     - data to be written in input register.
 *
 * @return  readBack - value read from register.
******************************************************************************/
unsigned short AD5449_SetInputShiftReg(unsigned short command,
                                       unsigned short data)
{
    unsigned short inputShiftReg = 0;
    unsigned short readBack = 0;
    unsigned char spiData[PKT_LENGTH] = {0, 0};
    unsigned char data_offset = MAX_RESOLUTION - \
                                ad5449_chip_info[act_device].resolution;

    if(command == AD5449_CTRL_REG)
    {
        inputShiftReg = ((command & CMD_MASK) << CMD_OFFSET) | \
                         (data & DATA_MASK);
    }
    else
    {
        inputShiftReg = ((command & CMD_MASK) << CMD_OFFSET) | \
                        ((data & DATA_MASK) << data_offset);
    }

    spiData[0] = (inputShiftReg & MSB_MASK) >> MSB_OFFSET;
    spiData[1] = (inputShiftReg & LSB_MASK) >> LSB_OFFSET;

    SPI_Read((unsigned char)AD5449_SLAVE_ID, spiData, PKT_LENGTH);
    readBack = ((unsigned short)spiData[0] << MSB_OFFSET) | \
               ((unsigned short)spiData[1] << LSB_OFFSET);

return readBack;
}

/**************************************************************************//**
 * @brief Loads and updates the selected DAC with a given value.
 *
 * @param   channel  - the chosen channel to write to.
 *                      Example: AD5449_CH_A = 0;
 *                               AD5449_CH_B = 1.
 *          dacValue - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5449_LoadUpdateChannel(unsigned char channel, unsigned short dacValue)
{
    if(ad5449_chip_info[act_device].num_channels > 1)
    {
        AD5449_SetInputShiftReg(AD5449_CTRL_LOADUPDATE(channel), dacValue);
    }
    else
    {
        AD5449_SetInputShiftReg(AD5449_CTRL_LOADUPDATE(AD5449_CH_A), dacValue);
    }
}

/**************************************************************************//**
 * @brief Load selected DAC input register with a given value.
 *
 * @param   channel  - the chosen channel to write to.
 *                      Example: AD5449_CH_A = 0;
 *                               AD5449_CH_B = 1.
 *          dacValue - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5449_LoadChannel(unsigned char channel, unsigned short dacValue)
{
    if(ad5449_chip_info[act_device].num_channels > 1)
    {
        AD5449_SetInputShiftReg(AD5449_CTRL_LOAD(channel), dacValue);
    }
    else
    {
        AD5449_SetInputShiftReg(AD5449_CTRL_LOAD(AD5449_CH_A), dacValue);
    }
}

/**************************************************************************//**
 * @brief Read from the selected DAC register.
 *
 * @param   channel  - the chosen channel to read from.
 *                      Example: AD5449_CH_A = 0;
 *                               AD5449_CH_B = 1.
 *
 * @return  dacValue - value read from the register.
******************************************************************************/
unsigned short AD5449_ReadbackChannel(unsigned char channel)
{
    unsigned short dacValue = 0;

    if(ad5449_chip_info[act_device].num_channels > 1)
    {
        AD5449_SetInputShiftReg(AD5449_CTRL_READBACK(channel), 0x0);
        dacValue = AD5449_SetInputShiftReg(AD5449_CTRL_NOP, 0x0) & DATA_MASK;
    }
    else
    {
        AD5449_SetInputShiftReg(AD5449_CTRL_READBACK(AD5449_CH_A), 0x0);
        dacValue = AD5449_SetInputShiftReg(AD5449_CTRL_NOP, 0x0) & DATA_MASK;
    }
return dacValue;
}

/**************************************************************************//**
 * @brief Update the DAC outputs. This function has a physical result just with *        devices with two channels
 *
 * @param  None.
 *
 * @return None.
******************************************************************************/
void AD5449_UpdateAll(void)
{
    if(ad5449_chip_info[act_device].num_channels > 1)
    {
        AD5449_SetInputShiftReg(AD5449_CTRL_UPDATEALL, 0x0);
    }
}

/**************************************************************************//**
 * @brief Load the DAC input registers. This function has a physical result
 *        just with devices with two channels
 *
 * @param dacValue - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5449_LoadAll(short dacValue)
{
    if(ad5449_chip_info[act_device].num_channels > 1)
    {
        AD5449_SetInputShiftReg(AD5449_CTRL_LOADALL, dacValue);
    }
}

/**************************************************************************//**
 * @brief Set up the scale where to the output will be cleared on active CLR
 *        signal
 *
 * @param  type - target scale when making a clear command
 *                Example: ZERO_SCALE = zero scale.
 *                         MIDSCALE   = midscale.
 *
 * @return None.
******************************************************************************/
void AD5449_ClearScaleSetup(char type)
{
    if(ad5449_chip_info[act_device].has_ctrl)
    {
        if(type == AD5449_ZERO_SCALE)
        {
            /* Reset the HCLR bit in the Control Register */
            controlReg &= ~AD5449_HCLR_MASK;
            AD5449_SetInputShiftReg(AD5449_CTRL_REG, controlReg);
        }
        else if(type == AD5449_MID_SCALE)
        {
            /* Set the HCLR bit in the Control Register */
            controlReg |= AD5449_HCLR_MASK;
            AD5449_SetInputShiftReg(AD5449_CTRL_REG, controlReg);
        }
    }
    else
    {
        if(type == AD5449_ZERO_SCALE)
        {
            AD5449_SetInputShiftReg(AD5449_CTRL_CLR_ZERO, 0x0);
        }
        else if (type == AD5449_MID_SCALE)
        {
            AD5449_SetInputShiftReg(AD5449_CTRL_CLR_MID, 0x0);
        }
    }
}

/**************************************************************************//**
 * @brief Enable/disable the Daisy-Chain mode
 *
 * @param  type - target scale when making a clear command
 *                Example: ZERO_SCALE = zero scale.
 *                         MIDSCALE   = midscale.
 *
 * @return None.
******************************************************************************/
void AD5449_DaisyChainSetup(char value)
{
    if(ad5449_chip_info[act_device].has_ctrl)
    {
        if(value == AD5449_DAISY_CHAIN_EN)
        {
            /* Set the DSY bit in the Control Register */
            controlReg |= AD5449_DSY_MASK;
            AD5449_SetInputShiftReg(AD5449_CTRL_REG, controlReg);
        }
        else if(value == AD5449_DAISY_CHAIN_DIS)
        {
            /* Reset the DSY bit in the Control Register */
            controlReg &= ~AD5449_DSY_MASK;
            AD5449_SetInputShiftReg(AD5449_CTRL_REG, controlReg);
        }
    }
    else
    {
        /* Disable daisy chain mode */
        if (value == AD5449_DAISY_CHAIN_DIS)
        {
            AD5449_SetInputShiftReg(AD5449_CTRL_DAISY_CHAIN, 0x0);
        }
    }
}

/**************************************************************************//**
 * @brief Control the SDO output driver strength. This function is supported
 * by the devices : AD5415, AD5429, AD5439, AD5449
 *
 * @param  type - target scale when making a clear command
 *                Example: AD5449_SDO_FULL = Full SDO driver.
 *                         AD5449_SDO_WEAK = Weak SDO driver.
 *                         AD5449_SDO_OPEN_DRAIN = SDO configured as open drain
 *                         AD5449_SDO_DISABLE = Disable SDO output
 * @return None.
******************************************************************************/
void AD5449_SDOControl(char controlBits)
{
    /* Check if the current device supports this functions */
    if(ad5449_chip_info[act_device].has_ctrl)
    {
        /* Reset the SDO bits of the local Control Register */
        controlReg &= ~AD5449_SDO_MASK;
        /* Set up the new SDO bits */
        controlReg |= (controlBits & TWO_BIT_MASK) << AD5449_SDO_BIT;
        /* Send the new Control Register value to the device */
        AD5449_SetInputShiftReg(AD5449_CTRL_REG, controlReg);
    }
    else /* Do Nothing! */
    {

    }
}

/**************************************************************************//**
 * @brief Set up the active clock edge of the SPI interface
 *
 * @param  type - target scale when making a clear command
 *                Example: AD5449_CLOCK_NEGEDGE = Falling edge. (Default)
 *                         AD5449_CLOCK_POSEDGE = Rising edge.
 *
 * @return None.
******************************************************************************/
void AD5449_SCLKSetup(char value)
{
    if(ad5449_chip_info[act_device].has_ctrl)
    {
        if(value == AD5449_CLOCK_POSEDGE)
        {
            /* Set the SCLK bit in the Control Register */
            controlReg |= AD5449_SCLK_MASK;
            AD5449_SetInputShiftReg(AD5449_CTRL_REG, controlReg);
        }
        else if(value == AD5449_CLOCK_NEGEDGE)
        {
            /* Reset the SCLK bit in the Control Register */
            controlReg &= ~AD5449_SCLK_MASK;
            AD5449_SetInputShiftReg(AD5449_CTRL_REG, controlReg);
        }
    }
    else
    {
        /* Clock data on rising edge */
        if (value == AD5449_CLOCK_POSEDGE)
        {
            AD5449_SetInputShiftReg(AD5449_CTRL_CLK_EDGE, 0x0);
        }
    }
}
