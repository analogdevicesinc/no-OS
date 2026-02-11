/***************************************************************************//**
 *   @file   iio_tmc5240.c
 *   @brief  Implementation of IIO TMC5240 driver.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "no_os_spi.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "iio.h"
#include "iio_tmc5240.h"
#include "tmc5240.h"

static int tmc5240_iio_reg_read(struct tmc5240_iio_dev *dev, uint32_t reg,
				uint32_t *readval);

static int tmc5240_iio_reg_write(struct tmc5240_iio_dev *dev, uint32_t reg,
				 uint32_t writeval);

static int tmc5240_iio_attr_store(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int tmc5240_iio_attr_show(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static const char* const tmc5240_rampmode_avail[] = {
	[TMC5240_RAMPMODE_POSITION] = "position",
	[TMC5240_RAMPMODE_POSITIVE_VELOCITY] = "velocity_positive",
	[TMC5240_RAMPMODE_NEGATIVE_VELOCITY] = "velocity_negative",
	[TMC5240_RAMPMODE_HOLD] = "hold",
	NULL
};

static struct iio_attribute tmc5240_iio_attrs[] = {
	{
		.name = "direction",
		.priv = TMC5240_ATTR_DIRECTION,
		.store = tmc5240_iio_attr_store,
		.show = tmc5240_iio_attr_show,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute tmc5240_iio_debug_attrs[] = {
	{
		.name = "stop",
		.priv = TMC5240_ATTR_STOP,
		.store = tmc5240_iio_attr_store,
		.show = NULL,
	},
	{
		.name = "amax",
		.priv = TMC5240_ATTR_AMAX,
		.store = tmc5240_iio_attr_store,
		.show = tmc5240_iio_attr_show,
	},
	{
		.name = "vmax",
		.priv = TMC5240_ATTR_VMAX,
		.store = tmc5240_iio_attr_store,
		.show = tmc5240_iio_attr_show,
	},
	{
		.name = "dmax",
		.priv = TMC5240_ATTR_DMAX,
		.store = tmc5240_iio_attr_store,
		.show = tmc5240_iio_attr_show,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute tmc5240_iio_ch_attrs[] = {
	{
		.name = "raw",
		.priv = TMC5240_CH_ATTR_RAW,
		.store = tmc5240_iio_attr_store,
		.show = tmc5240_iio_attr_show,
	},
	{
		.name = "scale",
		.priv = TMC5240_CH_ATTR_SCALE,
		.store = NULL,
		.show = tmc5240_iio_attr_show,

	},
	{
		.name = "calibscale",
		.priv = TMC5240_CH_ATTR_CALIBSCALE,
		.store = tmc5240_iio_attr_store,
		.show = tmc5240_iio_attr_show,
		.shared = IIO_SHARED_BY_ALL,

	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute tmc5240_iio_position_attrs[] = {
	{
		.name = "raw",
		.priv = TMC5240_CH_ATTR_RAW,
		.store = tmc5240_iio_attr_store,
		.show = tmc5240_iio_attr_show,
	},
	{
		.name = "scale",
		.priv = TMC5240_CH_ATTR_SCALE,
		.store = NULL,
		.show = tmc5240_iio_attr_show,

	},
	{
		.name = "calibscale",
		.priv = TMC5240_CH_ATTR_CALIBSCALE,
		.store = tmc5240_iio_attr_store,
		.show = tmc5240_iio_attr_show,
		.shared = IIO_SHARED_BY_ALL,

	},
	{
		.name = "preset",
		.priv = TMC5240_CH_ATTR_PRESET,
		.store = tmc5240_iio_attr_store,
		.show = tmc5240_iio_attr_show,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel tmc5240_iio_channels[] = {
	{
		.name = "angular_velocity",
		.ch_type = IIO_ANGL_VEL,
		.channel = 0,
		.attributes = tmc5240_iio_ch_attrs,
		.ch_out = true,
	},
	{
		.name = "angular_acceleration",
		.ch_type = IIO_ACCEL,
		.channel = 1,
		.attributes = tmc5240_iio_ch_attrs,
		.ch_out = true,
	},
	{
		.name = "angular_position",
		.ch_type = IIO_ANGL,
		.channel = 2,
		.attributes = tmc5240_iio_position_attrs,
		.ch_out = true,
	},
};

static struct iio_device tmc5240_iio_dev = {
	.attributes = tmc5240_iio_attrs,
	.debug_attributes = tmc5240_iio_debug_attrs,
	.channels = tmc5240_iio_channels,
	.num_ch = NO_OS_ARRAY_SIZE(tmc5240_iio_channels),
	.debug_reg_read = (int32_t (*)())tmc5240_iio_reg_read,
	.debug_reg_write = (int32_t (*)())tmc5240_iio_reg_write,
};

/**
 * @brief Initializes the tmc5240 IIO driver
 * @param iio_dev - The iio device structure.
 * @param init_param - Parameters for the initialization of iio_dev
 *
 * @return 0 in case of success, errno errors otherwise
 */
int tmc5240_iio_init(struct tmc5240_iio_dev **iio_dev,
		     struct tmc5240_iio_dev_init_param *init_param)
{
	int i, ret;
	struct tmc5240_iio_dev *descriptor;

	if (!init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = tmc5240_init(&descriptor->tmc5240_dev,
			   init_param->tmc5240_init_param);
	if (ret)
		goto init_spi_err;

	descriptor->iio_dev = &tmc5240_iio_dev;

	*iio_dev = descriptor;

	return 0;

init_spi_err:
	tmc5240_remove(descriptor->tmc5240_dev);
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - The iio device structure.
 *
 * @return 0 in case of success, errno errors otherwise
 */
int tmc5240_iio_remove(struct tmc5240_iio_dev *desc)
{
	int ret;

	ret = tmc5240_remove(desc->tmc5240_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

/**
 * @brief tmc5240 reg read wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 *
 * @return 0 in case of success, errno errors otherwise
 */
static int tmc5240_iio_reg_read(struct tmc5240_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	if (!dev)
		return -ENODEV;

	return tmc5240_read(dev->tmc5240_dev, reg, readval);
}

/**
 * @brief tmc5240 reg write wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 *
 * @return 0 in case of success, errno errors otherwise
 */
static int tmc5240_iio_reg_write(struct tmc5240_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	if (!dev)
		return -ENODEV;

	return tmc5240_write(dev->tmc5240_dev, reg, writeval);

}

/**
 * @brief Handles the read request for an attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int tmc5240_iio_attr_show(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	uint64_t clock_sq;
	uint64_t numerator;
	int ret;
	int32_t vals[2];


	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	switch(priv) {
	case TMC5240_ATTR_DIRECTION:
		bool dir;
		ret = tmc5240_get_shaft_dir(iio_tmc5240->tmc5240_dev, &dir);
		if (ret)	
			return ret;

		vals[0] = dir ? 1 : 0;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	case TMC5240_ATTR_AMAX:
		ret = tmc5240_get_amax(iio_tmc5240->tmc5240_dev, &vals[0]);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	case TMC5240_ATTR_VMAX:
		ret = tmc5240_get_vmax(iio_tmc5240->tmc5240_dev, &vals[0]);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	case TMC5240_ATTR_DMAX:
		ret = tmc5240_get_dmax(iio_tmc5240->tmc5240_dev, &vals[0]);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	case TMC5240_CH_ATTR_RAW:
		switch (channel->type) {
		case IIO_ACCEL:
			ret = tmc5240_get_current_accel(iio_tmc5240->tmc5240_dev,
								&vals[0]);
			if (ret)
				return ret;
			break;
		case IIO_ANGL_VEL:
			ret = tmc5240_get_current_vel(iio_tmc5240->tmc5240_dev,
						      &vals[0]);
			if (ret)
				return ret;
			break;
		case IIO_ANGL:
			ret = tmc5240_get_current_pos(iio_tmc5240->tmc5240_dev,
						      &vals[0]);
			if (ret)
				return ret;
			break;
		default:
			return -ENOTSUP;
		}

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	case TMC5240_CH_ATTR_SCALE:
		switch (channel->type) {
		case IIO_ACCEL:
			clock_sq = (uint64_t)iio_tmc5240->tmc5240_dev->clock *
				   iio_tmc5240->tmc5240_dev->clock;
			vals[0] = (int32_t)(clock_sq >> 40);
			vals[1] = 16 - iio_tmc5240->tmc5240_dev->microsteps_res;

			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2,
						2, vals);
		case IIO_ANGL_VEL:
			vals[0] = (iio_tmc5240->tmc5240_dev->clock);
			vals[1] = 32 - iio_tmc5240->tmc5240_dev->microsteps_res;

			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2,
						2, vals);
		case IIO_ANGL:
			/* Position scale: convert XACTUAL (microsteps) to degrees */
			vals[0] = 1;
			vals[1] = 1 << (8 - iio_tmc5240->tmc5240_dev->microsteps_res);

			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2,
						vals);
		default:
			return -ENOTSUP;
		}
	case TMC5240_CH_ATTR_CALIBSCALE:
		vals[0] = iio_tmc5240->tmc5240_dev->step_angle_millidegrees / 1000;
		vals[1] = iio_tmc5240->tmc5240_dev->step_angle_millidegrees % 1000 * 1000000;

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 1, vals);
	case TMC5240_CH_ATTR_PRESET:
		ret = tmc5240_get_current_pos(iio_tmc5240->tmc5240_dev, &vals[0]);
		if (ret)
			return ret;
	
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals[0]);
	default:
		return -ENOTSUP;
	}
}

/**
 * @brief Handles the write request for attribute.
 *
 * @param device - The iio device structure
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int tmc5240_iio_attr_store(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct tmc5240_iio_dev *iio_tmc5240;
	int ret;
	int32_t val;
	int32_t val2;

	if (!dev)
		return -ENODEV;

	iio_tmc5240 = (struct tmc5240_iio_dev *)dev;

	switch (priv) {
	case TMC5240_ATTR_DIRECTION:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		if (val != 0 && val != 1)
			return -EINVAL;

		ret = tmc5240_set_shaft_dir(iio_tmc5240->tmc5240_dev, (bool)val);
		if (ret)
			return ret;

		break;
	case TMC5240_ATTR_AMAX:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = tmc5240_set_amax(iio_tmc5240->tmc5240_dev, val);
		if (ret)
			return ret;

		iio_tmc5240->tmc5240_dev->amax = val;
		break;
	case TMC5240_ATTR_VMAX:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = tmc5240_set_vmax(iio_tmc5240->tmc5240_dev, val);
		if (ret)
			return ret;

		iio_tmc5240->tmc5240_dev->vmax = val;
		break;
	case TMC5240_ATTR_DMAX:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;
		ret = tmc5240_set_dmax(iio_tmc5240->tmc5240_dev, val);
		if (ret)
			return ret;

		iio_tmc5240->tmc5240_dev->dmax = val;
		break;
	case TMC5240_CH_ATTR_RAW:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		switch (channel->type) {
		case IIO_ACCEL:
			ret = tmc5240_set_amax(iio_tmc5240->tmc5240_dev, val);
			if (ret)
				return ret;

			break;
		case IIO_ANGL_VEL:
			ret = tmc5240_set_target_vel(iio_tmc5240->tmc5240_dev,
						     val);
			if (ret)
				return ret;

			break;
		case IIO_ANGL:
			ret = tmc5240_set_target_pos(iio_tmc5240->tmc5240_dev,
						     val);
			if (ret)
				return ret;

			break;
		default:
			return -ENOTSUP;
		}
		break;
	case TMC5240_CH_ATTR_CALIBSCALE:
		ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val, &val2);
		if (ret)
			return ret;

		val = val * 1000 + (float)val2 / 1000.0;
		iio_tmc5240->tmc5240_dev->step_angle_millidegrees = val;
		break;
	case TMC5240_CH_ATTR_PRESET:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = tmc5240_set_current_pos(iio_tmc5240->tmc5240_dev, val);
		if (ret)
			return ret;
		break;
	case TMC5240_ATTR_STOP:
		/* Buffer must contain "1" to issue disable */
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (val != 1)
			return -EINVAL;

		ret = tmc5240_stop(iio_tmc5240->tmc5240_dev);
		if (ret)
			return ret;
		break;
	default:
		return -ENOTSUP;
	
	}
	return len;
}
