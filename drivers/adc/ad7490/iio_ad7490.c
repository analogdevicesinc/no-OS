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

#define AD7490_V_FS_3V3		3300
#define AD7490_V_FS_5V		5000

#define AD7490_CHANNEL(_idx) {			\
	.ch_type = IIO_VOLTAGE,			\
	.ch_out = IIO_DIRECTION_INPUT,		\
	.indexed = true,			\
	.channel = _idx,			\
	.address = _idx,			\
	.scan_index = _idx,			\
	.scan_type = &ad7490_iio_scan_type,	\
	.attributes = ad7490_iio_attrs,		\
}

static struct scan_type ad7490_iio_scan_type = {
	.sign = 's',
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static const char *const ad7490_range_avail[2] = {
	[AD7490_RANGE_2XREFIN] = "2 x REF_IN",
	[AD7490_RANGE_REFIN] = "REF_IN",
};

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

	if (no_os_field_get(AD7490_RANGE_MASK, iio_desc->ad7490_desc->cfg))
		vals[0] = AD7490_V_FS_5V;
	else
		vals[0] = AD7490_V_FS_3V3;

	vals[1] = ad7490_iio_scan_type.realbits;
	ret = ad7490_read_ch(iio_desc->ad7490_desc, channel->address, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
}

//TBD
static int ad7490_iio_read_sign(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{

}

//TBD
static int ad7490_iio_write_sign(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{

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
		length += sprintf(buf + length, "%d ", ad7490_range_avail[i]);

	return length;
}

static int ad7490_iio_update_channels(void *device, uint32_t mask)
{
	struct ad7490_iio_desc *desc = device;

	desc->active_channels = mask;
	desc->no_of_active_channels = no_os_hweight16(mask);

	return ad7490_start_seq(desc->ad7490_desc, AD7490_SHADOW_SEQ, mask, 0);
}

static int ad7490_iio_read_samples(void *device, int *buff, uint32_t samples)
{
	struct ad7490_iio_desc *desc = device;
	int16_t channels_val[16];
	uint32_t i, j, k;
	int ret;

	while (i < samples * desc->no_of_active_channels) {
		ret = ad7490_read_seq(desc->ad7490_desc, channels_val);
		if (ret)
			return ret;

		k = 0;
		for (j = 0; j < 16; j++) {
			if (desc->active_channels & NO_OS_BIT(j)) {
				buff[i] = channels_val[k];
				i++;
				k++;
			}
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
	{
		.name = "sign",
		.show = ad7490_iio_read_sign,
		.store = ad7490_iio_write_sign,
	},
	{
		.name = "range",
		.show = ad7490_iio_read_range,
		.store = ad7490_iio_write_range,
	},
	{
		.name = "range_available",
		.show = ad7490_iio_read_range_avail,
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
	.read_dev = (int32_t (*)())ad7490_iio_read_samples,
	.pre_enable = (int32_t (*)())ad7490_iio_update_channels,
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

	iio_descriptor->iio_dev->channels->scan_type->sign = init_param->coding;

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
