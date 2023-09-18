/**************************************************************************//**
*   @file   iio_adm1177.c
*   @brief  Source file of iio_adm1177
*   @author Radu Sabau (radu.sabau@analog.com)
*******************************************************************************
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
******************************************************************************/
#include "iio_adm1177.h"
#include "adm1177.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

static struct scan_type adm1177_iio_scan_type = {
	.sign = 'u',
	.realbits = 12,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false,
};

static int adm1177_iio_read_raw(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct adm1177_iio_dev *iiodev = device;
	struct adm1177_dev *desc = (struct adm1177_dev *)iiodev->adm1177_dev;
	int32_t ret, value;
	uint16_t conv_voltage;
	uint16_t conv_current;

	switch (channel->type) {
	case IIO_VOLTAGE:
		ret = adm1177_read_conv(desc, &conv_voltage, &conv_current);
		if (ret)
			return ret;

		value = conv_voltage;
		break;
	case IIO_CURRENT:
		ret = adm1177_read_conv(desc, &conv_voltage, &conv_current);
		if (ret)
			return ret;

		value = conv_current;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}

static int adm1177_iio_read_scale(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int32_t vals[2];
	struct adm1177_iio_dev *iio_adm1177;
	struct adm1177_dev *adm1177;

	if (!device)
		return -EINVAL;

	iio_adm1177 = device;

	if (!iio_adm1177->adm1177_dev)
		return -EINVAL;

	adm1177 = iio_adm1177->adm1177_dev;

	switch (channel->type) {
	case IIO_VOLTAGE:
		if (adm1177->vrange == ADM1177_VRANGE_LOW)
			vals[0] = ADM1177_V_FULLSCALE_0;
		else
			vals[0] = ADM1177_V_FULLSCALE_1;
		vals[1] = adm1177_iio_scan_type.realbits;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2,
					vals);
	case IIO_CURRENT:
		vals[0] = ADM1177_I_FULLSCALE;
		vals[1] = ADM1177_RESOLUTION * ADM1177_R_SENSE;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2,
					vals);
	default:
		return -EINVAL;
	}
}

static int adm1177_iio_update_channels(void *device, uint32_t mask)
{
	struct adm1177_iio_dev *iio_adm1177;

	if (!device)
		return -EINVAL;

	iio_adm1177 = device;

	iio_adm1177->active_channels = mask;

	iio_adm1177->no_of_active_channels = no_os_hweight8(mask);

	return 0;
}

static int adm1177_iio_read_samples(void *device, int *buff, uint32_t samples)
{
	uint16_t conv_voltage, conv_current;
	uint32_t raw_voltage, raw_current;
	uint32_t i = 0;
	int ret;
	struct adm1177_iio_dev *iio_adm1177;
	struct adm1177_dev *adm1177;

	if (!device)
		return -EINVAL;

	iio_adm1177 = device;

	if (!iio_adm1177->adm1177_dev)
		return -EINVAL;

	adm1177 = iio_adm1177->adm1177_dev;

	while (i < samples * iio_adm1177->no_of_active_channels) {
		ret = adm1177_read_conv(adm1177, &conv_voltage, &conv_current);
		if(ret)
			return ret;
		raw_voltage = conv_voltage;
		raw_current = conv_current;

		if(iio_adm1177->active_channels & NO_OS_BIT(0)) {
			buff[i] = raw_voltage;
			i++;
		}
		if(iio_adm1177->active_channels & NO_OS_BIT(1)) {
			buff[i] = raw_current;
			i++;
		}
	}

	return samples;
}

static struct iio_attribute adm1177_iio_attrs[] = {
	{
		.name = "raw",
		.show = adm1177_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adm1177_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

enum adm1177_chan {
	ADM1177_VOLTAGE,
	ADM1177_CURRENT,
};

static struct iio_channel adm1177_channels[] = {
	[ADM1177_VOLTAGE] = {
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.indexed = true,
		.attributes = adm1177_iio_attrs,
		.address = ADM1177_VOLTAGE_EN,
		.scan_type = &adm1177_iio_scan_type,
		.scan_index = 0,
		.ch_out = false
	},
	[ADM1177_CURRENT] = {
		.ch_type = IIO_CURRENT,
		.channel = 0,
		.indexed = true,
		.attributes = adm1177_iio_attrs,
		.address = ADM1177_CURRENT_EN,
		.scan_type = &adm1177_iio_scan_type,
		.scan_index = 1,
		.ch_out = false
	}
};

static struct iio_device adm1177_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adm1177_channels),
	.channels = adm1177_channels,
	.read_dev = (int32_t (*)())adm1177_iio_read_samples,
	.pre_enable = (int32_t (*)())adm1177_iio_update_channels,
};

int adm1177_iio_init(struct adm1177_iio_dev **iio_dev,
		     struct adm1177_iio_init_param *init_param)
{
	int ret;
	struct adm1177_iio_dev *desc;

	desc = (struct adm1177_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adm1177_iio_dev;

	ret = adm1177_init(&desc->adm1177_dev, init_param->adm1177_initial);
	if (ret)
		goto error_desc;

	ret = adm1177_write(desc->adm1177_dev, ADM1177_CMD_I_CONT |
			    ADM1177_CMD_V_CONT, 0);
	if (ret)
		goto error_desc;

	*iio_dev = desc;

	return 0;
error_desc:
	no_os_free(desc);

	return ret;
}

int adm1177_iio_remove(struct adm1177_iio_dev *desc)
{
	int ret;

	ret = adm1177_remove(desc->adm1177_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
