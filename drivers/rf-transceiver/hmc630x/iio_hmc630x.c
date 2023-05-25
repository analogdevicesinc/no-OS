/***************************************************************************//**
*   @file   iio_hmc630x.c
*   @brief  Source file of hmc6300 and hmc6301 driver extension for IIOD.
*   @author Darius Berghe (darius.berghe@analog.com)
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
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "no_os_error.h"
#include "no_os_delay.h"
#include "iio.h"
#include "iio_hmc630x.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "hmc630x.h"
#include <string.h>

static int32_t _hmc630x_read_register2(struct hmc630x_iio_dev *iiodev,
				       uint32_t reg,
				       uint32_t *readval)
{
	int ret;
	uint8_t val;
	ret = hmc630x_read_row(iiodev->dev, reg, &val);
	if (ret)
		return ret;

	*readval = val;
	return 0;
}

static int32_t _hmc630x_write_register2(struct hmc630x_iio_dev *iiodev,
					uint32_t reg,
					uint32_t writeval)
{
	return hmc630x_write_row(iiodev->dev, reg, (uint8_t)writeval);
}

static int hmc630x_iio_read_attr(void *device, char *buf,
				 uint32_t len, const struct iio_ch_info *channel,
				 intptr_t priv)
{
	int ret;
	struct hmc630x_iio_dev *iiodev = (struct hmc630x_iio_dev *)device;
	struct hmc630x_dev *d = iiodev->dev;
	int32_t val;
	bool enable;
	uint64_t vco;
	uint8_t attn;
	uint8_t band;
	uint8_t lock;
	enum hmc6301_lna_attn lna_attn;
	enum hmc6301_bb_attn attn1, attn2;
	enum hmc6301_bb_attn_fine attni, attnq;
	enum hmc6301_bb_lpc lpc;
	enum hmc6301_bb_hpc hpc;

	switch (priv) {
	case HMC630X_IIO_ATTR_ENABLED:
		ret = hmc630x_get_enable(d, &enable);
		if (ret)
			return ret;

		val = enable;
		break;
	case HMC630X_IIO_ATTR_VCO:
		ret = hmc630x_get_vco(d, &vco);
		if (ret)
			return ret;

		val = vco / 1000;
		break;
	case HMC630X_IIO_ATTR_VCO_BAND:
		ret = hmc630x_read(d, HMC630X_VCO_BANDSEL, &band);
		if (ret)
			return ret;

		val = band;
		break;
	case HMC630X_IIO_ATTR_VCO_LOCK:
		ret = hmc630x_read(d, HMC630X_LOCKDET, &lock);
		if (ret)
			return ret;

		val = lock;
		break;
	case HMC630X_IIO_ATTR_IF_ATTN:
		ret = hmc630x_get_if_attn(d, &attn);
		if (ret)
			return ret;

		val = attn;
		break;
	case HMC630X_IIO_ATTR_TEMP_EN:
		ret = hmc630x_get_temp_en(d, &enable);
		if (ret)
			return ret;

		val = enable;
		break;
	case HMC6300_IIO_ATTR_RF_ATTN:
		ret = hmc6300_get_rf_attn(d, &attn);
		if (ret)
			return ret;

		val = attn;
		break;
	case HMC6301_IIO_ATTR_RF_LNA_GAIN:
		ret = hmc6301_get_lna_gain(d, &lna_attn);
		if (ret)
			return ret;

		val = lna_attn;
		break;
	case HMC6301_IIO_ATTR_BB_ATTN1:
	case HMC6301_IIO_ATTR_BB_ATTN2:
		ret = hmc6301_get_bb_attn(d, &attn1, &attn2);
		if (ret)
			return ret;

		if (priv == HMC6301_IIO_ATTR_BB_ATTN1)
			val = attn1;
		else
			val = attn2;
		break;
	case HMC6301_IIO_ATTR_BB_ATTNI_FINE:
	case HMC6301_IIO_ATTR_BB_ATTNQ_FINE:
		ret = hmc6301_get_bb_attn_fine(d, &attni, &attnq);
		if (ret)
			return ret;

		if (priv == HMC6301_IIO_ATTR_BB_ATTNI_FINE)
			val = attni;
		else
			val = attnq;
		break;
	case HMC6301_IIO_ATTR_BB_LPC:
	case HMC6301_IIO_ATTR_BB_HPC:
		ret = hmc6301_get_bb_lpc_hpc(d, &lpc, &hpc);
		if (ret)
			return ret;

		if (priv == HMC6301_IIO_ATTR_BB_LPC)
			val = lpc;
		else
			val = hpc;
		break;
	default:
		return -EINVAL;
	};

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

static int hmc630x_iio_write_attr(void *device, char *buf,
				  uint32_t len, const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int ret = 0;
	int32_t val;
	struct hmc630x_iio_dev *iiodev = (struct hmc630x_iio_dev *)device;
	struct hmc630x_dev *d = iiodev->dev;
	enum hmc6301_bb_attn attn1, attn2;
	enum hmc6301_bb_attn_fine attni, attnq;
	enum hmc6301_bb_lpc lpc;
	enum hmc6301_bb_hpc hpc;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	switch (priv) {
	case HMC630X_IIO_ATTR_ENABLED:
		ret = hmc630x_set_enable(d, (bool)val);
		break;
	case HMC630X_IIO_ATTR_VCO:
		ret = hmc630x_set_vco(d, (uint64_t)val * 1000);
		break;
	case HMC630X_IIO_ATTR_IF_ATTN:
		ret = hmc630x_set_if_attn(d, (uint8_t)val);
		break;
	case HMC630X_IIO_ATTR_TEMP_EN:
		ret = hmc630x_set_temp_en(d, (bool)val);
		break;
	case HMC6300_IIO_ATTR_RF_ATTN:
		ret = hmc6300_set_rf_attn(d, (uint8_t)val);
		break;
	case HMC6301_IIO_ATTR_RF_LNA_GAIN:
		ret = hmc6301_set_lna_gain(d, (enum hmc6301_lna_attn)val);
		break;
	case HMC6301_IIO_ATTR_BB_ATTN1:
	case HMC6301_IIO_ATTR_BB_ATTN2:
		ret = hmc6301_get_bb_attn(d, &attn1, &attn2);
		if (ret)
			break;

		if (priv == HMC6301_IIO_ATTR_BB_ATTN1)
			ret = hmc6301_set_bb_attn(d, (enum hmc6301_bb_attn)val, attn2);
		else
			ret = hmc6301_set_bb_attn(d, attn1, (enum hmc6301_bb_attn)val);
		break;
	case HMC6301_IIO_ATTR_BB_ATTNI_FINE:
	case HMC6301_IIO_ATTR_BB_ATTNQ_FINE:
		ret = hmc6301_get_bb_attn_fine(d, &attni, &attnq);
		if (ret)
			break;

		if (priv == HMC6301_IIO_ATTR_BB_ATTNI_FINE)
			ret = hmc6301_set_bb_attn_fine(d, (enum hmc6301_bb_attn_fine)val, attnq);
		else
			ret = hmc6301_set_bb_attn_fine(d, attni, (enum hmc6301_bb_attn_fine)val);
		break;
	case HMC6301_IIO_ATTR_BB_LPC:
	case HMC6301_IIO_ATTR_BB_HPC:
		ret = hmc6301_get_bb_lpc_hpc(d, &lpc, &hpc);
		if (ret)
			break;

		if (priv == HMC6301_IIO_ATTR_BB_LPC)
			ret = hmc6301_set_bb_lpc_hpc(d, (enum hmc6301_bb_lpc)val, hpc);
		else
			ret = hmc6301_set_bb_lpc_hpc(d, lpc, (enum hmc6301_bb_hpc)val);
		break;
	default:
		return -EINVAL;
	};

	if (ret)
		return ret;

	return len;
}

static int hmc630x_iio_read_vco_available(void *device, char *buf,
		uint32_t len, const struct iio_ch_info *channel,
		intptr_t priv)
{
	int ret = 0;
	struct hmc630x_iio_dev *iiodev = (struct hmc630x_iio_dev *)device;
	struct hmc630x_dev *d = iiodev->dev;
	const uint64_t *freqs;
	uint8_t n, nfreqs;
	size_t l = 0;

	if (priv != HMC630X_IIO_ATTR_VCO_AVAILABLE)
		return -EINVAL;

	ret = hmc630x_get_avail_vco(d, &freqs, &nfreqs);
	if (ret)
		return ret;

	for (n = 0; n < nfreqs; n++) {
		l += snprintf(buf + l, 12, "%lu ", (uint32_t)(freqs[n]/1000));

		if (l > len)
			return -E2BIG;
	}

	return l;
}

static int hmc630x_iio_read_raw(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	uint8_t temp;
	struct hmc630x_iio_dev *iiodev = (struct hmc630x_iio_dev *)device;
	struct hmc630x_dev *d = iiodev->dev;

	switch (channel->type) {
	case IIO_TEMP:
		ret = hmc630x_get_temp(d, &temp);
		if (ret)
			return ret;
		break;
	default:
		return -EINVAL;
	};

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&temp);
}

static struct iio_attribute hmc630x_iio_temp_attrs[] = {
	{
		.name = "raw",
		.show = hmc630x_iio_read_raw,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel hmc630x_channels[] = {
	{
		.ch_type = IIO_TEMP,
		.attributes = hmc630x_iio_temp_attrs,
	}
};

#define HMC630X_COMMON_ATTRS \
	{ \
		.name = "enabled", \
		.priv = HMC630X_IIO_ATTR_ENABLED, \
		.show = hmc630x_iio_read_attr, \
		.store = hmc630x_iio_write_attr, \
	}, \
	{ \
		.name = "vco", \
		.priv = HMC630X_IIO_ATTR_VCO, \
		.show = hmc630x_iio_read_attr, \
		.store = hmc630x_iio_write_attr, \
	}, \
	{ \
		.name = "vco_available", \
		.priv = HMC630X_IIO_ATTR_VCO_AVAILABLE, \
		.show = hmc630x_iio_read_vco_available, \
	}, \
	{ \
		.name = "vco_band", \
		.priv = HMC630X_IIO_ATTR_VCO_BAND, \
		.show = hmc630x_iio_read_attr, \
	}, \
	{ \
		.name = "vco_lock", \
		.priv = HMC630X_IIO_ATTR_VCO_LOCK, \
		.show = hmc630x_iio_read_attr, \
	}, \
	{ \
		.name = "if_attn", \
		.priv = HMC630X_IIO_ATTR_IF_ATTN, \
		.show = hmc630x_iio_read_attr, \
		.store = hmc630x_iio_write_attr, \
	}, \
	{ \
		.name = "temp_en", \
		.priv = HMC630X_IIO_ATTR_TEMP_EN, \
		.show = hmc630x_iio_read_attr, \
		.store = hmc630x_iio_write_attr, \
	}

static struct iio_attribute hmc6300_iio_attrs[] = {
	HMC630X_COMMON_ATTRS,
	{
		.name = "rf_attn",
		.priv = HMC6300_IIO_ATTR_RF_ATTN,
		.show = hmc630x_iio_read_attr,
		.store = hmc630x_iio_write_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute hmc6301_iio_attrs[] = {
	HMC630X_COMMON_ATTRS,
	{
		.name = "rf_lna_gain",
		.priv = HMC6301_IIO_ATTR_RF_LNA_GAIN,
		.show = hmc630x_iio_read_attr,
		.store = hmc630x_iio_write_attr,
	},
	{
		.name = "bb_attn1",
		.priv = HMC6301_IIO_ATTR_BB_ATTN1,
		.show = hmc630x_iio_read_attr,
		.store = hmc630x_iio_write_attr,
	},
	{
		.name = "bb_attn2",
		.priv = HMC6301_IIO_ATTR_BB_ATTN2,
		.show = hmc630x_iio_read_attr,
		.store = hmc630x_iio_write_attr,
	},
	{
		.name = "bb_attni_fine",
		.priv = HMC6301_IIO_ATTR_BB_ATTNI_FINE,
		.show = hmc630x_iio_read_attr,
		.store = hmc630x_iio_write_attr,
	},
	{
		.name = "bb_attnq_fine",
		.priv = HMC6301_IIO_ATTR_BB_ATTNQ_FINE,
		.show = hmc630x_iio_read_attr,
		.store = hmc630x_iio_write_attr,
	},
	{
		.name = "bb_lpc",
		.priv = HMC6301_IIO_ATTR_BB_LPC,
		.show = hmc630x_iio_read_attr,
		.store = hmc630x_iio_write_attr,
	},
	{
		.name = "bb_hpc",
		.priv = HMC6301_IIO_ATTR_BB_HPC,
		.show = hmc630x_iio_read_attr,
		.store = hmc630x_iio_write_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device hmc630x_iio_device_template = {
	.attributes = NULL, // filled at run-time
	.num_ch = NO_OS_ARRAY_SIZE(hmc630x_channels),
	.channels = hmc630x_channels,
	.debug_reg_read = (int32_t (*)())_hmc630x_read_register2,
	.debug_reg_write = (int32_t (*)())_hmc630x_write_register2
};

int32_t hmc630x_iio_init(struct hmc630x_iio_dev **iiodev,
			 struct hmc630x_iio_init_param *init_param)
{
	int ret;
	struct hmc630x_iio_dev *d;
	struct iio_device *d2;

	d = (struct hmc630x_iio_dev *)no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d2 = (struct iio_device *)no_os_calloc(1, sizeof(*d2));
	if (!d2)
		goto end_0;

	*d2 = hmc630x_iio_device_template;

	d->iio_dev = d2;

	ret = hmc630x_init(&d->dev, init_param->ip);
	if (ret)
		goto end_1;

	if (hmc630x_type(d->dev) == HMC6300)
		d->iio_dev->attributes = hmc6300_iio_attrs;
	else
		d->iio_dev->attributes = hmc6301_iio_attrs;

	*iiodev = d;

	return 0;
end_1:
	no_os_free(d2);
end_0:
	no_os_free(d);

	return ret;
}

int32_t hmc630x_iio_remove(struct hmc630x_iio_dev *iiodev)
{
	int ret;

	if (!iiodev)
		return 0;

	ret = hmc630x_remove(iiodev->dev);
	if (ret != 0)
		return ret;

	no_os_free(iiodev->iio_dev);
	no_os_free(iiodev);

	return 0;
}
