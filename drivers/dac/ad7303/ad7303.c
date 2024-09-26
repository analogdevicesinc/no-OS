/***************************************************************************//**
 *   @file   AD7303.c
 *   @brief  Implementation of AD7303 Driver.
 *   @author Mihai Bancisor(Mihai.Bancisor@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "ad7303.h"           // AD7303 definitions.
#include "no_os_alloc.h"

/***************************************************************************//**
 * @brief Initializes SPI communication.
 *
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return Result of the initialization procedure.
 *            Example: -1 - SPI peripheral was not initialized.
 *                      0 - SPI peripheral is initialized.
*******************************************************************************/
int8_t ad7303_init(struct ad7303_dev **device,
		   struct ad7303_init_param init_param)
{
	struct ad7303_dev *dev;
	int8_t status;

	dev = (struct ad7303_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -1;

	status = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

	*device = dev;

	return status;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7303_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int32_t ad7303_remove(struct ad7303_dev *dev)
{
	int32_t ret;

	ret = no_os_spi_remove(dev->spi_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Sends data to AD7303.
 *
 * @param dev         - The device structure.
 * @param control_reg - Value of control register.
 *                     Example:
 *                     AD7303_INT | AD7303_LDAC | AD7303_A  - enables internal
 *                     reference and loads DAC A input register from shift
 *                     register and updates both DAC A and DAC B DAC registers.
 * @param data_reg    - Value of data register.
 *
 * @return None.
*******************************************************************************/
void ad7303_write(struct ad7303_dev *dev,
		  uint8_t control_reg,
		  uint8_t data_reg)
{
	static uint8_t write_data[2] = {0, 0};

	write_data[0] = control_reg;
	write_data[1] = data_reg;
	no_os_spi_write_and_read(dev->spi_desc,
				 write_data,
				 2);
}
