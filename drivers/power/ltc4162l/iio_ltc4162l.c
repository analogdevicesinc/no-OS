/***************************************************************************//**
 *   @file   iio_ltc4162l.c
 *   @brief  Implementation of IIO LTC4162L Driver.
 *   @author Marc Paolo Sosa (marcpaolo.sosa@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include <errno.h>
#include "iio_ltc4162l.h"
#include "ltc4162l.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

#define LT8491_IIO_CH_ATTR_RW(_name, _priv)	\
	{					\
		.name = _name,			\
		.priv = _priv,			\
		.show = lt8491_ch_attr_show,	\
		.store = lt8491_ch_attr_store,	\
	}

#define LTC4162L_IIO_CH_ATTR_RW(_name, _priv)	\
	{					\
		.name = _name,			\
		.priv = _priv,			\
		.show = ltc4162l_ch_attr_show,	\
		.store = ltc4162l_ch_attr_store,\
	}

#define LTC4162L_IIO_CH_ATTR_RO(_name, _priv)	\
	{					\
		.name = _name,			\
		.priv = _priv,			\
		.show = ltc4162l_ch_attr_show,	\
		.store = NULL,			\
	}

#define LTC4162L_IIO_CH_ATTR_WO(_name, _priv)	\
	{					\
		.name = _name,			\
		.priv = _priv,			\
		.show = NULL,			\
		.store = ltc4162l_ch_attr_store,\
	}

#define LTC4162L_IIO_CH_ATTR_READ_RAW(_name, _priv)	\
	{						\
		.name = _name,				\
		.priv = _priv,				\
		.show = ltc4162l_read_raw,		\
		.store = NULL,				\
	}

#define LTC4162L_IIO_CH_ATTR_READ_SCALE(_name, _priv)	\
	{						\
		.name = _name,				\
		.priv = _priv,				\
		.show = ltc4162l_read_scale,		\
		.store = NULL,				\
	}

enum ltc4162l_attr_priv {
	LTC4162L_BATTERY_COUNT,
	LTC4162L_CHEM_TYPE,
	LTC4162L_CHARGER_STATUS,
	LTC4162L_CHARGE_STATE,
	LTC4162L_ONLINE_STATUS,
	LTC4162L_VBAT_VAL,
	LTC4162L_AVG_VBAT_VAL,
	LTC4162L_IBAT_VAL,
	LTC4162L_VIN_VAL,
	LTC4162L_IIN_VAL,
	LTC4162L_ICHARGE_SET,
	LTC4162L_VCHARGE_SET,
	LTC4162L_IIN_LIMIT_SET,
	LTC4162L_DIE_TEMP,
	LTC4162L_TERM_CURRENT,
	LTC4162L_TELEMETRY,
};

static int32_t ltc4162l_iio_reg_read(struct ltc4162l_iio_device *dev,
				     uint32_t reg, uint32_t *readval)
{
	uint16_t temp;
	int ret;

	ret = ltc4162l_reg_read(dev->dev, (uint8_t)reg, &temp);
	if (!ret)
		*readval = temp;

	return ret;
}

static int32_t ltc4162l_iio_reg_write(struct ltc4162l_iio_device *dev,
				      uint32_t reg, uint32_t writeval)
{
	return ltc4162l_reg_write(dev->dev, (uint8_t)reg, (uint16_t)writeval);
}

static int ltc4162l_ch_attr_show(void *ddev, char *buf,
				 uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ltc4162l_iio_device *dev = ddev;
	int ret;
	int32_t val;
	uint32_t uval;

	switch (priv) {
	case LTC4162L_BATTERY_COUNT:
		ret = ltc4162l_get_cell_count(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_CHEM_TYPE:
		ret = ltc4162l_get_chem_type(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_CHARGER_STATUS:
		ret = ltc4162l_get_charge_status(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_CHARGE_STATE:
		ret = ltc4162l_get_charger_state(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_ONLINE_STATUS:
		ret = ltc4162l_get_online_status(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_ICHARGE_SET:
		ret = ltc4162l_get_icharge_setting(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_VCHARGE_SET:
		ret = ltc4162l_get_vcharge_setting(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_IIN_LIMIT_SET:
		ret = ltc4162l_get_iin_limit(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_TERM_CURRENT:
		ret = ltc4162l_get_term_current(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_TELEMETRY:
		ret = ltc4162l_get_telemetry(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	default:
		return -EOPNOTSUPP;
	}
}

static int ltc4162l_ch_attr_store(void *ddev, char *buf,
				  uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ltc4162l_iio_device *dev = ddev;
	int16_t val;
	int ret;

	switch (priv) {
	case LTC4162L_ICHARGE_SET:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = ltc4162l_set_icharge_setting(dev->dev, val);
		if (ret)
			return ret;

		return len;
	case LTC4162L_VCHARGE_SET:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = ltc4162l_set_vcharge_setting(dev->dev, val);
		if (ret)
			return ret;

		return len;
	case LTC4162L_IIN_LIMIT_SET:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = ltc4162l_set_iin_limit(dev->dev, val);
		if (ret)
			return ret;

		return len;
	case LTC4162L_TERM_CURRENT:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = ltc4162l_set_term_current(dev->dev, val);
		if (ret)
			return ret;

		return len;
	case LTC4162L_TELEMETRY:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = ltc4162l_set_telemetry(dev->dev, val);
		if (ret)
			return ret;

		return len;

	default:
		return -EOPNOTSUPP;
	}
}

static struct iio_attribute ltc4162l_debug_attrs[] = {
	LTC4162L_IIO_CH_ATTR_RO("battery_count", LTC4162L_BATTERY_COUNT),
	LTC4162L_IIO_CH_ATTR_RO("chem_type", LTC4162L_CHEM_TYPE),
	LTC4162L_IIO_CH_ATTR_RO("charger_status", LTC4162L_CHARGER_STATUS),
	LTC4162L_IIO_CH_ATTR_RO("charge_state", LTC4162L_CHARGE_STATE),
	LTC4162L_IIO_CH_ATTR_RO("online_status", LTC4162L_ONLINE_STATUS),
	LTC4162L_IIO_CH_ATTR_RW("icharge_set", LTC4162L_ICHARGE_SET),
	LTC4162L_IIO_CH_ATTR_RW("vcharge_set", LTC4162L_VCHARGE_SET),
	LTC4162L_IIO_CH_ATTR_RW("iin_limit_set", LTC4162L_IIN_LIMIT_SET),
	LTC4162L_IIO_CH_ATTR_RW("term_current", LTC4162L_TERM_CURRENT),
	LTC4162L_IIO_CH_ATTR_RW("telemetry", LTC4162L_TELEMETRY),
	END_ATTRIBUTES_ARRAY
};

static int ltc4162l_read_raw(void *ddev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct ltc4162l_iio_device *dev = ddev;
	int ret;
	int32_t val;
	uint32_t uval;

	switch (priv) {
	case LTC4162L_VBAT_VAL:
		ret = ltc4162l_read_vbat(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_AVG_VBAT_VAL:
		ret = ltc4162l_read_avg_vbat(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_IBAT_VAL:
		ret = ltc4162l_read_ibat(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_VIN_VAL:
		ret = ltc4162l_read_input_voltage(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LTC4162L_IIN_VAL:
		ret = ltc4162l_read_input_current(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	default:
		return -EOPNOTSUPP;
	}
}

static int ltc4162l_read_scale(void *ddev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;

	switch (priv) {
	case LTC4162L_VBAT_VAL:
	case LTC4162L_AVG_VBAT_VAL:
	case LTC4162L_IBAT_VAL:
	case LTC4162L_VIN_VAL:
	case LTC4162L_IIN_VAL:
	case LTC4162L_DIE_TEMP:
		val = 1;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	default:
		return -EOPNOTSUPP;
	}
}

static struct scan_type ltc4162l_signed_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static struct scan_type ltc4162l_unsigned_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static struct iio_attribute ltc4162l_vbat_ch_attrs[] = {
	LTC4162L_IIO_CH_ATTR_READ_RAW("raw", LTC4162L_VBAT_VAL),
	LTC4162L_IIO_CH_ATTR_READ_SCALE("scale", LTC4162L_VBAT_VAL),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltc4162l_avg_vbat_ch_attrs[] = {
	LTC4162L_IIO_CH_ATTR_READ_RAW("raw", LTC4162L_AVG_VBAT_VAL),
	LTC4162L_IIO_CH_ATTR_READ_SCALE("scale", LTC4162L_AVG_VBAT_VAL),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltc4162l_ibat_ch_attrs[] = {
	LTC4162L_IIO_CH_ATTR_READ_RAW("raw", LTC4162L_IBAT_VAL),
	LTC4162L_IIO_CH_ATTR_READ_SCALE("scale", LTC4162L_IBAT_VAL),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltc4162l_vin_ch_attrs[] = {
	LTC4162L_IIO_CH_ATTR_READ_RAW("raw", LTC4162L_VIN_VAL),
	LTC4162L_IIO_CH_ATTR_READ_SCALE("scale", LTC4162L_VIN_VAL),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltc4162l_iin_ch_attrs[] = {
	LTC4162L_IIO_CH_ATTR_READ_RAW("raw", LTC4162L_IIN_VAL),
	LTC4162L_IIO_CH_ATTR_READ_SCALE("scale", LTC4162L_IIN_VAL),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltc4162l_die_temp_ch_attrs[] = {
	LTC4162L_IIO_CH_ATTR_READ_RAW("raw", LTC4162L_DIE_TEMP),
	LTC4162L_IIO_CH_ATTR_READ_SCALE("scale", LTC4162L_DIE_TEMP),
	END_ATTRIBUTES_ARRAY
};

struct iio_channel ltc4162l_channels[] = {
	{
		.name = "vbat",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &ltc4162l_unsigned_scan_type,
		.attributes = ltc4162l_vbat_ch_attrs,
		.ch_out = false,
	}, {
		.name = "avg_vbat",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &ltc4162l_unsigned_scan_type,
		.attributes = ltc4162l_avg_vbat_ch_attrs,
		.ch_out = false,
	}, {
		.name = "ibat",
		.ch_type = IIO_CURRENT,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &ltc4162l_signed_scan_type,
		.attributes = ltc4162l_ibat_ch_attrs,
		.ch_out = false,
	}, {
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &ltc4162l_unsigned_scan_type,
		.attributes = ltc4162l_vin_ch_attrs,
		.ch_out = false,
	}, {
		.name = "iin",
		.ch_type = IIO_CURRENT,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &ltc4162l_unsigned_scan_type,
		.attributes = ltc4162l_iin_ch_attrs,
		.ch_out = false,
	}, {
		.name = "die_temp",
		.ch_type = IIO_TEMP,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &ltc4162l_signed_scan_type,
		.attributes = ltc4162l_die_temp_ch_attrs,
		.ch_out = false,
	}
};

static struct iio_device ltc4162l_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ltc4162l_channels),
	.channels = ltc4162l_channels,
	.debug_reg_read = (int32_t (*)()) ltc4162l_iio_reg_read,
	.debug_reg_write = (int32_t (*)()) ltc4162l_iio_reg_write,
	.debug_attributes = ltc4162l_debug_attrs,
};

int ltc4162l_iio_init(struct ltc4162l_iio_device **iio_device,
		      struct ltc4162l_iio_init_param *iio_init_param)
{
	struct ltc4162l_iio_device *iio_device_temp;
	int ret;

	if (!iio_init_param || !iio_init_param->init_param)
		return -EINVAL;

	iio_device_temp = no_os_calloc(1, sizeof(*iio_device_temp));
	if (!iio_device_temp)
		return -ENOMEM;

	ret = ltc4162l_init(&iio_device_temp->dev, iio_init_param->init_param);
	if (ret)
		goto free_dev;

	iio_device_temp->iio_dev = &ltc4162l_iio_dev;

	*iio_device = iio_device_temp;

	return 0;

free_dev:
	no_os_free(iio_device_temp);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param iio_device - The iio device structure.
 * @return 0 in case of success, errno errors otherwise
 */
int ltc4162l_iio_remove(struct ltc4162l_iio_device *iio_device)
{
	ltc4162l_remove(iio_device->dev);

	no_os_free(iio_device);

	return 0;
}
