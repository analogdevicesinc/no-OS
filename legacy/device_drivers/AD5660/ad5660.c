/**************************************************************************//**
*   @file   ad5660.c
*   @brief  Implementation of ad5660 Driver for Microblaze processor.
*   @author Lucian Sin (Lucian.Sin@analog.com)
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ad5660.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
struct ad56x0_chip_info {
    unsigned char  bitsNumber;
    float          vRef;
    unsigned short fullScale;
};

static const struct ad56x0_chip_info ad56x0_chip_info[] = {
    [ID_AD5620_2_3] = {
        .bitsNumber = 12,
        .vRef       = 2.5,
        .fullScale  = 0xFFF,
    },
    [ID_AD5620_1] = {
        .bitsNumber = 12,
        .vRef       = 1.25,
        .fullScale  = 0xFFF,
    },
    [ID_AD5640_2_3] = {
        .bitsNumber = 14,
        .vRef       = 2.5,
        .fullScale  = 0x3FFF,
    },
    [ID_AD5640_1] = {
        .bitsNumber = 14,
        .vRef       = 1.25,
        .fullScale  = 0x3FFF,
    },
    [ID_AD5660_2_3] = {
        .bitsNumber = 16,
        .vRef       = 2.5,
        .fullScale  = 0xFFFF,
    },
    [ID_AD5660_1] = {
        .bitsNumber = 16,
        .vRef       = 1.25,
        .fullScale  = 0xFFFF,
    },
};

AD56x0_type act_device;

/******************************************************************************/
/*************************** Functions Definitions ****************************/
/******************************************************************************/

/**************************************************************************//**
 * @brief Initializes the communication with the device.
 *
 * @param None.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
******************************************************************************/
char ad5660_Init(AD56x0_type device)
{
    char status = -1;

    act_device = device;
    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 1, 1);

    return status;
}

/***************************************************************************//**
 * @brief Writes 12/14/16 bits of data to input register through SPI.
 *
 * @param data - Data which will be transmitted.
 *
 * @return status - Result of the write operation.
 *                  Example:  0 - if operation was successful;
 *                           -1 - if operation was unsuccessful.
*******************************************************************************/
char ad5660_WriteRegister(unsigned long data)
{
    unsigned char bytesWritten = 0;
    char          status       = 0;
    unsigned char dataBuf[3]   = {0, 0, 0};

    if(ad56x0_chip_info[act_device].bitsNumber == 16)
    {
        dataBuf[0]   = (data & 0x00FF0000) >> 16;
        dataBuf[1]   = (data & 0x0000FF00) >> 8;
        dataBuf[2]   = (data & 0x000000FF) >> 0;
        bytesWritten = SPI_Write(ad5660_SLAVE_ID, dataBuf, 3);
        if (bytesWritten != 3)
        {
            status = -1;
        }
    }
    else
    {
        if(ad56x0_chip_info[act_device].bitsNumber == 14)
        {
            dataBuf[0] = (data & 0xFF00) >> 8;
            dataBuf[1] = (data & 0x00FF) >> 0;
            bytesWritten = SPI_Write(ad5660_SLAVE_ID, dataBuf, 2);
            if (bytesWritten != 2)
            {
                status = -1;
            }
        }
        else
        {
            if(ad56x0_chip_info[act_device].bitsNumber == 12)
            {
                dataBuf[0]   = (data & 0x3FC0) >> 6;
                dataBuf[1]   = (data & 0x003F) << 2;
                bytesWritten = SPI_Write(ad5660_SLAVE_ID, dataBuf, 2);
                if (bytesWritten != 2)
                {
                    status = -1;
                }
            }
        }
    }

    return status;
}

/***************************************************************************//**
 * @brief Sets the output DAC voltage to a desired value.
 *
 * @param vOut - the desired output voltage of the DAC.
 *
 * @return None.
*******************************************************************************/
void ad5660_SetVoltage(float vOut)
{
    unsigned long dacCode = 0;

    dacCode = (unsigned long)((float)(vOut / ad56x0_chip_info[act_device].vRef) *
               ad56x0_chip_info[act_device].fullScale);
    dacCode |= ad5660_LOAD;
    ad5660_WriteRegister(dacCode);
}
