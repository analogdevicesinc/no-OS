/***************************************************************************//**
 *   @file   ADT7420.c
 *   @brief  Implementation of ADT7420 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "adt7420.h"

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param registerAddress - Address of the register.
 *
 * @return registerValue  - Value of the register.
*******************************************************************************/
unsigned char ADT7420_GetRegisterValue(adt7420_dev *dev,
                                       unsigned char registerAddress)
{
    unsigned char registerValue = 0;

    i2c_write(&dev->i2c_dev,
              &registerAddress,
              1,
              0);
    i2c_read(&dev->i2c_dev,
             &registerValue,
             1,
             1);

	return registerValue;
}

/***************************************************************************//**
 * @brief Sets the value of a register.
 *
 * @param registerAddress - Address of the register.
 * @param registerValue   - Value of the register.
 *
 * @return None.
*******************************************************************************/
void ADT7420_SetRegisterValue(adt7420_dev *dev,
                              unsigned char registerAddress,
                              unsigned char registerValue)
{
    unsigned char dataBuffer[2] = {0, 0};

    dataBuffer[0] = registerAddress;
    dataBuffer[1] = registerValue;
    i2c_write(&dev->i2c_dev,
              dataBuffer,
              2,
              1);
}

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the device is
 *        present.
 *
 * @return status - The result of the initialization procedure.
 *                  Example: -1 - I2C peripheral was not initialized or the
 *                                device is not present.
 *                            0 - I2C peripheral was initialized and the
 *                                device is present.
*******************************************************************************/
char ADT7420_Init(adt7420_dev **device,
		  adt7420_init_param init_param)
{
    adt7420_dev *dev;
    unsigned char status;
    unsigned char test   = 0;

    dev = (adt7420_dev *)malloc(sizeof(*dev));
    if (!dev) {
        return -1;
    }

    /* I2C */
    dev->i2c_dev.type = init_param.i2c_type;
    dev->i2c_dev.id = init_param.i2c_id;
    dev->i2c_dev.max_speed_hz = init_param.i2c_max_speed_hz;
    dev->i2c_dev.slave_address = init_param.i2c_slave_address;
    status = i2c_init(&dev->i2c_dev);

    /* Device Settings */
    dev->resolutionSetting = init_param.resolutionSetting;

    test   = ADT7420_GetRegisterValue(dev, ADT7420_REG_ID);
    if(test != ADT7420_DEFAULT_ID)
    {
        status = -1;
    }

	*device = dev;

    return status;
}

/***************************************************************************//**
 * @brief Resets the ADT7420.
 *        The ADT7420 does not respond to I2C bus commands while the default
 *        values upload (approximately 200 us).
 *
 * @return None.
*******************************************************************************/
void ADT7420_Reset(adt7420_dev *dev)
{
    unsigned char registerAddress = ADT7420_REG_RESET;

    i2c_write(&dev->i2c_dev,
              &registerAddress,
              1,
              1);
    dev->resolutionSetting = 0;
}

/***************************************************************************//**
 * @brief Sets the operational mode for ADT7420.
 *
 * @param mode - Operation mode.
 *               Example: ADT7420_OP_MODE_CONT_CONV - continuous conversion;
 *                        ADT7420_OP_MODE_ONE_SHOT  - one shot;
 *                        ADT7420_OP_MODE_1_SPS     - 1 SPS mode;
 *                        ADT7420_OP_MODE_SHUTDOWN  - shutdown.
 *
 * @return None.
*******************************************************************************/
void ADT7420_SetOperationMode(adt7420_dev *dev, unsigned char mode)
{
    unsigned char registerValue = 0;

    registerValue  = ADT7420_GetRegisterValue(dev, ADT7420_REG_CONFIG);
    registerValue &= ~ADT7420_CONFIG_OP_MODE(ADT7420_OP_MODE_SHUTDOWN);
    registerValue |= ADT7420_CONFIG_OP_MODE(mode);
    ADT7420_SetRegisterValue(dev, ADT7420_REG_CONFIG, registerValue);
}

/***************************************************************************//**
 * @brief Sets the resolution for ADT7420.
 *
 * @param resolution - Resolution.
 *                     Example: 0 - 13-bit resolution;
 *                              1 - 16-bit resolution.
 *
 * @return None.
*******************************************************************************/
void ADT7420_SetResolution(adt7420_dev *dev, unsigned char resolution)
{
    unsigned char registerValue = 0;

    registerValue  = ADT7420_GetRegisterValue(dev, ADT7420_REG_CONFIG);
    registerValue &= ~ADT7420_CONFIG_RESOLUTION;
    registerValue |= (resolution * ADT7420_CONFIG_RESOLUTION);
    ADT7420_SetRegisterValue(dev, ADT7420_REG_CONFIG, registerValue);
    dev->resolutionSetting = resolution;
}

/***************************************************************************//**
 * @brief Reads the temperature data and converts it to Celsius degrees.
 *
 * @return temperature - Temperature in degrees Celsius.
*******************************************************************************/
float ADT7420_GetTemperature(adt7420_dev *dev)
{
    unsigned char  msbTemp = 0;
    unsigned char  lsbTemp = 0;
    unsigned short temp    = 0;
    float          tempC   = 0;

    msbTemp = ADT7420_GetRegisterValue(dev, ADT7420_REG_TEMP_MSB);
    lsbTemp = ADT7420_GetRegisterValue(dev, ADT7420_REG_TEMP_LSB);
    temp    = ((unsigned short)msbTemp << 8) + lsbTemp;
    if(dev->resolutionSetting)
    {
        if(temp & 0x8000)
        {
            /*! Negative temperature */
            tempC = (float)((signed long)temp - 65536) / 128;
        }
        else
        {
            /*! Positive temperature */
            tempC = (float)temp / 128;
        }
    }
    else
    {
        temp >>= 3;
        if(temp & 0x1000)
        {
            /*! Negative temperature */
            tempC = (float)((signed long)temp - 8192) / 16;
        }
        else
        {
            /*! Positive temperature */
            tempC = (float)temp / 16;
        }
    }

    return tempC;
}
