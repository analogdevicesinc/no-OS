/**************************************************************************//**
 *   @file   iio_ad4692.c
 *   @brief  Source file of ad4692 IIO driver
 *   @author Radu Sabau (radu.sabau@analog.com)
*******************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
static int ad4692_iio_read_samples(void *desc, uint32_t *buff,
				   uint32_t samples);
static int ad4692_iio_submit_buffer(struct iio_device_data *iio_dev_data);

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

static struct iio_attribute ad4692_iio_manual_attrs[] = {
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
		.shared = IIO_SHARED_BY_ALL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type ad4692_iio_scan_type = {
	.sign = 'u',
	.realbits = 16,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = true,
};

static struct scan_type ad4692_iio_manual_scan_type = {
	.sign = 'u',
	.realbits = 16,
	.storagebits = 32,
	.shift = 8,
	.is_big_endian = true,
};

#define AD4692_CHANNEL(index) {				\
	.ch_type = IIO_VOLTAGE,				\
	.channel = index,				\
	.address = AD4692_AVG_IN_REG(index),		\
	.indexed = true,				\
	.scan_type = &ad4692_iio_scan_type,		\
	.scan_index = index,				\
	.attributes = ad4692_iio_attrs,			\
	.ch_out = false					\
}

#define AD4692_MANUAL_CHANNEL(index) {			\
	.ch_type = IIO_VOLTAGE,				\
	.channel = index,				\
	.address = AD4692_IN_COMMAND(index),		\
	.indexed = true,				\
	.scan_type = &ad4692_iio_manual_scan_type,	\
	.scan_index = index,				\
	.attributes = ad4692_iio_manual_attrs,		\
	.ch_out = false					\
}

static struct iio_channel ad4692_channels[] = {
	AD4692_CHANNEL(0),
	AD4692_CHANNEL(1),
	AD4692_CHANNEL(2),
	AD4692_CHANNEL(3),
	AD4692_CHANNEL(4),
	AD4692_CHANNEL(5),
	AD4692_CHANNEL(6),
	AD4692_CHANNEL(7),
	AD4692_CHANNEL(8),
	AD4692_CHANNEL(9),
	AD4692_CHANNEL(10),
	AD4692_CHANNEL(11),
	AD4692_CHANNEL(12),
	AD4692_CHANNEL(13),
	AD4692_CHANNEL(14),
	AD4692_CHANNEL(15),
};

static struct iio_channel ad4693_channels[] = {
	AD4692_CHANNEL(0),
	AD4692_CHANNEL(1),
	AD4692_CHANNEL(2),
	AD4692_CHANNEL(3),
	AD4692_CHANNEL(4),
	AD4692_CHANNEL(5),
	AD4692_CHANNEL(6),
	AD4692_CHANNEL(7),
};

static struct iio_channel ad4692_manual_channels[] = {
	AD4692_MANUAL_CHANNEL(0),
	AD4692_MANUAL_CHANNEL(1),
	AD4692_MANUAL_CHANNEL(2),
	AD4692_MANUAL_CHANNEL(3),
	AD4692_MANUAL_CHANNEL(4),
	AD4692_MANUAL_CHANNEL(5),
	AD4692_MANUAL_CHANNEL(6),
	AD4692_MANUAL_CHANNEL(7),
	AD4692_MANUAL_CHANNEL(8),
	AD4692_MANUAL_CHANNEL(9),
	AD4692_MANUAL_CHANNEL(10),
	AD4692_MANUAL_CHANNEL(11),
	AD4692_MANUAL_CHANNEL(12),
	AD4692_MANUAL_CHANNEL(13),
	AD4692_MANUAL_CHANNEL(14),
	AD4692_MANUAL_CHANNEL(15),
};

static struct iio_channel ad4693_manual_channels[] = {
	AD4692_MANUAL_CHANNEL(0),
	AD4692_MANUAL_CHANNEL(1),
	AD4692_MANUAL_CHANNEL(2),
	AD4692_MANUAL_CHANNEL(3),
	AD4692_MANUAL_CHANNEL(4),
	AD4692_MANUAL_CHANNEL(5),
	AD4692_MANUAL_CHANNEL(6),
	AD4692_MANUAL_CHANNEL(7),
};

static struct iio_device ad4692_iio_dev = {
	.pre_enable = (int32_t (*)())ad4692_iio_update_channels,
	.post_disable = (int32_t (*)())ad4692_iio_post_disable,
	.read_dev = (int32_t (*)())ad4692_iio_read_samples,
	.debug_reg_read = (int32_t (*)())ad4692_iio_reg_read,
	.debug_reg_write = (int32_t (*)())ad4692_iio_reg_write,
};

static void ad4692_iio_setup_channels(struct ad4692_iio_desc *desc)
{
	struct ad4692_desc *ad4692 = desc->ad4692_desc;

	if (ad4692->mode == AD4692_MANUAL_MODE) {
		if (ad4692->num_channels == 8) {
			desc->iio_dev->channels = ad4693_manual_channels;
		} else {
			desc->iio_dev->channels = ad4692_manual_channels;
		}
	} else {
		if (ad4692->num_channels == 8) {
			desc->iio_dev->channels = ad4693_channels;
		} else {
			desc->iio_dev->channels = ad4692_channels;
		}
	}

	desc->iio_dev->num_ch = ad4692->num_channels;
}


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
		ret = ad4692_get_ch(ad4692, channel->ch_num, &reg_val);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1,
					(int32_t *)&reg_val);
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
		vals[1] = 16;
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
						(int32_t *)&ad4692_int_osc_val[reg_val]);
		case AD4692_MANUAL_MODE:
			/*
			 * In MANUAL MODE, each channel requires a 24-bit SPI transfer,
			 * and we need N+1 transfers for N channels.
			 * If no channels active yet, assume single channel.
			 */
			freq = ad4692->comm_desc->max_speed_hz /
			       (24 * ((iio_ad4692->no_of_active_channels ?
				       iio_ad4692->no_of_active_channels : 1) + 1));

			return iio_format_value(buf, len, IIO_VAL_INT, 1,
						(int32_t *)&freq);
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
			iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
			if (val > ad4692->max_rate)
				return -EINVAL;

			val = NO_OS_DIV_ROUND_CLOSEST_ULL(NANO, val);

			return no_os_pwm_set_period(ad4692->conv_desc, val);
		case AD4692_CNV_BURST:
		case AD4692_AUTONOMOUS:
		case AD4692_SPI_BURST:
			iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

			for (i = 0; i < NO_OS_ARRAY_SIZE(ad4692_int_osc_val); i++) {
				if (val == ad4692_int_osc_val[i])
					break;
			}

			if (i == NO_OS_ARRAY_SIZE(ad4692_int_osc_val))
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
	uint32_t avail_size = NO_OS_ARRAY_SIZE(ad4692_int_osc_val);
	uint32_t length = 0;
	uint32_t i;

	for (i = 0; i < avail_size; i++)
		length += sprintf(buf + length, "%d ", ad4692_int_osc_val[i]);

	return length;
}

static int ad4692_iio_update_channels(void *desc, uint32_t mask)
{
	struct ad4692_iio_desc *iio_ad4692;
	uint32_t min_period_ns;
	int ret, i;

	if (!desc)
		return -EINVAL;

	iio_ad4692 = desc;

	iio_ad4692->active_channels = mask;
	iio_ad4692->no_of_active_channels = no_os_hweight16(mask);

	if (iio_ad4692->ad4692_desc->mode != AD4692_MANUAL_MODE) {
		ret = ad4692_reg_write(iio_ad4692->ad4692_desc,
				       AD4692_STATE_RESET_REG,
				       AD4692_STATE_RESET_ALL);
		if (ret)
			return ret;

		ret = ad4692_std_seq_ch(iio_ad4692->ad4692_desc, mask, true, 0);
		if (ret)
			return ret;

		ret = ad4692_reg_write(iio_ad4692->ad4692_desc, AD4692_ACC_MASK1_REG,
				       no_os_field_get(AD4692_ACC_MASK1_MASK, ~mask));
		if (ret)
			return ret;

		ret = ad4692_reg_write(iio_ad4692->ad4692_desc, AD4692_ACC_MASK2_REG,
				       no_os_field_get(AD4692_ACC_MASK2_MASK, ~mask));
		if (ret)
			return ret;

		for (i = 0; i < iio_ad4692->ad4692_desc->num_channels; i++) {
			if (mask & NO_OS_BIT(i)) {
				ret = ad4692_reg_write(iio_ad4692->ad4692_desc, AD4692_ACC_COUNT_LIMIT_REG(i),
						       iio_ad4692->no_of_active_channels);
				if (ret)
					return ret;
			} else {
				ret = ad4692_reg_write(iio_ad4692->ad4692_desc, AD4692_ACC_COUNT_LIMIT_REG(i),
						       0);
				if (ret)
					return ret;
			}
		}
	}

	switch (iio_ad4692->ad4692_desc->mode) {
	case AD4692_MANUAL_MODE:
		/* Manual mode uses submit_buffer for multi-channel reads */
		return 0;
	case AD4692_CNV_BURST:
		ret = ad4692_cnv_burst_period_ns(iio_ad4692->ad4692_desc,
						 iio_ad4692->no_of_active_channels, &min_period_ns);
		if (ret)
			return ret;

		ret = no_os_pwm_set_period(iio_ad4692->ad4692_desc->conv_desc,
					   min_period_ns);
		if (ret)
			return ret;

		return ad4692_sampling_enable(iio_ad4692->ad4692_desc, true);
	case AD4692_CNV_CLOCK:
		return ad4692_sampling_enable(iio_ad4692->ad4692_desc,
					      true);
	case AD4692_AUTONOMOUS:
	case AD4692_SPI_BURST:
		return ad4692_sampling_enable(iio_ad4692->ad4692_desc, true);
	default:
		return -EINVAL;
	}
}

static int ad4692_iio_post_disable(void *desc)
{
	struct ad4692_iio_desc *iio_ad4692;
	uint32_t reg_val;
	int ret;

	if (!desc)
		return -EINVAL;

	iio_ad4692 = desc;

	if (iio_ad4692->ad4692_desc->mode != AD4692_MANUAL_MODE) {
		ret = ad4692_sampling_enable(iio_ad4692->ad4692_desc, false);
		if (ret)
			return ret;

		ret = ad4692_reg_write(iio_ad4692->ad4692_desc,
				       AD4692_STD_SEQ_CONFIG_REG,
				       AD4692_SEQ_ALL_CHANNELS_OFF);
		if (ret)
			return ret;

		return ad4692_reg_write(iio_ad4692->ad4692_desc,
					AD4692_STATE_RESET_REG,
					AD4692_STATE_RESET_ALL);
	}

	/* Manual mode: just send a NOOP to complete any pending transfer */
	return ad4692_send_command(iio_ad4692->ad4692_desc, AD4692_NOOP, &reg_val);
}

static int32_t ad4692_trigger_handler(struct iio_device_data *desc_data)
{
	uint32_t data_buff[desc_data->buffer->samples];
	struct ad4692_iio_desc *iio_ad4692;
	struct ad4692_desc *ad4692;
	int ret, i, j;
	uint32_t reg;

	if (!desc_data)
		return -EINVAL;

	iio_ad4692 = desc_data->dev;

	if (!iio_ad4692->ad4692_desc)
		return -EINVAL;

	ad4692 = iio_ad4692->ad4692_desc;

	ad4692_sampling_enable(ad4692, false);
	j = 0;
	for (i = 0; i < ad4692->num_channels; i++) {
		if (iio_ad4692->active_channels & NO_OS_BIT(i)) {
			reg = iio_ad4692->iio_dev->channels[i].address;
			ret = ad4692_reg_read(ad4692, reg,
					      &data_buff[j]);
			if (ret)
				return ret;

			iio_buffer_push_scan(desc_data->buffer,
					     &data_buff[j]);
			j++;
		}
	}

	ret = ad4692_reg_write(ad4692, AD4692_STATE_RESET_REG,
			       AD4692_STATE_RESET_ALL);
	if (ret)
		return ret;

	return ad4692_sampling_enable(ad4692, true);
}

static int ad4692_iio_read_samples(void *desc, uint32_t *buff, uint32_t samples)
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

	i = no_os_find_first_set_bit(iio_ad4692->active_channels);
	for (j = 0; j < (int)samples; j++) {
		ret = ad4692_get_ch(ad4692, i, &buff[j]);
		if (ret)
			return ret;

		i++;
		while (!(iio_ad4692->active_channels & NO_OS_BIT(i))) {
			if (i >= iio_ad4692->ad4692_desc->num_channels)
				return -EIO;

			i++;
		}
	}

	return samples;
}

static int ad4692_iio_submit_buffer(struct iio_device_data *iio_dev_data)
{
	struct ad4692_iio_desc *iio_ad4692 = iio_dev_data->dev;
	struct ad4692_desc *ad4692;
	struct iio_buffer *buffer = iio_dev_data->buffer;
	uint32_t *push_data;
	uint32_t ch_val;
	uint32_t sample_idx = 0;
	uint32_t reg;
	int prev_chan, i;
	int ret;
	void *buff;

	if (!iio_ad4692->ad4692_desc)
		return -EINVAL;

	ad4692 = iio_ad4692->ad4692_desc;

	ret = iio_buffer_get_block(buffer, &buff);
	if (ret)
		return ret;

	push_data = buff;

	if (ad4692->mode == AD4692_MANUAL_MODE) {
		/*
		 * MANUAL_MODE with CNV tied to CS: each transfer triggers a
		 * conversion AND returns the previous conversion's result.
		 * First transfer returns garbage, so we do N+1 transfers for
		 * N channels.
		 */
		while (sample_idx < buffer->samples) {
			prev_chan = -1;

			/* Iterate through all active channels */
			for (i = 0; i < ad4692->num_channels; i++) {
				if (!(iio_ad4692->active_channels & NO_OS_BIT(i)))
					continue;

				ret = ad4692_send_command(ad4692,
							  AD4692_IN_COMMAND(i),
							  &ch_val);
				if (ret)
					return ret;

				/*
				 * ch_val holds the previous channel's conversion
				 * result. Skip only the first transfer (garbage).
				 */
				if (prev_chan >= 0) {
					push_data[sample_idx] = ch_val;
					sample_idx++;
					if (sample_idx >= buffer->samples)
						break;
				}
				prev_chan = i;
			}

			/* Final NOOP transfer to get last channel's data */
			if (sample_idx < buffer->samples && prev_chan >= 0) {
				ret = ad4692_send_command(ad4692, AD4692_NOOP,
							  &ch_val);
				if (ret)
					return ret;

				push_data[sample_idx] = ch_val;
				sample_idx++;
			}
		}
	} else {
		/*
		 * Non-MANUAL modes: read from accumulators.
		 * Sampling is started in pre_enable and stopped in post_disable.
		 */
		uint32_t delay_us, period_ns, osc_val;
		enum ad4692_int_osc_sel osc;

		/* Calculate sampling period based on mode */
		switch (ad4692->mode) {
		case AD4692_CNV_CLOCK:
			ret = no_os_pwm_get_period(ad4692->conv_desc, &period_ns);
			if (ret)
				return ret;
			delay_us = NO_OS_DIV_ROUND_UP(period_ns, 1000);
			break;
		case AD4692_CNV_BURST:
		case AD4692_AUTONOMOUS:
		case AD4692_SPI_BURST:
			ret = ad4692_get_osc(ad4692, &osc);
			if (ret)
				return ret;
			osc_val = ad4692_int_osc_val[osc];
			delay_us = NO_OS_DIV_ROUND_UP(MICRO, osc_val);
			break;
		default:
			return -EINVAL;
		}

		/* Delay = (num_active_channels + 1) * sampling_period */
		delay_us *= (iio_ad4692->no_of_active_channels + 1);

		while (sample_idx < buffer->samples) {
			/* Wait for all channels to be sampled */
			no_os_udelay(delay_us);

			ad4692_sampling_enable(ad4692, false);

			for (i = 0; i < ad4692->num_channels; i++) {
				if (!(iio_ad4692->active_channels & NO_OS_BIT(i)))
					continue;

				reg = iio_ad4692->iio_dev->channels[i].address;
				ret = ad4692_reg_read(ad4692, reg, &ch_val);
				if (ret)
					return ret;

				push_data[sample_idx] = ch_val;
				sample_idx++;

				if (sample_idx >= buffer->samples)
					break;
			}

			/* Reset and restart for next set of samples */
			ret = ad4692_reg_write(ad4692, AD4692_STATE_RESET_REG,
					       AD4692_STATE_RESET_ALL);
			if (ret)
				return ret;

			if (sample_idx < buffer->samples) {
				ret = ad4692_sampling_enable(ad4692, true);
				if (ret)
					return ret;
			}
		}
	}

	return iio_buffer_block_done(buffer);
}

int ad4692_iio_init(struct ad4692_iio_desc **iio_desc,
		    struct ad4692_iio_desc_init_param *init_param)
{
	struct ad4692_iio_desc *descriptor;
	struct iio_device *iio_dev;
	int ret;

	descriptor = (struct ad4692_iio_desc *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	/* Allocate a copy of the iio_device to avoid modifying the shared static */
	iio_dev = (struct iio_device *)no_os_calloc(1, sizeof(*iio_dev));
	if (!iio_dev) {
		ret = -ENOMEM;
		goto error;
	}

	memcpy(iio_dev, &ad4692_iio_dev, sizeof(*iio_dev));
	descriptor->iio_dev = iio_dev;

	ret = ad4692_init(&descriptor->ad4692_desc,
			  init_param->ad4692_init_param);
	if (ret)
		goto iio_dev_error;

	ad4692_iio_setup_channels(descriptor);

	/*
	 * submit_buffer is available for all modes (used when no trigger).
	 * trigger_handler is only available for non-MANUAL modes (used with
	 * DATA_READY GPIO interrupt trigger).
	 */
	descriptor->iio_dev->submit = (int32_t (*)())ad4692_iio_submit_buffer;
	if (init_param->ad4692_init_param->mode != AD4692_MANUAL_MODE)
		descriptor->iio_dev->trigger_handler = (int32_t (*)())ad4692_trigger_handler;
	else
		descriptor->iio_dev->trigger_handler = NULL;

	*iio_desc = descriptor;

	return 0;

iio_dev_error:
	no_os_free(iio_dev);
error:
	no_os_free(descriptor);
	return ret;
}

int ad4692_iio_remove(struct ad4692_iio_desc *iio_desc)
{
	int ret;

	ret = ad4692_remove(iio_desc->ad4692_desc);
	if (ret)
		return ret;

	no_os_free(iio_desc->iio_dev);
	no_os_free(iio_desc);

	return 0;
}
