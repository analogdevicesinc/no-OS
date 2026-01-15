/***************************************************************************//**
 *   @file   iio_lm75.c
 *   @brief  IIO Source file of LM75 Driver.
 *   @author Niel Acuna (niel.acuna@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 ********************************************************************************/
#include <errno.h>
#include "iio_lm75.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

static int lm75_ch_attr_show(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv);
static int lm75_ch_attr_store(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv);

#define DEFINE_LM75_CH_ATTR(_name, _priv) {\
	.name = _name, \
	.priv = _priv, \
	.show = lm75_ch_attr_show, \
	.store = lm75_ch_attr_store, \
}

/**
 * @brief Used to retrieve channel attribute data
 *
 * @param[in] device lm75 iio device descriptor.
 * @param[out] buf buffer where to write attribute data
 * @param[in] len maximum length of buffer
 * @param[in] channel channel information
 * @param[in] attribute identifier.
 *
 * @return <0 if failed.
 * @return length of buffer data on success.
 */
static int lm75_ch_attr_show(void *device, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	int err;
	uint16_t raw_temp;
	int millicel;
	enum lm75_temperature_type whence;
	struct iio_lm75 *iio_lm75 = device;
	struct lm75_dev *lm75 = iio_lm75->lm75;

	switch (priv) {
	case LM75_CH_ATTRIBUTE_INPUT:
		whence = lm75_die_temperature;
		break;
	case LM75_CH_ATTRIBUTE_MAX:
		whence = lm75_over_temperature;
		break;
	case LM75_CH_ATTRIBUTE_HYST:
		whence = lm75_hysteresis_temperature;
		break;
	default:
		return -ENODEV;
	}

	err = lm75_read_temperature(lm75, whence, &raw_temp);
	if (err)
		return err;

	millicel = lm75_raw_to_millicelsius(raw_temp);
	return sprintf(buf, "%d", millicel);
}

/**
 * @brief Used to store channel attribute data
 *
 * @param[in] device lm75 iio device descriptor.
 * @param[out] buf buffer where to write attribute data
 * @param[in] len maximum length of buffer
 * @param[in] channel channel information
 * @param[in] attribute identifier.
 *
 * @return <0 if failed.
 * @return maximum length of buffer on success.
 */
static int lm75_ch_attr_store(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	int err;
	uint16_t raw_temp;
	int millicel;
	char *endptr;
	enum lm75_temperature_type whence;
	struct iio_lm75 *iio_lm75 = device;
	struct lm75_dev *lm75 = iio_lm75->lm75;

	switch (priv) {
	case LM75_CH_ATTRIBUTE_INPUT:
		/* LM75 die temperature is a read only register */
		return -EINVAL;

	case LM75_CH_ATTRIBUTE_MAX:
		whence = lm75_over_temperature;
		break;
	case LM75_CH_ATTRIBUTE_HYST:
		whence = lm75_hysteresis_temperature;
		break;
	default:
		return -ENODEV;
	}

	millicel = strtol(buf, &endptr, 10);
	if (*endptr != '\0')
		return -EINVAL;

	/* LM75 operates only on this valid min-max range */
	if (millicel < LM75_MIN_TEMP_MILLICEL)
		return -ERANGE;
	if (millicel > LM75_MAX_TEMP_MILLICEL)
		return -ERANGE;

	raw_temp = lm75_millicelsius_to_raw(millicel);
	err = lm75_write_temperature(lm75, whence, raw_temp);
	if (err)
		return err;

	return len;
}

/**
 * @brief Used exclusively to report LM75 global device attributes.
 *
 * @param[in] device lm75 iio device descriptor.
 * @param[out] buf buffer where to write attribute data
 * @param[in] len maximum length of buffer
 * @param[in] channel channel information
 * @param[in] attribute identifier.
 *
 * @return 0 if failed.
 * @return length of buffer data on success.
 */
static int lm75_global_attr_show(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	switch (priv) {
	case LM75_UPDATE_INTERVAL:
		return sprintf(buf, "%d", LM75_UPDATE_INTERVAL);
	}
	return 0;
}

static struct iio_attribute lm75_ch_attrs[] = {
	DEFINE_LM75_CH_ATTR("input", LM75_CH_ATTRIBUTE_INPUT),
	DEFINE_LM75_CH_ATTR("max", LM75_CH_ATTRIBUTE_MAX),
	DEFINE_LM75_CH_ATTR("max_hyst", LM75_CH_ATTRIBUTE_HYST),
	END_ATTRIBUTES_ARRAY,
};

static struct iio_channel lm75_channels[] = {
	{
		.name = "temp",
		.ch_type = IIO_TEMP,
		.channel = 1,
		.indexed = true,
		.ch_out = false,
		.attributes = lm75_ch_attrs,
	},
};

static struct iio_attribute lm75_glob_attrs[] = {
	{
		.name = "update_interval",
		.priv = LM75_UPDATE_INTERVAL,
		.show = lm75_global_attr_show,
	},
	END_ATTRIBUTES_ARRAY,
};

/**
 * @brief Initialize the LM75 IIO device descriptor with channels and
 *        other attributes.
 *
 * @param[in,out] dev lm75 iio device descriptor.
 *
 * @return 0 if success.
 * @return -EINVAL if iiodev is NULL
 */
static int iio_lm75_prepare_iiodev(struct iio_device *iiodev)
{
	if (!iiodev)
		return -EINVAL;

	iiodev->num_ch = sizeof lm75_channels / sizeof lm75_channels[0];
	iiodev->channels = lm75_channels;

	/* okay this here is really just to align it with the linux version
	 * tied to the hwmon subsystem which exposes a device specific
	 * update_interval. so pyadi-iio scripts expecting this device
	 * attribute will not fail. */
	iiodev->attributes = lm75_glob_attrs;

	return 0;
}

/**
 * @brief Allocate and initialize resources used by LM75 IIO driver.
 *
 * @param[in] dev lm75 iio device descriptor.
 */
int iio_lm75_init(struct iio_lm75 **iio_lm75pp,
		  struct lm75_dev *dev)
{
	int err;
	struct iio_lm75 *liio_desc;

	if (!iio_lm75pp)
		return -EINVAL;
	if (!dev)
		return -EINVAL;

	liio_desc = no_os_calloc(1, sizeof * liio_desc);
	if (!liio_desc)
		return -ENOMEM;

	liio_desc->lm75 = dev;

	err = iio_lm75_prepare_iiodev(&liio_desc->iio_dev);
	if (err)
		goto err_cleanup;

	*iio_lm75pp = liio_desc;

	return 0;

err_cleanup:
	no_os_free(liio_desc);
	return err;
}

/**
 * @brief Release resources used by LM75 IIO driver back to system.
 *
 * @param[in] dev lm75 iio device descriptor.
 */
int iio_lm75_remove(struct iio_lm75 *iio_lm75)
{
	if (!iio_lm75)
		return -EINVAL;

	no_os_free(iio_lm75);

	return 0;
}
