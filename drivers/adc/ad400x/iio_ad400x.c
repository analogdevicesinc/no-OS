/***************************************************************************//**
* @file   iio_ad400x.c
* @brief  Implementation of AD400X IIO interface
********************************************************************************
* Copyright (c) 2024 Analog Devices, Inc.
* All rights reserved.
*
* This software is proprietary to Analog Devices, Inc. and its licensors.
* By using this software you agree to the terms of the associated
* Analog Devices Software License Agreement.
*******************************************************************************/
#ifdef IIO_SUPPORT

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "ad400x.h"
#include "iio_ad400x.h"
#include "iio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

struct ad400x_iio {
	struct ad400x_dev *drv_dev;
	struct iio_device *iio_dev;
};

static struct scan_type ad400x_iio_scan_types[] = {
	[ID_AD4000] = { .sign = 'u', .realbits = 18 },
	[ID_AD4001] = { .sign = 's', .realbits = 16 },
	[ID_AD4002] = { .sign = 'u', .realbits = 18 },
	[ID_AD4003] = { .sign = 's', .realbits = 18 },
	[ID_AD4004] = { .sign = 'u', .realbits = 16 },
	[ID_AD4005] = { .sign = 's', .realbits = 16 },
	[ID_AD4006] = { .sign = 'u', .realbits = 18 },
	[ID_AD4007] = { .sign = 's', .realbits = 18 },
	[ID_AD4011] = { .sign = 's', .realbits = 18 },
	[ID_AD4020] = { .sign = 's', .realbits = 20 },
	[ID_ADAQ4003] = { .sign = 's', .realbits = 18},
};

static struct scan_type ad400x_iio_scan_type;

/**
 * @brief Read the debug register value
 * @param device - Pointer to IIO device instance
 * @param reg - Register address to read from
 * @param val - Pointer to variable to read data into
 * @return 0 in case of success, negative value otherwise
 */
static int32_t iio_ad400x_debug_reg_read(void *device, uint32_t reg,
		uint32_t *val)
{
	struct ad400x_iio_dev *iio_dev = device;
	struct ad400x_dev *dev = iio_dev->ad400x_dev;
	int ret;
	uint8_t val8;

	ret = ad400x_spi_reg_read(dev, &val8);
	if (ret)
		return ret;

	*val = val8;
	return 0;
}

/**
 * @brief Write the debug register value
 * @param device - Pointer to IIO device instance
 * @param reg - Register address to read from
 * @param val - Pointer to variable to write data into
 * @return 0 in case of success, negative value otherwise
 */
static int32_t iio_ad400x_debug_reg_write(void *device, uint32_t reg,
		uint32_t val)
{
	struct ad400x_iio_dev *iio_dev = device;
	struct ad400x_dev *dev = iio_dev->ad400x_dev;

	return ad400x_spi_reg_write(dev, (uint8_t)val);
}

static int get_raw(void *device, char *buf, uint32_t len,
		   const struct iio_ch_info *channel, intptr_t id)
{
	struct ad400x_iio_dev *iio_dev = device;
	struct ad400x_dev *dev = iio_dev->ad400x_dev;
	uint32_t results = 0;
	int32_t ret;

	ret = ad400x_spi_single_conversion(dev, &results);
	if (ret)
		return ret;

	return sprintf(buf, "%lx", results);
}

static int get_scale(void *device, char *buf, uint32_t len,
		     const struct iio_ch_info *channel, intptr_t id)
{
	struct ad400x_iio_dev *iio_dev = device;
	struct ad400x_dev *dev = iio_dev->ad400x_dev;
	int32_t vals[2];

	vals[0] = 5000;
	vals[1] = ad400x_iio_scan_types[dev->dev_id].realbits;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 1, vals);
}

/**
 * @brief Read buffer data corresponding to AD400X IIO device
 * @param iio_dev_data - Pointer to IIO device data structure
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t iio_ad400x_submit_buffer(struct iio_device_data *iio_dev_data)
{
	struct ad400x_iio_dev *iio_dev = iio_dev_data->dev;
	struct ad400x_dev *dev = iio_dev->ad400x_dev;
	uint32_t samples = iio_dev_data->buffer->samples;
	uint32_t *results;
	uint32_t i;
	int32_t ret;

	results = no_os_malloc(sizeof(*results) * samples);
	if (!results)
		return -ENOMEM;

	for (i = 0; i < samples; i++) {
		ret = ad400x_spi_single_conversion(dev, &results[i]);
		if (ret)
			goto err_free;
	}

	for (i = 0; i < samples; i++) {
		ret = iio_buffer_push_scan(iio_dev_data->buffer, &results[i]);
		if (ret)
			goto err_free;
	}

	no_os_free(results);

	return 0;

err_free:
	no_os_free(results);
	return ret;
}

static struct iio_attribute ad400x_iio_ch_attributes[] = {
	{ .name = "raw", .show = get_raw },
	{ .name = "scale", .show = get_scale, },
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ad400x_channel = {
	.name = "voltage0",
	.ch_type = IIO_VOLTAGE,
	.ch_out = IIO_DIRECTION_INPUT,
	.indexed = true,
	.channel = 0,
	.scan_index = 0,
	.attributes = ad400x_iio_ch_attributes,
};

static struct iio_device ad400x_iio_device_template = {
	.num_ch = 1,
	.channels = &ad400x_channel,
	.debug_reg_read = iio_ad400x_debug_reg_read,
	.debug_reg_write= iio_ad400x_debug_reg_write,
	.submit = iio_ad400x_submit_buffer,
};


/**
 * @brief Initialize AD400X for IIO interfacing
 * @param dev - The device structure.
 * @param init_param - IIO init parameter structure
 * @return 0 on success, an error code otherwise
 */
int ad400x_iio_init(struct ad400x_iio_dev **dev,
		    struct ad400x_init_param *init_param)
{
	struct ad400x_iio_dev *desc;
	int ret;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &ad400x_iio_device_template;

	/* fill scan_type based on device id */
	memcpy(&ad400x_iio_scan_type, &ad400x_iio_scan_types[init_param->dev_id],
	       sizeof(ad400x_iio_scan_type));
	ad400x_iio_scan_type.is_big_endian = true;
	ad400x_iio_scan_type.storagebits = 32;
	ad400x_iio_scan_type.shift =
		ad400x_iio_scan_type.storagebits - ad400x_iio_scan_type.realbits;
	desc->iio_dev->channels[0].scan_type = &ad400x_iio_scan_type;

	ret = ad400x_init(&desc->ad400x_dev, init_param);
	if (ret)
		goto error_setup;

	*dev = desc;

	return 0;

error_setup:
	no_os_free(desc);
	return ret;
}

int ad400x_iio_remove(struct ad400x_iio_dev *dev)
{
	if (!dev)
		return -ENODEV;

	no_os_free(dev);
	return 0;
}

#endif
