/***************************************************************************//**
 *   @file   iio_ltc2983.c
 *   @brief  Implementation of IIO LTC2983 driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <errno.h>
#include "iio_ltc2983.h"
#include "ltc2983.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio.h"

/******************************************************************************/
/************************** Macros Definitions ********************************/
/******************************************************************************/

#define LTC2983_CHAN(_type, _index) ({ \
	struct iio_channel __chan = { \
		.ch_type = _type, \
		.indexed = true, \
		.channel = _index, \
		.attributes = ltc2983_iio_attrs, \
		.address = _index, \
	}; \
	__chan; \
})

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

static int ltc2983_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv);
static int ltc2983_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);
static int ltc2983_iio_reg_read(struct ltc2983_iio_desc *dev, uint32_t reg,
				uint32_t *readval);
static int ltc2983_iio_reg_write(struct ltc2983_iio_desc *dev, uint32_t reg,
				 uint32_t writeval);

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

static struct iio_attribute ltc2983_iio_attrs[] = {
	{
		.name = "raw",
		.shared = IIO_SEPARATE,
		.show = ltc2983_iio_read_raw,
		.store = NULL,
	},
	{
		.name = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show = ltc2983_iio_read_scale,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device ltc2983_iio_dev = {
	.debug_reg_read = (int32_t (*)())ltc2983_iio_reg_read,
	.debug_reg_write = (int32_t (*)())ltc2983_iio_reg_write,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Initializes the LTC2983 IIO driver
 * @param iio_dev - The iio device structure.
 * @param init_param - Parameters for the initialization of iio_dev
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_iio_init(struct ltc2983_iio_desc **iio_dev,
		     struct ltc2983_iio_desc_init_param *init_param)
{
	int ret, i, chan = 0;
	struct ltc2983_iio_desc *descriptor;
	struct iio_channel *ltc2983_channels;
	enum iio_chan_type ch_type;

	if (!init_param || !init_param->ltc2983_desc_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = ltc2983_init(&descriptor->ltc2983_dev,
			   init_param->ltc2983_desc_init_param);
	if (ret)
		goto free_desc;

	descriptor->iio_dev = &ltc2983_iio_dev;

	ltc2983_channels = no_os_calloc(descriptor->ltc2983_dev->num_channels,
					sizeof(*ltc2983_channels));
	if (!ltc2983_channels) {
		ret = -ENOMEM;
		goto free_dev;
	}

	for (i = 0; i < descriptor->ltc2983_dev->max_channels_nr; i++) {
		if (descriptor->ltc2983_dev->sensors[i] &&
		    descriptor->ltc2983_dev->sensors[i]->type !=
		    LTC2983_RSENSE) {
			if (descriptor->ltc2983_dev->sensors[i]->type ==
			    LTC2983_DIRECT_ADC)
				ch_type = IIO_VOLTAGE;
			else
				ch_type = IIO_TEMP;

			ltc2983_channels[chan++] = LTC2983_CHAN(ch_type, i + 1);
		}
	}

	descriptor->iio_dev->num_ch = descriptor->ltc2983_dev->num_channels;
	descriptor->iio_dev->channels = ltc2983_channels;

	*iio_dev = descriptor;

	return 0;
free_dev:
	ltc2983_remove(descriptor->ltc2983_dev);
free_desc:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - The iio device structure.
 * @return 0 in case of success, errno errors otherwise
 */
int ltc2983_iio_remove(struct ltc2983_iio_desc *desc)
{
	int ret;

	ret = ltc2983_remove(desc->ltc2983_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return        - 0 in case of success, errno errors otherwise
*/
static int ltc2983_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	int ret;
	uint32_t uval;
	int32_t val;
	struct ltc2983_iio_desc *ltc2983_iio = dev;

	ret = ltc2983_chan_read_raw(ltc2983_iio->ltc2983_dev, channel->address,
				    &uval);
	if (ret)
		return ret;

	val = no_os_sign_extend32(uval, 23);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the read request for scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return        - 0 in case of success, errno errors otherwise
*/
static int ltc2983_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int ret;
	uint32_t val[2];
	struct ltc2983_iio_desc *ltc2983_iio = dev;

	ret = ltc2983_chan_read_scale(ltc2983_iio->ltc2983_dev,
				      channel->address, &val[0], &val[1]);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 1,
				(int32_t *)&val);
}

/**
 * @brief LTC2983 IIO reg read wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address (unused)
 * @param readval - Register value
 * @return 0 in case of success, errno errors otherwise
 */
static int ltc2983_iio_reg_read(struct ltc2983_iio_desc *dev, uint32_t reg,
				uint32_t *readval)
{
	return ltc2983_reg_read(dev->ltc2983_dev, (uint16_t)reg,
				(uint8_t*)readval);
}

/**
 * @brief LTC2983 IIO reg write wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address (unused)
 * @param writeval - Register value
 * @return 0 in case of success, errno errors otherwise
 */
static int ltc2983_iio_reg_write(struct ltc2983_iio_desc *dev, uint32_t reg,
				 uint32_t writeval)
{
	return ltc2983_reg_write(dev->ltc2983_dev, (uint16_t)reg,
				 (uint8_t)writeval);
}
