/***************************************************************************//**
 *   @file   adf5902.c
 *   @brief  Implementation of adf5902 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include "adf5902.h"
#include "error.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Writes 4 bytes of data to ADF5902.
 * @param dev - The device structure.
 * @param data - Data value to write.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf5902_write(struct adf5902_dev *device, uint32_t data)
{
	uint8_t buff[4];
	uint8_t buff_size = 4;

	buff[0] = data >> 24;
	buff[1] = data >> 16;
	buff[2] = data >> 8;
	buff[3] = data;

	return spi_write_and_read(dev->spi_desc, buff, buff_size);
}

/**
 * @brief Readback data from ADF5902.
 * @param dev - The device structure.
 * @param data - Data to be read.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t adf5902_readback(struct adf5902_dev *device, uint32_t *data)
{
	int32_t ret;
	uint32_t i;
	uint8_t buff[4];
	uint8_t buff_size = 4;

	buff[0] = *data >> 24;
	buff[1] = *data >> 16;
	buff[2] = *data >> 8;
	buff[3] = *data;

	ret = spi_write_and_read(dev->spi_desc, buff, buff_size);
	if (ret != SUCCESS)
		return ret;

	for (i = 0; i < buff_size; i++)
		*data = (*data << 8) | buff[i];

	return ret;
}

/**
 * @brief Initializes the ADF5902.
 * @param device - The device structure.
 * @param init_param - The structure containing the device initial parameters.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf5902_init(struct adf5902_dev **device,
		     adf5902_init_param *init_param)
{
	int32_t ret;
	struct adf5902_dev *dev;

	dev = (struct adf5902_dev *)calloc(1, sizeof(*dev));
	if (!dev)
		return FAILUE;

	/* SPI */
	ret = spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret != SUCCESS)
		return ret;

	*device = dev;

	return ret;
}

/**
 * @brief Free resoulces allocated for ADF5902
 * @param device - The device structure.
 * @return Returns 0 in case of success or negative error code.
 */
int32_t adf5902_remove(struct adf5902_dev *device)
{
	int32_t ret;

	ret = spi_remove(device->spi_desc);

	free(device);

	return ret;
}