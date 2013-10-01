/***************************************************************************//**
*   @file   AD5686.c
*   @brief  Implementation of AD5686 Driver.
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
#include "AD5686.h"
#include "Communication.h"

#define MAX_RESOLUTION  16     /* Maximum resolution of the supported devices */

#define CMD_MASK        0xFF   /* Mask for Command bits */
#define ADDR_MASK       0xFF   /* Mask for Address bits */
#define CMD_OFFSET      4      /* Offset for Command */
#define MSB_MASK        0xFF00 /* Most significant byte of the data word */
#define MSB_OFFSET      8
#define LSB_MASK        0x00FF /* Least significant byte of the data word */
#define LSB_OFFSET      0
#define PKT_LENGTH      3      /* SPI packet length in byte */

/*****************************************************************************/
/***************************** Variable definition ***************************/
/*****************************************************************************/
typedef enum
{
    spi,
    i2c
} comm_type_t;

struct ad5686_chip_info {
    unsigned char resolution;
    comm_type_t communication;
};

static const struct ad5686_chip_info ad5686_chip_info[] = {
    [ID_AD5684R] = {
        .resolution = 12,
        .communication = spi,
    },
    [ID_AD5685R] = {
        .resolution = 14,
        .communication = spi,
    },
    [ID_AD5686R] = {
        .resolution = 16,
        .communication = spi,
    },
    [ID_AD5694R] = {
        .resolution = 12,
        .communication = i2c,
    },
    [ID_AD5695R] = {
        .resolution = 14,
        .communication = i2c,
    },
    [ID_AD5696R] = {
        .resolution = 16,
        .communication = i2c,
    }
};

AD5686_type_t act_device;

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5686 Board.
 *
 * @param device    - Type of the desired device.
 *                    Example: ID_AD5415, ID_AD5426, ID_AD5429, ID_AD5432,
 *                             ID_AD5439, ID_AD5443, ID_AD5686,
 * @return retValue - Result of the initialization.
 *                    Example: 0 - if initialization was successful;
 *                            -1 - if initialization was unsuccessful.
******************************************************************************/
char AD5686_Init(AD5686_type_t device)
{
    char status = -1;

    act_device = device;

    /* Initialize SPI communication. */
    if(ad5686_chip_info[act_device].communication == spi)
    {
        status = SPI_Init(0, 1000000, 0, 0);
    }
    else
    {
        status = I2C_Init(100000);
    }

    /* Set GPIO pins. */
    AD5686_LDAC_OUT;
    AD5686_LDAC_LOW;
    AD5686_RESET_OUT;
    AD5686_RESET_HIGH;

return status;
}

/**************************************************************************//**
 * @brief Write to input shift register.
 *
 * @param   command  - command control bits.
 *          data     - data to be written in input register.
 *
 * @return  readBack - value read from register.
******************************************************************************/
unsigned short AD5686_SetShiftReg(unsigned char command,
                                  unsigned char address,
                                  unsigned short data)
{
    unsigned char dataBuff[PKT_LENGTH] = {0, 0, 0};
    unsigned short readBackData = 0;

    dataBuff[0] = ((command & CMD_MASK) << CMD_OFFSET) | \
                  (address & ADDR_MASK);
    dataBuff[1] = (data & MSB_MASK) >> MSB_OFFSET;
    dataBuff[2] = (data & LSB_MASK);

    if(ad5686_chip_info[act_device].communication == spi)
    {
        SPI_Write((unsigned char)AD5686_SLAVE_ID, dataBuff, PKT_LENGTH);
        readBackData = (dataBuff[1] << MSB_OFFSET) | dataBuff[2];
    }
    else
    {
        I2C_Write(AD5686_I2C_ADDR, dataBuff, PKT_LENGTH, 1);
    }
return readBackData;
}

/**************************************************************************//**
 * @brief Write to Input Register n (dependent on LDAC)
 *
 * @param   address  - the chosen channel to write to.
 *                      Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                                channel A and C
 *          data - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5686_WriteRegister(unsigned char address, unsigned short data)
{
    unsigned char dataOffset = MAX_RESOLUTION - \
                                ad5686_chip_info[act_device].resolution;

    AD5686_SetShiftReg(AD5686_CTRL_WRITE, address, data << dataOffset);
}

/**************************************************************************//**
 * @brief Update DAC Register n with contents of Input Register n
 *
 * @param   address  - the chosen channel to write to.
 *                     Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                                channel A and C
 *
 * @return None.
******************************************************************************/
void AD5686_UpdateRegister(unsigned char address)
{
    AD5686_SetShiftReg(AD5686_CTRL_UPDATE, address, 0);
}

/**************************************************************************//**
 * @brief Write to and update DAC channel n
 *
 * @param   address  - the chosen channel to write to.
 *                      Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                                channel A and C
 *          data - desired value to be written in register.
 *
 * @return None.
******************************************************************************/
void AD5686_WriteUpdateRegister(unsigned char address, unsigned short data)
{
    unsigned dataOffset = MAX_RESOLUTION - \
                                ad5686_chip_info[act_device].resolution;

    AD5686_SetShiftReg(AD5686_CTRL_WRITEUPDATE, address, data << dataOffset);
}

/**************************************************************************//**
 * @brief Read back Input Register n
 *
 * @param   address  - the channel which will be read back. Note: only one
 *                     channel should be selected, if there will be selected
 *                     more than one channel, the channel A will be read back
 *                     by default
 *                     Example: 'AD5686_CH_C' will read back the channel C
 *
 * @return None.
******************************************************************************/
unsigned short AD5686_ReadBackRegister(unsigned char address)
{

    unsigned short readBackData = 0;
    unsigned short offset = MAX_RESOLUTION - \
            ad5686_chip_info[act_device].resolution;

    if(ad5686_chip_info[act_device].communication == spi)
    {
        AD5686_SetShiftReg(AD5686_CTRL_RB_REG, address, 0);
        readBackData = AD5686_SetShiftReg(AD5686_CTRL_NOP, 0, 0);
        readBackData >>= offset;
    }

return readBackData;
}

/**************************************************************************//**
 * @brief Write to and update DAC channel n
 *
 * @param   address  - the chosen channel to write to.
 *                     Example : 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                                channel A and C
 *          mode     - Power-down operation modes.
                       Accepted values:
                        'AD5686_PWRM_NORMAL' - Normal Mode
 *                      'AD5686_PWRM_1K' - Power-down mode 1kOhm to GND
 *                      'AD5686_PWRM_100K' - Power-down mode 100kOhm to GND
 *                      'AD5686_PWRM_THREESTATE' - Three-State
 *
 * @return None.
******************************************************************************/
void AD5686_PowerMode(unsigned char address, unsigned char mode)
{
    unsigned short data = 0;

    data |= (address & AD5686_CH_A) ? (mode << AD5686_PWRM_CHA_OFFSET) : 0x0;
    data |= (address & AD5686_CH_B) ? (mode << AD5686_PWRM_CHB_OFFSET) : 0x0;
    data |= (address & AD5686_CH_C) ? (mode << AD5686_PWRM_CHC_OFFSET) : 0x0;
    data |= (address & AD5686_CH_D) ? (mode << AD5686_PWRM_CHD_OFFSET) : 0x0;

    AD5686_SetShiftReg(AD5686_CTRL_PWR, address, data);
}

/**************************************************************************//**
 * @brief Set hardware LDAC mask register
 *
 * @param   ldacMask  - in case of which channel ignore transitions on the LDAC *                      pin
 *                      Example: 'AD5686_CH_A | AD5686_CH_C' will write to \
 *                                channel A and C
 *
 * @return None.
******************************************************************************/
void AD5686_LdacMask(unsigned char ldacMask)
{
    AD5686_SetShiftReg(AD5686_CTRL_LDAC_MASK, 0, ldacMask);
}

/**************************************************************************//**
 * @brief Software reset (power-on reset)
 *
 * @param  None.
 *
 * @return None.
******************************************************************************/
void AD5686_SoftwareReset()
{
    AD5686_SetShiftReg(AD5686_CTRL_SWRESET, 0, 0);
}


/**************************************************************************//**
 * @brief Write to Internal reference setup register
 * @param  value - the internal reference register value
 *                  Example : 'AD5686_INTREF_EN' - enable internal reference
 *                            'AD5686_INTREF_DIS' - disable internal reference
 *
 * @return None.
******************************************************************************/
void AD5686_InternalReference(unsigned char value)
{
    AD5686_SetShiftReg(AD5686_CTRL_IREF_REG, 0, value);
}

/**************************************************************************//**
 * @brief Set up DCEN register (daisy-chain enable)
 *
 * @param  value - Enable or disable daisy-chain mode
                   Example : 'AD5686_DC_EN' - daisy-chain enable
 *                           'AD5686_DC_DIS' - daisy-chain disable
 *
 * @return None.
******************************************************************************/
void AD5686_DaisyChainEn(unsigned char value)
{
    AD5686_SetShiftReg(AD5686_CTRL_DCEN, 0, value);
}

/**************************************************************************//**
 * @brief Set up readback register (readback enable)
 *
 * @param  value - Enable or disable daisy-chain mode
                   Example : 'AD5686_RB_EN' - daisy-chain enable
 *                           'AD5686_RB_DIS' - daisy-chain disable
 *
 * @return None.
******************************************************************************/
void AD5686_ReadBackEn(unsigned char value)
{
    AD5686_SetShiftReg(AD5686_CTRL_RB_REG, 0, value);
}
