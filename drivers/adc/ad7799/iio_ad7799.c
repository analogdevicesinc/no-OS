/***************************************************************************//**
 *   @file   iio_ad7799.c
 *   @brief  Implementation of AD7799 iio.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include "iio_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ad7799.h"
#include "no_os_util.h"
#include "no_os_error.h"

/**
 * @brief Read ADC Channel data.
 * @param device - Device driver descriptor.
 * @param buf - Input buffer.
 * @param len - Length of the input buffer.
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7799_iio_channel_read(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel)
{
	struct ad7799_dev *dev = (struct ad7799_dev *)device;
	int32_t ret;
	uint32_t data;

	ret = ad7799_get_channel(dev, channel->ch_num, &data);
	if (ret != 0)
		return ret;

	return snprintf(buf, len, "%d", data);
}

/**
 * @brief Read ADC Gain value.
 * @param device - Device driver descriptor.
 * @param buf - Input buffer.
 * @param len - Length of the input buffer.
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7799_iio_gain_read(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ad7799_dev *dev = (struct ad7799_dev *)device;
	uint8_t gain;
	int32_t ret;

	ret = ad7799_get_gain(dev, &gain);
	if (ret != 0)
		return ret;

	return snprintf(buf, len, "%d", gain);
}

/**
 * @brief Write ADC Gain value.
 * @param device - Device driver descriptor.
 * @param buf - Input buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7799_iio_gain_write(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad7799_dev *dev = (struct ad7799_dev *)device;
	int32_t ret;

	uint32_t gain = no_os_str_to_uint32(buf);

	ret = ad7799_set_gain(dev, (uint8_t)gain);
	if (ret != 0)
		return ret;

	return len;
}

/** ADC IIO channel attributes*/
static struct iio_attribute ad7799_iio_channel_attributes[] = {
	{
		.name = "volts",
		.show = ad7799_iio_channel_read,
		.store = NULL
	},
	END_ATTRIBUTES_ARRAY,
};

/** Channel Scan Type */
static struct scan_type channel_scan_type = {
	.sign = 'u',
	.realbits = 12,
	.storagebits = 24,
	.shift = 0,
	.is_big_endian = false
};

#define AD7799_IIO_CHANN_DEF(nm, ch) \
	{ \
		.name = nm, \
		.ch_type = IIO_VOLTAGE, \
		.channel = ch, \
		.scan_type = &channel_scan_type, \
		.attributes = ad7799_iio_channel_attributes, \
		.ch_out = false, \
		.indexed = 1, \
		.diferential = true, \
	}

/** IIO ADC Channels */
static struct iio_channel ad7799_iio_channels[] = {
	AD7799_IIO_CHANN_DEF("channel0", 0),
	AD7799_IIO_CHANN_DEF("channel1", 1),
	AD7799_IIO_CHANN_DEF("channel2", 2),
	END_ATTRIBUTES_ARRAY
};

/** IIO attributes */
static struct iio_attribute ad7799_iio_attributes[] = {
	{
		.name = "gain",
		.show = ad7799_iio_gain_read,
		.store = ad7799_iio_gain_write,
	},
	END_ATTRIBUTES_ARRAY,
};

/** IIO Descriptor */
struct iio_device const ad7799_iio_descriptor = {
	.num_ch = 3,
	.channels = ad7799_iio_channels,
	.attributes = ad7799_iio_attributes,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.debug_reg_read = (int32_t (*)())ad7799_read,
	.debug_reg_write = (int32_t (*)())ad7799_write,
};

