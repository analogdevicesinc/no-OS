/***************************************************************************//**
 *   @file   iio_max31827.c
 *   @brief  Implementation of IIO MAX31827 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
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
#include "iio_max31827.h"
#include "max31827.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

#define MAX31827_IIO_CH_ATTR_RW(_name, _priv)  \
	{                                       \
		.name = _name,                      \
		.priv = _priv,                      \
		.show = max31827_ch_attr_show,      \
		.store = max31827_ch_attr_store,    \
	}

#define MAX31827_IIO_CH_ATTR_RO(_name, _priv)  \
	{                                       \
		.name = _name,                      \
		.priv = _priv,                      \
		.show = max31827_ch_attr_show,      \
		.store = NULL,    \
	}

enum max31827_ch_attr_priv {
	MAX31827_T_ENABLE,
	MAX31827_T_INPUT,
	MAX31827_T_MIN,
	MAX31827_T_MIN_HYST,
	MAX31827_T_MIN_ALARM,
	MAX31827_T_MAX,
	MAX31827_T_MAX_HYST,
	MAX31827_T_MAX_ALARM,
	MAX31827_T_RESOLUTION,
	MAX31827_C_UPDATE_INTERVAL,
};

static int32_t max31827_iio_reg_read(struct max31827_iio_device *dev,
				     uint32_t reg, uint32_t *readval)
{
	return max31827_reg_read(dev->dev, (uint8_t)reg, (uint16_t*)readval);
}

static int32_t max31827_iio_reg_write(struct max31827_iio_device *dev,
				      uint32_t reg, uint32_t writeval)
{
	return max31827_reg_write(dev->dev, reg, writeval);
}

static int max31827_ch_attr_show(void *ddev, char *buf,
				 uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct max31827_iio_device *dev = ddev;
	int ret;
	int32_t val;
	uint16_t uval;

	switch (priv) {
	case MAX31827_T_ENABLE:
		ret = max31827_reg_read(dev->dev, MAX31827_CONF_REG, &uval);
		if (ret)
			return ret;

		val = !!no_os_field_get(MAX31827_CONF_1SHOT_MASK |
					MAX31827_CONF_CNV_RATE_MASK, uval);

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case MAX31827_T_INPUT:
		ret = max31827_read_temp_input(dev->dev, &val);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case MAX31827_T_MIN:
		ret = max31827_read_temp(dev->dev, MAX31827_TL_REG, &val);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case MAX31827_T_MIN_HYST:
		ret = max31827_read_temp(dev->dev, MAX31827_TL_HYST_REG, &val);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case MAX31827_T_MIN_ALARM:
		ret = max31827_reg_read(dev->dev, MAX31827_CONF_REG, &uval);
		if (ret)
			return ret;

		val = MAX31827_CONF_U_TEMP_STAT(uval);

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case MAX31827_T_MAX:
		ret = max31827_read_temp(dev->dev, MAX31827_TH_REG, &val);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case MAX31827_T_MAX_HYST:
		ret = max31827_read_temp(dev->dev, MAX31827_TH_HYST_REG, &val);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case MAX31827_T_MAX_ALARM:
		ret = max31827_reg_read(dev->dev, MAX31827_CONF_REG, &uval);
		if (ret)
			return ret;

		val = MAX31827_CONF_O_TEMP_STAT(uval);

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case MAX31827_T_RESOLUTION:
		ret = max31827_reg_read(dev->dev, MAX31827_CONF_REG, &uval);
		if (ret)
			return ret;

		val = max31827_resolutions[MAX31827_CONF_RESO(uval)];

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case MAX31827_C_UPDATE_INTERVAL:
		ret = max31827_reg_read(dev->dev, MAX31827_CONF_REG, &uval);
		if (ret)
			return ret;

		val = max31827_conversions[MAX31827_CONF_CNV_RATE(uval)];

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EOPNOTSUPP;
	}
}

static int max31827_ch_attr_store(void *ddev, char *buf,
				  uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct max31827_iio_device *dev = ddev;
	unsigned int idx;
	int32_t val;
	int ret;

	switch (priv) {
	case MAX31827_T_ENABLE:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		if (val >> 1)
			return -EOPNOTSUPP;

		dev->dev->enable = val;

		ret = max31827_reg_update_bits(dev->dev,
					       MAX31827_CONF_REG,
					       MAX31827_CONF_1SHOT_MASK |
					       MAX31827_CONF_CNV_RATE_MASK,
					       MAX31827_DEVICE_ENABLE(val));

		break;
	case MAX31827_T_MIN:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		ret = max31827_write_alarm_val(dev->dev, MAX31827_TL_REG, val);

		break;
	case MAX31827_T_MIN_HYST:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		ret = max31827_write_alarm_val(dev->dev, MAX31827_TL_HYST_REG,
					       val);

		break;
	case MAX31827_T_MAX:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		ret = max31827_write_alarm_val(dev->dev, MAX31827_TH_REG, val);

		break;
	case MAX31827_T_MAX_HYST:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		ret = max31827_write_alarm_val(dev->dev, MAX31827_TH_HYST_REG,
					       val);

		break;
	case MAX31827_T_RESOLUTION:
		idx = MAX31827_RES_8_BIT;

		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		/**
		 * Convert the desired resolution into register
		 * bits. idx is already initialized with 0.
		 *
		 * This was inspired by lm73 driver.
		 */
		while (idx < NO_OS_ARRAY_SIZE(max31827_resolutions) &&
		       val < max31827_resolutions[idx])
			idx++;

		if (idx == NO_OS_ARRAY_SIZE(max31827_resolutions))
			idx--;

		ret = max31827_shutdown_write(dev->dev, MAX31827_CONF_REG,
					      MAX31827_CONF_RESO_MASK, idx);

		if (!ret)
			dev->dev->resolution = idx;

		break;
	case MAX31827_C_UPDATE_INTERVAL:
		idx = MAX31827_CNV_1_DIV_64_HZ;

		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		if (!dev->dev->enable)
			return -EOPNOTSUPP;

		/**
		 * Convert the desired conversion rate into register
		 * bits. idx is already initialized with 1.
		 *
		 * This was inspired by lm73 driver.
		 */
		while (idx < NO_OS_ARRAY_SIZE(max31827_conversions) &&
		       val < max31827_conversions[idx])
			idx++;

		if (idx == NO_OS_ARRAY_SIZE(max31827_conversions))
			idx--;

		ret = max31827_reg_update_bits(dev->dev, MAX31827_CONF_REG,
					       MAX31827_CONF_CNV_RATE_MASK,
					       idx);
		if (!ret)
			dev->dev->update_interval = val;

		break;
	default:
		return -EOPNOTSUPP;
	}

	if (ret)
		return ret;

	return len;
}

static struct iio_attribute max31827_iio_ch_attrs[] = {
	MAX31827_IIO_CH_ATTR_RW("temp_enable", MAX31827_T_ENABLE),
	MAX31827_IIO_CH_ATTR_RO("temp_input", MAX31827_T_INPUT),
	MAX31827_IIO_CH_ATTR_RW("temp_min", MAX31827_T_MIN),
	MAX31827_IIO_CH_ATTR_RW("temp_min_hyst", MAX31827_T_MIN_HYST),
	MAX31827_IIO_CH_ATTR_RO("temp_min_alarm", MAX31827_T_MIN_ALARM),
	MAX31827_IIO_CH_ATTR_RW("temp_max", MAX31827_T_MAX),
	MAX31827_IIO_CH_ATTR_RW("temp_max_hyst", MAX31827_T_MAX_HYST),
	MAX31827_IIO_CH_ATTR_RO("temp_max_alarm", MAX31827_T_MAX_ALARM),
	MAX31827_IIO_CH_ATTR_RW("temp_resolution", MAX31827_T_RESOLUTION),
	MAX31827_IIO_CH_ATTR_RW("update_interval", MAX31827_C_UPDATE_INTERVAL),
	END_ATTRIBUTES_ARRAY
};

static struct scan_type max31827_iio_scan_type = {
	.sign = 's',
	.realbits = 13,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

static struct iio_channel max31827_channels[] = {
	{
		.name = "temp",
		.ch_type = IIO_TEMP,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &max31827_iio_scan_type,
		.attributes = max31827_iio_ch_attrs,
		.ch_out = false,
	}
};

static struct iio_device max31827_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(max31827_channels),
	.channels = max31827_channels,
	.debug_reg_read = (int32_t (*)()) max31827_iio_reg_read,
	.debug_reg_write = (int32_t (*)()) max31827_iio_reg_write,
	.attributes = max31827_iio_ch_attrs,
};

/**
 * @brief Initializes the MAX31827 IIO driver
 * @param iio_device - The iio device structure.
 * @param iio_init_param - Parameters for the initialization of iio_dev
 * @return 0 in case of success, errno errors otherwise
 */
int max31827_iio_init(struct max31827_iio_device **iio_device,
		      struct max31827_iio_init_param *iio_init_param)
{
	struct max31827_iio_device *iio_device_temp;
	int ret;

	if (!iio_init_param || !iio_init_param->init_param)
		return -EINVAL;

	iio_device_temp = no_os_calloc(1, sizeof(*iio_device_temp));
	if (!iio_device_temp)
		return -ENOMEM;

	ret = max31827_init(&iio_device_temp->dev, iio_init_param->init_param);
	if (ret) {
		no_os_free(iio_device_temp);
		return ret;
	}

	iio_device_temp->iio_dev = &max31827_iio_dev;

	*iio_device = iio_device_temp;

	return 0;
}

/**
 * @brief Free resources allocated by the init function
 * @param iio_device - The iio device structure.
 * @return 0 in case of success, errno errors otherwise
 */
int max31827_iio_remove(struct max31827_iio_device *iio_device)
{
	int ret;

	ret = max31827_remove(iio_device->dev);
	if (ret)
		return ret;

	no_os_free(iio_device);

	return 0;
}
