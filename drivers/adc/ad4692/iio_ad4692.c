/**************************************************************************//**
*   @file   iio_ad4692.c
*   @brief  Source file of ad4692 IIO driver
*   @author Radu Sabau (radu.sabau@analog.com)
*******************************************************************************
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
******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_delay.h"

#include "ad4692.h"
#include "iio_ad4692.h"

static const int ad4692_iio_int_osc_sel[] = {
	[AD4692_OSC_1MHZ] = 1000000,
	[AD4692_OSC_500KHZ] = 500000,
	[AD4692_OSC_400KHZ] = 400000,
	[AD4692_OSC_250KHZ] = 250000,
	[AD4692_OSC_200KHZ] = 200000,
	[AD4692_OSC_167KHZ] = 167000,
	[AD4692_OSC_133KHZ] = 133000,
	[AD4692_OSC_125KHZ] = 125000,
	[AD4692_OSC_100KHZ] = 100000,
	[AD4692_OSC_50KHZ] = 50000,
	[AD4692_OSC_25KHZ] = 25000,
	[AD4692_OSC_12P5KHZ] = 12500,
	[AD4692_OSC_10KHZ] = 10000,
	[AD4692_OSC_5KHZ] = 5000,
	[AD4692_OSC_2P5KHZ] = 2500,
	[AD4692_OSC_1P25KHZ] = 1500,
};

static int ad4692_iio_reg_read(struct ad4692_iio_desc *desc, uint32_t reg,
			       uint32_t *readval);
static int ad4692_iio_reg_write(struct ad4692_iio_desc *desc, uint32_t reg,
				uint32_t writeval);
static int ad4692_iio_read_raw(void *desc, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv);
static int ad4692_iio_read_scale(void *desc, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);
static int ad4692_iio_read_freq(void *desc, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv);
static int ad4692_iio_write_freq(void *desc, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);
static int ad4692_iio_read_freq_avail(void *desc, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv);
static int ad4692_iio_update_channels(void *desc, uint32_t mask);
static int ad4692_iio_post_disable(void *desc);
static int32_t ad4692_trigger_handler(struct iio_device_data *desc_data);
static int ad4692_iio_read_samples(void *desc, int *buff, uint32_t samples);

static struct iio_attribute ad4692_iio_attrs[] = {
	{
		.name = "raw",
		.show = ad4692_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ad4692_iio_read_scale,
	},
	{
		.name = "sampling_frequency",
		.show = ad4692_iio_read_freq,
		.store = ad4692_iio_write_freq,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "sampling_frequency_available",
		.show = ad4692_iio_read_freq_avail,
		.shared = IIO_SHARED_BY_ALL
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type ad4692_iio_scan_type[] = {
	[ID_AD4692] = {
		.sign = 'u',
		.realbits = 16,
		.storagebits = 16,
		.shift = 0,
		.is_big_endian = true,
	},
	[ID_AD4691] = {
		.sign = 'u',
		.realbits = 18,
		.storagebits = 32,
		.shift = 0,
		.is_big_endian = true,
	},
	[ID_AD4694] = {
		.sign = 'u',
		.realbits = 20,
		.storagebits = 32,
		.shift = 0,
		.is_big_endian = true,
	},
	[ID_AD4693] = {
		.sign = 'u',
		.realbits = 14,
		.storagebits = 16,
		.shift = 0,
		.is_big_endian = true,
	},
};

#define AD4692_CHANNEL(index, id) {	\
	.ch_type = IIO_VOLTAGE,			\
	.channel = index,			\
	.address = index,			\
	.indexed = 1,				\
	.scan_type = &ad4692_iio_scan_type[id],	\
	.scan_index = index,			\
	.attributes = ad4692_iio_attrs,		\
	.ch_out = false				\
}

static struct iio_channel ad4692_channels[] = {
	AD4692_CHANNEL(0, ID_AD4692),
	AD4692_CHANNEL(1, ID_AD4692),
	AD4692_CHANNEL(2, ID_AD4692),
	AD4692_CHANNEL(3, ID_AD4692),
	AD4692_CHANNEL(4, ID_AD4692),
	AD4692_CHANNEL(5, ID_AD4692),
	AD4692_CHANNEL(6, ID_AD4692),
	AD4692_CHANNEL(7, ID_AD4692),
	AD4692_CHANNEL(8, ID_AD4692),
	AD4692_CHANNEL(9, ID_AD4692),
	AD4692_CHANNEL(10, ID_AD4692),
	AD4692_CHANNEL(11, ID_AD4692),
	AD4692_CHANNEL(12, ID_AD4692),
	AD4692_CHANNEL(13, ID_AD4692),
	AD4692_CHANNEL(14, ID_AD4692),
	AD4692_CHANNEL(15, ID_AD4692),
};

static struct iio_channel ad4691_channels[] = {
	AD4692_CHANNEL(0, ID_AD4691),
	AD4692_CHANNEL(1, ID_AD4691),
	AD4692_CHANNEL(2, ID_AD4691),
	AD4692_CHANNEL(3, ID_AD4691),
	AD4692_CHANNEL(4, ID_AD4691),
	AD4692_CHANNEL(5, ID_AD4691),
	AD4692_CHANNEL(6, ID_AD4691),
	AD4692_CHANNEL(7, ID_AD4691),
	AD4692_CHANNEL(8, ID_AD4691),
	AD4692_CHANNEL(9, ID_AD4691),
	AD4692_CHANNEL(10, ID_AD4691),
	AD4692_CHANNEL(11, ID_AD4691),
	AD4692_CHANNEL(12, ID_AD4691),
	AD4692_CHANNEL(13, ID_AD4691),
	AD4692_CHANNEL(14, ID_AD4691),
	AD4692_CHANNEL(15, ID_AD4691),
};

static struct iio_channel ad4694_channels[] = {
	AD4692_CHANNEL(0, ID_AD4694),
	AD4692_CHANNEL(1, ID_AD4694),
	AD4692_CHANNEL(2, ID_AD4694),
	AD4692_CHANNEL(3, ID_AD4694),
	AD4692_CHANNEL(4, ID_AD4694),
	AD4692_CHANNEL(5, ID_AD4694),
	AD4692_CHANNEL(6, ID_AD4694),
	AD4692_CHANNEL(7, ID_AD4694),
};

static struct iio_channel ad4693_channels[] = {
	AD4692_CHANNEL(0, ID_AD4693),
	AD4692_CHANNEL(1, ID_AD4693),
	AD4692_CHANNEL(2, ID_AD4693),
	AD4692_CHANNEL(3, ID_AD4693),
	AD4692_CHANNEL(4, ID_AD4693),
	AD4692_CHANNEL(5, ID_AD4693),
	AD4692_CHANNEL(6, ID_AD4693),
	AD4692_CHANNEL(7, ID_AD4693),
};

static struct iio_device ad4692_iio_dev[] = {
	[ID_AD4692] = {
		.num_ch = NO_OS_ARRAY_SIZE(ad4692_channels),
		.channels = ad4692_channels,
		.pre_enable = (int32_t (*)())ad4692_iio_update_channels,
		.post_disable = (int32_t (*)())ad4692_iio_post_disable,
		.trigger_handler = (int32_t (*)())ad4692_trigger_handler,
		.read_dev = (int32_t (*)())ad4692_iio_read_samples,
		.debug_reg_read = (int32_t (*)())ad4692_iio_reg_read,
		.debug_reg_write = (int32_t (*)())ad4692_iio_reg_write,
	},
	[ID_AD4691] = {
		.num_ch = NO_OS_ARRAY_SIZE(ad4691_channels),
		.channels = ad4691_channels,
		.pre_enable = (int32_t (*)())ad4692_iio_update_channels,
		.post_disable = (int32_t (*)())ad4692_iio_post_disable,
		.trigger_handler = (int32_t (*)())ad4692_trigger_handler,
		.read_dev = (int32_t (*)())ad4692_iio_read_samples,
		.debug_reg_read = (int32_t (*)())ad4692_iio_reg_read,
		.debug_reg_write = (int32_t (*)())ad4692_iio_reg_write,
	},
	[ID_AD4694] = {
		.num_ch = NO_OS_ARRAY_SIZE(ad4694_channels),
		.channels = ad4694_channels,
		.pre_enable = (int32_t (*)())ad4692_iio_update_channels,
		.post_disable = (int32_t (*)())ad4692_iio_post_disable,
		.trigger_handler = (int32_t (*)())ad4692_trigger_handler,
		.read_dev = (int32_t (*)())ad4692_iio_read_samples,
		.debug_reg_read = (int32_t (*)())ad4692_iio_reg_read,
		.debug_reg_write = (int32_t (*)())ad4692_iio_reg_write,
	},
	[ID_AD4693] = {
		.num_ch = NO_OS_ARRAY_SIZE(ad4693_channels),
		.channels = ad4693_channels,
		.pre_enable = (int32_t (*)())ad4692_iio_update_channels,
		.post_disable = (int32_t (*)())ad4692_iio_post_disable,
		.trigger_handler = (int32_t (*)())ad4692_trigger_handler,
		.read_dev = (int32_t (*)())ad4692_iio_read_samples,
		.debug_reg_read = (int32_t (*)())ad4692_iio_reg_read,
		.debug_reg_write = (int32_t (*)())ad4692_iio_reg_write,
	},
};


static int ad4692_iio_reg_read(struct ad4692_iio_desc *desc, uint32_t reg,
			       uint32_t *readval)
{
	return ad4692_reg_read(desc->ad4692_desc, reg, readval);
}

static int ad4692_iio_reg_write(struct ad4692_iio_desc *desc, uint32_t reg,
				uint32_t writeval)
{
	return ad4692_reg_write(desc->ad4692_desc, reg, writeval);
}

/* SPI Engine approach may be different. */
static int ad4692_iio_read_raw(void *desc, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct ad4692_iio_desc *iio_ad4692;
	struct ad4692_desc *ad4692;
	uint32_t reg_val;
	int ret;

	if (!desc)
		return -EINVAL;

	iio_ad4692 = desc;

	if (!iio_ad4692->ad4692_desc)
		return -EINVAL;

	ad4692 = iio_ad4692->ad4692_desc;

	switch (channel->type) {
	case IIO_VOLTAGE:
		switch (ad4692->mode) {
		case AD4692_CNV_CLOCK:
			ret = ad4692_std_seq_ch(ad4692, AD4692_CHANNEL_MASK(channel->ch_num), true, 0);
			if (ret)
				return ret;

			ret = no_os_pwm_get_period(ad4692->conv_desc, &reg_val);
			if (ret)
				return ret;

			ret = ad4692_sampling_enable(ad4692, true);
			if (ret)
				return ret;

			no_os_udelay(NO_OS_DIV_ROUND_CLOSEST(reg_val, MILLI));

			ret = ad4692_sampling_enable(ad4692, false);
			if (ret)
				return ret;

			ret = ad4692_get_ch(ad4692, channel->ch_num, &reg_val);
			if (ret)
				return ret;

			ret = ad4692_reg_write(ad4692, AD4692_STATE_RESET_REG, AD4692_STATE_RESET_ALL);
			if (ret)
				return ret;

			break;
		case AD4692_CNV_BURST:
			ret = ad4692_std_seq_ch(ad4692, AD4692_CHANNEL_MASK(channel->ch_num), true, 0);
			if (ret)
				return ret;

			ret = ad4692_get_osc(ad4692, (enum ad4692_int_osc_sel *)&reg_val);
			if (ret)
				return ret;

			ret = ad4692_sampling_enable(ad4692, true);
			if (ret)
				return ret;

			no_os_udelay(NO_OS_DIV_ROUND_CLOSEST_ULL(MICRO,
					ad4692_iio_int_osc_sel[reg_val]));

			ret = ad4692_sampling_enable(ad4692, false);
			if (ret)
				return ret;

			ret = ad4692_get_ch(ad4692, channel->ch_num, &reg_val);
			if (ret)
				return ret;

			ret = ad4692_reg_write(ad4692, AD4692_STATE_RESET_REG, AD4692_STATE_RESET_ALL);
			if (ret)
				return ret;

			break;
		case AD4692_AUTONOMOUS:
			ret = ad4692_std_seq_ch(ad4692, AD4692_CHANNEL_MASK(channel->ch_num), true, 0);
			if (ret)
				return ret;

			ret = ad4692_get_osc(ad4692, (enum ad4692_int_osc_sel *)&reg_val);
			if (ret)
				return ret;

			ret = ad4692_reg_write(ad4692, AD4692_CONV_START_REG,
					       AD4692_CONV_START);
			if (ret)
				return ret;

			no_os_udelay(NO_OS_DIV_ROUND_CLOSEST_ULL(MICRO,
					ad4692_iio_int_osc_sel[reg_val]));

			ret = ad4692_reg_write(ad4692, AD4692_CONV_START_REG,
					       AD4692_CONV_STOP);
			if (ret)
				return ret;

			ret = ad4692_get_ch(ad4692, channel->ch_num, &reg_val);
			if (ret)
				return ret;

			ret = ad4692_reg_write(ad4692, AD4692_STATE_RESET_REG, AD4692_STATE_RESET_ALL);
			if (ret)
				return ret;

			break;
		case AD4692_SPI_BURST:
			ret = ad4692_std_seq_ch(ad4692, AD4692_CHANNEL_MASK(channel->ch_num), true, 0);
			if (ret)
				return ret;

			ret = ad4692_get_osc(ad4692, (enum ad4692_int_osc_sel *)&reg_val);
			if (ret)
				return ret;

			ret = ad4692_reg_write(ad4692, AD4692_CONV_START_REG, AD4692_CONV_START);
			if (ret)
				return ret;

			no_os_udelay(NO_OS_DIV_ROUND_CLOSEST_ULL(MICRO,
					ad4692_iio_int_osc_sel[reg_val]));

			ret = ad4692_get_ch(ad4692, channel->ch_num, &reg_val);
			if (ret)
				return ret;

			ret = ad4692_reg_write(ad4692, AD4692_STATE_RESET_REG, AD4692_STATE_RESET_ALL);
			if (ret)
				return ret;

			break;
		case AD4692_MANUAL_MODE:
			ret = ad4692_get_ch(ad4692, channel->ch_num, &reg_val);
			if (ret)
				return ret;

			break;
		default:
			return -EINVAL;
		}

		return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&reg_val);
	default:
		return -EINVAL;
	}
}

static int ad4692_iio_read_scale(void *desc, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad4692_iio_desc *iio_ad4692;
	struct ad4692_desc *ad4692;
	uint32_t vals[2];

	if (!desc)
		return -EINVAL;

	iio_ad4692 = desc;

	if (!iio_ad4692->ad4692_desc)
		return -EINVAL;

	ad4692 = iio_ad4692->ad4692_desc;

	switch (channel->type) {
	case IIO_VOLTAGE:
		vals[0] = NO_OS_DIV_ROUND_CLOSEST(ad4692->vref, MILLIVOLT_PER_VOLT);
		vals[1] = ad4692->resolution;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, (int32_t *)vals);
	default:
		return -EINVAL;
	}
}

static int ad4692_iio_read_freq(void *desc, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ad4692_iio_desc *iio_ad4692;
	struct ad4692_desc *ad4692;
	uint32_t reg_val, freq;
	int ret;

	if (!desc)
		return -EINVAL;

	iio_ad4692 = desc;

	if (!iio_ad4692->ad4692_desc)
		return -EINVAL;

	ad4692 = iio_ad4692->ad4692_desc;

	switch (channel->type) {
	case IIO_VOLTAGE:
		switch (ad4692->mode) {
		case AD4692_MANUAL_MODE:
			freq = (uint32_t)ad4692->max_rate;

			return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&freq);
		case AD4692_CNV_CLOCK:
			ret = no_os_pwm_get_period(ad4692->conv_desc, &freq);
			if (ret)
				return ret;

			freq = NO_OS_DIV_ROUND_CLOSEST_ULL(NANO, freq);

			return iio_format_value(buf, len, IIO_VAL_INT, 1,
						(int32_t *)&freq);
		case AD4692_CNV_BURST:
		case AD4692_AUTONOMOUS:
		case AD4692_SPI_BURST:
			ret = ad4692_get_osc(ad4692, (enum ad4692_int_osc_sel *)&reg_val);
			if (ret)
				return ret;

			return iio_format_value(buf, len, IIO_VAL_INT, 1,
						(int32_t *)&ad4692_iio_int_osc_sel[reg_val]);
		default:
			return -EINVAL;
		}
	default:
		return -EINVAL;
	}
}
static int ad4692_iio_write_freq(void *desc, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad4692_iio_desc *iio_ad4692;
	struct ad4692_desc *ad4692;
	uint32_t i;
	int32_t val;
	int ret;

	if (!desc)
		return -EINVAL;

	iio_ad4692 = desc;

	if (!iio_ad4692->ad4692_desc)
		return -EINVAL;

	ad4692 = iio_ad4692->ad4692_desc;

	switch (channel->type) {
	case IIO_VOLTAGE:
		switch (ad4692->mode) {
		case AD4692_MANUAL_MODE: /* Manual Mode sampling rate depends on SPI frequency. */
			return -EOPNOTSUPP;
		case AD4692_CNV_CLOCK:
			iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
			if (val > ad4692->max_rate)
				return -EINVAL;

			return no_os_pwm_set_period(ad4692->conv_desc, NO_OS_DIV_ROUND_CLOSEST_ULL(NANO,
						    val));
		case AD4692_CNV_BURST:
			iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

			for (i = 0; i < NO_OS_ARRAY_SIZE(ad4692_iio_int_osc_sel); i++) {
				if (val == ad4692_iio_int_osc_sel[i])
					break;
			}

			if (i == NO_OS_ARRAY_SIZE(ad4692_iio_int_osc_sel))
				return -EINVAL;

			ret = ad4692_set_osc(ad4692, (enum ad4692_int_osc_sel)i);
			if (ret)
				return ret;

			return no_os_pwm_set_period(ad4692->conv_desc,
						    16 * NO_OS_DIV_ROUND_CLOSEST_ULL(NANO,
								    val));
		case AD4692_AUTONOMOUS:
		case AD4692_SPI_BURST:
			iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

			for (i = 0; i < NO_OS_ARRAY_SIZE(ad4692_iio_int_osc_sel); i++) {
				if (val == ad4692_iio_int_osc_sel[i])
					break;
			}

			if (i == NO_OS_ARRAY_SIZE(ad4692_iio_int_osc_sel))
				return -EINVAL;

			return ad4692_set_osc(ad4692, (enum ad4692_int_osc_sel)i);
		default:
			return -EINVAL;
		}
	default:
		return -EINVAL;
	}
}

static int ad4692_iio_read_freq_avail(void *desc, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	uint32_t avail_size = NO_OS_ARRAY_SIZE(ad4692_iio_int_osc_sel);
	uint32_t length = 0;
	uint32_t i;

	for (i = 0; i < avail_size; i++)
		length += sprintf(buf + length, "%d ", ad4692_iio_int_osc_sel[i]);

	return length;
}

static int ad4692_iio_update_channels(void *desc, uint32_t mask)
{
	struct ad4692_iio_desc *iio_ad4692;
	int ret;

	if (!desc)
		return -EINVAL;

	iio_ad4692 = desc;

	iio_ad4692->active_channels = mask;

	if (mask > NO_OS_BIT(iio_ad4692->iio_dev->num_ch))
		return -EINVAL;

	switch (iio_ad4692->ad4692_desc->mode) {
	case AD4692_CNV_CLOCK:
	case AD4692_CNV_BURST:
	case AD4692_AUTONOMOUS:
	case AD4692_SPI_BURST:
		ret = ad4692_std_seq_ch(iio_ad4692->ad4692_desc, mask, true, 0);
		if (ret)
			return ret;

		ret = ad4692_reg_write(iio_ad4692->ad4692_desc, AD4692_ACC_MASK1_REG,
				       no_os_field_get(AD4692_ACC_MASK1_MASK, mask));
		if (ret)
			return ret;

		ret = ad4692_reg_write(iio_ad4692->ad4692_desc, AD4692_ACC_MASK2_REG,
				       no_os_field_get(AD4692_ACC_MASK2_MASK, mask));
		if (ret)
			return ret;

		break;
	default:
		return -EINVAL;
	}

	iio_ad4692->no_of_active_channels = no_os_hweight32(mask);

	return 0;
}

static int ad4692_iio_post_disable(void *desc)
{
	struct ad4692_iio_desc *iio_ad4692;
	int ret;

	if (!desc)
		return -EINVAL;

	iio_ad4692 = desc;

	switch (iio_ad4692->ad4692_desc->mode) {
	case AD4692_CNV_CLOCK:
	case AD4692_CNV_BURST:
		ret = ad4692_sampling_enable(iio_ad4692->ad4692_desc, false);
		if (ret)
			return ret;

		ret = ad4692_reg_write(iio_ad4692->ad4692_desc,
				       AD4692_STD_SEQ_CONFIG,
				       AD4692_SEQ_ALL_CHANNELS_OFF);
		if (ret)
			return ret;

		break;
	case AD4692_AUTONOMOUS:
		ret = ad4692_reg_write(iio_ad4692->ad4692_desc,
				       AD4692_CONV_START_REG,
				       AD4692_CONV_STOP);
		if (ret)
			return ret;

		break;
	case AD4692_SPI_BURST:
		ret = ad4692_reg_write(iio_ad4692->ad4692_desc,
				       AD4692_STD_SEQ_CONFIG,
				       AD4692_SEQ_ALL_CHANNELS_OFF);
		if (ret)
			return ret;

		break;
	default:
		return -EINVAL;
	}

	return ad4692_reg_write(iio_ad4692->ad4692_desc, AD4692_STATE_RESET_REG,
				AD4692_STATE_RESET_ALL);
}

static int32_t ad4692_trigger_handler(struct iio_device_data *desc_data)
{
	uint32_t data_buff[desc_data->buffer->samples];
	struct ad4692_iio_desc *iio_ad4692;
	struct ad4692_desc *ad4692;
	int ret, i, j;

	if (!desc_data)
		return -EINVAL;

	iio_ad4692 = desc_data->dev;

	if (!iio_ad4692->ad4692_desc)
		return -EINVAL;

	ad4692 = iio_ad4692->ad4692_desc;

	/* STOP Conversion if needed. */
	switch (ad4692->mode) {
	case AD4692_CNV_BURST:
	case AD4692_CNV_CLOCK:
		ad4692_sampling_enable(ad4692, false);
		break;
	case AD4692_AUTONOMOUS:
		ad4692_reg_write(ad4692, AD4692_CONV_START_REG,
				 AD4692_CONV_STOP);
		break;
	case AD4692_SPI_BURST: /* CONV_START bit clears itself. */
		break;
	default:
		return -EINVAL;
	}

	j = 0;
	for (i = 0; i < iio_ad4692->no_of_active_channels; i++) {
		if (iio_ad4692->active_channels & NO_OS_BIT(i)) {
			ret = ad4692_get_ch(ad4692, i, (uint32_t *)&data_buff[j]);
			if (ret)
				return ret;
			j++;
		}
	}

	iio_buffer_push_scan(desc_data->buffer, &data_buff[0]);

	ret = ad4692_reg_write(ad4692, AD4692_STATE_RESET_REG, AD4692_STATE_RESET_ALL);
	if (ret)
		return ret;

	switch (ad4692->mode) {
	case AD4692_CNV_CLOCK:
	case AD4692_CNV_BURST:
		return ad4692_sampling_enable(ad4692, true);
	case AD4692_AUTONOMOUS:
	case AD4692_SPI_BURST:
		return ad4692_reg_write(ad4692, AD4692_CONV_START_REG, AD4692_CONV_START);
	default:
		return -EINVAL;
	}
}

static int ad4692_iio_read_samples(void *desc, int *buff, uint32_t samples)
{
	struct ad4692_iio_desc *iio_ad4692;
	struct ad4692_desc *ad4692;
	int ret, i, j;

	if (!desc)
		return -EINVAL;

	iio_ad4692 = desc;

	if (!iio_ad4692->ad4692_desc)
		return -EINVAL;

	ad4692 = iio_ad4692->ad4692_desc;

	if (ad4692->mode != AD4692_MANUAL_MODE)
		return -EOPNOTSUPP;

	i = no_os_find_first_set_bit(iio_ad4692->active_channels);
	j = 0;

	ret = ad4692_send_command(ad4692, AD4692_IN_COMMAND(i), (uint32_t *)&buff[j]);
	if (ret)
		return ret;

	ret = ad4692_sampling_enable(ad4692, true);
	if (ret)
		return ret;

	ret = ad4692_sampling_enable(ad4692, false);
	if (ret)
		return ret;

	i++;
	while (i <= no_os_find_last_set_bit(iio_ad4692->active_channels)) {
		if (iio_ad4692->active_channels & NO_OS_BIT(i)) {
			if (j == samples)
				return -EINVAL;

			ret = ad4692_send_command(ad4692, AD4692_IN_COMMAND(i), (uint32_t *)&buff[j]);
			if (ret)
				return ret;

			ret = ad4692_sampling_enable(ad4692, true);
			if (ret)
				return ret;

			ret = ad4692_sampling_enable(ad4692, false);
			if (ret)
				return ret;

			j++;
		}
		i++;
	}

	ret = ad4692_send_command(ad4692, AD4692_NOOP, (uint32_t *)&buff[j]);
	if (ret)
		return ret;

	ret = ad4692_sampling_enable(ad4692, true);
	if (ret)
		return ret;

	ret = ad4692_sampling_enable(ad4692, false);
	if (ret)
		return ret;

	return samples;
}

int ad4692_iio_init(struct ad4692_iio_desc **iio_desc,
		    struct ad4692_iio_desc_init_param *init_param)
{
	struct ad4692_iio_desc *descriptor;
	int ret;

	descriptor = (struct ad4692_iio_desc *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->iio_dev = &ad4692_iio_dev[init_param->ad4692_init_param->id];

	ret = ad4692_init(&descriptor->ad4692_desc, init_param->ad4692_init_param);
	if (ret)
		goto error_ad4692_init;

	*iio_desc = descriptor;

	return 0;

error_ad4692_init:
	no_os_free(descriptor);
	return ret;
}

int ad4692_iio_remove(struct ad4692_iio_desc *iio_desc)
{
	int ret;

	ret = ad4692_remove(iio_desc->ad4692_desc);
	if (ret)
		return ret;

	no_os_free(iio_desc);

	return 0;
}
