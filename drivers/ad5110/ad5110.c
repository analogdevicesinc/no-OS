/**************************************************************************//**
*   @file   AD5110.c
*   @brief  Implementation of AD5110 Driver for Microblaze processor.
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
******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include "platform_drivers.h"
#include "ad5110.h"

/***************************************************************************//**
 * @brief Initializes the communication with the device.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example:  0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
*******************************************************************************/
char ad5110_Init(ad5110_dev **device,
		 ad5110_init_param init_param)
{
	ad5110_dev *dev;
    char status;

	dev = (ad5110_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	dev->ad5110DevAddr = init_param.ad5110DevAddr;
	status = i2c_init(&dev->i2c_desc, init_param.i2c_init);

	*device = dev;

    return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5110_Init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t ad5110_remove(ad5110_dev *dev)
{
	int32_t ret;

	ret = i2c_remove(dev->i2c_desc);

	free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Write the content of serial register data to RDAC.
 *
 * @param dev       - The device structure.
 * @param rdacValue - Value of the serial register.
 *
 * @return none.
*******************************************************************************/
void ad5110_WriteRdac(ad5110_dev *dev,
		      unsigned char rdacValue)
{
    unsigned char dataBuffer[2];

    dataBuffer[0] = CMD_WR_RDAC;
    dataBuffer[1] = rdacValue;
	i2c_write(dev->i2c_desc,
		  dataBuffer,
		  2,
		  1);
}

/***************************************************************************//**
 * @brief Read the content of RDAC register.
 *
 * @param dev - The device structure.
 *
 * @return rdacValue - Value read from register.
*******************************************************************************/
unsigned char ad5110_ReadRdac(ad5110_dev *dev)
{
    unsigned char dataBuffer[2];

    dataBuffer[0] = CMD_RD_RDAC;
    dataBuffer[1] = 0;
	i2c_write(dev->i2c_desc,
		  dataBuffer,
		  2,
		  1);

	i2c_read(dev->i2c_desc,
		 dataBuffer,
		 1,
		 1);

    return dataBuffer[0];
}

/***************************************************************************//**
 * @brief Write the content of RDAC register to EEPROM.
 *
 * @param dev - The device structure.
 *
 * @return none.
*******************************************************************************/
void ad5110_WriteRdacEeprom(ad5110_dev *dev)
{
    unsigned char dataBuffer[2];

    dataBuffer[0] = CMD_WR_RDAC_EEPROM;
    dataBuffer[1] = 0;
	i2c_write(dev->i2c_desc,
		  dataBuffer,
		  2,
		  1);
}

/***************************************************************************//**
 * @brief Read wiper position from EEPROM.
 *
 * @param dev - The device structure.
 *
 * @return wiperValue - Value read from EEPROM.
*******************************************************************************/
unsigned char ad5110_ReadWiper(ad5110_dev *dev)
{
    unsigned char dataBuffer[2];

    dataBuffer[0] = CMD_RD_EEPROM;
    dataBuffer[1] = WIPER_POSITION;
	i2c_write(dev->i2c_desc,
		  dataBuffer,
		  2,
		  1);

	i2c_read(dev->i2c_desc,
		 dataBuffer,
		 1,
		 1);

    return dataBuffer[0];
}

/***************************************************************************//**
 * @brief Read resistor tolerance from EEPROM.
 *
 * @param dev - The device structure.
 *
 * @return resTolerance - Value read from EEPROM.
*******************************************************************************/
unsigned char ad5110_ReadResTolerance(ad5110_dev *dev)
{
    unsigned char dataBuffer[2];

    dataBuffer[0] = CMD_RD_EEPROM;
    dataBuffer[1] = RESISTOR_TOLERANCE;
	i2c_write(dev->i2c_desc,
		  dataBuffer,
		  2,
		  1);

	i2c_read(dev->i2c_desc,
		 dataBuffer,
		 1,
		 1);

    return dataBuffer[0];
}

/***************************************************************************//**
 * @brief Software reset; makes a refresh of RDAC register with EEPROM.
 *
 * @param dev - The device structure.
 *
 * @return none.
*******************************************************************************/
void ad5110_Reset(ad5110_dev *dev)
{
    unsigned char dataBuffer[2];

    dataBuffer[0] = CMD_RESET;
    dataBuffer[1] = 0;
	i2c_write(dev->i2c_desc,
		  dataBuffer,
		  2,
		  1);
}

/***************************************************************************//**
 * @brief Software shutdown.
 *
 * @param dev   - The device structure.
 * @param value - the value written to shut down register.
 *              Example: ShutDownOff - the device is on.
 *                       ShutDownOn  - the device is shut down.
 *
 * @return none.
*******************************************************************************/
void ad5110_ShutDown(ad5110_dev *dev,
		     unsigned char value)
{
    unsigned char dataBuffer[2];

    dataBuffer[0] = CMD_SHUT_DOWN;
    dataBuffer[1] = value;
	i2c_write(dev->i2c_desc,
		  dataBuffer,
		  2,
		  1);
}
