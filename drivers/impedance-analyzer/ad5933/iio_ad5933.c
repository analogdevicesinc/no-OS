/***************************************************************************//**
 *   @file   iio_ad5933.c
 *   @brief  Implementation of the AD5933 IIO driver.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
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

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include "iio_ad5933.h"
#include "ad5933.h"
#include "iio.h"
#include "no_os_alloc.h"
#include "no_os_util.h"
#include <string.h>

/* Temperature scale is 1/32 degC/LSB = 0.031250. */
#define AD5933_TEMP_SCALE_INT		0
#define AD5933_TEMP_SCALE_MICRO		31250

/** Per-channel selector carried through iio_channel.address. */
static enum ad5933_iio_chan {
	AD5933_CH_REAL,
	AD5933_CH_IMAG,
	AD5933_CH_TEMP,
};

/** Device-attribute selector carried through iio_attribute.priv. */
static enum ad5933_iio_attr_priv {
	AD5933_ATTR_PGA_GAIN,
	AD5933_ATTR_PGA_GAIN_AVAILABLE,
	AD5933_ATTR_OUTPUT_RANGE,
	AD5933_ATTR_OUTPUT_RANGE_AVAILABLE,
	AD5933_ATTR_START_FREQ,
	AD5933_ATTR_FREQ_INCREMENT,
	AD5933_ATTR_FREQ_POINTS,
	AD5933_ATTR_SETTLING_CYCLES,
	AD5933_ATTR_SWEEP_INITIALIZED,
	AD5933_ATTR_SWEEP_STARTED,
	AD5933_ATTR_CURRENT_OUTPUT_FREQ,
	AD5933_ATTR_REPEAT_MEASUREMENT,
	AD5933_ATTR_INCREMENTED_MEASUREMENT,
};

static char* ad5933_pga_gain_available[] = {
	[AD5933_GAIN_X5] = "0.2",
	[AD5933_GAIN_X1] = "1",
};

static unsigned int ad5933_output_range_available[] = {
	[AD5933_RANGE_2000mVpp] = 1982,
	[AD5933_RANGE_200mVpp] = 198,
	[AD5933_RANGE_400mVpp] = 383,
	[AD5933_RANGE_1000mVpp] = 986,
};

static char* ad5933_measure_mode_available[] = {
	[AD5933_MEASURE_MODE_SINGLE] = "single",
	[AD5933_MEASURE_MODE_SWEEP] = "sweep"
};

static int ad5933_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int ad5933_iio_read_scale(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int ad5933_iio_read_dev_attr(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);
static int ad5933_iio_write_dev_attr(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);
static int ad5933_iio_pre_enable(void *dev, uint32_t mask);
static int ad5933_iio_post_disable(void *dev);
static int ad5933_iio_submit(struct iio_device_data *dev_data);

static int ad5933_iio_reg_read(void *device, uint32_t reg_addr,
			       uint32_t *reg_data);
static int ad5933_iio_reg_write(void *device, uint32_t reg_addr,
				uint32_t reg_data);

static struct iio_attribute ad5933_temp_attrs[] = {
	{ .name = "raw", .show = ad5933_iio_read_raw },
	{ .name = "scale", .show = ad5933_iio_read_scale },
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad5933_raw_attrs[] = {
	{ .name = "raw", .show = ad5933_iio_read_raw },
	END_ATTRIBUTES_ARRAY
};

static struct scan_type ad5933_sweep_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
};

static struct iio_channel ad5933_channels[] = {

	{
		.ch_type = IIO_VOLTAGE,
		.name = "real",
		.channel = 0,
		.indexed = true,
		.address = AD5933_REG_REAL_DATA,
		.attributes = ad5933_raw_attrs,
		.ch_out = false,
		.scan_index = 0,
		.scan_type = &ad5933_sweep_scan_type,
	},
	{
		.ch_type = IIO_VOLTAGE,
		.name = "imag",
		.channel = 1,
		.indexed = true,
		.address = AD5933_REG_IMAG_DATA,
		.attributes = ad5933_raw_attrs,
		.ch_out = false,
		.scan_index = 1,
		.scan_type = &ad5933_sweep_scan_type,
	},
	{
		.ch_type = IIO_TEMP,
		.channel = 0,
		.address = AD5933_REG_TEMP_DATA,
		.attributes = ad5933_temp_attrs,
		.ch_out = false,
		.scan_index = -1,
	},
};

static struct iio_attribute ad5933_iio_debug_attrs[] = {
	{
		.name = "sweep_initialized",
		.show = ad5933_iio_read_dev_attr,
		.store = ad5933_iio_write_dev_attr,
		.priv = AD5933_ATTR_SWEEP_INITIALIZED,
	},
	{
		.name = "sweep_started",
		.show = ad5933_iio_read_dev_attr,
		.store = ad5933_iio_write_dev_attr,
		.priv = AD5933_ATTR_SWEEP_STARTED,
	},
	{
		.name = "current_output_frequency",
		.show = ad5933_iio_read_dev_attr,
		.priv = AD5933_ATTR_CURRENT_OUTPUT_FREQ,
	},
	{
		.name = "repeat_measurement",
		.store = ad5933_iio_write_dev_attr,
		.show = ad5933_iio_read_dev_attr,
		.priv = AD5933_ATTR_REPEAT_MEASUREMENT,
	},
	{
		.name = "incremented_measurement",
		.store = ad5933_iio_write_dev_attr,
		.show = ad5933_iio_read_dev_attr,
		.priv = AD5933_ATTR_INCREMENTED_MEASUREMENT,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad5933_iio_dev_attrs[] = {
	{
		.name = "in_voltage0_scale",
		.show = ad5933_iio_read_dev_attr,
		.store = ad5933_iio_write_dev_attr,
		.priv = AD5933_ATTR_PGA_GAIN,
	},
	{
		.name = "in_voltage0_scale_available",
		.show = ad5933_iio_read_dev_attr,
		.priv = AD5933_ATTR_PGA_GAIN_AVAILABLE,
	},
	{
		.name = "out_altvoltage0_raw",
		.show = ad5933_iio_read_dev_attr,
		.store = ad5933_iio_write_dev_attr,
		.priv = AD5933_ATTR_OUTPUT_RANGE,
	},
	{
		.name = "out_altvoltage0_scale_available",
		.show = ad5933_iio_read_dev_attr,
		.priv = AD5933_ATTR_OUTPUT_RANGE_AVAILABLE,
	},
	{
		.name = "out_altvoltage0_frequency_start",
		.show = ad5933_iio_read_dev_attr,
		.store = ad5933_iio_write_dev_attr,
		.priv = AD5933_ATTR_START_FREQ,
	},
	{
		.name = "out_altvoltage0_frequency_increment",
		.show = ad5933_iio_read_dev_attr,
		.store = ad5933_iio_write_dev_attr,
		.priv = AD5933_ATTR_FREQ_INCREMENT,
	},
	{
		.name = "out_altvoltage0_frequency_points",
		.show = ad5933_iio_read_dev_attr,
		.store = ad5933_iio_write_dev_attr,
		.priv = AD5933_ATTR_FREQ_POINTS,
	},
	{
		.name = "out_altvoltage0_settling_cycles",
		.show = ad5933_iio_read_dev_attr,
		.store = ad5933_iio_write_dev_attr,
		.priv = AD5933_ATTR_SETTLING_CYCLES,
	},

	END_ATTRIBUTES_ARRAY
};

static struct iio_device ad5933_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ad5933_channels),
	.channels = ad5933_channels,
	.attributes = ad5933_iio_dev_attrs,
	.debug_attributes = ad5933_iio_debug_attrs,
	.buffer_attributes = NULL,
	.pre_enable = ad5933_iio_pre_enable,
	.post_disable = ad5933_iio_post_disable,
	.submit = ad5933_iio_submit,
	.debug_reg_read = ad5933_iio_reg_read,
	.debug_reg_write = ad5933_iio_reg_write,
};

static int ad5933_iio_reg_read(void *device, uint32_t reg_addr,
			       uint32_t *reg_data)
{
	struct ad5933_dev *ad5933 = device;
	int ret;
	uint8_t reg_val;

	if (!device || !reg_data)
		return -EINVAL;

	if (!reg_addr)
		return 0;

	ret = ad5933_reg_read(ad5933, (uint8_t)reg_addr, &reg_val);

	if (ret)
		return ret;

	*reg_data = (uint32_t)reg_val;

	return 0;
}

static int ad5933_iio_reg_write(void *device, uint32_t reg_addr,
				uint32_t reg_data)
{
	struct ad5933_dev *ad5933 = device;
	uint8_t reg_val;

	if (!device)
		return -EINVAL;

	return ad5933_reg_write(ad5933, (uint8_t)reg_addr, (uint8_t)reg_data);
}

static int ad5933_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad5933_iio_dev *iio_ad5933;
	struct ad5933_dev *ad5933;
	int32_t raw;
	int32_t temp_raw;
	int16_t real;
	int16_t imag;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_ad5933 = dev;
	ad5933 = iio_ad5933->ad5933_dev;

	switch (channel->address) {
	case AD5933_REG_TEMP_DATA:
		ret = ad5933_get_raw_temperature(ad5933, &temp_raw);
		if (ret)
			return ret;
		raw = temp_raw;
		break;
	case AD5933_REG_REAL_DATA:
	case AD5933_REG_IMAG_DATA:
		ret = ad5933_get_current_data(ad5933, &real, &imag);
		if (ret)
			return ret;
		raw = (channel->address == AD5933_REG_REAL_DATA) ? real : imag;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &raw);
}

/**
 * @brief Read the "scale" attribute (temperature channel only).
 */
static int ad5933_iio_read_scale(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];

	if (!dev)
		return -EINVAL;

	switch (channel->address) {
	case AD5933_REG_TEMP_DATA:
		vals[0] = AD5933_TEMP_SCALE_INT;
		vals[1] = AD5933_TEMP_SCALE_MICRO;
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
					vals);
	default:
		return -EINVAL;
	}
}


/**
 * @brief Read a device-global attribute.
 */
static int ad5933_iio_read_dev_attr(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ad5933_iio_dev *iio_ad5933;
	struct ad5933_dev *ad5933;
	int32_t val;
	int length = 0;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_ad5933 = dev;
	ad5933 = iio_ad5933->ad5933_dev;

	switch (priv) {
	case AD5933_ATTR_PGA_GAIN:
		val = ad5933->pga_gain;
		return snprintf(buf, len, "%s", ad5933_pga_gain_available[val]);
	case AD5933_ATTR_PGA_GAIN_AVAILABLE:
		return snprintf(buf, len, "%s %s", ad5933_pga_gain_available[1],
				ad5933_pga_gain_available[0]);
	case AD5933_ATTR_OUTPUT_RANGE:
		val = ad5933->output_range;
		return snprintf(buf, len, "%u", ad5933_output_range_available[val]);
	case AD5933_ATTR_OUTPUT_RANGE_AVAILABLE:
		return snprintf(buf, len, "%u %u %u %u", ad5933_output_range_available[0],
				ad5933_output_range_available[3], ad5933_output_range_available[2],
				ad5933_output_range_available[1]);
	case AD5933_ATTR_START_FREQ:
		val = ad5933->start_freq;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_FREQ_INCREMENT:
		val = ad5933->freq_increment;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_FREQ_POINTS:
		val = ad5933->num_increments;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_SETTLING_CYCLES:
		val = ad5933->settling_cycles;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_SWEEP_INITIALIZED:
		val = iio_ad5933->state == AD5933_FUNCTION_INIT_START_FREQ;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_SWEEP_STARTED:
		val = iio_ad5933->state == AD5933_FUNCTION_START_SWEEP;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_CURRENT_OUTPUT_FREQ:
		val = ad5933->current_output_freq;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_REPEAT_MEASUREMENT:
	case AD5933_ATTR_INCREMENTED_MEASUREMENT:
		val = ad5933->current_output_freq;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Write a device-global attribute.
 */
static int ad5933_iio_write_dev_attr(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad5933_iio_dev *iio_ad5933;
	struct ad5933_dev *ad5933;
	int32_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_ad5933 = dev;
	ad5933 = iio_ad5933->ad5933_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	if (ret < 0)
		return ret;

	switch (priv) {
	case AD5933_ATTR_PGA_GAIN:
		for (int i = 0; i < NO_OS_ARRAY_SIZE(ad5933_pga_gain_available); i++) {
			if (!strcmp(buf, ad5933_pga_gain_available[i])) {
				ret = ad5933_set_gain(ad5933, i);
				if (ret)
					return ret;
				break;
			}
		}
		break;
	case AD5933_ATTR_OUTPUT_RANGE:
		for (int i = 0; i < NO_OS_ARRAY_SIZE(ad5933_output_range_available); i++) {
			if (val == ad5933_output_range_available[i]) {
				ret = ad5933_set_range(ad5933, i);
				if (ret)
					return ret;
				return len;
			}
		}
		return -EINVAL;
	case AD5933_ATTR_START_FREQ:
		ret = ad5933_config_sweep(ad5933, val,
					  ad5933->freq_increment,
					  ad5933->num_increments);
		if (ret)
			return ret;
		break;
	case AD5933_ATTR_FREQ_INCREMENT:
		ret = ad5933_config_sweep(ad5933, ad5933->start_freq,
					  val,
					  ad5933->num_increments);
		if (ret)
			return ret;
		break;
	case AD5933_ATTR_FREQ_POINTS:
		ret = ad5933_config_sweep(ad5933, ad5933->start_freq,
					  ad5933->freq_increment,
					  val);
		if (ret)
			return ret;
		break;
	case AD5933_ATTR_SETTLING_CYCLES:
		ret = ad5933_set_settling_time(ad5933, val);
		if (ret)
			return ret;
		break;
	case AD5933_ATTR_SWEEP_INITIALIZED:
		if (val == 1) {
			ret = ad5933_initialize_sweep(ad5933);
			if (ret)
				return ret;
			iio_ad5933->state = AD5933_FUNCTION_INIT_START_FREQ;
		}
		break;
	case AD5933_ATTR_SWEEP_STARTED:
		if (val) {
			ret = ad5933_start_sweep(ad5933);
			if (ret)
				return ret;
			iio_ad5933->state = AD5933_FUNCTION_START_SWEEP;
		}
		break;
	case AD5933_ATTR_REPEAT_MEASUREMENT:
		if (val) {
			if (iio_ad5933->state != AD5933_FUNCTION_START_SWEEP)
				return -EINVAL;
			ret = ad5933_repeat_freq(ad5933);
			if (ret)
				return ret;
			ret = ad5933_wait_status(ad5933, AD5933_STAT_DATA_VALID, NULL);
			if (ret)
				return ret;
		}
		break;
	case AD5933_ATTR_INCREMENTED_MEASUREMENT:
		if (val) {
			if (iio_ad5933->state != AD5933_FUNCTION_START_SWEEP)
				return -EINVAL;
			ret = ad5933_increment_freq(ad5933);
			if (ret)
				return ret;
			ret = ad5933_wait_status(ad5933, AD5933_STAT_DATA_VALID, NULL);
			if (ret)
				return ret;
		}
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return len;
}

/**
 * @brief Buffer pre-enable: record the active scan-channel mask.
 * @param dev  - The AD5933 IIO device descriptor.
 * @param mask - Active-channel mask (bit position == channel array index).
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5933_iio_pre_enable(void *dev, uint32_t mask)
{
	struct ad5933_iio_dev *iio_ad5933 = dev;
	int ret;

	if (!iio_ad5933)
		return -EINVAL;


	iio_ad5933->active_channels = mask;
	iio_ad5933->no_of_active_channels = no_os_hweight32(mask);

	ret = ad5933_initialize_sweep(iio_ad5933->ad5933_dev);

	if (ret)
		return ret;

	memset(iio_ad5933->channel_data, 0, sizeof(iio_ad5933->channel_data));

	no_os_mdelay(100);

	ret = ad5933_start_sweep(iio_ad5933->ad5933_dev);
	if (ret)
		return 0;

	iio_ad5933->state = AD5933_FUNCTION_START_SWEEP;

	return 0;
}

static int ad5933_iio_post_disable(void *dev)
{
	struct ad5933_iio_dev *iio_ad5933 = dev;
	int ret;

	ret = ad5933_power_down(iio_ad5933->ad5933_dev);
	if (ret)
		return ret;

	iio_ad5933->state = AD5933_FUNCTION_POWER_DOWN;

	return 0;
}

/**
 * @brief Buffer submit: copy the collected sweep points into the ring.
 *
 * @param dev_data - The IIO device data structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad5933_iio_submit(struct iio_device_data *dev_data)
{
	struct ad5933_iio_dev *iio_ad5933;
	struct ad5933_dev *ad5933;
	struct iio_buffer *buffer;
	uint32_t mask;
	uint32_t points;
	uint32_t max_samples;
	uint32_t i;
	int16_t scan[3];
	int16_t real, imag;
	uint8_t status;
	uint32_t sweep_done;
	int ret;

	if (!dev_data)
		return -EINVAL;

	iio_ad5933 = dev_data->dev;
	ad5933 = iio_ad5933->ad5933_dev;
	buffer = dev_data->buffer;
	mask = buffer->active_mask;
	points = buffer->samples;
	max_samples = ad5933->num_increments + 1;

	ret = ad5933_sweep_done(ad5933, &sweep_done);
	if (ret)
		return ret;

	if (!sweep_done) {
		for (i = 0; i <= ad5933->num_increments; i++) {
			ret = ad5933_wait_status(ad5933, AD5933_STAT_DATA_VALID, &status);
			if (ret)
				return ret;

			ret = ad5933_get_current_data(iio_ad5933->ad5933_dev,
						      &iio_ad5933->channel_data[iio_ad5933->ad5933_dev->sweep_point * 2],
						      &iio_ad5933->channel_data[iio_ad5933->ad5933_dev->sweep_point * 2 + 1]);
			if (ret)
				return ret;

			ret = ad5933_increment_freq(ad5933);
			if (ret)
				return ret;
		}
	}

	for (i = 0; i < points; i++) {
		uint8_t k = 0;

		if (mask & NO_OS_BIT(AD5933_CH_REAL))
			scan[k++] = i < max_samples ? iio_ad5933->channel_data[i * 2] : INT16_MAX;
		if (mask & NO_OS_BIT(AD5933_CH_IMAG))
			scan[k++] = i < max_samples ? iio_ad5933->channel_data[i * 2 + 1] : INT16_MAX;
		ret = iio_buffer_push_scan(buffer, scan);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Initialize the AD5933 IIO driver.
 * @param iio_dev    - The AD5933 IIO device descriptor.
 * @param init_param - The AD5933 IIO device initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5933_iio_init(struct ad5933_iio_dev **iio_dev,
		    struct ad5933_iio_dev_init_param *init_param)
{
	struct ad5933_iio_dev *desc;
	int ret;

	if (!iio_dev || !init_param || !init_param->ad5933_dev_ip)
		return -EINVAL;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &ad5933_iio_dev;

	ret = ad5933_init(&desc->ad5933_dev, init_param->ad5933_dev_ip);

	if (ret) {
		goto error_desc;
	}

	ret = ad5933_config_sweep(desc->ad5933_dev, init_param->start_freq,
				  init_param->freq_increment, init_param->freq_points);

	if (ret)
		goto error_dev;

	desc->state = AD5933_FUNCTION_POWER_DOWN;

	*iio_dev = desc;

	return 0;

error_dev:
	ad5933_remove(desc->ad5933_dev);
error_desc:
	no_os_free(desc);
	return ret;
}

/**
 * @brief Free the resources allocated by ad5933_iio_init().
 * @param desc - The AD5933 IIO device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5933_iio_remove(struct ad5933_iio_dev *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = ad5933_remove(desc->ad5933_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
