/***************************************************************************//**
 *   @file   iio_ad405x.c
 *   @brief  Implementation of IIO AD405X Driver.
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_ad405x.h"
#include "ad405x.h"
#include "no_os_units.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
extern int ad405x_read_device_data(struct ad405x_dev *dev,
				    uint8_t base_address, uint16_t size, uint8_t *read_data);
extern int ad405x_write_device_data(struct ad405x_dev *dev,
				     uint8_t base_address, uint16_t size, uint8_t *write_data);
static int ad405x_iio_read_samples(void* dev, int* buff, uint32_t samples);
static int ad405x_iio_read_reg(struct ad405x_iio_dev *dev, uint32_t reg,
				uint32_t *readval);
static int ad405x_iio_write_reg(struct ad405x_iio_dev *dev, uint32_t reg,
				 uint32_t writeval);
static int ad405x_iio_read_raw(void *dev, char *buf, uint32_t len, intptr_t priv);
static int32_t ad405x_trigger_handler(struct iio_device_data *dev_data);

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

static struct iio_device ad405x_iio_dev = {
	.trigger_handler = (int32_t (*)())ad405x_trigger_handler,
	.read_dev = (int32_t (*)())ad405x_iio_read_samples,
	.debug_reg_read = (int32_t (*)())ad405x_iio_read_reg,
	.debug_reg_write = (int32_t (*)())ad405x_iio_write_reg
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Wrapper for reading AD405X register.
 *
 * @param device  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 *
 * @return ret    - Result of the reading procedure.
*******************************************************************************/
static int ad405x_iio_read_reg(struct ad405x_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	return ad405x_read_device_data(dev->ad405x_dev, reg, 1, (uint8_t *)readval);
}

/***************************************************************************//**
 * @brief Wrapper for writing to AD405X register.
 *
 * @param device   - The iio device structure.
 * @param reg	   - Address of the register to be written to.
 * @param writeval - Data to be written.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int ad405x_iio_write_reg(struct ad405x_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	uint8_t val = writeval;
	return ad405x_write_device_data(dev->ad405x_dev, reg, 1, &val);
}

/***************************************************************************//**
 * @brief Handles the read request for raw attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int ad405x_iio_read_raw(void *dev, char *buf, uint32_t len, intptr_t priv)
{
	int32_t ret;
	uint32_t data;
	struct ad405x_iio_dev *iio_ad405x;
	struct ad405x_dev *ad405x;

	if (!dev)
		return -EINVAL;

	iio_ad405x = (struct ad405x_iio_dev *)dev;

	if (!iio_ad405x->ad405x_dev)
		return -EINVAL;

	ad405x = iio_ad405x->ad405x_dev;

	ret = ad405x_get_raw(ad405x, &data);
	if (ret)
		return ret;
}

/***************************************************************************//**
 * @brief Handles trigger: reads one data-set and writes it to the buffer.
 *
 * @param dev_data  - The iio device data structure.
 *
 * @return ret - Result of the handling procedure. In case of success, the size
 * 				 of the written data is returned.
*******************************************************************************/
static int32_t ad405x_trigger_handler(struct iio_device_data *dev_data)
{
	int32_t data_buff[1];
	uint32_t data;
	uint8_t i = 0;

	struct ad405x_iio_dev *iio_ad405x;
	struct ad405x_dev *ad405x;

	if (!dev_data)
		return -EINVAL;

	iio_ad405x = (struct ad405x_iio_dev *)dev_data->dev;

	if (!iio_ad405x->ad405x_dev)
		return -EINVAL;

	ad405x = iio_ad405x->ad405x_dev;

	ad405x_get_raw(ad405x, &data);

	if (dev_data->buffer->active_mask & NO_OS_BIT(0)) {
		data_buff[0] = no_os_sign_extend32(data, 19);
		i++;
	}

	return iio_buffer_push_scan(dev_data->buffer, &data_buff[0]);
}

/***************************************************************************//**
 * @brief Initializes the AD405X IIO driver
 *
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int ad405x_iio_init(struct ad405x_iio_dev **iio_dev,
		     struct ad405x_iio_dev_init_param *init_param)
{
	int ret;
	struct ad405x_iio_dev *desc;

	desc = (struct ad405x_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &ad405x_iio_dev;

	// Initialize AD405X driver
	ret = ad405x_init(&desc->ad405x_dev, *(init_param->ad405x_dev_init));
	if (ret)
		goto error_ad405x_init;

	// Perform soft reset
	// In some cases it not might be successfull at first try
	// Retrial mechanism can be implemented
	ret = ad405x_soft_reset(desc->ad405x_dev);
	if (ret)
		goto error_config;

	// TODO config procedure

	*iio_dev = desc;

	return 0;

error_ad405x_init:
	no_os_free(desc);
	return ret;
error_config:
	ad405x_remove(desc->ad405x_dev);
	no_os_free(desc);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad405x_iio_init().
 *
 * @param desc - The IIO device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int ad405x_iio_remove(struct ad405x_iio_dev *desc)
{
	int ret;

	ret = ad405x_remove(desc->ad405x_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
