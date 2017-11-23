/***************************************************************************//**
 *   @file   AD74xx.c
 *   @brief  Implementation of AD74xx Driver.
 *   @author DNechita(Dan.Nechita@analog.com)
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
#include "AD74xx.h"		// AD74xx definitions.

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Initializes the communication peripheral and the initial Values for
 *        AD74xx Board.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return ret - The result of the initialization procedure.
 *               Example: -1 - SPI peripheral was not initialized or the
 *                             device is not present.
 *                         0 - SPI peripheral was initialized and the
 *                             device is present.
*******************************************************************************/
char AD74xx_Init(ad74xx_dev **device,
		 ad74xx_init_param init_param)
{
	ad74xx_dev *dev;
    unsigned char status;

	dev = (ad74xx_dev *)malloc(sizeof(*dev));
	if (!dev)
		return -1;

	/* SPI */
	status = spi_init(&dev->spi_desc, init_param.spi_init);

	/* GPIO */
	status |= gpio_get(&dev->gpio_cs, init_param.gpio_cs);

	if (dev->gpio_cs)
		status |= gpio_direction_output(dev->gpio_cs,
						GPIO_HIGH);

	dev->deviceBitsNumber = init_param.deviceBitsNumber;
	dev->partNumber = init_param.partNumber;

	*device = dev;

    return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by AD74xx_Init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int32_t AD74xx_remove(ad74xx_dev *dev)
{
	int32_t status;

	status = spi_remove(dev->spi_desc);

	status = gpio_remove(dev->gpio_cs);

	free(dev);

	return status;
}

/***************************************************************************//**
 * @brief Powers down the device. This function is supported only by:
 *        AD7475(partial power down), AD7476, AD7477, AD7478, AD7476A, AD7477A,
 *        AD7478A, AD7479(partial power down).
 *
 * @param dev - The device structure.
 *
 * @return None.
*******************************************************************************/
void AD74xx_PowerDown(ad74xx_dev *dev)
{
    unsigned char dummyValue = 0;

    AD74XX_CS_LOW;
	spi_write_and_read(dev->spi_desc, &dummyValue, 1);
    AD74XX_CS_HIGH;     // CS is brought "High" between 2nd falling edge of SCLK
                        // and 10th falling edge of SCLK(8th falling edge, here)
	spi_write_and_read(dev->spi_desc, &dummyValue, 1);
}

/***************************************************************************//**
 * @brief Powers up the device by performing a dummy read. This function is
 *        supported only by: AD7475(partial power down), AD7476, AD7477, AD7478,
 *        AD7476A, AD7477A, AD7478A, AD7479(partial power down).
 *
 * @param dev - The device structure.
 *
 * @return None.
*******************************************************************************/
void AD74xx_PowerUp(ad74xx_dev *dev)
{
    unsigned char dummyValue[2] = {0, 0};

	spi_write_and_read(dev->spi_desc, dummyValue, 2);
}

/***************************************************************************//**
 * @brief Reads the conversion value.
 *
 * @param dev - The device structure.
 *
 * @return convResult - conversion data.
*******************************************************************************/
unsigned short AD74xx_GetRegisterValue(ad74xx_dev *dev)
{
    unsigned char  dataWord[2] = {0, 0};
    unsigned short convResult  = 0;

	spi_write_and_read(dev->spi_desc, dataWord, 2);
    switch(dev->deviceBitsNumber){
		case 8:
			convResult = (((unsigned short)(dataWord[0] & 0x1F)) << 8) +
                         (dataWord[1] & 0xE0);
            convResult = convResult >> 5;
			break;
		case 10:
			convResult = (((unsigned short)(dataWord[0] & 0x1F)) << 8) +
                         (dataWord[1] & 0xF8);
            convResult = convResult >> 3;
			break;
		case 12:
			convResult = (((unsigned short)(dataWord[0] & 0x1F)) << 8) +
                         (dataWord[1] & 0xFE);
            convResult = convResult >> 1;
			break;
		default: // 16 bits
			convResult = (((unsigned short)(dataWord[0] & 0xFF)) << 8) +
                         dataWord[1];
			break;
	}

    return convResult;
}

/***************************************************************************//**
 * @brief Converts a raw sample to volts.
 *
 * @param dev      - The device structure.
 * @param rawValue - The data sample.
 * @param vRef     - The value of the voltage reference used by the device.
 *
 * @return voltage - The result of the conversion expressed as volts.
*******************************************************************************/
float AD74xx_ConvertToVolts(ad74xx_dev *dev,
			    unsigned short rawValue,
			    float vRef)
{
    float voltage = 0;

    voltage = (rawValue) * vRef / (1 << dev->deviceBitsNumber);

    return voltage;
}
