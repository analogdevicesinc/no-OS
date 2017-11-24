/***************************************************************************//**
 *   @file   AD799x.c
 *   @brief  Implementation of AD799x Driver.
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
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "AD799x.h"    // AD799x definitions.

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes I2C.
 *
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status       - Initialization status.
 *                        Example: -1 - Initialization failed;
 *                                  0 - Initialization succeeded.
*******************************************************************************/
char AD799x_Init(ad799x_dev **device,
		 ad799x_init_param init_param)
{
	ad799x_dev *dev;
    char status = -1;

	dev = (ad799x_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

    /* Initialize I2C peripheral. */
	status = i2c_init(&dev->i2c_desc, init_param.i2c_init);

    /* Determine the number of bits available for a conversion. */
    switch(init_param.partNumber)
    {
        case AD7991:
            dev->bitsNumber = 12;
            break;
        case AD7995:
            dev->bitsNumber = 10;
            break;
        case AD7999:
            dev->bitsNumber = 8;
            break;
    }

	*device = dev;

    return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by AD799x_Init().
 *
 * @param dev - The device structure.
 *
 * @return SUCCESS in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad799x_remove(ad799x_dev *dev)
{
	int32_t ret;

	ret = i2c_remove(dev->i2c_desc);

	free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Writes data into the Configuration Register.
 *
 * @param dev           - The device structure.
 * @param registerValue - Data value to write.
 *
 * @return None.
*******************************************************************************/
void AD799x_SetConfigurationReg(ad799x_dev *dev,
				unsigned char registerValue)
{
	i2c_write(dev->i2c_desc, &registerValue, 1, 1);
}

/***************************************************************************//**
 * @brief Reads the High byte and the Low byte of the conversion.
 *
 * @param dev       - The device structure.
 * @param convValue - It is used to store the conversion value.
 * @param channel   - Stores the channel number for the current conversion.
 *
 * @return none.
*******************************************************************************/
void AD799x_GetConversionResult(ad799x_dev *dev,
				short* convValue,
				char* channel)
{
    unsigned char rxData[2] = {0, 0};
    short         convWord  = 0;

	i2c_read(dev->i2c_desc, rxData, 2, 1);
    convWord = (rxData[0] << 8) + rxData[1];
    *channel = (convWord & 0x3000) >> 12;
    *convValue = (convWord & 0x0FFF) >> (12 - dev->bitsNumber);
}

/***************************************************************************//**
 * @brief Converts a raw sample to volts.
 *
 * @param rawSample - The data sample.
 * @param vRef      - The value of the voltage reference used by the device.
 *
 * @return voltage  - The result of the conversion expressed as volts.
*******************************************************************************/
float AD799x_ConvertToVolts(ad799x_dev *dev,
			    short rawSample,
			    float vRef)
{
    float voltage = 0;

    voltage = vRef * (float)rawSample / (1 << dev->bitsNumber);

    return voltage;
}
