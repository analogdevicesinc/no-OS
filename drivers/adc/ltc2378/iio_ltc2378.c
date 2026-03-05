/***************************************************************************//**
 *   @file   iio_ltc2378.c
 *   @brief  Implementation of iio_ltc2378.c.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
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
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "iio.h"
#include "ltc2378.h"
#include "iio_ltc2378.h"

enum ltc2378_iio_attr {
	LTC2378_IIO_RAW,
	LTC2378_IIO_SCALE,
	LTC2378_IIO_PROCESSED,
};

/**
 * @brief Read from ADC and provide IIO data
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested attr val.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ltc2378_iio_read(void *dev, char *buf, uint32_t len,
			    const struct iio_ch_info *channel, intptr_t priv)
{
	struct ltc2378_iio_desc *iio_ltc2378 = dev;
	struct ltc2378_dev *ltc2378 = iio_ltc2378->ltc2378_dev;
	int ret;
	int32_t vals[2];
	uint32_t raw_data;
	int32_t voltage_uv;

	(void)channel;

	if (!dev || !iio_ltc2378->ltc2378_dev)
		return -EINVAL;

	switch (priv) {
	case LTC2378_IIO_RAW:
		ret = ltc2378_read_raw(ltc2378, &raw_data);
		if (ret)
			return ret;

		vals[0] = raw_data;
		vals[1] = 0;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);

	case LTC2378_IIO_SCALE:
		vals[0] = ltc2378->vref_uv;
		if (ltc2378->input_mode == LTC2378_UNIPOLAR) {
			vals[1] = LTC2378_IIO_SCALE_UNIPOLAR_SHIFT;
		} else {
			vals[1] = LTC2378_IIO_SCALE_BIPOLAR_SHIFT;
		}
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);

	case LTC2378_IIO_PROCESSED:
		ret = ltc2378_read_raw(ltc2378, &raw_data);
		if (ret)
			return ret;

		ret = ltc2378_raw_to_uv(ltc2378, raw_data, &voltage_uv);
		if (ret)
			return ret;

		vals[0] = voltage_uv;
		vals[1] = 0;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);

	default:
		return -EINVAL;
	}
}

static struct iio_attribute ltc2378_iio_attrs[] = {
	{
		.name = "raw",
		.show = ltc2378_iio_read,
		.store = NULL,
		.priv = LTC2378_IIO_RAW,
	},
	{
		.name = "scale",
		.show = ltc2378_iio_read,
		.store = NULL,
		.priv = LTC2378_IIO_SCALE,
	},
	{
		.name = "processed",
		.show = ltc2378_iio_read,
		.store = NULL,
		.priv = LTC2378_IIO_PROCESSED,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ltc2378_iio_channels[] = {
	{
		.name = "voltage0",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.indexed = true,
		.attributes = ltc2378_iio_attrs,
	}
};

/**
 * @brief Read debug register
 * @param dev - The iio device structure.
 * @param reg - Register address to read from
 *              0x00: Raw ADC data
 *              0x01: Input mode (0=bipolar, 1=unipolar)
 *              0x02: Reference voltage (lower 16 bits in uV)
 *              0x03: Reference voltage (upper 16 bits in uV)
 * @param val - Pointer to variable to store read value
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ltc2378_iio_debug_reg_read(void *dev, uint32_t reg,
		uint32_t *val)
{
	struct ltc2378_iio_desc *iio_ltc2378 = dev;
	struct ltc2378_dev *ltc2378;
	uint32_t raw_data;
	int ret;

	if (!dev || !val)
		return -EINVAL;

	ltc2378 = iio_ltc2378->ltc2378_dev;
	if (!ltc2378)
		return -EINVAL;

	switch (reg) {
	case 0x00:  /* Raw ADC data */
		ret = ltc2378_read_raw(ltc2378, &raw_data);
		if (ret)
			return ret;
		*val = raw_data;
		break;

	case 0x01:  /* Input mode */
		*val = (uint32_t)ltc2378->input_mode;
		break;

	case 0x02:  /* Reference voltage lower 16 bits */
		*val = ltc2378->vref_uv & 0xFFFF;
		break;

	case 0x03:  /* Reference voltage upper 16 bits */
		*val = (ltc2378->vref_uv >> 16) & 0xFFFF;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Write debug register
 * @param dev - The iio device structure.
 * @param reg - Register address to write to
 *              0x01: Input mode (0=bipolar, 1=unipolar)
 * @param val - Value to write
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ltc2378_iio_debug_reg_write(void *dev, uint32_t reg,
		uint32_t val)
{
	struct ltc2378_iio_desc *iio_ltc2378 = dev;
	struct ltc2378_dev *ltc2378;

	if (!dev)
		return -EINVAL;

	ltc2378 = iio_ltc2378->ltc2378_dev;
	if (!ltc2378)
		return -EINVAL;

	switch (reg) {
	case 0x01:  /* Input mode */
		if (val != LTC2378_BIPOLAR && val != LTC2378_UNIPOLAR)
			return -EINVAL;
		ltc2378->input_mode = (enum ltc2378_input_mode)val;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static struct iio_device ltc2378_iio_dev = {
	.channels = ltc2378_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(ltc2378_iio_channels),
	.debug_reg_read = ltc2378_iio_debug_reg_read,
	.debug_reg_write = ltc2378_iio_debug_reg_write,
};

/**
 * @brief Initialize the IIO LTC2378 driver
 * @param iio_desc - IIO device descriptor.
 * @param init_param - Init parameter structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2378_iio_init(struct ltc2378_iio_desc **iio_desc,
		     struct ltc2378_iio_desc_init_param *init_param)
{
	struct ltc2378_iio_desc *descriptor;
	int ret;

	if (!iio_desc || !init_param || !init_param->ltc2378_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = ltc2378_init(&descriptor->ltc2378_dev, init_param->ltc2378_init_param);
	if (ret)
		goto error_free_desc;

	descriptor->iio_dev = &ltc2378_iio_dev;

	*iio_desc = descriptor;

	return 0;

error_free_desc:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free the resources allocated by ltc2378_iio_init()
 * @param iio_desc - IIO device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc2378_iio_remove(struct ltc2378_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -EINVAL;

	ltc2378_remove(iio_desc->ltc2378_dev);
	no_os_free(iio_desc);

	return 0;
}
