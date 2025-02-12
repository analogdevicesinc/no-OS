/***************************************************************************//**
 *   @file   iio_admfm2000.c
 *   @brief  Implementation of admfm2000 IIO Driver.
 *   @author Ramona Nechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_admfm2000.h"
#include "no_os_alloc.h"

/**
 * @brief Get the channel gain.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int admfm2000_iio_read_gain(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct admfm2000_iio_dev *iio_admfm2000 = (struct admfm2000_iio_dev *)dev;
	struct admfm2000_dev *admfm2000_dev;
	int32_t gain;
	int32_t vals[2];
	int32_t tmp;
	int32_t ret;

	if (!iio_admfm2000)
		return -EINVAL;

	admfm2000_dev = iio_admfm2000->admfm2000_dev;

	if (!admfm2000_dev)
		return -EINVAL;

	ret = admfm2000_get_dsa_raw(admfm2000_dev, channel->ch_num, &gain);
	if (ret)
		return ret;
	tmp = ~(gain) * -1000;
	vals[0] = tmp / 1000;
	vals[1] = (tmp % 1000) * 1000;
	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO_DB, 1,
				(int32_t *)vals);
}

/**
 * @brief Set the channel gain.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int admfm2000_iio_write_gain(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct admfm2000_iio_dev *iio_admfm2000 = (struct admfm2000_iio_dev *)dev;
	struct admfm2000_dev *admfm2000_dev;
	int32_t val, val2;
	int32_t gain, tmp;
	int32_t ret;

	if (!iio_admfm2000)
		return -EINVAL;

	admfm2000_dev = iio_admfm2000->admfm2000_dev;

	if (!admfm2000_dev)
		return -EINVAL;

	ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO_DB, &val, &val2);
	if (ret)
		return ret;

	if (val < 0)
		tmp = (val * 1000) - (val2 / 1000);
	else
		tmp = (val * 1000) + (val2 / 1000);
	gain = ~((abs(tmp) / 1000)) & 0x1F;
	ret = admfm2000_set_dsa_raw(admfm2000_dev, channel->ch_num, gain);
	if (ret)
		return ret;

	return len;
}

static struct iio_attribute admfm2000_iio_ch_attrs[] = {
	{
		.name = "hardwaregain",
		.shared = IIO_SEPARATE,
		.show = admfm2000_iio_read_gain,
		.store = admfm2000_iio_write_gain,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel admfm2000_channels[] = {
	{
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.ch_out = true,
		.indexed = true,
		.attributes = admfm2000_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.ch_type = IIO_VOLTAGE,
		.channel = 1,
		.ch_out = true,
		.indexed = true,
		.attributes = admfm2000_iio_ch_attrs,
		.scan_type = NULL,
	},
};

static struct iio_device admfm2000_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(admfm2000_channels),
	.channels = admfm2000_channels,
};

/**
 * @brief Initialize the iio device.
 * @param iio_dev - The iio device structure.
 * @param init_param - The iio device initialization structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int admfm2000_iio_init(struct admfm2000_iio_dev **iio_dev,
		       struct admfm2000_iio_dev_init_param *init_param)
{
	struct admfm2000_iio_dev *iio_admfm2000;
	struct admfm2000_dev *admfm2000_dev;
	int32_t ret;

	iio_admfm2000 = (struct admfm2000_iio_dev *)no_os_calloc(1,
			sizeof(*iio_admfm2000));
	if (!iio_admfm2000)
		return -ENOMEM;

	iio_admfm2000->iio_dev = &admfm2000_iio_dev;

	ret = admfm2000_init(&iio_admfm2000->admfm2000_dev,
			     init_param->admfm2000_dev_init);
	if (ret)
		return ret;

	*iio_dev = iio_admfm2000;

	return 0;
}

/**
 * @brief Free the resources allocated by admfm2000_iio_init().
 * @param desc - The iio device structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int admfm2000_iio_remove(struct admfm2000_iio_dev *desc)
{
	int32_t ret;

	ret = admfm2000_remove(desc->admfm2000_dev);
	if (ret)
		return ret;

	no_os_free(desc->admfm2000_dev);
	no_os_free(desc);

	return 0;
}
