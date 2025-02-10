/***************************************************************************//**
*   @file   iio_lt3074.c
*   @brief  Source file for the LT3074 IIO Driver
*   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
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

#include "lt3074.h"
#include "iio_lt3074.h"

static const int32_t lt3074_margin_avail[] = {0, 1, 3, 5, 10, 15, 20, 25, 30};

enum lt3074_iio_chan_type {
	LT3074_IIO_VIN_CHAN,
	LT3074_IIO_VOUT_CHAN,
	LT3074_IIO_IOUT_CHAN,
	LT3074_IIO_TEMP_CHAN,
	LT3074_IIO_VBIAS_CHAN,
};

enum lt3074_iio_attr_id {
	LT3074_IIO_CHAN_ATTR_RAW,
	LT3074_IIO_CHAN_ATTR_SCALE,
	LT3074_IIO_GLOBAL_ATTR_VOUT_MARGIN_LOW,
	LT3074_IIO_GLOBAL_ATTR_VOUT_MARGIN_HIGH,
	LT3074_IIO_GLOBAL_ATTR_VOUT_MARGIN_AVAILABLE,
	LT3074_IIO_GLOBAL_ATTR_VOUT_OV_WARN_LIMIT = LT3074_VOUT_OV_WARN_LIMIT_TYPE,
	LT3074_IIO_GLOBAL_ATTR_VOUT_UV_WARN_LIMIT = LT3074_VOUT_UV_WARN_LIMIT_TYPE,
	LT3074_IIO_GLOBAL_ATTR_IOUT_OC_FAULT_LIMIT = LT3074_IOUT_OC_FAULT_LIMIT_TYPE,
	LT3074_IIO_GLOBAL_ATTR_OT_WARN_LIMIT = LT3074_OT_WARN_LIMIT_TYPE,
	LT3074_IIO_GLOBAL_ATTR_VIN_OV_WARN_LIMIT = LT3074_VIN_OV_WARN_LIMIT_TYPE,
	LT3074_IIO_GLOBAL_ATTR_VIN_UV_WARN_LIMIT = LT3074_VIN_UV_WARN_LIMIT_TYPE,
	LT3074_IIO_GLOBAL_ATTR_VBIAS_OV_WARN_LIMIT = LT3074_VBIAS_OV_WARN_LIMIT_TYPE,
	LT3074_IIO_GLOBAL_ATTR_VBIAS_UV_WARN_LIMIT = LT3074_VBIAS_UV_WARN_LIMIT_TYPE,
	LT3074_IIO_GLOBAL_ATTR_IOUT_MIN_WARN_LIMIT = LT3074_IOUT_MIN_WARN_LIMIT_TYPE,
	LT3074_IIO_DBG_ATTR_STATUS_BYTE = LT3074_STATUS_BYTE,
	LT3074_IIO_DBG_ATTR_STATUS_VOUT = LT3074_STATUS_VOUT,
	LT3074_IIO_DBG_ATTR_STATUS_IOUT = LT3074_STATUS_IOUT,
	LT3074_IIO_DBG_ATTR_STATUS_INPUT = LT3074_STATUS_INPUT,
	LT3074_IIO_DBG_ATTR_STATUS_TEMPERATURE = LT3074_STATUS_TEMPERATURE,
	LT3074_IIO_DBG_ATTR_STATUS_CML = LT3074_STATUS_CML,
	LT3074_IIO_DBG_ATTR_STATUS_MFR_SPECIFIC = LT3074_STATUS_MFR_SPECIFIC,
	LT3074_IIO_DBG_ATTR_STATUS_WORD = LT3074_STATUS_WORD,
};

static struct iio_device lt3074_iio_dev;

/**
 * @brief Handles read request for all attributes.
 *
 * @param dev - IIO device structure.
 * @param buf - Command buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return ret - Result of the reading procedure.
 */
static int lt3074_iio_attr_read(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;
	int ret, data;
	int32_t value = 0, vals[2];
	uint32_t i, regval;

	if (!dev)
		return -EINVAL;

	if (!lt3074)
		return -EINVAL;

	switch (priv) {
	case LT3074_IIO_CHAN_ATTR_RAW:
		switch (channel->address) {
		case LT3074_IIO_VIN_CHAN:
			ret = lt3074_read_value(lt3074, LT3074_VIN, &data);
			break;
		case LT3074_IIO_IOUT_CHAN:
			ret = lt3074_read_value(lt3074, LT3074_IOUT, &data);
			break;
		case LT3074_IIO_VOUT_CHAN:
			ret = lt3074_read_value(lt3074, LT3074_VOUT, &data);
			break;
		case LT3074_IIO_TEMP_CHAN:
			ret = lt3074_read_value(lt3074, LT3074_TEMP, &data);
			break;
		case LT3074_IIO_VBIAS_CHAN:
			ret = lt3074_read_value(lt3074, LT3074_VBIAS, &data);
			break;
		default:
			return -EINVAL;
		}
		if (ret)
			return ret;

		value = data;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);

	case LT3074_IIO_CHAN_ATTR_SCALE:
		vals[0] = 1;
		vals[1] = MILLI;

		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);

	case LT3074_IIO_GLOBAL_ATTR_VOUT_MARGIN_LOW:
	case LT3074_IIO_GLOBAL_ATTR_VOUT_MARGIN_HIGH:
		ret = lt3074_reg_read(lt3074, LT3074_MFR_MARGIN, &regval);
		if (ret)
			return ret;

		if (priv == LT3074_IIO_GLOBAL_ATTR_VOUT_MARGIN_LOW)
			regval = no_os_field_get(LT3074_MARGIN_LOW_MSK, regval);
		else
			regval = no_os_field_get(LT3074_MARGIN_HIGH_MSK, regval);

		regval = no_os_clamp(regval, 0, 8);

		return sprintf(buf, "%d ", lt3074_margin_avail[regval]);

	case LT3074_IIO_GLOBAL_ATTR_VOUT_MARGIN_AVAILABLE:
		for (i = 0; i < NO_OS_ARRAY_SIZE(lt3074_margin_avail); i++)
			value += sprintf(buf + value, "%d ",
					 lt3074_margin_avail[i]);

		return value;

	case LT3074_IIO_GLOBAL_ATTR_VOUT_OV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_VOUT_UV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_IOUT_OC_FAULT_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_OT_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_VIN_OV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_VIN_UV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_VBIAS_OV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_VBIAS_UV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_IOUT_MIN_WARN_LIMIT:
		ret = lt3074_read_word_data(lt3074, (uint32_t)priv, &data);
		if (ret)
			return ret;

		vals[0] = data / MILLI;
		vals[1] = data * MILLI % MICRO;

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO,
					2, vals);

	case LT3074_IIO_DBG_ATTR_STATUS_BYTE:
	case LT3074_IIO_DBG_ATTR_STATUS_VOUT:
	case LT3074_IIO_DBG_ATTR_STATUS_IOUT:
	case LT3074_IIO_DBG_ATTR_STATUS_INPUT:
	case LT3074_IIO_DBG_ATTR_STATUS_TEMPERATURE:
	case LT3074_IIO_DBG_ATTR_STATUS_CML:
	case LT3074_IIO_DBG_ATTR_STATUS_MFR_SPECIFIC:
		ret = lt3074_reg_read(lt3074, priv, &regval);
		if (ret)
			return ret;

		value = regval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);

	case LT3074_IIO_DBG_ATTR_STATUS_WORD:
		ret = lt3074_reg_read(lt3074, LT3074_STATUS_WORD, &regval);
		if (ret)
			return ret;

		value = regval;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);

	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles write request for all attributes.
 *
 * @param dev - IIO device structure.
 * @param buf - Command buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return ret - Result of the writing procedure.
 */
static int lt3074_iio_attr_write(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct lt3074_iio_desc *iio_lt3074 = dev;
	struct lt3074_dev *lt3074 = iio_lt3074->lt3074_dev;
	int ret;
	int32_t val1, val2;
	uint32_t i;
	uint8_t byte;

	if (!dev)
		return -EINVAL;

	if (!lt3074)
		return -EINVAL;

	switch (priv) {
	case LT3074_IIO_GLOBAL_ATTR_VOUT_MARGIN_LOW:
	case LT3074_IIO_GLOBAL_ATTR_VOUT_MARGIN_HIGH:
		iio_parse_value(buf, IIO_VAL_INT, &val1, NULL);

		for (i = 0; i < NO_OS_ARRAY_SIZE(lt3074_margin_avail); i++)
			if (val1 == lt3074_margin_avail[i])
				break;

		if (i == NO_OS_ARRAY_SIZE(lt3074_margin_avail))
			return -EINVAL;

		ret = lt3074_read_byte(lt3074, LT3074_MFR_MARGIN, &byte);
		if (ret)
			return ret;

		if (priv == LT3074_IIO_GLOBAL_ATTR_VOUT_MARGIN_LOW) {
			byte &= ~LT3074_MARGIN_LOW_MSK;
			byte |= no_os_field_prep(LT3074_MARGIN_LOW_MSK, i);
		} else {
			byte &= ~LT3074_MARGIN_HIGH_MSK;
			byte |= no_os_field_prep(LT3074_MARGIN_HIGH_MSK, i);
		}

		return lt3074_write_byte(lt3074, LT3074_MFR_MARGIN, byte);

	case LT3074_IIO_GLOBAL_ATTR_VOUT_OV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_VOUT_UV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_IOUT_OC_FAULT_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_OT_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_VIN_OV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_VIN_UV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_VBIAS_OV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_VBIAS_UV_WARN_LIMIT:
	case LT3074_IIO_GLOBAL_ATTR_IOUT_MIN_WARN_LIMIT:
		iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val1, &val2);

		val1 = val1 * MILLI + val2 / MILLI;

		return lt3074_set_limit(lt3074, (enum lt3074_limit_type)priv,
					val1);

	default:
		return -EINVAL;
	}
}

/**
 * @brief Initializes the LT3074 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int lt3074_iio_init(struct lt3074_iio_desc **iio_desc,
		    struct lt3074_iio_desc_init_param *init_param)
{
	struct lt3074_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->lt3074_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = lt3074_init(&descriptor->lt3074_dev,
			  init_param->lt3074_init_param);
	if (ret)
		goto dev_err;

	descriptor->iio_dev = &lt3074_iio_dev;

	*iio_desc = descriptor;

	return 0;

dev_err:
	lt3074_iio_remove(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int lt3074_iio_remove(struct lt3074_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	lt3074_remove(iio_desc->lt3074_dev);
	no_os_free(iio_desc);

	return 0;
}

#define LT3074_IIO_CHAN_ATTR(_name, _value) \
	{ \
		.name = #_name, \
		.show = lt3074_iio_attr_read, \
		.priv = LT3074_IIO_CHAN_ATTR_##_value, \
	}

#define LT3074_IIO_GLOBAL_ATTR(_name, _value) \
	{ \
		.name = #_name, \
		.show = lt3074_iio_attr_read, \
		.store = lt3074_iio_attr_write, \
		.priv = LT3074_IIO_GLOBAL_ATTR_##_value, \
	}

#define LT3074_IIO_DBG_ATTR(_name, _value) \
	{ \
		.name = #_name, \
		.show = lt3074_iio_attr_read, \
		.priv = LT3074_IIO_DBG_ATTR_##_value, \
	}

static struct iio_attribute lt3074_chan_attrs[] = {
	LT3074_IIO_CHAN_ATTR(raw, RAW),
	LT3074_IIO_CHAN_ATTR(scale, SCALE),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt3074_global_attrs[] = {
	LT3074_IIO_GLOBAL_ATTR(vout_margin_low, VOUT_MARGIN_LOW),
	LT3074_IIO_GLOBAL_ATTR(vout_margin_high, VOUT_MARGIN_HIGH),
	LT3074_IIO_GLOBAL_ATTR(vout_margin_available, VOUT_MARGIN_AVAILABLE),
	LT3074_IIO_GLOBAL_ATTR(vout_ov_warn_limit, VOUT_OV_WARN_LIMIT),
	LT3074_IIO_GLOBAL_ATTR(vout_uv_warn_limit, VOUT_UV_WARN_LIMIT),
	LT3074_IIO_GLOBAL_ATTR(iout_oc_fault_limit, IOUT_OC_FAULT_LIMIT),
	LT3074_IIO_GLOBAL_ATTR(ot_warn_limit, OT_WARN_LIMIT),
	LT3074_IIO_GLOBAL_ATTR(vin_ov_warn_limit, VIN_OV_WARN_LIMIT),
	LT3074_IIO_GLOBAL_ATTR(vin_uv_warn_limit, VIN_UV_WARN_LIMIT),
	LT3074_IIO_GLOBAL_ATTR(vbias_ov_warn_limit, VBIAS_OV_WARN_LIMIT),
	LT3074_IIO_GLOBAL_ATTR(vbias_uv_warn_limit, VBIAS_UV_WARN_LIMIT),
	LT3074_IIO_GLOBAL_ATTR(iout_min_warn_limit, IOUT_MIN_WARN_LIMIT),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt3074_debug_attrs[] = {
	LT3074_IIO_DBG_ATTR(status_byte, STATUS_BYTE),
	LT3074_IIO_DBG_ATTR(status_vout, STATUS_VOUT),
	LT3074_IIO_DBG_ATTR(status_iout, STATUS_IOUT),
	LT3074_IIO_DBG_ATTR(status_input, STATUS_INPUT),
	LT3074_IIO_DBG_ATTR(status_temperature, STATUS_TEMPERATURE),
	LT3074_IIO_DBG_ATTR(status_cml, STATUS_CML),
	LT3074_IIO_DBG_ATTR(status_mfr_specific, STATUS_MFR_SPECIFIC),
	LT3074_IIO_DBG_ATTR(status_word, STATUS_WORD),
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel lt3074_channels[] = {
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LT3074_IIO_VIN_CHAN,
		.address = LT3074_IIO_VIN_CHAN,
		.attributes = lt3074_chan_attrs,
		.ch_out = false
	},
	{
		.name = "vout",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LT3074_IIO_VOUT_CHAN,
		.address = LT3074_IIO_VOUT_CHAN,
		.attributes = lt3074_chan_attrs,
		.ch_out = true,
	},
	{
		.name = "iout",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LT3074_IIO_IOUT_CHAN,
		.address = LT3074_IIO_IOUT_CHAN,
		.attributes = lt3074_chan_attrs,
		.ch_out = true,
	},
	{
		.name = "temperature",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = LT3074_IIO_TEMP_CHAN,
		.address = LT3074_IIO_TEMP_CHAN,
		.attributes = lt3074_chan_attrs,
		.ch_out = false,
	},
	{
		.name = "vbias",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LT3074_IIO_VBIAS_CHAN,
		.address = LT3074_IIO_VBIAS_CHAN,
		.attributes = lt3074_chan_attrs,
		.ch_out = false,
	},
};

static struct iio_device lt3074_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(lt3074_channels),
	.channels = lt3074_channels,
	.attributes = lt3074_global_attrs,
	.debug_attributes = lt3074_debug_attrs,
};
