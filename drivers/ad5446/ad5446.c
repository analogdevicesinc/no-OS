/***************************************************************************//**
*   @file   AD5446.c
*   @brief  Implementation of AD5446 Driver. This driver supporting the
*              following devices: AD5553, AD5543, AD5542A, AD5541A, AD5512A,
*                                 AD5453, AD5452, AD5451, AD5450, AD5446, AD5444
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
#include "AD5446.h"
#include "Communication.h"

#define MAX_RESOLUTION  16      /* Maximum resolution of supported devices */
#define DATA_MASK(x)    (0xFFFF >> (MAX_RESOLUTION - (x)))
#define DATA_OFFSET(x)  ((MAX_RESOLUTION - 2) - (x))
#define MSB_MASK        0xFF00
#define MSB_OFFSET      8
#define LSB_MASK        0x00FF
#define LSB_OFFSET      0
#define CMD_MASK        0x3
#define CMD_OFFSET      15
#define PKT_LENGTH      2       /* SPI packet length in byte */

/*****************************************************************************/
/***************************** Variable definition ***************************/
/*****************************************************************************/
/* Custom type for active clock edge */
typedef enum
{
    negedge,
    posedge
} active_clk_t;

/* Custom boolean type */
typedef enum
{
    false,
    true
} bool_t;

/* Data structure for chip's attributes */
struct ad5446_chip_info {
    unsigned char resolution;
    active_clk_t  data_clock_in;
    bool_t        has_ctrl;
};

/* Device 'table' */
static const struct ad5446_chip_info chip_info[] = {
    [ID_AD5553] = {
        .resolution = 14,
        .data_clock_in = posedge,
        .has_ctrl = false,
    },
    [ID_AD5543] = {
        .resolution = 16,
        .data_clock_in = posedge,
        .has_ctrl = false,
    },
    [ID_AD5542A] = {
        .resolution = 16,
        .data_clock_in = posedge,
        .has_ctrl = false,
    },
    [ID_AD5541A] = {
        .resolution = 16,
        .data_clock_in = posedge,
        .has_ctrl = false,
    },
    [ID_AD5512A] = {
        .resolution = 12,
        .data_clock_in = posedge,
        .has_ctrl = false,
    },
    [ID_AD5453] = {
        .resolution = 14,
        .data_clock_in = negedge,
        .has_ctrl = true,
    },
    [ID_AD5452] = {
        .resolution = 12,
        .data_clock_in = negedge,
        .has_ctrl = true,
    },
    [ID_AD5451] = {
        .resolution = 10,
        .data_clock_in = negedge,
        .has_ctrl = true,
    },
    [ID_AD5450] = {
        .resolution = 8,
        .data_clock_in = negedge,
        .has_ctrl = true,
    },
    [ID_AD5446] = {
        .resolution = 14,
        .data_clock_in = negedge,
        .has_ctrl = true,
    },
    [ID_AD5444] = {
        .resolution = 12,
        .data_clock_in = negedge,
        .has_ctrl = true,
    }
};

/* Actual device type */
AD5446_type_t act_device;

/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5446 Board.
 *
 * @param None.
 *
 * @return retValue - Result of the initialization.
 *                    Example: 0 - if initialization was successful;
 *                            -1 - if initialization was unsuccessful.
******************************************************************************/
char AD5446_Init(AD5446_type_t device)
{
    char status = -1;

    act_device = device;

    /* Initialize SPI communication. */
    if(chip_info[act_device].data_clock_in == posedge)
    {
        status = SPI_Init(0, 1000000, 1, 0);
    }
    else
    {
        status = SPI_Init(0, 1000000, 0, 0);
    }

    /* Initialize configuration pins, if exist. */
    if(act_device == ID_AD5542A)
    {
        AD5446_LDAC_OUT;
        AD5446_LDAC_LOW;
        AD5446_CLR_OUT;
        AD5446_CLR_HIGH;
    }
    else if(act_device == ID_AD5541A)
    {
        AD5446_LDAC_OUT;
        AD5446_LDAC_LOW;
    }
    else if(act_device == ID_AD5446) /* Enable the SDO line */
    {
        /* AD5446_CLR is mapped to GPIO0 */
        AD5446_CLR_OUT;
        AD5446_CLR_LOW;
    }

return status;
}
/**************************************************************************//**
 * @brief Writes to input register via SPI.
 *
 * @param   data - data to be written in input register.
 *
 * @return  None.
******************************************************************************/
void AD5446_SetRegister(unsigned char command,
                         unsigned short data)
{
    unsigned short inputShiftReg = 0;
    unsigned char spiData[PKT_LENGTH] = {0, 0};

    if(chip_info[act_device].has_ctrl == true)
    {
        inputShiftReg = ((command & CMD_MASK) << CMD_OFFSET) |
                     ( data & DATA_MASK(chip_info[act_device].resolution) \
                      << DATA_OFFSET(chip_info[act_device].resolution));
        spiData[0] = (inputShiftReg & MSB_MASK) >> MSB_OFFSET;
        spiData[1] = (inputShiftReg & LSB_MASK) >> LSB_OFFSET;
    }
    else
    {
        inputShiftReg = data & (DATA_MASK(chip_info[act_device].resolution));
        spiData[0] = (inputShiftReg & MSB_MASK) >> MSB_OFFSET;
        spiData[1] = (inputShiftReg & LSB_MASK);
    }
    SPI_Write((unsigned char)AD5446_SLAVE_ID, spiData, PKT_LENGTH);
}
/***************************************************************************//**
 * @brief Sets the output voltage.
 *
 * @param outVoltage - The voltage value in volts
 * @param vref       - The voltage reference used by the device in volts.
 *
 * @return Actual voltage that the device can output.
*******************************************************************************/
float AD5446_SetVoltage(float voltage, float vref, vout_type_t vout_type)
{
    unsigned short  registerValue = 0;
    float           actualVout = 0;
    float           code = 0;
    unsigned short  max_value = DATA_MASK(chip_info[act_device].resolution);

    /* Get raw data from the user's desired voltage value. */
    switch(vout_type)
    {
        case unipolar :
        {
            code = (voltage * max_value) / vref;
            break;
        }
        case unipolar_inv :
        {
            code = (-1) * (voltage * max_value) / vref;
            break;
        }
        case bipolar :
        {
            code = ((voltage + vref) * (max_value/2)) / vref;
            break;
        }
    }

    /* Round to the nearest integer. */
    registerValue = (unsigned short)code;

    /* Check to value which will be written to register. */
    if (registerValue > (max_value - 1) )
    {
        registerValue = (max_value - 1);
    }

    /* Write to DAC register. */
    AD5446_SetRegister(0, registerValue);
    /* Calculate the output voltage value. */
    switch(vout_type)
    {
        case unipolar :
        {
            actualVout = ((float)registerValue / max_value) * vref;
            break;
        }
        case unipolar_inv :
        {
            actualVout = (-1) * ((float)registerValue / max_value) * vref;
            break;
        }
        case bipolar :
        {
            actualVout = (vref * (float)registerValue / (max_value/2)) - vref;
            break;
        }
    }

return actualVout;
}
