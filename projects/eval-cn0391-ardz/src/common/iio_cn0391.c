/***************************************************************************//**
 *   @file   iio_cn0391.c
 *   @brief  Implementation of IIO CN0391 driver.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <stdio.h>
#include "iio_cn0391.h"
#include "cn0391.h"
#include "no_os_error.h"

#ifdef CN0391_IIO_SUPPORT

/* Private attribute IDs used as priv in iio_attribute */
enum cn0391_iio_attr_id {
	CN0391_ATTR_HOT_JUNCTION_TEMP,
	CN0391_ATTR_COLD_JUNCTION_TEMP,
	CN0391_ATTR_TC_VOLTAGE,
	CN0391_ATTR_RTD_RESISTANCE,
};

/**
 * @brief Read a processed channel value from one full temperature measurement.
 * @param device  - CN0391 device descriptor.
 * @param buf     - Output buffer.
 * @param len     - Output buffer length.
 * @param channel - IIO channel info; ch_num selects the IIO channel (0=CH0..3=CH3).
 * @param priv    - One of cn0391_iio_attr_id.
 * @return Number of bytes written, or negative error code.
 */
static int cn0391_iio_read_processed(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct cn0391_dev *dev = (struct cn0391_dev *)device;
	uint8_t ch_idx = (uint8_t)channel->ch_num;
	double value;
	int ret;

	/*
	 * Each IIO channel exposes 4 attributes (hot_junction_temp,
	 * cold_junction_temp, tc_voltage, rtd_resistance), all derived from a
	 * single cn0391_read_temperature() call that reads 3 ADC channels.
	 * Without caching, every attribute read would trigger a full ADC
	 * measurement cycle, spending up to ~2.5 s per channel waiting in the
	 * 12-channel continuous-mode rotation — unacceptable latency when
	 * multiple attributes are read in sequence.
	 *
	 * The cache is keyed on ch_idx: if the same IIO channel is queried
	 * again (consecutive attribute reads), the cached result is returned
	 * directly. A new measurement is only triggered when a different
	 * channel is requested, reducing ADC activity from 16 to 4 measurement
	 * cycles per full client read of all four IIO channels.
	 */
	if (dev->cache_ch != (int8_t)ch_idx) {
		ret = cn0391_read_temperature(dev, ch_idx,
					      &dev->cache.hot_junction_temp,
					      &dev->cache.cold_junction_temp,
					      &dev->cache.tc_voltage,
					      &dev->cache.rtd_resistance);
		if (ret)
			return ret;
		dev->cache_ch = (int8_t)ch_idx;
	}

	/* Extract the requested field from the cached measurement */
	switch (priv) {
	case CN0391_ATTR_HOT_JUNCTION_TEMP:
		value = dev->cache.hot_junction_temp;
		break;
	case CN0391_ATTR_COLD_JUNCTION_TEMP:
		value = dev->cache.cold_junction_temp;
		break;
	case CN0391_ATTR_TC_VOLTAGE:
		value = dev->cache.tc_voltage;
		break;
	case CN0391_ATTR_RTD_RESISTANCE:
		value = dev->cache.rtd_resistance;
		break;
	default:
		return -EINVAL;
	}

	return snprintf(buf, len, "%.4f", value);
}

/* All 4 measurements are attributes of a single thermocouple channel */
static struct iio_attribute cn0391_ch_attrs[] = {
	{
		.name = "hot_junction_temp",
		.priv = CN0391_ATTR_HOT_JUNCTION_TEMP,
		.show = cn0391_iio_read_processed,
		.store = NULL,
	},
	{
		.name = "cold_junction_temp",
		.priv = CN0391_ATTR_COLD_JUNCTION_TEMP,
		.show = cn0391_iio_read_processed,
		.store = NULL,
	},
	{
		.name = "tc_voltage",
		.priv = CN0391_ATTR_TC_VOLTAGE,
		.show = cn0391_iio_read_processed,
		.store = NULL,
	},
	{
		.name = "rtd_resistance",
		.priv = CN0391_ATTR_RTD_RESISTANCE,
		.show = cn0391_iio_read_processed,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel cn0391_channels[] = {
	{ .name = "CH0", .ch_type = IIO_TEMP, .channel = 0,
	  .attributes = cn0391_ch_attrs, .ch_out = false, .indexed = true },
	{ .name = "CH1", .ch_type = IIO_TEMP, .channel = 1,
	  .attributes = cn0391_ch_attrs, .ch_out = false, .indexed = true },
	{ .name = "CH2", .ch_type = IIO_TEMP, .channel = 2,
	  .attributes = cn0391_ch_attrs, .ch_out = false, .indexed = true },
	{ .name = "CH3", .ch_type = IIO_TEMP, .channel = 3,
	  .attributes = cn0391_ch_attrs, .ch_out = false, .indexed = true },
};

struct iio_device iio_cn0391_device = {
	.num_ch = NO_OS_ARRAY_SIZE(cn0391_channels),
	.channels = cn0391_channels,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = NULL,
	.post_disable = NULL,
	.read_dev = NULL,
};

#endif /* CN0391_IIO_SUPPORT */

/**
 * @brief Read hot junction temperature as a float string for the "raw" attribute.
 *        Matches the Zephyr convention for demo project: raw = temperature [°C],
 *        scale = "1000" (fixed).
 * @param device  - CN0391 device descriptor.
 * @param buf     - Output buffer.
 * @param len     - Output buffer length.
 * @param channel - IIO channel info; ch_num selects the IIO channel (0..3).
 * @param priv    - Unused.
 * @return Number of bytes written, or negative error code.
 */
static int cn0391_iio_read_raw_tc(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct cn0391_dev *dev = (struct cn0391_dev *)device;
	uint8_t ch_idx = (uint8_t)channel->ch_num;
	int ret;

	if (dev->cache_ch != (int8_t)ch_idx) {
		ret = cn0391_read_temperature(dev, ch_idx,
					      &dev->cache.hot_junction_temp,
					      &dev->cache.cold_junction_temp,
					      &dev->cache.tc_voltage,
					      &dev->cache.rtd_resistance);
		if (ret)
			return ret;
		dev->cache_ch = (int8_t)ch_idx;
	}

	return snprintf(buf, len, "%.5f", dev->cache.hot_junction_temp);
}

/**
 * @brief Return the fixed scale factor for the custom ad7124-8 channels.
 *        Matches the Zephyr convention for demo project (scale = "1000").
 * @param device  - Unused.
 * @param buf     - Output buffer.
 * @param len     - Output buffer length.
 * @param channel - Unused.
 * @param priv    - Unused.
 * @return Number of bytes written.
 */
static int cn0391_iio_read_scale_tc(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	return snprintf(buf, len, "1000");
}

static struct iio_attribute cn0391_ad7124_ch_attrs[] = {
	{
		.name = "raw",
		.show = cn0391_iio_read_raw_tc,
		.store = NULL,
	},
	{
		.name = "scale",
		.show = cn0391_iio_read_scale_tc,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel cn0391_ad7124_channels[] = {
	{ .name = "CH0", .ch_type = IIO_VOLTAGE, .channel = 0,
	  .attributes = cn0391_ad7124_ch_attrs, .ch_out = false, .indexed = true },
	{ .name = "CH1", .ch_type = IIO_VOLTAGE, .channel = 1,
	  .attributes = cn0391_ad7124_ch_attrs, .ch_out = false, .indexed = true },
	{ .name = "CH2", .ch_type = IIO_VOLTAGE, .channel = 2,
	  .attributes = cn0391_ad7124_ch_attrs, .ch_out = false, .indexed = true },
	{ .name = "CH3", .ch_type = IIO_VOLTAGE, .channel = 3,
	  .attributes = cn0391_ad7124_ch_attrs, .ch_out = false, .indexed = true },
};

/* Custom AD7124-8 IIO device for demo purposes.
 * Exposes 4 thermocouple channels (voltage0..voltage3) with raw (hot junction
 * temperature as float [°C]) and scale ("1000") attributes, matching the
 * Zephyr CN0391 convention for demo project. Device is backed by cn0391_dev. */
struct iio_device iio_ad7124_cn0391_device = {
	.num_ch = NO_OS_ARRAY_SIZE(cn0391_ad7124_channels),
	.channels = cn0391_ad7124_channels,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = NULL,
	.post_disable = NULL,
	.read_dev = NULL,
};
