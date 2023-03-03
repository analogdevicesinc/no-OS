/***************************************************************************//**
 *   @file   iio_max31855.c
 *   @brief  Implementation of IIO MAX31855 driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include <errno.h>
#include <stdlib.h>
#include "iio_max31855.h"
#include "max31855.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

static int max31855_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int max31855_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);
static int max31855_iio_update_channels(void *dev, uint32_t mask);
static int max31855_iio_reg_read(struct max31855_iio_dev *dev, uint32_t reg,
				 uint32_t *readval);
static int max31855_iio_read_samples(void* dev, uint16_t* buff,
				     uint32_t samples);

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

static struct iio_attribute max31855_iio_temp_attrs[] = {
	{
		.name = "raw",
		.show = max31855_iio_read_raw
	},
	{
		.name = "scale",
		.show = max31855_iio_read_scale
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type scan_internal = {
	.sign = 's',
	.realbits = 12,
	.storagebits = 16,
	.shift = 4,
	.is_big_endian = false
};

static struct scan_type scan_thermocouple = {
	.sign = 's',
	.realbits = 14,
	.storagebits = 16,
	.shift = 2,
	.is_big_endian = false
};

static struct iio_channel max31855_channels[] = {
	{
		.name = "i_temp",
		.ch_type = IIO_TEMP,
		.address = 0,
		.channel2 = IIO_MOD_TEMP_AMBIENT,
		.modified = true,
		.scan_index = 1,
		.scan_type = &scan_internal,
		.attributes = max31855_iio_temp_attrs
	},
	{
		.name = "t_temp",
		.ch_type = IIO_TEMP,
		.address = 2,
		.scan_index = 0,
		.scan_type = &scan_thermocouple,
		.attributes = max31855_iio_temp_attrs
	}
};

static struct iio_device max31855_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(max31855_channels),
	.channels = max31855_channels,
	.pre_enable = (int32_t (*)())max31855_iio_update_channels,
	.read_dev = (int32_t (*)())max31855_iio_read_samples,
	.debug_reg_read = (int32_t (*)())max31855_iio_reg_read
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Initializes the MAX31855 IIO driver
 * @param iio_dev - The iio device structure.
 * @param init_param - Parameters for the initialization of iio_dev
 * @return 0 in case of success, errno errors otherwise
 */
int max31855_iio_init(struct max31855_iio_dev **iio_dev,
		      struct max31855_iio_dev_init_param *init_param)
{
	int ret;
	struct max31855_iio_dev *descriptor;

	if (!init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = max31855_init(&descriptor->max31855_desc, init_param->max31855_dev_init);
	if (ret)
		goto init_err;

	descriptor->iio_dev = &max31855_iio_dev;

	*iio_dev = descriptor;

	return 0;
init_err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - The iio device structure.
 * @return 0 in case of success, errno errors otherwise
 */
int max31855_iio_remove(struct max31855_iio_dev *desc)
{
	int ret;

	ret = max31855_remove(desc->max31855_desc);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

/**
 * @brief Updates the number of active channels
 * @param dev - The iio device structure.
 * @param mask - Bit mask containing active channels
 * @return 0 in case of success, errno errors otherwise
 */
static int max31855_iio_update_channels(void *dev, uint32_t mask)
{
	struct max31855_iio_dev *iio_max31855;

	if (!dev)
		return -EINVAL;

	iio_max31855 = dev;

	iio_max31855->active_channels = mask;
	iio_max31855->no_of_active_channels = no_os_hweight8((uint8_t)mask);

	return 0;
}

/**
 * @brief MAX31855 reg read wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address (unused)
 * @param readval - Register value
 * @return 0 in case of success, errno errors otherwise
 */
static int max31855_iio_reg_read(struct max31855_iio_dev *dev, uint32_t reg,
				 uint32_t *readval)
{
	return max31855_read_raw(dev->max31855_desc, readval);
}

/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int max31855_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	struct max31855_iio_dev *max31855_iio;
	struct max31855_dev *max31855_dev;
	uint32_t buff;
	uint32_t temp;

	if (!dev)
		return -EINVAL;

	max31855_iio = dev;
	max31855_dev = max31855_iio->max31855_desc;

	switch (channel->type) {
	case IIO_TEMP:
		ret = max31855_read_raw(max31855_dev, &buff);
		if (ret)
			return ret;

		switch(channel->address) {
		case 0:
			temp = no_os_field_get(NO_OS_GENMASK(15, 4), buff);
			temp = no_os_sign_extend32(temp, 11);
			break;
		case 2:
			temp = no_os_field_get(NO_OS_GENMASK(31, 18), buff);
			temp = no_os_sign_extend32(temp, 13);
			break;
		default:
			return -EINVAL;
		}

		return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&temp);

	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int max31855_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val[2] = {0};

	switch (channel->type) {
	case IIO_TEMP:
		switch (channel->address) {
		case 0:
			val[0] = 62;
			val[1] = 500000;
			return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
		case 2:
			val[0] = 250;
			return iio_format_value(buf, len, IIO_VAL_INT, 1, val);
		default:
			return -EINVAL;
		}
	default:
		return -EINVAL;
	}
}

/**
 * @brief Reads the number of given samples for the selected channels
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param samples - Number of samples to be returned
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int max31855_iio_read_samples(void* dev, uint16_t* buff,
				     uint32_t samples)
{
	int ret;
	int16_t i_temp;
	int16_t t_temp;
	uint32_t raw_buff;
	struct max31855_iio_dev *max31855_iio;
	struct max31855_dev *max31855_dev;

	if (!dev)
		return -EINVAL;

	max31855_iio = dev;
	max31855_dev = max31855_iio->max31855_desc;

	for (uint32_t i = 0; i < samples * max31855_iio->no_of_active_channels;) {
		ret = max31855_read_raw(max31855_dev, &raw_buff);
		if (ret)
			return ret;

		if (max31855_iio->active_channels & NO_OS_BIT(0)) {
			t_temp = no_os_field_get(NO_OS_GENMASK(31, 16), raw_buff);
			buff[i] = t_temp;
			i++;
		}

		if (max31855_iio->active_channels & NO_OS_BIT(1)) {
			i_temp = no_os_field_get(NO_OS_GENMASK(15, 0), raw_buff);
			buff[i] = i_temp;
			i++;
		}
	}

	return samples;
}
