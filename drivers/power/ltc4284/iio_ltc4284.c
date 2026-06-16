/***************************************************************************//**
 *   @file   iio_ltc4284.c
 *   @brief  Implementation of LTC4284 IIO Driver
 *   @author Carlos Jones Jr <carlos.jones.jr@analog.com>
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#include <errno.h>
#include "iio_ltc4284.h"
#include "no_os_alloc.h"
#include "no_os_units.h"

/**
 * @brief Read raw value from LTC4284 IIO channel
 * @param dev - IIO device descriptor
 * @param buf - Output buffer
 * @param len - Buffer length
 * @param channel - IIO channel info
 * @param priv - Private data
 * @return Number of bytes written, or negative error code
 */
static int ltc4284_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ltc4284_iio_desc *iio_ltc4284 = dev;
	struct ltc4284_dev *ltc4284 = iio_ltc4284->ltc4284_dev;
	int32_t vals[2];
	uint16_t value16;
	uint32_t value32;
	uint64_t value64;
	int ret;

	switch (channel->address) {
	case LTC4284_IIO_VIN_CHAN:
		ret = ltc4284_read_vin(ltc4284, &value16);
		vals[0] = (int32_t)value16;
		break;
	case LTC4284_IIO_IIN_CHAN:
		ret = ltc4284_read_iin(ltc4284, &value16);
		vals[0] = (int32_t)value16;
		break;
	case LTC4284_IIO_VOUT_CHAN:
		ret = ltc4284_read_vout(ltc4284, &value16);
		vals[0] = (int32_t)value16;
		break;
	case LTC4284_IIO_POWER_CHAN:
		ret = ltc4284_read_power(ltc4284, &value32);
		vals[0] = (int32_t)value32;
		break;
	case LTC4284_IIO_ENERGY_CHAN:
		ret = ltc4284_read_energy(ltc4284, &value64);
		vals[0] = (int32_t)value64;
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);
}

/**
 * @brief Read scale value from LTC4284 IIO channel
 * @param dev - IIO device descriptor
 * @param buf - Output buffer
 * @param len - Buffer length
 * @param channel - IIO channel info
 * @param priv - Private data
 * @return Number of bytes written, or negative error code
 */
static int ltc4284_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int32_t vals[2];

	/* Scale factor: raw value (in mV/mA/mW/mJ) to base units (V/A/W/J)
	 * Scale = 1/1000 for all channels
	 */
	vals[0] = 1;
	vals[1] = (int32_t)MILLI;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

/**
 * @brief IIO channel attributes for voltage (input)
 */
static struct iio_attribute ltc4284_voltage_input_attrs[] = {
	{
		.name = "raw",
		.show = ltc4284_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltc4284_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

/**
 * @brief IIO channel attributes for current (input)
 */
static struct iio_attribute ltc4284_current_input_attrs[] = {
	{
		.name = "raw",
		.show = ltc4284_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltc4284_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

/**
 * @brief IIO channel attributes for power
 */
static struct iio_attribute ltc4284_power_attrs[] = {
	{
		.name = "raw",
		.show = ltc4284_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltc4284_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

/**
 * @brief IIO channel attributes for energy
 */
static struct iio_attribute ltc4284_energy_attrs[] = {
	{
		.name = "raw",
		.show = ltc4284_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltc4284_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

/**
 * @brief LTC4284 IIO channels definition
 */
static struct iio_channel ltc4284_channels[] = {
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 0,
		.address = LTC4284_IIO_VIN_CHAN,
		.attributes = ltc4284_voltage_input_attrs,
		.ch_out = false
	},
	{
		.name = "iin",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = 0,
		.address = LTC4284_IIO_IIN_CHAN,
		.attributes = ltc4284_current_input_attrs,
		.ch_out = false
	},
	{
		.name = "vout",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 1,
		.address = LTC4284_IIO_VOUT_CHAN,
		.attributes = ltc4284_voltage_input_attrs,
		.ch_out = false
	},
	{
		.name = "power",
		.ch_type = IIO_POWER,
		.indexed = 1,
		.channel = 0,
		.address = LTC4284_IIO_POWER_CHAN,
		.attributes = ltc4284_power_attrs,
		.ch_out = false
	},
	{
		.name = "energy",
		.ch_type = IIO_POWER,
		.indexed = 1,
		.channel = 0,
		.address = LTC4284_IIO_ENERGY_CHAN,
		.attributes = ltc4284_energy_attrs,
		.ch_out = false
	}
};

/**
 * @brief LTC4284 IIO device structure
 */
static struct iio_device ltc4284_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ltc4284_channels),
	.channels = ltc4284_channels,
};

/**
 * @brief Initialize LTC4284 IIO interface
 * @param iio_desc - Pointer to IIO descriptor pointer
 * @param init_param - Initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_iio_init(struct ltc4284_iio_desc **iio_desc,
		     const struct ltc4284_iio_init_param *init_param)
{
	struct ltc4284_iio_desc *desc;
	int ret;

	if (!iio_desc || !init_param || !init_param->ltc4284_init)
		return -EINVAL;

	desc = (struct ltc4284_iio_desc *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	ret = ltc4284_init(&desc->ltc4284_dev, init_param->ltc4284_init);
	if (ret)
		goto error_desc;

	desc->iio_dev = &ltc4284_iio_dev;

	*iio_desc = desc;

	return 0;

error_desc:
	no_os_free(desc);
	return ret;
}

/**
 * @brief Remove LTC4284 IIO interface
 * @param desc - IIO descriptor
 * @return 0 in case of success, negative error code otherwise
 */
int ltc4284_iio_remove(struct ltc4284_iio_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = ltc4284_remove(desc->ltc4284_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
