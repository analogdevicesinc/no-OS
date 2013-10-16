/******************************************************************************
*   @file   AD5172.c
*   @brief  Implementation of AD5172 Driver for Microblaze processor.
*   @author Istvan Csomortani (istvan.csomortani@analog.com)
*
*******************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
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

/*****************************************************************************/
/* Include Files                                                             */
/*****************************************************************************/
#include "ad5172.h"
#include "Communication.h"

#define ADDR_MASK_1B        0x1
#define ADDR_MASK_2B        0x3
/*****************************************************************************/
/*  Private variables                                                        */
/*****************************************************************************/
struct ad5172_chip_info {
    unsigned char num_channels;
    comm_type_t comm_type;
};

static const struct ad5172_chip_info AD5172_CHIP_INFO[] = {
    [ID_AD5162] = {
        .num_channels = 2,
        .comm_type = SPI,
    },
    [ID_AD5172] = {
        .num_channels = 2,
        .comm_type = I2C,
    },
    [ID_AD5173] = {
        .num_channels = 2,
        .comm_type = I2C,
    },
    [ID_AD8403] = {
        .num_channels = 4,
        .comm_type = SPI,
    },
    [ID_AD8402] = {
        .num_channels = 2,
        .comm_type = SPI,
    },
    [ID_AD8400] = {
        .num_channels = 1,
        .comm_type = SPI,
    }
};

/* Actual device type */
AD5172_type_t this_device;
/**************************************************************************//**
 * @brief Initialize SPI and Initial Values for AD5172 Board.
 *
 * @param The current device type.
 *
 * @return success
******************************************************************************/
char AD5172_Init(AD5172_type_t device)
{
    char status = -1;

    this_device = device;

    if(AD5172_CHIP_INFO[this_device].comm_type == SPI)
    {
        status = SPI_Init(0, 100000, 0, 0);
    }
    else
    {
        status = I2C_Init(100000);
    }

    /* Deactivate Hardware Reset */
    AD5172_RESET_OUT;
    AD5172_RESET_HIGH;
    /* Deactivate Shutdown Pin  */
    AD5172_SHUTDOWN_OUT;
    AD5172_SHUTDOWN_HIGH;

return status;
}

/******************************************************************************
 * @brief Send a command to the device.
 *
 * @param channel - will be used to set the channel to be programmed
 *                  (valid values 0-1)
 *        command - will be used to set special futures in case of
 *                  AD5172/AD5173
 * @param data - will be used to set the value to be set on the channel
 *               (valid values 0-255)
 *
 * @return None
******************************************************************************/

void AD5172_Write(unsigned char channel,
                  unsigned char command,
                  unsigned char data)
{
    unsigned char dataBuffer[2] = {0,};
    unsigned char i2cSlaveAddr = 0;

    if(AD5172_CHIP_INFO[this_device].comm_type == SPI)
    {
        if(AD5172_CHIP_INFO[this_device].num_channels == 2)
        {
            dataBuffer[0] = channel & ADDR_MASK_1B;
        }
        else
        {
            dataBuffer[0] = channel & ADDR_MASK_2B;
        }

        dataBuffer[1] = data;
        SPI_Write(AD5172_SLAVE_ID,
                  dataBuffer,
                  2);
    }
    else
    {   /* Setup I2C address */
        i2cSlaveAddr |= (this_device == ID_AD5173) ? (AD5172_I2C_PIN_ADDR1 << 1) | \
                                                     AD5172_I2C_PIN_ADDR0 : \
                                                     AD5172_I2C_PIN_ADDR_MASK;
        i2cSlaveAddr |= ((AD5172_I2C_HARD_ADDR << 2));
        dataBuffer[0] |= (channel << AD5172_CNTR_A0_OFFSET) | \
                      (command & AD5172_CNTR_MASK);
        dataBuffer[1] = data;

        I2C_Write(i2cSlaveAddr,
                  dataBuffer,
                  2,
                  1);
    }
}

/******************************************************************************
 * @brief Reading data from a previously selected RDAC register in write mode,
 *        the fuseStatus will store the OTP validation bits
 *
 * @param channel - will be used to set the channel to be programmed
 *                  (valid values 0-1)
 *        fuseStatus - a char pointer, will be used to store the OTP
 *                  validation bits
 *
 * @return The value of the previously selected wiper register
******************************************************************************/
short AD5172_Read(unsigned char channel,
                  unsigned char* fuseStatus)
{

    unsigned char dataBuffer[2] = {0,};
    unsigned char i2cSlaveAddr = 0;
    unsigned char readStatus = 0;

    if(AD5172_CHIP_INFO[this_device].comm_type == I2C)
    {
        /* Setup I2C address pin-programmable address bits */
        i2cSlaveAddr |= (this_device == ID_AD5173) ? (AD5172_I2C_PIN_ADDR1 << 1) |\
                                                   AD5172_I2C_PIN_ADDR0  : \
                                                   AD5172_I2C_PIN_ADDR_MASK;
        /* Setup the read bit */
        i2cSlaveAddr |= (AD5172_I2C_HARD_ADDR << 2);
        readStatus = I2C_Read(i2cSlaveAddr,
                              dataBuffer,
                              2,
                              1);
        *fuseStatus = (dataBuffer[1] >> AD5172_OTP_VALID_OFFSET) & \
                      AD5172_OTP_VALID_MASK;

        if(!readStatus)
        {
            return -1;
        }
        else
        {
            return (short)dataBuffer[0];
        }
    }
return 0;
}
