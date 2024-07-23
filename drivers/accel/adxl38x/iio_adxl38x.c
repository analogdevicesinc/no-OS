/***************************************************************************//**
 *   @file   iio_adxl38x.c
 *   @brief  Implementation of IIO ADXL38x Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "iio_adxl38x.h"
#include "adxl38x.h"
#include "no_os_units.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ACCEL_AXIS_X (uint32_t) 0
#define ACCEL_AXIS_Y (uint32_t) 1
#define ACCEL_AXIS_Z (uint32_t) 2

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum adxl38x_iio_accel_chans {
	chan_x,
	chan_y,
	chan_z,
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int adxl38x_iio_read_reg(struct adxl38x_iio_dev *dev, uint32_t reg,
				uint32_t *readval);
static int adxl38x_iio_write_reg(struct adxl38x_iio_dev *dev, uint32_t reg,
				 uint32_t writeval);
static int adxl38x_iio_update_channels(void* dev, uint32_t mask);
static int adxl38x_iio_read_samples(void* dev, int* buff, uint32_t samples);

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

static struct scan_type adxl38x_iio_accel_scan_type = {
	.sign = 's',
	.realbits = 12,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false
};

#define ADXL38x_ACCEL_CHANNEL(index, reg, axis) {     \
	.ch_type = IIO_ACCEL,                         \
	.channel = index,                             \
	.address = reg,                               \
	.modified = true,                             \
	.channel2 = IIO_MOD_##axis,                   \
	.scan_type = &adxl38x_iio_accel_scan_type,    \
	.scan_index = index,                          \
	.attributes = NULL,                           \
	.ch_out = false                               \
}

static struct iio_channel adxl38x_channels[] = {
	ADXL38x_ACCEL_CHANNEL(0, chan_x, X),
	ADXL38x_ACCEL_CHANNEL(1, chan_y, Y),
	ADXL38x_ACCEL_CHANNEL(2, chan_z, Z),
};

static struct iio_device adxl38x_iio_dev = {
	.debug_reg_read = (int32_t (*)())adxl38x_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adxl38x_iio_write_reg,
	.num_ch = NO_OS_ARRAY_SIZE(adxl38x_channels),
	.channels = adxl38x_channels,
	.pre_enable = (int32_t (*)())adxl38x_iio_update_channels,
	.read_dev = (int32_t (*)())adxl38x_iio_read_samples,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Wrapper for reading ADXL38x register.
 *
 * @param device  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 *
 * @return ret    - Result of the reading procedure.
*******************************************************************************/
static int adxl38x_iio_read_reg(struct adxl38x_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	return adxl38x_read_device_data(dev->adxl38x_dev, reg, 1, (uint8_t *)readval);
}

/***************************************************************************//**
 * @brief Wrapper for writing to ADXL38x register.
 *
 * @param device   - The iio device structure.
 * @param reg	   - Address of the register to be written to.
 * @param writeval - Data to be written.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int adxl38x_iio_write_reg(struct adxl38x_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	uint8_t val = writeval;

	return adxl38x_write_device_data(dev->adxl38x_dev, reg, 1, &val);
}

/***************************************************************************//**
 * @brief Updates the number of active channels and the total number of
 * 		  active channels
 *
 * @param dev  - The iio device structure.
 * @param mask - Mask of the active channels
 *
 * @return ret - Result of the updating procedure.
*******************************************************************************/
static int adxl38x_iio_update_channels(void* dev, uint32_t mask)
{
	struct adxl38x_iio_dev *iio_adxl38x;
	uint8_t counter = 0;

	if (!dev)
		return -EINVAL;

	iio_adxl38x = (struct adxl38x_iio_dev *)dev;

	iio_adxl38x->active_channels = mask;

	if (mask & NO_OS_BIT(0))
		counter ++;
	if (mask & NO_OS_BIT(1))
		counter ++;
	if (mask & NO_OS_BIT(2))
		counter ++;

	iio_adxl38x->no_of_active_channels = counter;

	return 0;
}

/***************************************************************************//**
 * @brief Reads the number of given samples for the selected channels
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param samples - Number of samples to be returned
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl38x_iio_read_samples(void* dev, int* buff, uint32_t samples)
{
	struct adxl38x_iio_dev *iio_adxl38x;
	struct adxl38x_dev *adxl38x;
	uint16_t data_x;
	uint16_t data_y;
	uint16_t data_z;

	if (!dev)
		return -EINVAL;

	iio_adxl38x = (struct adxl38x_iio_dev *)dev;

	if (!iio_adxl38x->adxl38x_dev)
		return -EINVAL;

	adxl38x = iio_adxl38x->adxl38x_dev;

	for(uint32_t i = 0; i < samples*iio_adxl38x->no_of_active_channels;) {
		adxl38x_get_raw_xyz(adxl38x, &data_x, &data_y, &data_z);

		if (iio_adxl38x->active_channels & NO_OS_BIT(0)) {
			buff[i] = data_x;
			i++;
		}
		if (iio_adxl38x->active_channels & NO_OS_BIT(1)) {
			buff[i] = data_y;
			i++;
		}
		if (iio_adxl38x->active_channels & NO_OS_BIT(2)) {
			buff[i] = data_z;
			i++;
		}
	}

	return samples;
}

/***************************************************************************//**
 * @brief Initializes the ADXL38x IIO driver
 *
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int adxl38x_iio_init(struct adxl38x_iio_dev **iio_dev,
		     struct adxl38x_iio_dev_init_param *init_param)
{
	int ret;
	struct adxl38x_iio_dev *desc;

	desc = (struct adxl38x_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adxl38x_iio_dev;

	ret = adxl38x_init(&desc->adxl38x_dev, *(init_param->adxl38x_dev_init));
	if (ret)
		goto error_init;

	ret = adxl38x_soft_reset(desc->adxl38x_dev);
	if (ret)
		goto error_config;

	*iio_dev = desc;

	return 0;

error_init:
	no_os_free(desc);
	return ret;
error_config:
	adxl38x_remove(desc->adxl38x_dev);
	no_os_free(desc);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adxl38x_iio_init().
 *
 * @param desc - The IIO device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int adxl38x_iio_remove(struct adxl38x_iio_dev *desc)
{
	int ret;

	ret = adxl38x_remove(desc->adxl38x_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
