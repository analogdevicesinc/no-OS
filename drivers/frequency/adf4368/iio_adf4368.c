/***************************************************************************//**
 *   @file   iio_adf4368.c
 *   @brief  Implementation of IIO ADF4368 Driver.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
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
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_adf4368.h"
#include "adf4368.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

/**
 * @brief Supported charge pump currents
 */
static const int ci_table[16][2] = {
	{0, 790000},
	{0, 990000},
	{1, 190000},
	{1, 380000},
	{1, 590000},
	{1, 980000},
	{2, 390000},
	{2, 790000},
	{3, 180000},
	{3, 970000},
	{4, 770000},
	{5, 570000},
	{6, 330000},
	{7, 910000},
	{9, 510000},
	{11, 100000}
};

/**
 * @brief Helper function to read a pair of values in a table with 2 elements.
 * @param tb	- Input table.
 * @param size	- Size of the table.
 * @param val1	- The first value to be found.
 * @param val2	- The second value to be found.
 * @return i	- The index of the pair if found or an error if there is no
 * 		  pair of values in the table.
 */
static int adf4368_iio_find_2d_row(const int (*tbl)[2], const int size,
				   const int val1, const int val2)
{
	int i;

	for (i = 0; i < size; i++) {
		if (tbl[i][0] == val1 && tbl[i][1] == val2)
			return i;
	}

	return -EINVAL;
}

/**
 * @brief Wrapper for reading adf4368 register.
 * @param dev	  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 * @return	  - Result of the reading procedure.
 */
static int adf4368_iio_read_reg(struct adf4368_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	uint8_t tmp;
	int ret;

	ret = adf4368_spi_read(dev->adf4368_dev, (uint16_t)reg, &tmp);
	if (ret)
		return ret;

	*readval = tmp;
	return 0;
}

/**
 * @brief Wrapper for writing adf4368 register.
 * @param dev	   - The iio device structure.
 * @param reg	   - Address of the register to be written to.
 * @param writeval - Data to be written.
 * @return	   - Result of the writing procedure.
 */
static int adf4368_iio_write_reg(struct adf4368_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	return adf4368_spi_write(dev->adf4368_dev, (uint16_t)reg,
				 (uint8_t)writeval);
}

/**
 * @brief Handles the read request for the device attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int adf4368_iio_read_device_attr(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{

	struct adf4368_iio_dev *iio_adf4368 = (struct adf4368_iio_dev *)dev;
	struct adf4368_dev *adf4368;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int32_t temperature;
	int32_t val_b[2];
	char buffer[5];
	uint8_t check;
	int32_t cp_i;
	uint8_t i;
	bool en;
	int ret;


	if (!iio_adf4368)
		return -EINVAL;

	adf4368 = iio_adf4368->adf4368_dev;

	if (!adf4368)
		return -EINVAL;

	switch (priv) {
	case ADF4368_IIO_DEV_ATTR_BLEED:
		ret = adf4368_get_bleed_word(adf4368, &val);
		if (ret)
			return ret;

		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4368_IIO_DEV_ATTR_CP:

		ret = adf4368_get_cp_i(adf4368, &cp_i);
		if (ret)
			return ret;

		val_b[0] = ci_table[cp_i][0];
		val_b[1] = ci_table[cp_i][1];
		ret = iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val_b);
		break;

	case ADF4368_IIO_DEV_ATTR_CP_AVAIL:
		strcpy(buf, "");
		for (i = 0; i < NO_OS_ARRAY_SIZE(ci_table); i++) {
			val_b[0] = ci_table[i][0];
			val_b[1] = ci_table[i][1];
			iio_format_value(buffer, len, IIO_VAL_INT_PLUS_MICRO, 2, val_b);
			strcat(buf, buffer);
			if (i < NO_OS_ARRAY_SIZE(ci_table) - 1)
				strcat(buf, " ");
		}

		ret = strlen(buf);
		break;

	case ADF4368_IIO_DEV_ATTR_DEF_REG:
		ret = adf4368_get_default_regs(adf4368, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4368_IIO_DEV_ATTR_REF_DIV:
		ret = adf4368_get_ref_div(adf4368, &val);
		if (ret)
			return ret;

		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4368_IIO_DEV_ATTR_REF_DB:
		ret = adf4368_get_en_ref_doubler(adf4368, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4368_IIO_DEV_ATTR_REF_FREQ:
		ret = adf4368_get_ref_clk(adf4368, &val_64);
		if (ret)
			return ret;

		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;

	case ADF4368_IIO_DEV_ATTR_SYNC_SETUP:
		ret = adf4368_get_sync_setup(adf4368, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4368_IIO_DEV_ATTR_SW_SYNC:
		ret = adf4368_get_sw_sync(adf4368, &check);
		if (ret)
			return ret;

		val = check;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4368_IIO_DEV_ATTR_TEMP:
		ret = adf4368_get_temperature(adf4368, &temperature);
		if (ret)
			return ret;

		val = temperature;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	default:
		return -EINVAL;
	};

	return ret;
}

/**
 * @brief Handles the write request for the device attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int adf4368_iio_write_device_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{

	struct adf4368_iio_dev *iio_adf4368 = (struct adf4368_iio_dev *)dev;
	struct adf4368_dev *adf4368;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int32_t val_b[2];
	int8_t index;
	int ret;

	if (!iio_adf4368)
		return -EINVAL;

	adf4368 = iio_adf4368->adf4368_dev;

	if (!adf4368)
		return -EINVAL;

	switch (priv) {
	case ADF4368_IIO_DEV_ATTR_BLEED:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = adf4368_set_bleed_word(adf4368, val);
		break;

	case ADF4368_IIO_DEV_ATTR_CP:
		ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val_b[0], &val_b[1]);
		if (ret)
			return ret;

		index = adf4368_iio_find_2d_row(ci_table, NO_OS_ARRAY_SIZE(ci_table),
						val_b[0], val_b[1]);
		if (index < 0)
			return index;

		ret = adf4368_set_cp_i(adf4368, index);
		break;

	case ADF4368_IIO_DEV_ATTR_DEF_REG:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = adf4368_set_default_regs(adf4368, val);
		break;

	case ADF4368_IIO_DEV_ATTR_REF_DIV:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = adf4368_set_ref_div(adf4368, val);
		break;

	case ADF4368_IIO_DEV_ATTR_REF_DB:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = adf4368_set_en_ref_doubler(adf4368, val);
		break;

	case ADF4368_IIO_DEV_ATTR_REF_FREQ:
		sscanf(buf, "%"PRIu64, &val_64);
		ret = adf4368_set_ref_clk(adf4368, val_64);
		break;

	case ADF4368_IIO_DEV_ATTR_SYNC_SETUP:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = adf4368_set_sync_setup(adf4368, val);
		break;

	case ADF4368_IIO_DEV_ATTR_SW_SYNC:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = adf4368_set_sw_sync(adf4368, val);
		break;

	case ADF4368_IIO_DEV_ATTR_TEMP:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		ret = adf4368_set_temperature(adf4368, val);
		break;

	default:
		return -EINVAL;
	};

	return ret;
}

/**
 * @brief Handles the read request for the channel attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int adf4368_iio_read_channel_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{

	struct adf4368_iio_dev *iio_adf4368 = (struct adf4368_iio_dev *)dev;
	struct adf4368_dev *adf4368;
	int64_t val = -EINVAL;
	bool phase_pol = false;
	int32_t opwr;
	uint64_t freq;
	uint32_t phase;
	bool en;
	int ret;

	if (!iio_adf4368)
		return -EINVAL;

	adf4368 = iio_adf4368->adf4368_dev;

	if (!adf4368)
		return -EINVAL;

	switch (priv) {
	case ADF4368_IIO_CH_ATTR_ENABLE:
		ret = adf4368_get_en_chan(adf4368, channel->ch_num, &en);
		if (ret)
			return ret;

		val = en;
		break;

	case ADF4368_IIO_CH_ATTR_FREQ:
		ret = adf4368_get_rfout(adf4368, &freq);
		if (ret)
			return ret;

		val = freq;
		break;
	case ADF4368_IIO_CH_ATTR_OPWR:
		ret = adf4368_get_out_power(adf4368, channel->ch_num, &opwr);
		if (ret)
			return ret;

		val = opwr;
		break;
	case ADF4368_IIO_CH_ATTR_PHASE:
		ret = adf4368_get_phase(adf4368, &phase, &phase_pol);
		if (ret)
			return ret;

		val = phase_pol ? -phase : phase;
		break;
	default:
		return -EINVAL;
	};

	return snprintf(buf, len, "%"PRIu64, val);
}

/**
 * @brief Handles the write request for the channel attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int adf4368_iio_write_channel_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{

	struct adf4368_iio_dev *iio_adf4368 = (struct adf4368_iio_dev *)dev;
	struct adf4368_dev *adf4368;
	int64_t val = -EINVAL;
	bool phase_pol = false;

	int ret;

	if (!iio_adf4368)
		return -EINVAL;

	adf4368 = iio_adf4368->adf4368_dev;

	if (!adf4368)
		return -EINVAL;

	sscanf(buf, "%"PRIu64, &val);

	switch (priv) {
	case ADF4368_IIO_CH_ATTR_ENABLE:
		ret = adf4368_set_en_chan(adf4368, channel->ch_num, val);
		break;

	case ADF4368_IIO_CH_ATTR_FREQ:
		ret = adf4368_set_rfout(adf4368, val);
		break;

	case ADF4368_IIO_CH_ATTR_OPWR:
		ret = adf4368_set_out_power(adf4368, channel->ch_num, val);
		break;

	case ADF4368_IIO_CH_ATTR_PHASE:
		phase_pol = val < 0 ? true : false;
		val = llabs(val);
		ret = adf4368_set_phase(adf4368, val, phase_pol);
		break;
	default:
		return -EINVAL;
	};

	if (ret)
		return ret;

	return 0;
}

static struct iio_attribute adf4368_iio_attrs[] = {
	{
		.name = "bleed_current",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4368_IIO_DEV_ATTR_BLEED,
		.show = adf4368_iio_read_device_attr,
		.store = adf4368_iio_write_device_attr,
	},
	{
		.name = "charge_pump_current",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4368_IIO_DEV_ATTR_CP,
		.show = adf4368_iio_read_device_attr,
		.store = adf4368_iio_write_device_attr,
	},
	{
		.name = "charge_pump_current_available",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4368_IIO_DEV_ATTR_CP_AVAIL,
		.show = adf4368_iio_read_device_attr,
	},
	{
		.name = "default_regs",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4368_IIO_DEV_ATTR_DEF_REG,
		.show = adf4368_iio_read_device_attr,
		.store = adf4368_iio_write_device_attr,
	},
	{
		.name = "reference_divider",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4368_IIO_DEV_ATTR_REF_DIV,
		.show = adf4368_iio_read_device_attr,
		.store = adf4368_iio_write_device_attr,
	},
	{
		.name = "reference_doubler_en",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4368_IIO_DEV_ATTR_REF_DB,
		.show = adf4368_iio_read_device_attr,
		.store = adf4368_iio_write_device_attr,
	},
	{
		.name = "reference_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4368_IIO_DEV_ATTR_REF_FREQ,
		.show = adf4368_iio_read_device_attr,
		.store = adf4368_iio_write_device_attr,
	},
	{
		.name = "sync_setup",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4368_IIO_DEV_ATTR_SYNC_SETUP,
		.show = adf4368_iio_read_device_attr,
		.store = adf4368_iio_write_device_attr,
	},
	{
		.name = "sw_sync",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4368_IIO_DEV_ATTR_SW_SYNC,
		.show = adf4368_iio_read_device_attr,
		.store = adf4368_iio_write_device_attr,
	},
	{
		.name = "temperature",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4368_IIO_DEV_ATTR_TEMP,
		.show = adf4368_iio_read_device_attr,
		.store = adf4368_iio_write_device_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adf4368_iio_ch_attrs[] = {
	{
		.name = "en",
		.shared = IIO_SEPARATE,
		.priv = ADF4368_IIO_CH_ATTR_ENABLE,
		.show = adf4368_iio_read_channel_attr,
		.store = adf4368_iio_write_channel_attr,
	},
	{
		.name = "frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.priv = ADF4368_IIO_CH_ATTR_FREQ,
		.show = adf4368_iio_read_channel_attr,
		.store = adf4368_iio_write_channel_attr,
	},
	{
		.name = "output_power",
		.shared = IIO_SEPARATE,
		.priv = ADF4368_IIO_CH_ATTR_OPWR,
		.show = adf4368_iio_read_channel_attr,
		.store = adf4368_iio_write_channel_attr,
	},
	{
		.name = "phase",
		.shared = IIO_SHARED_BY_TYPE,
		.priv = ADF4368_IIO_CH_ATTR_PHASE,
		.show = adf4368_iio_read_channel_attr,
		.store = adf4368_iio_write_channel_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adf4368_channels[] = {
	{
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 0,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4368_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 1,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4368_iio_ch_attrs,
		.scan_type = NULL,
	},
};

static struct iio_device adf4368_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adf4368_channels),
	.channels = adf4368_channels,
	.attributes = adf4368_iio_attrs,
	.debug_reg_read = (int32_t (*)())adf4368_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adf4368_iio_write_reg
};

/**
 * @brief Initializes the ADF4368 IIO driver
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial parameters.
 * @return ret	     - Result of the initialization procedure.
 */
int adf4368_iio_init(struct adf4368_iio_dev **iio_dev,
		     struct adf4368_iio_dev_init_param *init_param)
{
	struct adf4368_iio_dev *dev;
	int ret;

	dev = (struct adf4368_iio_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->iio_dev = &adf4368_iio_dev;

	ret = adf4368_init(&dev->adf4368_dev, init_param->adf4368_dev_init);
	if (ret)
		goto error;

	*iio_dev = dev;

	return ret;

error:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free the resources allocated by adf4368_iio_init().
 * @param dev	- The IIO device structure.
 * @return ret 	- Result of the remove procedure.
*/
int adf4368_iio_remove(struct adf4368_iio_dev *dev)
{
	int ret;

	ret = adf4368_remove(dev->adf4368_dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
