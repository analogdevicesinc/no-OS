/***************************************************************************//**
 *   @file   iio_adxl355.c
 *   @brief  Implementation of IIO ADXL355 Driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "iio_adxl355.h"
#include "adxl355.h"
#include "no_os_units.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ACCEL_AXIS_X (uint32_t) 0
#define ACCEL_AXIS_Y (uint32_t) 1
#define ACCEL_AXIS_Z (uint32_t) 2

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum adxl355_iio_accel_chans {
	chan_x,
	chan_y,
	chan_z,
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int adxl355_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_offset(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_samples(void* dev, int* buff, uint32_t samples);
static int adxl355_iio_update_channels(void* dev, uint32_t mask);

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

static struct iio_attribute adxl355_iio_accel_attrs[] = {
	{
		.name = "raw",
		.show = adxl355_iio_read_raw,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type adxl355_iio_accel_scan_type = {
	.sign = 's',
	.realbits = 20,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false
};

#define ADXL355_ACCEL_CHANNEL(index, reg, axis) { \
	.ch_type = IIO_ACCEL,                         \
	.channel = index,                             \
	.address = reg,                               \
	.modified = true,                             \
	.channel2 = IIO_MOD_##axis,                   \
	.scan_type = &adxl355_iio_accel_scan_type,    \
	.scan_index = index,                          \
	.attributes = adxl355_iio_accel_attrs,        \
	.ch_out = false                               \
}

static struct iio_channel adxl355_channels[] = {
	ADXL355_ACCEL_CHANNEL(0, chan_x, X),
	ADXL355_ACCEL_CHANNEL(1, chan_y, Y),
	ADXL355_ACCEL_CHANNEL(2, chan_z, Z),
};

static struct iio_device adxl355_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adxl355_channels),
	.channels = adxl355_channels,
	.pre_enable = (int32_t (*)())adxl355_iio_update_channels,
	.read_dev = (int32_t (*)())adxl355_iio_read_samples,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

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
static int adxl355_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	uint16_t temp_raw;
	uint32_t accel_x;
	uint32_t accel_y;
	uint32_t accel_z;
	struct adxl355_iio_dev *iio_adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	switch (channel->type) {
	case IIO_ACCEL:
		accel_x = (iio_adxl355->test_ramp + 2048) % 524288 - 262144;
		accel_y = (iio_adxl355->test_ramp + 2048) % 524288 - 262144;
		accel_z = 0;
		iio_adxl355->test_ramp += 1024;

		switch (channel->address) {
		case ACCEL_AXIS_X:
			ret = no_os_sign_extend32(accel_x, 19);
			break;
		case ACCEL_AXIS_Y:
			ret = no_os_sign_extend32(accel_y, 19);
			break;
		case ACCEL_AXIS_Z:
			ret = no_os_sign_extend32(accel_z, 19);
			break;

		default:
			return -EINVAL;
		}

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);

	default:
		return -EINVAL;
	}
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
static int adxl355_iio_read_samples(void* dev, int* buff, uint32_t samples)
{
	uint32_t data_x;
	uint32_t data_y;
	uint32_t data_z;
	uint16_t raw_temp;
	struct adxl355_iio_dev *iio_adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	for(uint32_t i = 0; i < samples*iio_adxl355->no_of_active_channels;) {
		data_x = (iio_adxl355->test_ramp + 2048) % 524288 - 262144;
		data_y = (iio_adxl355->test_ramp + 2048) % 524288 - 262144;
		data_z = 0;

		if (iio_adxl355->active_channels & NO_OS_BIT(0)) {
			buff[i] = data_x;//no_os_sign_extend32(data_x, 19);
			i++;
		}
		if (iio_adxl355->active_channels & NO_OS_BIT(1)) {
			buff[i] = data_y;//no_os_sign_extend32(data_y, 19);
			i++;
		}
		if (iio_adxl355->active_channels & NO_OS_BIT(2)) {
			buff[i] = data_z;//no_os_sign_extend32(data_z, 19);
			i++;
		}
	}

	return samples;
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
static int adxl355_iio_update_channels(void* dev, uint32_t mask)
{
	struct adxl355_iio_dev *iio_adxl355;
	uint8_t counter = 0;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	iio_adxl355->active_channels = mask;

	if (mask & NO_OS_BIT(0))
		counter ++;
	if (mask & NO_OS_BIT(1))
		counter ++;
	if (mask & NO_OS_BIT(2))
		counter ++;
	if (mask & NO_OS_BIT(3))
		counter ++;

	iio_adxl355->no_of_active_channels = counter;

	return 0;
}

/***************************************************************************//**
 * @brief Initializes the ADXL355 IIO driver
 *
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int adxl355_iio_init(struct adxl355_iio_dev **iio_dev,
		     struct adxl355_iio_dev_init_param *init_param)
{
	int ret;
	struct adxl355_iio_dev *desc;

	desc = (struct adxl355_iio_dev *)calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adxl355_iio_dev;

	*iio_dev = desc;

	return 0;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adxl355_iio_init().
 *
 * @param desc - The IIO device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int adxl355_iio_remove(struct adxl355_iio_dev *desc)
{
	int ret;

	free(desc);

	return 0;
}
