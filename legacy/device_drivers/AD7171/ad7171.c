/**************************************************************************//**
*   @file   ad7171.c
*   @brief  Implementation of ad7171 Driver for Microblaze processor.
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
#include "ad7171.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
const unsigned char MAX_RESOLUTION = 16;

/******************************************************************************/
/************************** Variables Definitions *****************************/
/******************************************************************************/
struct ad7171_chip_info {
    unsigned char  resolution;
    unsigned short midscale;
};

static const struct ad7171_chip_info ad7171_chip_info[] = {
    [ID_AD7170] = {
        .resolution = 12,
        .midscale   = 2048,
    },
    [ID_AD7171] = {
        .resolution = 16,
        .midscale   = 32768,
    }
};

AD7171_type act_device;

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
char ad7171_Init(AD7171_type device)
{

    char status = -1;

    act_device = device;
    /* Setup SPI Interface */
    status = SPI_Init(0, 1000000, 1, 0);

    return status;
}

/**************************************************************************//**
 * @brief  Receives 24 bits from SPI and transmits 0x0.
 *
 * @param  ctrl   - control word of 24 bits.
 *
 * @return rxData - Received data in case of success.
******************************************************************************/
unsigned long ad7171_Read24Bits(unsigned long ctrl)
{
    unsigned long rxData;
    char          bytesRead = 0;
    unsigned char data[3]   = {0, 0, 0};

    data[0] = (ctrl & 0x00FF0000) >> 16;
    data[1] = (ctrl & 0x0000FF00) >> 8;
    data[2] = (ctrl & 0x000000FF) >> 0;

    bytesRead = SPI_Read(ad7171_SLAVE_ID, data, 3);
    if (bytesRead != 3)
    {
        return -1;
    }

    rxData = ((unsigned long)data[0] << 16) + ((unsigned long)data[1] << 8) +
             ((unsigned long)data[2] << 0);

    return(rxData);
}

/**************************************************************************//**
 * @brief  Calculates the input voltage and the pattern of the conversion as
 *         parameter.
 *
 * @param  ctrl    - control word of 24 bits;
 *         pattern - pointer to the pattern of the conversion.
 *
 * @return vin - input voltage.
******************************************************************************/
float ad7171_GetVoltage(unsigned long   ctrl,
                        unsigned char*  pattern,
                        float vRef)
{
    unsigned long  data      = 0;
    unsigned short dataValue = 0;
    float          vin       = 0;

    data = ad7171_Read24Bits(ctrl);
    /* Extract the ADC code from the 24 bits word. */
    dataValue = (data & 0xFFFF00) >> (8 + MAX_RESOLUTION -
                ad7171_chip_info[act_device].resolution);
    /* Calculate the input voltage corresponding to the current ADC code. */
    vin = (((float)dataValue / ad7171_chip_info[act_device].midscale) - 1) *
          vRef;
    /* Extract the pattern from the 24 bits word. */
    *pattern = (unsigned char)(data & 0x1F);

    return vin;
}

/**************************************************************************//**
 * @brief  Returns the ADC code and the pattern of the conversion as
 *         parameter.
 *
 * @param  ctrl    - control word of 24 bits;
 *         pattern - pointer to the pattern of the conversion.
 *
 * @return adcCode.
******************************************************************************/
unsigned short ad7171_GetAdcCode(unsigned long ctrl, unsigned char* pattern)
{
    unsigned long  data      = 0;
    unsigned short dataValue = 0;
    unsigned short adcCode   = 0;

    data = ad7171_Read24Bits(ctrl);
    /* Extract the ADC code from the 24 bits word. */
    dataValue = (data & 0xFFFF00) >> (8 + MAX_RESOLUTION -
                ad7171_chip_info[act_device].resolution);
    adcCode = dataValue;
    /* Extract the pattern from the 24 bits word. */
    *pattern = (unsigned char)(data & 0x1F);

    return adcCode;
}
