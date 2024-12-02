/***************************************************************************//**
* @file   iio_ad7606.c
* @brief  Implementation of AD7606 IIO interface
********************************************************************************
* Copyright (c) 2024 Analog Devices, Inc.
* Copyright (c) 2024 BayLibre, SAS.
* All rights reserved.
*
* This software is proprietary to Analog Devices, Inc. and its licensors.
* By using this software you agree to the terms of the associated
* Analog Devices Software License Agreement.
*******************************************************************************/

#include <stdio.h>
#include <math.h>

#include "ad7606.h"
#include "iio_ad7606.h"
#include "iio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

#define AD7606_CH(_idx, bits) {\
	.name = "voltage" #_idx, \
	.ch_type = IIO_VOLTAGE,\
	.ch_out = IIO_DIRECTION_INPUT,\
	.indexed = true,\
	.channel = _idx,\
	.scan_index = _idx,\
	.scan_type = &ad7606_iio_scan_type_ ## bits ## bit,\
	.attributes = ad7606_iio_ch_attributes,\
}

#define AD7606_CHN_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = iio_ad7606_attr_get,\
	.store = iio_ad7606_attr_set,\
}

#define MAX_CHANNELS 8

enum ad7606_iio_attributes_id {
	OSR_ATTR_ID,
	OSR_AVAILABLE_ID,
};

/**
 * @brief       Read the debug register value
 * @param       device - Pointer to IIO device instance
 * @param       reg - Register address to read from
 * @param       val - Pointer to variable to read data into
 * @return      0 in case of success, negative value otherwise
 */
static int32_t iio_ad7606_debug_reg_read(void *device, uint32_t reg,
		uint32_t *val)
{
	struct ad7606_iio_dev *iio_dev = device;
	struct ad7606_dev *dev = iio_dev->ad7606_dev;
	uint8_t read_val;
	int ret;

	ret = ad7606_reg_read(dev, reg, &read_val);
	if (ret)
		return ret;

	*val = read_val;

	return 0;
}

/**
 * @brief       Write the debug register value
 * @param       device - Pointer to IIO device instance
 * @param       reg - Register address to read from
 * @param       val - Pointer to variable to write data into
 * @return      0 in case of success, negative value otherwise
 */
static int32_t iio_ad7606_debug_reg_write(void *device, uint32_t reg,
		uint32_t val)
{
	struct ad7606_iio_dev *iio_dev = device;
	struct ad7606_dev *dev = iio_dev->ad7606_dev;

	return ad7606_reg_write(dev, reg, val);
}

static int get_raw(void *device,
		   char *buf,
		   uint32_t len,
		   const struct iio_ch_info *channel,
		   intptr_t id)
{
	struct ad7606_iio_dev *iio_dev = device;
	struct ad7606_dev *dev = iio_dev->ad7606_dev;
	uint32_t num_chan = ad7606_get_channels_number(dev);
	uint32_t *res;
	int ret;

	res = no_os_calloc(num_chan, sizeof(*res));
	if (!res)
		return -ENOMEM;

	ret = ad7606_capture_pre_enable(dev);
	if (ret)
		goto error;

	ret = ad7606_read_samples(dev, res, num_chan);
	ad7606_capture_post_disable(dev);

	if (ret)
		goto error;

	ret = sprintf(buf, "%ld", no_os_sign_extend32(res[channel->ch_num],
			iio_dev->sign_bit));
error:
	no_os_free(res);

	return ret;
}

static int get_scale(void *device,
		     char *buf,
		     uint32_t len,
		     const struct iio_ch_info *channel,
		     intptr_t id)
{
	struct ad7606_iio_dev *iio_dev = device;
	struct ad7606_dev *dev = iio_dev->ad7606_dev;
	double scale;
	int32_t ret;

	ret = ad7606_get_ch_scale(dev, channel->ch_num, &scale);
	if (ret)
		return ret;

	return sprintf(buf, "%f", scale);
}

static int set_over_sampling_ratio(struct ad7606_dev *dev,
				   char *buf,
				   uint32_t len)
{
	struct ad7606_oversampling osr = {};
	static const int ratio_table[] = {
		[AD7606_OSR_1] = 1,
		[AD7606_OSR_2] = 2,
		[AD7606_OSR_4] = 4,
		[AD7606_OSR_8] = 8,
		[AD7606_OSR_16] = 16,
		[AD7606_OSR_32] = 32,
		[AD7606_OSR_64] = 64,
		[AD7606_OSR_128] = 128,
		[AD7606_OSR_256] = 256,
	};
	uint32_t i;
	int val;

	if (sscanf(buf, "%u", &val) != 1)
		return -EINVAL;

	switch (val) {
	case 256:
		if (!ad7606_sw_mode_enabled(dev))
			return -EINVAL;
		osr.os_ratio = AD7606_OSR_256;
		return ad7606_set_oversampling(dev, osr);
	default:
		for (i = 0; i < NO_OS_ARRAY_SIZE(ratio_table); i++) {
			if (ratio_table[i] == val) {
				osr.os_ratio = i;
				return ad7606_set_oversampling(dev, osr);
			}
		}
		return -EINVAL;
	}
}

static int get_scale_avail(void *device,
			   char *buf,
			   uint32_t len,
			   const struct iio_ch_info *channel,
			   intptr_t id)
{
	struct ad7606_iio_dev *iio_dev = device;
	struct ad7606_dev *dev = iio_dev->ad7606_dev;
	const struct ad7606_range *rt;
	uint32_t i, off, num_ranges;
	int32_t ret;
	double div;

	rt = ad7606_get_ch_ranges(dev, channel->ch_num, &num_ranges);
	if (!rt)
		return -EINVAL;

	ret = ad7606_get_resolution_bits(dev);
	if (ret < 0)
		return ret;

	div = 1 << ret;

	for (off = 0, i = 0; i < num_ranges; i++) {
		double scale = (double)(rt[i].max - rt[i].min) / div;
		ret = sprintf(&buf[off], "%f ", scale);
		if (ret < 0)
			return ret;
		off += ret;
	}

	return off;
}

static int set_scale(void *device,
		     char *buf,
		     uint32_t len,
		     const struct iio_ch_info *channel,
		     intptr_t id)
{
	struct ad7606_iio_dev *iio_dev = device;
	struct ad7606_dev *dev = iio_dev->ad7606_dev;
	const struct ad7606_range *rt;
	uint32_t i, num_ranges;
	float div, val;
	int32_t ret;

	ret = sscanf(buf, "%f", &val);
	if (ret != 1)
		return -EINVAL;

	rt = ad7606_get_ch_ranges(dev, channel->ch_num, &num_ranges);
	if (!rt)
		return -EINVAL;

	ret = ad7606_get_resolution_bits(dev);
	if (ret < 0)
		return ret;

	div = 1 << ret;

	for (i = 0; i < num_ranges; i++) {
		double scale = (double)(rt[i].max - rt[i].min) / div;
		if (fabs(scale - val) <= 0.000001f)
			return ad7606_set_ch_range(dev, channel->ch_num, *rt);
	}

	return -EINVAL;
}

static int iio_ad7606_attr_get(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct ad7606_iio_dev *iio_dev = device;
	struct ad7606_dev *dev = iio_dev->ad7606_dev;
	struct ad7606_oversampling osr;
	int32_t ret;

	switch (priv) {
	case OSR_ATTR_ID:
		ret = ad7606_get_oversampling(dev, &osr);
		if (ret)
			return ret;
		return sprintf(buf, "%u", 1 << osr.os_ratio);
	case OSR_AVAILABLE_ID:
		if (!ad7606_sw_mode_enabled(dev))
			return sprintf(buf, "1 2 4 8 16 32 64 128");
		return sprintf(buf, "1 2 4 8 16 32 64 128 256");
	default:
		return -EINVAL;
	}
}

static int iio_ad7606_attr_set(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct ad7606_iio_dev *iio_dev = device;
	struct ad7606_dev *dev = iio_dev->ad7606_dev;

	switch (priv) {
	case OSR_ATTR_ID:
		return set_over_sampling_ratio(dev, buf, len);
	default:
		return -EINVAL;
	}
}

/**
 * @brief	Enable PWM before doing buffer transfers
 * @param	dev - Pointer to IIO device data structure
 * @param	mask - Scan-mask for pre-enable
 * @return	0 in case of success, negative error code otherwise
 */
static int32_t iio_ad7606_pre_enable(void *dev, uint32_t mask)
{
	struct ad7606_iio_dev *iio_dev = dev;

	return ad7606_capture_pre_enable(iio_dev->ad7606_dev);
}

/**
 * @brief	Disable PWM after doing buffer transfers
 * @param	dev - Pointer to IIO device data structure
 * @return	0 in case of success, negative error code otherwise
 */
static int32_t iio_ad7606_post_disable(void *dev)
{
	struct ad7606_iio_dev *iio_dev = dev;

	ad7606_capture_post_disable(iio_dev->ad7606_dev);

	return 0;
}

/**
 * @brief	Read buffer data corresponding to AD7606 IIO device
 * @param	iio_dev_data - Pointer to IIO device data structure
 * @return	0 in case of success, negative error code otherwise
 */
static int32_t iio_ad7606_submit_buffer(struct iio_device_data *iio_dev_data)
{
	struct ad7606_iio_dev *iio_dev = iio_dev_data->dev;
	struct ad7606_dev *dev = iio_dev->ad7606_dev;
	struct iio_buffer *buffer = iio_dev_data->buffer;
	uint32_t num_chan = ad7606_get_channels_number(dev);
	uint32_t *push_data, *data;
	uint32_t total_samples;
	void *buff;
	uint32_t i, j, k;
	int32_t ret;

	ret = iio_buffer_get_block(buffer, &buff);
	if (ret)
		return ret;

	total_samples = num_chan * buffer->samples;

	data = buff;
	ret = ad7606_read_samples(dev, buff, total_samples);
	if (ret)
		return ret;

	push_data = buff;
	for (k = 0, i = 0; i < total_samples; i += num_chan) {
		uint32_t active_mask = buffer->active_mask;

		for (j = 0; j < num_chan; j++) {
			if ((NO_OS_BIT(j) & active_mask) == 0)
				continue;
			push_data[k++] = data[i + j];
		}
	}

	return iio_buffer_block_done(buffer);
}

static struct scan_type ad7606_iio_scan_type_16bit = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false
};

static struct scan_type ad7606_iio_scan_type_18bit = {
	.sign = 's',
	.realbits = 18,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false
};

static struct iio_attribute ad7606_iio_ch_attributes[] = {
	{ .name = "raw", .show = get_raw },
	{ .name = "scale", .show = get_scale, .store = set_scale },
	{ .name = "scale_avail", .show = get_scale_avail },
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad7606_iio_global_attributes[] = {
	AD7606_CHN_ATTR("oversampling_ratio", OSR_ATTR_ID),
	AD7606_CHN_ATTR("oversampling_ratio_available", OSR_AVAILABLE_ID),
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ad7606_channels_16bit[] = {
	AD7606_CH(0, 16),
	AD7606_CH(1, 16),
	AD7606_CH(2, 16),
	AD7606_CH(3, 16),
	AD7606_CH(4, 16),
	AD7606_CH(5, 16),
	AD7606_CH(6, 16),
	AD7606_CH(7, 16),
};

static struct iio_channel ad7606_channels_18bit[] = {
	AD7606_CH(0, 18),
	AD7606_CH(1, 18),
	AD7606_CH(2, 18),
	AD7606_CH(3, 18),
	AD7606_CH(4, 18),
	AD7606_CH(5, 18),
	AD7606_CH(6, 18),
	AD7606_CH(7, 18),
};

static struct iio_device ad7606_iio_device_16bit_template = {
	.num_ch = NO_OS_ARRAY_SIZE(ad7606_channels_16bit),
	.channels = ad7606_channels_16bit,
	.attributes = ad7606_iio_global_attributes,
	.debug_reg_read = iio_ad7606_debug_reg_read,
	.debug_reg_write = iio_ad7606_debug_reg_write,
	.submit = iio_ad7606_submit_buffer,
	.pre_enable = iio_ad7606_pre_enable,
	.post_disable = iio_ad7606_post_disable,
};

static struct iio_device ad7606_iio_device_18bit_template = {
	.num_ch = NO_OS_ARRAY_SIZE(ad7606_channels_18bit),
	.channels = ad7606_channels_18bit,
	.attributes = ad7606_iio_global_attributes,
	.debug_reg_read = iio_ad7606_debug_reg_read,
	.debug_reg_write = iio_ad7606_debug_reg_write,
	.pre_enable = iio_ad7606_pre_enable,
	.post_disable = iio_ad7606_post_disable,
	.submit = iio_ad7606_submit_buffer,
};

/**
 * @brief Initialize AD7606 for IIO interfacing
 * @param dev - The device structure.
 * @param init_param - The initialization parameters for the AD7606x device.
 * @return 0 on success, an error code otherwise
 */
int ad7606_iio_init(struct ad7606_iio_dev **dev,
		    struct ad7606_init_param *init_param)
{
	struct ad7606_iio_dev *desc;
	int ret;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	ret = ad7606_init(&desc->ad7606_dev, init_param);
	if (ret)
		goto error_setup;

	desc->sign_bit = ad7606_get_resolution_bits(desc->ad7606_dev) - 1;

	if (desc->sign_bit > 15)
		desc->iio_dev = &ad7606_iio_device_18bit_template;
	else
		desc->iio_dev = &ad7606_iio_device_16bit_template;

	*dev = desc;

	return 0;

error_setup:
	no_os_free(desc);
	return ret;
}

int ad7606_iio_remove(struct ad7606_iio_dev *dev)
{
	if (!dev)
		return -ENODEV;

	ad7606_remove(dev->ad7606_dev);

	no_os_free(dev);

	return 0;
}
