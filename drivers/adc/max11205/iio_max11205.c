/***************************************************************************//**
 *   @file   iio_max11205.c
 *   @brief  Implementation of iio max11205 driver
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "max11205.h"
#include "iio_max11205.h"
#include "errno.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define MAX11205_BIT_SCALE		15
#define MAX11205_NEW_DATA_TIMEOUT	12000000
#define MAX11205A_OUT_DATA_RATE		116
#define MAX11205B_OUT_DATA_RATE		13

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure. In case of success, the size of the read data is returned.
 */
static int max11205_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int32_t val;
	int16_t data_raw;
	bool new_data_avail = false;
	struct max11205_iio_dev *iio_max11205;
	uint32_t timeout = MAX11205_NEW_DATA_TIMEOUT;

	if (!dev)
		return -EINVAL;

	iio_max11205 = (struct max11205_iio_dev *)dev;

	if (!iio_max11205->max11205_dev)
		return -EINVAL;

	switch (channel->type) {
	case IIO_VOLTAGE:
		while (!new_data_avail && timeout) {
			ret = max11205_get_data_raw(iio_max11205->max11205_dev, &new_data_avail,
						    &data_raw);
			if (ret)
				return ret;
			timeout --;
		}

		if (!timeout)
			return -EIO;

		val = (int32_t) data_raw;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for scale attribute.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure. In case of success, the size of the read data is returned.
 */
static int max11205_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct max11205_iio_dev *iio_max11205;

	if (!dev)
		return -EINVAL;

	iio_max11205 = (struct max11205_iio_dev *)dev;

	if (!iio_max11205->max11205_dev)
		return -EINVAL;

	switch (channel->type) {
	case IIO_VOLTAGE:
		vals[0] = iio_max11205->max11205_dev->vref_mv;
		vals[1] = MAX11205_BIT_SCALE;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for sampling frequency attribute.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure. In case of success, the size of the read data is returned.
 */
static int max11205_iio_read_sampling_frequency(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;
	struct max11205_iio_dev *iio_max11205;

	if (!dev)
		return -EINVAL;

	iio_max11205 = (struct max11205_iio_dev *)dev;

	switch (channel->type) {
	case IIO_VOLTAGE:
		val = iio_max11205->sampling_frequency;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Reads the number of given samples
 *
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with requested data.
 * @param samples - Number of samples to be returned.
 * @return ret    - Result of the reading procedure. In case of success, the size of the read data is returned.
 */
static int max11205_iio_read_samples(void* dev, int16_t* buff, uint32_t samples)
{
	int ret;
	bool new_data_avail = false;
	struct max11205_iio_dev *iio_max11205;
	uint32_t timeout = MAX11205_NEW_DATA_TIMEOUT;
	uint32_t i;

	if (!dev)
		return -EINVAL;

	iio_max11205 = (struct max11205_iio_dev *)dev;

	if (!iio_max11205->max11205_dev)
		return -EINVAL;

	for (i = 0; i < samples; i++) {
		while (!new_data_avail && timeout) {
			ret = max11205_get_data_raw(iio_max11205->max11205_dev, &new_data_avail,
						    &buff[i]);
			if (ret)
				return ret;
			timeout --;
		}

		if (!timeout)
			return -EIO;

		new_data_avail = false;
	}

	return samples;
}

static struct iio_attribute max11205_iio_adc_attrs[] = {
	{
		.name = "raw",
		.show = max11205_iio_read_raw,
	},
	{
		.name = "scale",
		.show = max11205_iio_read_scale,
	},
	{
		.name = "sampling_frequency",
		.show = max11205_iio_read_sampling_frequency,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct scan_type max11205_iio_voltage_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.is_big_endian = false,
};

static struct iio_channel max11205_channels[] = {
	{
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.scan_type = &max11205_iio_voltage_scan_type,
		.attributes = max11205_iio_adc_attrs,
	},
};

static struct iio_device max11205_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(max11205_channels),
	.channels = max11205_channels,
	.read_dev = (int32_t (*)())max11205_iio_read_samples,
};

/**
 * @brief Initializes the max11205 iio driver
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial parameters.
 * @return ret       - Result of the initialization procedure.
 */
int max11205_iio_init(struct max11205_iio_dev **iio_dev,
		      struct max11205_iio_dev_init_param *init_param)
{
	int ret;
	struct max11205_iio_dev *desc;

	desc = (struct max11205_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &max11205_iio_dev;
	if (init_param->dev_id == MAX11205A)
		desc->sampling_frequency = MAX11205A_OUT_DATA_RATE;
	else
		desc->sampling_frequency = MAX11205B_OUT_DATA_RATE;

	ret = max11205_init(&desc->max11205_dev, *(init_param->max11205_dev_init));
	if (ret)
		goto error_max11205_init;

	*iio_dev = desc;

	return 0;

error_max11205_init:
	no_os_free(desc);
	return ret;
}

/**
 * @brief Free the resources allocated by max11205_iio_init().
 * @param desc - The IIO device structure.
 * @return ret - Result of the remove procedure.
 */
int max11205_iio_remove(struct max11205_iio_dev *desc)
{
	int ret;

	ret = max11205_remove(desc->max11205_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
