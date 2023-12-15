/***************************************************************************//**
* @file   iio_ad738x.c
* @brief  Implementation of AD738X IIO interface
********************************************************************************
* Copyright (c) 2023 Analog Devices, Inc.
* Copyright (c) 2023 BayLibre, SAS.
* All rights reserved.
*
* This software is proprietary to Analog Devices, Inc. and its licensors.
* By using this software you agree to the terms of the associated
* Analog Devices Software License Agreement.
*******************************************************************************/
#ifdef IIO_SUPPORT

#include <stdio.h>
#include <math.h>

#include "ad738x.h"
#include "iio_ad738x.h"
#include "iio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

#define AD738X_CH(_idx) {\
	.name = "voltage" #_idx, \
	.ch_type = IIO_VOLTAGE,\
	.ch_out = IIO_DIRECTION_INPUT,\
	.indexed = true,\
	.channel = _idx,\
	.scan_index = _idx,\
	.scan_type = &ad738x_iio_scan_type,\
	.attributes = ad738x_iio_ch_attributes,\
}

#define CHN_STORAGE_BITS 16
#define ADC_RESOLUTION		16
#define ADC_REF_VOLTAGE		3.3
#define ADC_MAX_COUNT_BIPOLAR	(uint32_t)(1 << (ADC_RESOLUTION-1))
#define AD738X_DEF_IIO_SCALE (ADC_REF_VOLTAGE / ADC_MAX_COUNT_BIPOLAR) * 1000

struct ad738x_iio {
	struct ad738x_dev *drv_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief       Read the debug register value
 * @param       dev[in, out]- Pointer to IIO device instance
 * @param       reg[in]- Register address to read from
 * @param       readval[out]- Pointer to variable to read data into
 * @return      0 in case of success, negative value otherwise
 */
static int32_t iio_ad738x_debug_reg_read(void *device, uint32_t reg,
		uint32_t *val)
{
	struct ad738x_iio_dev *iio_dev = device;
	struct ad738x_dev *dev = iio_dev->ad738x_dev;

	return ad738x_spi_reg_read(dev,  (uint8_t) reg, (uint16_t *)val);
}

/**
 * @brief       Write the debug register value
 * @param       dev[in, out]- Pointer to IIO device instance
 * @param       reg[in]- Register address to read from
 * @param       writeval[out]- Pointer to variable to write data into
 * @return      0 in case of success, negative value otherwise
 */
static int32_t iio_ad738x_debug_reg_write(void *device, uint32_t reg,
		uint32_t val)
{
	struct ad738x_iio_dev *iio_dev = device;
	struct ad738x_dev *dev = iio_dev->ad738x_dev;

	return ad738x_spi_reg_write(dev, (uint8_t)reg, (uint16_t)val);
}

static int get_raw(void *device,
		   char *buf,
		   uint32_t len,
		   const struct iio_ch_info *channel,
		   intptr_t id)
{
	struct ad738x_iio_dev *iio_dev = device;
	struct ad738x_dev *dev = iio_dev->ad738x_dev;
	uint16_t results[2];
	int32_t ret;
	ret = ad738x_spi_single_conversion(dev, results);
	if (ret)
		return ret;

	return sprintf(buf, "%i", results[channel->ch_num]);
}

static int get_scale(void *device,
		     char *buf,
		     uint32_t len,
		     const struct iio_ch_info *channel,
		     intptr_t id)
{
	return snprintf(buf, len, "%10f", AD738X_DEF_IIO_SCALE);
}

static int get_offset(void *device,
		      char *buf,
		      uint32_t len,
		      const struct iio_ch_info *channel,
		      intptr_t id)
{
	return snprintf(buf, len, "%d", 0);
}


/**
 * @brief	Read buffer data corresponding to AD738X IIO device
 * @param	iio_dev_data[in] - Pointer to IIO device data structure
 * @return	0 in case of success, negative error code otherwise
 */
static int32_t iio_ad738x_submit_buffer(struct iio_device_data *iio_dev_data)
{
	struct ad738x_iio_dev *iio_dev = iio_dev_data->dev;
	struct ad738x_dev *dev = iio_dev->ad738x_dev;
	uint16_t results[2];
	uint32_t i;
	int32_t ret;

	for (i = 0; i < iio_dev_data->buffer->samples; i++) {
		ret = ad738x_spi_single_conversion(dev, results);
		if (ret)
			return ret;

		/* iio_buffer_push_scan will consume the number of bytes
		 * accoridg to the number of enabled channels.
		 * if the second channel is the only enabled channel
		 * we push only the second result.
		 */
		if ((iio_dev_data->buffer->active_mask & NO_OS_BIT(1)) &&
		    (!(iio_dev_data->buffer->active_mask & NO_OS_BIT(0)))) {
			iio_buffer_push_scan(iio_dev_data->buffer, &results[1]);
			continue;
		}

		iio_buffer_push_scan(iio_dev_data->buffer, &results[0]);

	}

	return 0;
}

static struct scan_type ad738x_iio_scan_type = {
	.sign = 'u',
	.realbits = CHN_STORAGE_BITS,
	.storagebits = CHN_STORAGE_BITS,
};

static struct iio_attribute ad738x_iio_ch_attributes[] = {
	{ .name = "raw", .show = get_raw },
	{ .name = "scale", .show = get_scale, },
	{ .name = "offset", .show = get_offset, },
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ad738x_channels[] = {
	AD738X_CH(0),
	AD738X_CH(1),
};

static struct iio_device ad738x_iio_device_template = {
	.num_ch = NO_OS_ARRAY_SIZE(ad738x_channels),
	.channels = ad738x_channels,
	.debug_reg_read = iio_ad738x_debug_reg_read,
	.debug_reg_write= iio_ad738x_debug_reg_write,
	.submit = iio_ad738x_submit_buffer,
};

/**
 * @brief Initialize AD738X for IIO interfacing
 * @param dev - The device structure.
 * @param iio_desc - IIO handle
 * @return 0 on success, an error code otherwise
 */
int ad738x_iio_init(struct ad738x_iio_dev **dev,
		    struct ad738x_init_param *init_param)
{
	struct ad738x_iio_dev *desc;
	int ret;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &ad738x_iio_device_template;

	ret = ad738x_init(&desc->ad738x_dev, init_param);
	if (ret)
		goto error_setup;

	*dev = desc;

	return 0;

error_setup:
	no_os_free(desc);
	return ret;
}

#endif
