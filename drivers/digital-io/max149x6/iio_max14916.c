/***************************************************************************//**
 *   @file   iio_max14916.c
 *   @brief  Source file of IIO MAX14916 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "iio_max14916.h"

#define MAX14916_CHANNEL(_addr)		\
	{				\
		.ch_type = IIO_VOLTAGE,	\
		.indexed = 1,		\
		.channel = _addr,	\
		.address = _addr,	\
	}

static int max14916_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int max14916_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

static int max14916_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

static int max14916_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int max14916_iio_reg_read(struct max14916_iio_desc *, uint32_t,
				 uint32_t *);

static int max14916_iio_reg_write(struct max14916_iio_desc *, uint32_t,
				  uint32_t );

static struct iio_attribute max14916_attrs[] = {
	{
		.name = "raw",
		.show = max14916_iio_read_raw,
		.store = max14916_iio_write_raw,
	},
	{
		.name = "offset",
		.show = max14916_iio_read_offset,
	},
	{
		.name = "scale",
		.show = max14916_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device max14916_iio_dev = {
	.debug_reg_read = (int32_t (*)())max14916_iio_reg_read,
	.debug_reg_write = (int32_t (*)())max14916_iio_reg_write,
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
static int max14916_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct max14916_iio_desc *desc = dev;
	uint32_t val;
	int ret;

	ret = max14916_ch_get(desc->max14916_desc, channel->address, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Write the raw attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int max14916_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct max14916_iio_desc *desc = dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return max14916_ch_set(desc->max14916_desc, channel->address, val);
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
static int max14916_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val = 0;

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
static int max14916_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int32_t val = 1;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

int max14916_iio_setup_channels(struct max14916_iio_desc *desc, bool *ch_enable)
{
	struct iio_channel *max14916_iio_channels;
	uint32_t enabled_ch = 0;
	uint32_t ch_offset = 0;
	uint32_t i;

	for (i = 0; i < MAX14916_CHANNELS; i++)
		if (ch_enable[i])
			enabled_ch++;

	max14916_iio_channels = no_os_calloc(enabled_ch,
					     sizeof(*max14916_iio_channels));
	if (!max14916_iio_channels)
		return -ENOMEM;

	for (i = 0; i < MAX14916_CHANNELS; i++) {
		if (ch_enable[i]) {
			max14916_iio_channels[ch_offset] = (struct iio_channel)MAX14916_CHANNEL(i);
			max14916_iio_channels[ch_offset].attributes = max14916_attrs;
			max14916_iio_channels[ch_offset].ch_out = 1;
			ch_offset++;
		}
	}

	desc->iio_dev->channels = max14916_iio_channels;
	desc->iio_dev->num_ch = enabled_ch;

	return 0;
}

/**
 * @brief Register read wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value
 * @return 0 in case of success, error code otherwise
 */
static int max14916_iio_reg_read(struct max14916_iio_desc *dev, uint32_t reg,
				 uint32_t *readval)
{
	return max149x6_reg_read(dev->max14916_desc, reg, readval);
}

/**
 * @brief Register write wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value
 * @return 0 in case of success, error code otherwise
 */
static int max14916_iio_reg_write(struct max14916_iio_desc *dev, uint32_t reg,
				  uint32_t writeval)
{
	return max149x6_reg_write(dev->max14916_desc, reg, writeval);
}

/**
 * @brief Initializes the MAX14916 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int max14916_iio_init(struct max14916_iio_desc **iio_desc,
		      struct max14916_iio_desc_init_param *init_param)
{
	struct max14916_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->max14916_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = max14916_init(&descriptor->max14916_desc,
			    init_param->max14916_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &max14916_iio_dev;

	ret = max14916_iio_setup_channels(descriptor, init_param->ch_enable);
	if (ret)
		goto free_dev;

	*iio_desc = descriptor;

	return 0;
free_dev:
	max14916_remove(descriptor->max14916_desc);
free_desc:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int max14916_iio_remove(struct max14916_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	no_os_free(iio_desc->iio_dev->channels);
	max14916_remove(iio_desc->max14916_desc);
	no_os_free(iio_desc);

	return 0;
}
