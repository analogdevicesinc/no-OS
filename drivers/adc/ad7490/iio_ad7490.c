/**************************************************************************//**
*   @file   iio_ad7490.c
*   @brief  Source file of iio_ad7490
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
******************************************************************************/
#include "iio_ad7490.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

#define AD7490_V_FS_2V5		2500
#define AD7490_V_FS_5V		5000

#define AD7490_CHANNEL(_idx) {			\
	.ch_type = IIO_VOLTAGE,			\
	.channel = _idx,			\
	.indexed = true,			\
	.attributes = ad7490_iio_attrs,		\
	.address = _idx,			\
	.scan_type = &ad7490_iio_scan_type,	\
	.scan_index = _idx			\
}

static struct scan_type ad7490_iio_scan_type = {
	.sign = 'u',
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static const char *const ad7490_range_avail[2] = {
	[AD7490_RANGE_2XREFIN] = "2X_REF_IN",
	[AD7490_RANGE_REFIN] = "REF_IN",
};

static const char *const ad7490_polarity_avail[2] = {
	[AD7490_CODING_TWOS] = "BIPOLAR",
	[AD7490_CODING_BINARY] = "UNIPOLAR",
};

static int ad7490_read_samples(struct ad7490_desc *desc, int16_t *data,
			       uint32_t samples)
{
	uint32_t i, sample_size;
	int16_t channels_val[16];
	int ret;

	sample_size = desc->nb_channels_seq * sizeof(int16_t);

	for (i = 0; i < samples; i++) {
		ret = ad7490_read_seq(desc, data);
		if (ret)
			return ret;
		data += sample_size;
	}

	return 0;
}

static int ad7490_iio_read_raw(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct ad7490_iio_desc *iio_desc = device;
	int16_t val;
	int ret;

	ret = ad7490_read_ch(iio_desc->ad7490_desc, channel->address, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int ad7490_iio_read_scale(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad7490_iio_desc *iio_desc = device;
	int32_t vals[2];
	int16_t val;
	int ret;

	ret = ad7490_read_ch(iio_desc->ad7490_desc, channel->address, &val);
	if (ret)
		return ret;

	if (no_os_field_get(AD7490_RANGE_MASK,
			    iio_desc->ad7490_desc->cfg) == AD7490_RANGE_2XREFIN)
		vals[0] = AD7490_V_FS_5V;
	else
		vals[0] = AD7490_V_FS_2V5;

	vals[1] = ad7490_iio_scan_type.realbits;
	ret = ad7490_read_ch(iio_desc->ad7490_desc, channel->address, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
}

static int ad7490_iio_read_polarity(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ad7490_iio_desc *desc = device;

	return sprintf(buf, "%s ",
		       ad7490_polarity_avail[no_os_field_get(AD7490_CODING_MASK,
					     desc->ad7490_desc->cfg)]);
}

static int ad7490_iio_write_polarity(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad7490_iio_desc *desc = device;
	enum ad7490_range range;
	enum ad7490_dout_state dout_state;
	uint32_t i;
	int ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad7490_polarity_avail); i++)
		if (!strcmp(buf, ad7490_polarity_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(ad7490_polarity_avail))
		return -EINVAL;

	range = no_os_field_get(AD7490_RANGE_MASK, desc->ad7490_desc->cfg);
	dout_state = no_os_field_get(AD7490_WEAKTRI_MASK, desc->ad7490_desc->cfg);
	ret = ad7490_config(desc->ad7490_desc, dout_state, range, i);
	if (ret)
		return ret;

	desc->iio_dev->channels->scan_type->sign = (i == AD7490_CODING_BINARY) ? 'u' :
			's';

	return 0;
}

static int ad7490_iio_read_polarity_avail(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	uint32_t avail_size = NO_OS_ARRAY_SIZE(ad7490_polarity_avail);
	uint32_t length = 0;
	uint32_t i;

	for (i = 0; i < avail_size; i++)
		length += sprintf(buf + length, "%s ", ad7490_polarity_avail[i]);

	return length;
}

static int ad7490_iio_read_range(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad7490_iio_desc *desc = device;

	return sprintf(buf, "%s ", ad7490_range_avail[no_os_field_get(AD7490_RANGE_MASK,
			desc->ad7490_desc->cfg)]);
}

static int ad7490_iio_write_range(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ad7490_iio_desc *desc = device;
	enum ad7490_coding coding;
	enum ad7490_dout_state dout_state;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad7490_range_avail); i++)
		if (!strcmp(buf, ad7490_range_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(ad7490_range_avail))
		return -EINVAL;

	coding = no_os_field_get(AD7490_CODING_MASK, desc->ad7490_desc->cfg);
	dout_state = no_os_field_get(AD7490_WEAKTRI_MASK, desc->ad7490_desc->cfg);
	return ad7490_config(desc->ad7490_desc, dout_state, i, coding);
}

static int ad7490_iio_read_range_avail(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	uint32_t avail_size = NO_OS_ARRAY_SIZE(ad7490_range_avail);
	uint32_t length = 0;
	uint32_t i;

	for (i = 0; i < avail_size; i++)
		length += sprintf(buf + length, "%s ", ad7490_range_avail[i]);

	return length;
}

static int ad7490_iio_read_sampling_freq(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int32_t val;

	val = 1000000;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int ad7490_iio_pre_enable(void *device, uint32_t mask)
{
	struct ad7490_iio_desc *desc = device;

	desc->active_channels = mask;
	desc->no_of_active_channels = no_os_hweight16(mask);

	return ad7490_start_seq(desc->ad7490_desc, AD7490_SHADOW_SEQ, mask, 0);
}

static int ad7490_iio_post_disable(void *device, uint32_t mask)
{
	struct ad7490_iio_desc *desc = device;

	return ad7490_stop_seq(desc->ad7490_desc);
}

static int iio_ad7490_submit_buffer(struct iio_device_data *iio_dev_data)
{
	struct ad7490_iio_desc *iio_desc = iio_dev_data->dev;
	struct ad7490_desc *desc = iio_desc->ad7490_desc;
	struct iio_buffer *buffer = iio_dev_data->buffer;
	uint32_t total_samples;
	int16_t *push_data, channels_val[16] = {0};
	uint32_t i = 0, j;
	int ret;
	void *buff;

	ret = iio_buffer_get_block(buffer, &buff);
	if (ret)
		return ret;

	total_samples = iio_desc->no_of_active_channels * buffer->samples;

	push_data = buff;
	while (i < total_samples) {
		ret = ad7490_read_seq(desc, channels_val);
		if (ret)
			return ret;

		for (j = 0; j < iio_desc->no_of_active_channels; j++) {
			push_data[i] = channels_val[j];
			i++;
		}
	}

	return iio_buffer_block_done(buffer);
}

static int ad7490_iio_read_samples(void *device, int *buff, uint32_t samples)
{
	struct ad7490_iio_desc *desc = device;
	int16_t channels_val[16];
	uint32_t i = 0, j;
	int ret;

	while (i < samples * desc->no_of_active_channels) {
		ret = ad7490_read_seq(desc->ad7490_desc, channels_val);
		if (ret)
			return ret;

		for (j = 0; j < desc->no_of_active_channels; j++) {
			buff[i] = channels_val[j];
			i++;
		}
	}

	return samples;
}

static struct iio_attribute ad7490_iio_attrs[] = {
	{
		.name = "raw",
		.show = ad7490_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ad7490_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad7490_global_attrs[] = {
	{
		.name = "polarity",
		.show = ad7490_iio_read_polarity,
		.store = ad7490_iio_write_polarity,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "polarity_available",
		.show = ad7490_iio_read_polarity_avail,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "range",
		.show = ad7490_iio_read_range,
		.store = ad7490_iio_write_range,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "range_available",
		.show = ad7490_iio_read_range_avail,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "sampling_frequency",
		.show = ad7490_iio_read_sampling_freq,
		.shared = IIO_SHARED_BY_ALL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ad7490_channels[] = {
	AD7490_CHANNEL(0),
	AD7490_CHANNEL(1),
	AD7490_CHANNEL(2),
	AD7490_CHANNEL(3),
	AD7490_CHANNEL(4),
	AD7490_CHANNEL(5),
	AD7490_CHANNEL(6),
	AD7490_CHANNEL(7),
	AD7490_CHANNEL(8),
	AD7490_CHANNEL(9),
	AD7490_CHANNEL(10),
	AD7490_CHANNEL(11),
	AD7490_CHANNEL(12),
	AD7490_CHANNEL(13),
	AD7490_CHANNEL(14),
	AD7490_CHANNEL(15),
};

static struct iio_device ad7490_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ad7490_channels),
	.channels = ad7490_channels,
	.attributes = ad7490_global_attrs,
	.read_dev = (int32_t (*)())ad7490_iio_read_samples,
	.pre_enable = (int32_t (*)())ad7490_iio_pre_enable,
	.post_disable = (int32_t (*)())ad7490_iio_post_disable,
	.submit = (int32_t (*)())iio_ad7490_submit_buffer,
};

int ad7490_iio_init(struct ad7490_iio_desc **iio_desc,
		    struct ad7490_iio_init_param *init_param)
{
	struct ad7490_iio_desc *iio_descriptor;
	int ret;

	iio_descriptor = (struct ad7490_iio_desc *)no_os_calloc(1,
			 sizeof(*iio_descriptor));
	if (!iio_descriptor)
		return -ENOMEM;

	iio_descriptor->iio_dev = &ad7490_iio_dev;
	ret = ad7490_init(&iio_descriptor->ad7490_desc, init_param->ad7490_init);
	if (ret)
		goto free_desc;

	ret = ad7490_config(iio_descriptor->ad7490_desc, init_param->dout_state,
			    init_param->range, init_param->coding);
	if (ret)
		goto free_ad7490;

	iio_descriptor->iio_dev->channels->scan_type->sign = (init_param->coding ==
			AD7490_CODING_BINARY) ? 'u' : 's';

	*iio_desc = iio_descriptor;

	return 0;

free_ad7490:
	ad7490_remove(iio_descriptor->ad7490_desc);
free_desc:
	no_os_free(iio_descriptor);

	return 0;
}

int ad7490_iio_remove(struct ad7490_iio_desc *iio_desc)
{
	ad7490_remove(iio_desc->ad7490_desc);
	no_os_free(iio_desc);

	return 0;
}
