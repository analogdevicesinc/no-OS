/***************************************************************************//**
 *   @file   iio_ad717x.c
 *   @brief  Implementation of iio_ad717x
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#ifdef IIO_SUPPORT

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "ad717x.h"
#include "iio_ad717x.h"
#include "no_os_error.h"
#include "iio.h"

/* Read device register */
int32_t iio_ad717x_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	*readval = 0;

	return 0;
}

/* Write device register */
int32_t iio_ad717x_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	return 0;
}


static int32_t iio_ad717x_read_dev(void *dev,
				   int32_t *buff,
				   uint32_t nb_samples)
{
	struct ad717x_dev *desc = (struct ad717x_dev *)dev;
	int ret;
	uint i;

	for (i = 0; i < nb_samples; i++) {
		ret = ad717x_wait_for_ready(desc , AD717X_CONV_TIMEOUT);
		if(ret < 0)
			return ret;

		ret = ad717x_read_data(desc , &buff[i]);
		if(ret < 0)
			return ret;
	}

	return nb_samples;
}

static int ad717x_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct ad717x_dev *desc = (struct ad717x_dev *)dev;
	int32_t value;
	int ret;

	ret = ad717x_wait_for_ready(desc , AD717X_CONV_TIMEOUT);
	if(ret < 0)
		return ret;

	ret = ad717x_read_data(desc , &value);
	if(ret < 0)
		return ret;

	return snprintf(buf, len, "%"PRId32"", (uint32_t)value);
}

static const int ad4111_iio_odr_table[18][3] = {
	{31250, 0, 0x0},
	{15625, 0, 0x6},
	{10417, 0, 0x7},
	{5208, 300000, 0x8},
	{25974, 0, 0x9},
	{1007, 0, 0xa},
	{503, 800000, 0xb},
	{381, 0, 0xc},
	{200, 300000, 0xd},
	{100, 200000, 0xe},
	{59, 520000, 0xf},
	{49, 680000, 0x10},
	{20, 10000, 0x11},
	{16, 630000, 0x12},
	{10, 0, 0x13},
	{5, 0, 0x14},
	{2, 500000, 0x15},
	{1, 250000, 0x16},
};

static int ad717x_iio_read_samp_freq_avail(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	char buffer[20];
	uint8_t i;

	strcpy(buf, "");

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad4111_iio_odr_table); i++) {
		vals[0] = ad4111_iio_odr_table[i][0];
		vals[1] = ad4111_iio_odr_table[i][1];
		iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT_PLUS_MICRO, 2, vals);
		strcat(buf, buffer);
		if (i < NO_OS_ARRAY_SIZE(ad4111_iio_odr_table) - 1)
			strcat(buf, " ");
	}

	return strlen(buf);
}

static int ad717x_iio_write_samp_freq(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad717x_dev *desc = (struct ad717x_dev *)dev;
	int32_t vals[2];
	uint i;

	iio_parse_value(buf, IIO_VAL_FRACTIONAL, &vals[0], &vals[1]);

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad4111_iio_odr_table); i++) {
		if (ad4111_iio_odr_table[i][0] == vals[0] && ad4111_iio_odr_table[i][1] == vals[1])
		{
			ad717x_configure_device_odr(desc, 0, ad4111_iio_odr_table[i][2]);
			return 0;
		}
	}

	return -1;
}

struct scan_type ad717x_iio_scan_type = {
	.sign = 'u',
	.realbits = 24,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false
};

static int ad717x_iio_read_samp_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad717x_dev *desc = (struct ad717x_dev *)dev;
	struct ad717x_st_reg *filtcon_reg;
	int32_t vals[2] = {0, 0};
	uint i;

	ad717x_read_register(desc, AD717X_FILTCON0_REG);
	filtcon_reg = ad717x_get_reg(desc, AD717X_FILTCON0_REG + 0);
	if (!filtcon_reg) {
		return -EINVAL;
	}
	filtcon_reg->value &= AD717x_ODR_MSK;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad4111_iio_odr_table); i++) {
		if (ad4111_iio_odr_table[i][2] == filtcon_reg->value) {
			vals[0] = ad4111_iio_odr_table[i][0];
			vals[1] = ad4111_iio_odr_table[i][1];
		}
	}

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
}

static struct iio_attribute channel_attributes[] = {
	{
		.name = "raw",
		.priv = 0,
		.show = ad717x_iio_read_raw,
		.store = NULL
	},
	{
		.name = "sampling_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ad717x_iio_read_samp_freq,
		.store = ad717x_iio_write_samp_freq
	},
	{
		.name = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ad717x_iio_read_samp_freq_avail,
		.store = NULL
	},
	END_ATTRIBUTES_ARRAY
};

#define AD717X_IIO_CHANN_DEF(nm, ch1, ch2) \
	{ \
		.name = nm, \
		.ch_type = IIO_VOLTAGE, \
		.channel = ch1, \
		.channel2 = ch2, \
		.scan_type = &ad717x_iio_scan_type, \
		.ch_out = 0, \
		.indexed = 1, \
		.diferential = true, \
		.attributes = channel_attributes, \
	}

static struct iio_channel ad717x_channels[] = {
	AD717X_IIO_CHANN_DEF("ch0", 6, 7),
};

struct iio_device ad717x_iio_descriptor = {
	.num_ch = NO_OS_ARRAY_SIZE(ad717x_channels),
	.channels = ad717x_channels,
	.read_dev = (int32_t (*)())iio_ad717x_read_dev,
	.debug_reg_read = (int32_t (*)())iio_ad717x_reg_read,
	.debug_reg_write = (int32_t (*)())iio_ad717x_reg_write,
};

#endif /* IIO_SUPPORT */
