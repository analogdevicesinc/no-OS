/***************************************************************************//**
 *   @file   iio_lt8491.c
 *   @brief  Implementation of IIO LT8491 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include "iio_lt8491.h"
#include "lt8491.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

#define LT8491_IIO_CH_ATTR_RW(_name, _priv)	\
	{					\
		.name = _name,			\
		.priv = _priv,			\
		.show = lt8491_ch_attr_show,	\
		.store = lt8491_ch_attr_store,	\
	}

#define LT8491_IIO_CH_ATTR_RO(_name, _priv)	\
	{					\
		.name = _name,			\
		.priv = _priv,			\
		.show = lt8491_ch_attr_show,	\
		.store = NULL,			\
	}

#define LT8491_IIO_CH_ATTR_WO(_name, _priv)	\
	{					\
		.name = _name,			\
		.priv = _priv,			\
		.show = NULL,			\
		.store = lt8491_ch_attr_store,	\
	}

#define LT8491_IIO_CH_ATTR_READ_RAW(_name, _priv)	\
	{						\
		.name = _name,				\
		.priv = _priv,				\
		.show = lt8491_read_raw,		\
		.store = NULL,				\
	}

#define LT8491_IIO_CH_ATTR_READ_SCALE(_name, _priv)	\
	{						\
		.name = _name,				\
		.priv = _priv,				\
		.show = lt8491_read_scale,		\
		.store = NULL,				\
	}

enum lt8491_attr_priv {
	LT8491_TBAT,
	LT8491_POUT,
	LT8491_PIN,
	LT8491_EFF,
	LT8491_IOUT,
	LT8491_IIN,
	LT8491_VBAT,
	LT8491_VIN,
	LT8491_CHARGING_STAGE,
	LT8491_CHARGING_STATUS,
	LT8491_ENABLE,
	LT8491_RESTART,
	LT8491_SCRATCH,
	LT8491_SERIAL_ID,
};

static int32_t lt8491_iio_reg_read(struct lt8491_iio_device *dev,
				   uint32_t reg, uint32_t *readval)
{
	uint16_t temp;
	int ret;

	ret = lt8491_reg_read(dev->dev, (uint8_t)reg, &temp, false);
	if (!ret)
		*readval = temp;

	return ret;
}

static int32_t lt8491_iio_reg_write(struct lt8491_iio_device *dev,
				    uint32_t reg, uint32_t writeval)
{
	return lt8491_reg_write(dev->dev, (uint8_t)reg, (uint16_t)writeval,
				false);
}

static int lt8491_ch_attr_show(void *ddev, char *buf,
			       uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct lt8491_iio_device *dev = ddev;
	int ret;
	int32_t val;
	uint32_t uval, uvals[3];

	switch (priv) {
	case LT8491_POUT:
		ret = lt8491_read_pout(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_PIN:
		ret = lt8491_read_pin(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_EFF:
		ret = lt8491_read_efficiency(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_CHARGING_STAGE:
		ret = lt8491_read_charging_stage(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_CHARGING_STATUS:
		ret = lt8491_read_charging_status(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_ENABLE:
		ret = lt8491_read_enable(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_SCRATCH:
		ret = lt8491_read_scratch(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_SERIAL_ID:
		ret = lt8491_read_serial_id(dev->dev, uvals);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT_MULTIPLE,
					NO_OS_ARRAY_SIZE(uvals),
					(int32_t *)uvals);
	default:
		return -EOPNOTSUPP;
	}
}

static int lt8491_ch_attr_store(void *ddev, char *buf,
				uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct lt8491_iio_device *dev = ddev;
	int32_t val;
	int ret;

	switch (priv) {
	case LT8491_ENABLE:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		if (val >> 1)
			return -EINVAL;

		ret = lt8491_write_enable(dev->dev, val);
		if (ret)
			return ret;

		return len;
	case LT8491_RESTART:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		if (val >> 1)
			return -EINVAL;

		if (val) {
			ret = lt8491_restart_chip(dev->dev);
			if (ret)
				return ret;
		}

		return len;
	case LT8491_SCRATCH:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = lt8491_write_scratch(dev->dev, val);
		if (ret)
			return ret;

		return len;
	default:
		return -EOPNOTSUPP;
	}
}

static struct iio_attribute lt8491_debug_attrs[] = {
	LT8491_IIO_CH_ATTR_RO("pout", LT8491_POUT),
	LT8491_IIO_CH_ATTR_RO("pin", LT8491_PIN),
	LT8491_IIO_CH_ATTR_RO("efficiency", LT8491_EFF),
	LT8491_IIO_CH_ATTR_RO("charging_stage", LT8491_CHARGING_STAGE),
	LT8491_IIO_CH_ATTR_RO("charging_status", LT8491_CHARGING_STATUS),
	LT8491_IIO_CH_ATTR_RW("enable", LT8491_ENABLE),
	LT8491_IIO_CH_ATTR_WO("reset", LT8491_RESTART),
	LT8491_IIO_CH_ATTR_RW("scratch", LT8491_SCRATCH),
	LT8491_IIO_CH_ATTR_RO("serial_id", LT8491_SERIAL_ID),
	END_ATTRIBUTES_ARRAY
};

static int lt8491_read_raw(void *ddev, char *buf, uint32_t len,
			   const struct iio_ch_info *channel, intptr_t priv)
{
	struct lt8491_iio_device *dev = ddev;
	int ret;
	int32_t val;
	uint32_t uval;

	switch (priv) {
	case LT8491_TBAT:
		ret = lt8491_read_tbat(dev->dev, &val);
		if (ret)
			return ret;

		val /= 100;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_IOUT:
		ret = lt8491_read_iout(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_IIN:
		ret = lt8491_read_iin(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_VBAT:
		ret = lt8491_read_vbat(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval / 10;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_VIN:
		ret = lt8491_read_vin(dev->dev, &uval);
		if (ret)
			return ret;

		val = uval / 10;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EOPNOTSUPP;
	}
}

static int lt8491_read_scale(void *ddev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;

	switch (priv) {
	case LT8491_TBAT:
		val = 100;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_IOUT:
	case LT8491_IIN:
		val = 1;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case LT8491_VBAT:
	case LT8491_VIN:
		val = 10;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EOPNOTSUPP;
	}
}

static struct scan_type lt8491_signed_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static struct scan_type lt8491_unsigned_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static struct iio_attribute lt8491_temp_ch_attrs[] = {
	LT8491_IIO_CH_ATTR_READ_RAW("raw", LT8491_TBAT),
	LT8491_IIO_CH_ATTR_READ_SCALE("scale", LT8491_TBAT),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt8491_out_current_ch_attrs[] = {
	LT8491_IIO_CH_ATTR_READ_RAW("raw", LT8491_IOUT),
	LT8491_IIO_CH_ATTR_READ_SCALE("scale", LT8491_IOUT),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt8491_in_current_ch_attrs[] = {
	LT8491_IIO_CH_ATTR_READ_RAW("supply_raw", LT8491_IIN),
	LT8491_IIO_CH_ATTR_READ_SCALE("supply_scale", LT8491_IIN),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt8491_out_voltage_ch_attrs[] = {
	LT8491_IIO_CH_ATTR_READ_RAW("raw", LT8491_VBAT),
	LT8491_IIO_CH_ATTR_READ_SCALE("scale", LT8491_VBAT),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt8491_in_voltage_ch_attrs[] = {
	LT8491_IIO_CH_ATTR_READ_RAW("supply_raw", LT8491_VIN),
	LT8491_IIO_CH_ATTR_READ_SCALE("supply_scale", LT8491_VIN),
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel lt8491_channels[] = {
	{
		.name = "temp",
		.ch_type = IIO_TEMP,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &lt8491_signed_scan_type,
		.attributes = lt8491_temp_ch_attrs,
		.ch_out = false,
	}, {
		.name = "current",
		.ch_type = IIO_CURRENT,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &lt8491_unsigned_scan_type,
		.attributes = lt8491_out_current_ch_attrs,
		.ch_out = true,
	}, {
		.name = "current",
		.ch_type = IIO_CURRENT,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &lt8491_unsigned_scan_type,
		.attributes = lt8491_in_current_ch_attrs,
		.ch_out = false,
	}, {
		.name = "voltage",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &lt8491_unsigned_scan_type,
		.attributes = lt8491_out_voltage_ch_attrs,
		.ch_out = true,
	}, {
		.name = "voltage",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &lt8491_unsigned_scan_type,
		.attributes = lt8491_in_voltage_ch_attrs,
		.ch_out = false,
	}
};

static struct iio_device lt8491_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(lt8491_channels),
	.channels = lt8491_channels,
	.debug_reg_read = (int32_t (*)()) lt8491_iio_reg_read,
	.debug_reg_write = (int32_t (*)()) lt8491_iio_reg_write,
	.debug_attributes = lt8491_debug_attrs,
};

/**
 * @brief Initializes the LT8491 IIO driver
 * @param iio_device - The iio device structure.
 * @param iio_init_param - Parameters for the initialization of iio_dev
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_iio_init(struct lt8491_iio_device **iio_device,
		    struct lt8491_iio_init_param *iio_init_param)
{
	struct lt8491_iio_device *iio_device_temp;
	int ret;

	if (!iio_init_param || !iio_init_param->init_param)
		return -EINVAL;

	iio_device_temp = no_os_calloc(1, sizeof(*iio_device_temp));
	if (!iio_device_temp)
		return -ENOMEM;

	ret = lt8491_init(&iio_device_temp->dev, iio_init_param->init_param);
	if (ret)
		goto free_dev;

	ret = lt8491_restart_chip(iio_device_temp->dev);
	if (ret)
		goto remove_dev;

	ret = lt8491_write_enable(iio_device_temp->dev, false);
	if (ret)
		goto remove_dev;

	ret = lt8491_configure_telemetry(iio_device_temp->dev,
					 iio_init_param->init_param);
	if (ret)
		goto remove_dev;

	iio_device_temp->iio_dev = &lt8491_iio_dev;

	*iio_device = iio_device_temp;

	return 0;

remove_dev:
	lt8491_remove(iio_device_temp->dev);
free_dev:
	no_os_free(iio_device_temp);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param iio_device - The iio device structure.
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_iio_remove(struct lt8491_iio_device *iio_device)
{
	lt8491_remove(iio_device->dev);

	no_os_free(iio_device);

	return 0;
}
