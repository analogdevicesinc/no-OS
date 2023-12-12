/***************************************************************************//**
* @file   iio_ad7616.c
* @brief  Implementation of AD7616 IIO interface
********************************************************************************
* Copyright (c) 2024 Analog Devices, Inc.
* Copyright (c) 2024 BayLibre, SAS.
* All rights reserved.
*
* This software is proprietary to Analog Devices, Inc. and its licensors.
* By using this software you agree to the terms of the associated
* Analog Devices Software License Agreement.
*******************************************************************************/
#ifdef IIO_SUPPORT

#include <stdio.h>
#include <math.h>

#include "ad7616.h"
#include "iio_ad7616.h"
#include "iio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

#define AD7616_CH(_idx) {\
	.name = "voltage" #_idx, \
	.ch_type = IIO_VOLTAGE,\
	.ch_out = IIO_DIRECTION_INPUT,\
	.indexed = true,\
	.channel = _idx,\
	.scan_index = _idx,\
	.scan_type = &ad7616_iio_scan_type,\
	.attributes = ad7616_iio_ch_attributes,\
}

#define AD7616_CHN_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = iio_ad7616_attr_get,\
	.store = iio_ad7616_attr_set,\
}

#define CHN_STORAGE_BITS 16
/* 16 IIO channel */
#define CHANNEL_NUMBER 16
#define RESULTS_NUMBER CHANNEL_NUMBER
/* Each AD7616 channel has 8 sources */
#define CHANNEL_SOURCES_NUMBER 8

struct ad7616_iio {
	struct ad7616_dev *drv_dev;
	struct iio_device *iio_dev;
};

enum ad7616_iio_attributes_id {
	CRC_ATTR_ID,
	OSR_ATTR_ID,
	OSR_AVAILABLE_ID,
};

/**
 * @brief	Check if the channel number correspond to the channel A of AD7616
 * @param	channel_id - Id of the channel
 * @returns	1 if true, 0 otherwise
 */
static int32_t is_channel_a(uint32_t channel_id)
{
	return channel_id < CHANNEL_SOURCES_NUMBER;
}

/**
 * @brief       Read the debug register value
 * @param       device - Pointer to IIO device instance
 * @param       reg - Register address to read from
 * @param       val - Pointer to variable to read data into
 * @return      0 in case of success, negative value otherwise
 */
static int32_t iio_ad7616_debug_reg_read(void *device, uint32_t reg,
		uint32_t *val)
{
	struct ad7616_iio_dev *iio_dev = device;
	struct ad7616_dev *dev = iio_dev->ad7616_dev;
	uint16_t read_val;
	int ret;

	ret = ad7616_read(dev, (uint8_t)reg, &read_val);
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
static int32_t iio_ad7616_debug_reg_write(void *device, uint32_t reg,
		uint32_t val)
{
	struct ad7616_iio_dev *iio_dev = device;
	struct ad7616_dev *dev = iio_dev->ad7616_dev;

	return ad7616_write(dev, (uint8_t)reg, (uint16_t)val);
}

static int get_raw(void *device,
		   char *buf,
		   uint32_t len,
		   const struct iio_ch_info *channel,
		   intptr_t id)
{
	int ret;
	int is_channel_a_res = is_channel_a(channel->ch_num);
	uint8_t index = is_channel_a_res ? channel->ch_num :
			channel->ch_num - CHANNEL_SOURCES_NUMBER;
	struct ad7616_conversion_result res[RESULTS_NUMBER];
	struct ad7616_iio_dev *iio_dev = device;
	struct ad7616_dev *dev = iio_dev->ad7616_dev;

	ret = ad7616_read_data_serial(dev, res, 1);
	if (ret)
		return ret;

	if (is_channel_a_res)
		return sprintf(buf, "%i", res[index].channel_a);

	return sprintf(buf, "%i", res[index].channel_b);
}

static int get_scale(void *device,
		     char *buf,
		     uint32_t len,
		     const struct iio_ch_info *channel,
		     intptr_t id)
{
	struct ad7616_iio_dev *iio_dev = device;
	struct ad7616_dev *dev = iio_dev->ad7616_dev;

	enum ad7616_range range = is_channel_a(channel->ch_num) ?
				  dev->va[channel->ch_num] : dev->vb[channel->ch_num - CHANNEL_SOURCES_NUMBER];

	switch (range) {
	case AD7616_10V:
		return sprintf(buf, "10");
	case AD7616_5V:
		return sprintf(buf, "5");
	case AD7616_2V5:
		return sprintf(buf, "2.5");
	default:
		return -EINVAL;
	}
}

static int set_over_sampling_ratio(struct ad7616_dev *dev,
				   char *buf,
				   uint32_t len)
{
	enum ad7616_osr osr;

	sscanf(buf, "%u", (int *)&osr);
	osr = log2(osr);

	switch (osr) {
	case AD7616_OSR_0 ... AD7616_OSR_128:
		return ad7616_set_oversampling_ratio(dev, osr);
	default:
		return -EINVAL;
	}
}

static int set_scale(void *device,
		     char *buf,
		     uint32_t len,
		     const struct iio_ch_info *channel,
		     intptr_t id)
{
	int ret;
	int val;
	enum ad7616_range range;
	enum ad7616_ch ch = is_channel_a(channel->ch_num) ? channel->ch_num : AD7616_VB0
			    +
			    channel->ch_num - CHANNEL_SOURCES_NUMBER;
	struct ad7616_iio_dev *iio_dev = device;
	struct ad7616_dev *dev = iio_dev->ad7616_dev;

	ret = sscanf(buf, "%u", &val);
	if (ret != 1)
		return -EINVAL;

	switch (val) {
	case 10:
		range = AD7616_10V;
		break;
	case 5:
		range = AD7616_5V;
		break;
	case 2:
		range = AD7616_2V5;
		break;
	default:
		return -EINVAL;
	}

	return ad7616_set_range(dev, ch, range);
}

static int iio_ad7616_attr_get(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct ad7616_iio_dev *iio_dev = device;
	struct ad7616_dev *dev = iio_dev->ad7616_dev;
	enum ad7616_osr osr;

	switch (priv) {
	case CRC_ATTR_ID:
		return sprintf(buf, "%u", dev->crc);
	case OSR_ATTR_ID:
		osr = (int)pow(2, dev->osr);
		if (osr == 0)
			osr++;
		return sprintf(buf, "%u", osr);
	case OSR_AVAILABLE_ID:
		return sprintf(buf, "1 2 4 8 16 32 64 128");
	default:
		return -EINVAL;
	}
}

static int iio_ad7616_attr_set(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct ad7616_iio_dev *iio_dev = device;
	struct ad7616_dev *dev = iio_dev->ad7616_dev;

	switch (priv) {
	case OSR_ATTR_ID:
		return set_over_sampling_ratio(dev, buf, len);
	case CRC_ATTR_ID:
	case OSR_AVAILABLE_ID:
	default:
		return -EINVAL;
	}
}

/*
 * @brief Setup AD7616's sequencer to ask the channel found in the iio buffer active_mask
 * @param dev - The ad7616 device structure
 * @param active_mask - IIO buffer active_mask
 * @return Result of call to ad7616_setup_sequencer
 */
static int32_t setup_sequencer_layers_from_active_mask(struct ad7616_dev *dev,
		uint32_t active_mask)
{
	uint32_t i;
	uint32_t index_a, index_b;
	uint8_t active_mask_a = active_mask & 0xFF;
	uint8_t active_mask_b = (active_mask >> 8) & 0xFF;
	uint32_t nb_layers = no_os_max(no_os_hweight8(active_mask_a),
				       no_os_hweight8(active_mask_b));
	enum ad7616_ch ch_a, ch_b;
	struct ad7616_sequencer_layer layers[nb_layers];

	for (i = 0; i < nb_layers; i++) {
		index_a = no_os_find_first_set_bit(active_mask_a);
		if (index_a == 32) {
			// No more A channel requested, default to self test channel
			ch_a = AD7616_VA_SELF_TEST;
		} else {
			active_mask_a = active_mask_a & ~NO_OS_BIT(index_a);
			ch_a = AD7616_VA0 + index_a;
		}

		index_b = no_os_find_first_set_bit(active_mask_b);
		if (index_b == 32) {
			// No more B channel requested, default to self test channel
			ch_b = AD7616_VB_SELF_TEST;
		} else {
			active_mask_b = active_mask_b & ~NO_OS_BIT(index_b);
			ch_b = AD7616_VB0 + index_b;
		}

		layers[i] = (struct ad7616_sequencer_layer) {
			ch_a, ch_b
		};
	}

	return ad7616_setup_sequencer(dev, layers, nb_layers, 1);
}

/**
 * @brief	Read buffer data corresponding to AD7616 IIO device
 * @param	iio_dev_data - Pointer to IIO device data structure
 * @return	0 in case of success, negative error code otherwise
 */
static int32_t iio_ad7616_submit_buffer(struct iio_device_data *iio_dev_data)
{
	struct ad7616_iio_dev *iio_dev = iio_dev_data->dev;
	struct ad7616_dev *dev = iio_dev->ad7616_dev;
	struct ad7616_conversion_result *results;
	uint16_t data[iio_dev_data->buffer->bytes_per_scan / 2];
	uint8_t active_mask_a;
	uint8_t active_mask_b;
	uint32_t index_a;
	uint32_t index_b;
	int32_t ret;
	uint32_t i;
	uint32_t j;
	uint32_t k;

	results = no_os_calloc(dev->layers_nb * iio_dev_data->buffer->samples, sizeof(
				       *results));
	if (!results)
		return -ENOMEM;

	// Setup AD7616's sequencer to ask the channel found in the iio buffer active_mask
	ret = setup_sequencer_layers_from_active_mask(dev,
			iio_dev_data->buffer->active_mask);
	if (ret)
		goto cleanup;

	ret = ad7616_read_data_serial(dev, results, iio_dev_data->buffer->samples);
	if (ret)
		goto cleanup;

	for (i = 0; i < iio_dev_data->buffer->samples; i++) {
		active_mask_a = iio_dev_data->buffer->active_mask & 0xFF;
		active_mask_b = (iio_dev_data->buffer->active_mask >> 8) & 0xFF;
		k = 0;

		for (j = 0; active_mask_a; j++) {
			index_a = no_os_find_first_set_bit(active_mask_a);
			active_mask_a = active_mask_a & ~NO_OS_BIT(index_a);
			data[k++] = results[i * dev->layers_nb + j].channel_a;
		}

		for (j = 0; active_mask_b; j++) {
			index_b = no_os_find_first_set_bit(active_mask_b);
			active_mask_b = active_mask_b & ~NO_OS_BIT(index_b);
			data[k++] = results[i * dev->layers_nb + j].channel_b;
		}

		ret = iio_buffer_push_scan(iio_dev_data->buffer, &data);
		if (ret)
			goto cleanup;
	}

	return 0;

cleanup:
	no_os_free(results);
	return ret;
}

static struct scan_type ad7616_iio_scan_type = {
	.sign = 's',
	.realbits = CHN_STORAGE_BITS,
	.storagebits = CHN_STORAGE_BITS,
	.shift = 0,
	.is_big_endian = false
};

static struct iio_attribute ad7616_iio_ch_attributes[] = {
	{ .name = "raw", .show = get_raw },
	{ .name = "scale", .show = get_scale, .store = set_scale },
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad7616_iio_global_attributes[] = {
	AD7616_CHN_ATTR("crc", CRC_ATTR_ID),
	AD7616_CHN_ATTR("oversampling_ratio", OSR_ATTR_ID),
	AD7616_CHN_ATTR("oversampling_ratio_available", OSR_AVAILABLE_ID),
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ad7616_channels[] = {
	AD7616_CH(0),
	AD7616_CH(1),
	AD7616_CH(2),
	AD7616_CH(3),
	AD7616_CH(4),
	AD7616_CH(5),
	AD7616_CH(6),
	AD7616_CH(7),
	AD7616_CH(8),
	AD7616_CH(9),
	AD7616_CH(10),
	AD7616_CH(11),
	AD7616_CH(12),
	AD7616_CH(13),
	AD7616_CH(14),
	AD7616_CH(15),
};

static struct iio_device ad7616_iio_device_template = {
	.num_ch = NO_OS_ARRAY_SIZE(ad7616_channels),
	.channels = ad7616_channels,
	.attributes = ad7616_iio_global_attributes,
	.debug_reg_read = iio_ad7616_debug_reg_read,
	.debug_reg_write= iio_ad7616_debug_reg_write,
	.submit = iio_ad7616_submit_buffer,
};

/**
 * @brief Initialize AD7616 for IIO interfacing
 * @param dev - The device structure.
 * @param iio_desc - IIO handle
 * @return 0 on success, an error code otherwise
 */
int ad7616_iio_init(struct ad7616_iio_dev **dev,
		    struct ad7616_init_param *init_param)
{
	int ret;
	struct ad7616_sequencer_layer layers[] = {
		{ AD7616_VA0, AD7616_VB0 },
		{ AD7616_VA1, AD7616_VB1 },
		{ AD7616_VA2, AD7616_VB2 },
		{ AD7616_VA3, AD7616_VB3 },
		{ AD7616_VA4, AD7616_VB4 },
		{ AD7616_VA5, AD7616_VB5 },
		{ AD7616_VA6, AD7616_VB6 },
		{ AD7616_VA7, AD7616_VB7 },
	};
	struct ad7616_iio_dev *desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &ad7616_iio_device_template;

	ret = ad7616_setup(&desc->ad7616_dev, init_param);
	if (ret)
		goto error_setup;

	ad7616_setup_sequencer(desc->ad7616_dev, layers, NO_OS_ARRAY_SIZE(layers), 1);

	*dev = desc;

	return 0;

error_setup:
	no_os_free(desc);
	return ret;
}

int ad7616_iio_remove(struct ad7616_iio_dev *dev)
{
	if (!dev)
		return -ENODEV;

	ad7616_remove(dev->ad7616_dev);

	no_os_free(dev);

	return 0;
}

#endif
