/***************************************************************************//**
 *   @file   iio_ad400x.c
 *   @brief  Implementation of AD400x IIO Driver.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
*******************************************************************************/
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "ad400x.h"
#include "iio_ad400x.h"
#include "iio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

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
	struct scan_type *scan_type;
	uint32_t results;
	int32_t results_signed;
	int ret;

	scan_type = iio_dev->iio_dev->channels[channel->ch_num].scan_type;

	ret = ad400x_read_data(dev, &results, 1);
	if (ret)
		return ret;

	if (scan_type->sign == 's') {
		results_signed = no_os_sign_extend32(results, scan_type->realbits - 1);
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &results_signed);
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&results);
}

static int get_scale(void *device, char *buf, uint32_t len,
		     const struct iio_ch_info *channel, intptr_t id)
{
	struct ad400x_iio_dev *iio_dev = device;
	struct scan_type *scan_type;
	int32_t vals[2];

	scan_type = iio_dev->iio_dev->channels[channel->ch_num].scan_type;
	vals[0] = iio_dev->ref_voltage_mv;
	if (scan_type->sign == 'u')
		vals[1] = scan_type->realbits;
	else
		vals[1] = scan_type->realbits - 1;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
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
	struct iio_buffer *buffer = iio_dev_data->buffer;
	void *buff;
	int ret;

	ret = iio_buffer_get_block(iio_dev_data->buffer, &buff);
	if (ret)
		return ret;

	ret = ad400x_read_data(dev, buff, buffer->samples);
	if (ret)
		return ret;

	return iio_buffer_block_done(buffer);
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
 * @param iio_init_param - IIO init parameter structure
 * @return 0 on success, an error code otherwise
 */
int ad400x_iio_init(struct ad400x_iio_dev **dev,
		    struct ad400x_iio_init_param *iio_init_param)
{
	struct ad400x_iio_dev *desc;
	int ret;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &ad400x_iio_device_template;
	desc->ref_voltage_mv = iio_init_param->ref_voltage_mv;
	desc->iio_dev->channels[0].scan_type = &desc->scan_type;

	ret = ad400x_init(&desc->ad400x_dev, iio_init_param->init_param);
	if (ret)
		goto error_setup;

	/* fill scan_type based on device id */
	desc->scan_type.sign = desc->ad400x_dev->dev_info->sign;
	desc->scan_type.realbits = desc->ad400x_dev->dev_info->resolution;
	desc->scan_type.storagebits = 32;
	desc->scan_type.is_big_endian = false;
	desc->scan_type.shift = 0;

	*dev = desc;

	return 0;

error_setup:
	no_os_free(desc);
	return ret;
}

/**
 * @brief Release resources.
 * @param iio_dev - iio device.
 * @return 0 in case of success, error otherwise.
 */
int ad400x_iio_remove(struct ad400x_iio_dev *iio_dev)
{
	if (!iio_dev)
		return -EINVAL;

	ad400x_remove(iio_dev->ad400x_dev);

	no_os_free(iio_dev);
	return 0;
}
