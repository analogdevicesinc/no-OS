/***************************************************************************//**
 *   @file   iio_adt75.c
 *   @brief  Source file of IIO ADT75 Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include <string.h>
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio_adt75.h"
#include "adt75.h"
#include "iio.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int adt75_iio_read_raw(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv);
static int adt75_iio_read_scale(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int adt75_iio_read_offset(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int adt75_iio_read_samples(void *dev, int16_t *buf, uint32_t samples);

/******************************************************************************/
/************************ Variable Declarations *******************************/
/******************************************************************************/
static struct iio_attribute adt75_attrs[] = {
	{
		.name = "raw",
		.show = adt75_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adt75_iio_read_scale,
	},
	{
		.name = "offset",
		.show = adt75_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type adt75_scan = {
	.sign = 's',
	.realbits = 12,
	.storagebits = 16,
	.shift = 4,
	.is_big_endian = false,
};

static struct iio_channel adt75_channels[] = {
	{
		.name = "temp",
		.ch_type = IIO_TEMP,
		.ch_out = 0,
		.attributes = adt75_attrs,
		.scan_type = &adt75_scan,
	},
};

static struct iio_device adt75_iio_dev = {
	.channels = adt75_channels,
	.num_ch = 1,
	.read_dev = (int32_t (*)())adt75_iio_read_samples,
};

/**
 * @brief Read the raw attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int adt75_iio_read_raw(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct adt75_iio_desc *desc = dev;
	uint16_t reg_val;
	int32_t val;
	int ret;

	ret = adt75_reg_read(desc->adt75_desc, ADT75_TEMP_VALUE_REG, &reg_val);
	if (ret)
		return ret;

	reg_val = no_os_field_get(ADT75_TEMP_MASK, reg_val);
	val = no_os_sign_extend32(reg_val, ADT75_SIGN_BIT);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the scale attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int adt75_iio_read_scale(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val[2];

	val[0] = 62;
	val[1] = 500000;

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val);
}

/**
 * @brief Read the offset attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int adt75_iio_read_offset(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read a number of samples from the device.
 * @param dev - The iio device structure.
 * @param buf - Buffer to store the samples in.
 * @param samples - The number of samples
 * @return The number of samples in case of success, an error code otherwise
 */
static int adt75_iio_read_samples(void *dev, int16_t *buf, uint32_t samples)
{
	struct adt75_iio_desc *desc = dev;
	uint16_t val;
	uint32_t i;
	int ret;

	for (i = 0; i < samples; i++) {
		ret = adt75_reg_read(desc->adt75_desc, ADT75_TEMP_VALUE_REG, &val);
		if (ret)
			return ret;

		buf[i] = val;
	}

	return samples;
}

/**
 * @brief Initializes the ADT75 IIO descriptor.
 * @param desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int adt75_iio_init(struct adt75_iio_desc **desc,
		   struct adt75_iio_init_param *init_param)
{
	struct adt75_iio_desc *descriptor;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = adt75_init(&descriptor->adt75_desc, init_param->adt75_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &adt75_iio_dev;

	*desc = descriptor;

	return 0;

free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free an iio descriptor.
 * @param desc - The descriptor to be freed.
 * @return 0 in case of success, an error code otherwise.
 */
int adt75_iio_remove(struct adt75_iio_desc *desc)
{
	int ret;

	ret = adt75_remove(desc->adt75_desc);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
