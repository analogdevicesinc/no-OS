/***************************************************************************//**
 *   @file   iio_ltc2983.c
 *   @brief  Implementation of IIO LTC2983 driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include <errno.h>
#include "iio_ltc2983.h"
#include "ltc2983.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio.h"

#define LTC2983_CHAN(_type, _index) ({ \
	struct iio_channel __chan = { \
		.ch_type = _type, \
		.indexed = true, \
		.channel = _index, \
		.attributes = ltc2983_iio_attrs, \
		.address = _index, \
	}; \
	__chan; \
})

#define LTC2983_RESISTANCE_CHAN(_index) ({ \
	struct iio_channel __chan = { \
		.ch_type = IIO_RESISTANCE, \
		.indexed = true, \
		.channel = _index, \
		.attributes = ltc2983_res_iio_attrs, \
		.address = _index, \
	}; \
	__chan; \
})

static int ltc2983_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv);
static int ltc2983_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);
static int ltc2983_iio_reg_read(void *dev, uint32_t reg,
				uint32_t *readval);
static int ltc2983_iio_reg_write(void *dev, uint32_t reg,
				 uint32_t writeval);

static struct iio_attribute ltc2983_iio_attrs[] = {
	{
		.name = "raw",
		.shared = IIO_SEPARATE,
		.show = ltc2983_iio_read_raw,
		.store = NULL,
	},
	{
		.name = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ltc2983_iio_read_scale,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltc2983_res_iio_attrs[] = {
	{
		.name = "raw",
		.shared = IIO_SEPARATE,
		.show = ltc2983_iio_read_raw,
		.store = NULL,
	},
	{
		.name = "scale",
		.shared = IIO_SEPARATE,
		.show = ltc2983_iio_read_scale,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device ltc2983_iio_dev = {
	.debug_reg_read = ltc2983_iio_reg_read,
	.debug_reg_write = ltc2983_iio_reg_write,
};

/******************************************************************************/

/**
 * @brief Check if a sensor has a custom lookup table programmed.
 * @param dev - LTC2983 descriptor.
 * @param i   - Zero-based sensor index into dev->sensors[].
 * @return true if the sensor has a custom table, false otherwise.
 */
static bool ltc2983_sensor_has_custom(struct ltc2983_desc *dev, int i)
{
	uint8_t type = dev->sensors[i]->type;

	if (type == LTC2983_RTD_CUSTOM)
		return !!to_rtd(dev->sensors[i])->custom;
	if (type == LTC2983_THERMISTOR_CUSTOM)
		return !!to_thermistor(dev->sensors[i])->custom;
	return false;
}

/**
 * @brief Initializes the LTC2983 IIO driver
 * @param iio_dev - The iio device structure.
 * @param init_param - Parameters for the initialization of iio_dev
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_iio_init(struct ltc2983_iio_desc **iio_dev,
		     struct ltc2983_iio_desc_init_param *init_param)
{
	int ret, i, chan = 0, iio_chan_count = 0;
	struct ltc2983_iio_desc *descriptor;
	struct ltc2983_desc *dev;
	struct iio_channel *ltc2983_channels;
	uint8_t type;

	if (!init_param || !init_param->ltc2983_desc_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = ltc2983_init(&descriptor->ltc2983_dev,
			   init_param->ltc2983_desc_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &ltc2983_iio_dev;
	dev = descriptor->ltc2983_dev;

	for (i = 0; i < dev->max_channels_nr; i++) {
		if (!dev->sensors[i] || dev->sensors[i]->type == LTC2983_RSENSE)
			continue;

		type = dev->sensors[i]->type;
		iio_chan_count++;

		/*
		 * On ADT7604, sensors with a custom table emit both an IIO_TEMP
		 * channel (interpolated result from temperature bank) and an
		 * IIO_RESISTANCE channel (raw resistance from resistance bank).
		 */
		if (dev->has_copper_trace &&
		    (type == LTC2983_RTD_CUSTOM || type == LTC2983_THERMISTOR_CUSTOM) &&
		    ltc2983_sensor_has_custom(dev, i))
			iio_chan_count++;
	}

	ltc2983_channels = no_os_calloc(iio_chan_count, sizeof(*ltc2983_channels));
	if (!ltc2983_channels) {
		ret = -ENOMEM;
		goto free_dev;
	}

	for (i = 0; i < dev->max_channels_nr; i++) {
		if (!dev->sensors[i] || dev->sensors[i]->type == LTC2983_RSENSE)
			continue;

		type = dev->sensors[i]->type;

		if (dev->has_copper_trace &&
		    (type == LTC2983_RTD_CUSTOM || type == LTC2983_THERMISTOR_CUSTOM)) {
			if (ltc2983_sensor_has_custom(dev, i)) {
				/* Custom table: temp (coverage) + resistance */
				ltc2983_channels[chan++] = LTC2983_CHAN(IIO_TEMP, i + 1);
				ltc2983_channels[chan++] = LTC2983_RESISTANCE_CHAN(i + 1);
			} else {
				/* No custom table: resistance only */
				ltc2983_channels[chan++] = LTC2983_RESISTANCE_CHAN(i + 1);
			}
		} else if (type == LTC2983_DIRECT_ADC) {
			ltc2983_channels[chan++] = LTC2983_CHAN(IIO_VOLTAGE, i + 1);
		} else {
			ltc2983_channels[chan++] = LTC2983_CHAN(IIO_TEMP, i + 1);
		}
	}

	descriptor->iio_dev->num_ch = iio_chan_count;
	descriptor->iio_dev->channels = ltc2983_channels;

	*iio_dev = descriptor;

	return 0;
free_dev:
	ltc2983_remove(descriptor->ltc2983_dev);
free_desc:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - The iio device structure.
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_iio_remove(struct ltc2983_iio_desc *desc)
{
	int ret;

	ret = ltc2983_remove(desc->ltc2983_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return        - 0 in case of success, errno errors otherwise
*/
static int ltc2983_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ltc2983_iio_desc *ltc2983_iio = dev;
	struct ltc2983_desc *ltc2983_dev = ltc2983_iio->ltc2983_dev;
	uint32_t uval;
	int32_t val;
	int ret;

	if (channel->type == IIO_RESISTANCE) {
		ret = ltc2983_chan_read_resistance(ltc2983_dev, channel->address,
						   &uval);
		if (ret)
			return ret;

		val = (int32_t)uval;
	} else {
		ret = ltc2983_chan_read_raw(ltc2983_dev, channel->address, &uval);
		if (ret)
			return ret;

		val = no_os_sign_extend32(uval, 23);
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the read request for scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return        - 0 in case of success, errno errors otherwise
*/
static int ltc2983_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ltc2983_iio_desc *ltc2983_iio = dev;
	struct ltc2983_desc *ltc2983_dev = ltc2983_iio->ltc2983_dev;
	int32_t val[2];
	int ret;

	if (channel->type == IIO_RESISTANCE)
		ret = ltc2983_chan_read_scale_resistance(ltc2983_dev,
				channel->address,
				(uint32_t *)&val[0],
				(uint32_t *)&val[1]);
	else
		ret = ltc2983_chan_read_scale(ltc2983_dev, channel->address,
					      (uint32_t *)&val[0],
					      (uint32_t *)&val[1]);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 1, val);
}

/**
 * @brief LTC2983 IIO reg read wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address (unused)
 * @param readval - Register value
 * @return 0 in case of success, errno errors otherwise
 */
static int ltc2983_iio_reg_read(void *dev, uint32_t reg,
				uint32_t *readval)
{
	struct ltc2983_iio_desc *iio_desc = dev;

	return ltc2983_reg_read(iio_desc->ltc2983_dev, (uint16_t)reg,
				(uint8_t*)readval);
}

/**
 * @brief LTC2983 IIO reg write wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address (unused)
 * @param writeval - Register value
 * @return 0 in case of success, errno errors otherwise
 */
static int ltc2983_iio_reg_write(void *dev, uint32_t reg,
				 uint32_t writeval)
{
	struct ltc2983_iio_desc *iio_desc = dev;

	return ltc2983_reg_write(iio_desc->ltc2983_dev, (uint16_t)reg,
				 (uint8_t)writeval);
}
