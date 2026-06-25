/***************************************************************************//**
 *   @file   iio_adf4371.c
 *   @brief  Implementation of IIO ADF4371 Driver.
 *   @author Jude Osemene (jude.osemene@analog.com)
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
#include "iio_adf4371.h"
#include "adf4371.h"
#include "no_os_alloc.h"

/**
 * @brief Supported charge pump currents
 */
static const int ci_table[16][2] = {
	{0, 350000},
	{0, 700000},
	{1, 050000},
	{1, 400000},
	{1, 750000},
	{2, 100000},
	{2, 450000},
	{2, 800000},
	{3, 150000},
	{3, 500000},
	{3, 850000},
	{4, 200000},
	{4, 550000},
	{4, 900000},
	{5, 250000},
	{5, 500000},
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

static int adf4371_iio_find_2d_row(const int (*tbl)[2], const int size,
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
 * @brief Wrapper for reading adf4371 register.
 * @param dev	  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 * @return	  - Result of the reading procedure.
 */
static int adf4371_iio_read_reg(struct adf4371_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	uint8_t tmp;
	int ret;

	ret = adf4371_read(dev->adf4371_dev, (uint16_t)reg, &tmp);
	if (ret)
		return ret;

	*readval = tmp;
	return 0;
}

/**
 * @brief Wrapper for writing adf4371 register.
 * @param dev	   - The iio device structure.
 * @param reg	   - Address of the register to be written to.
 * @param writeval - Data to be written.
 * @return	   - Result of the writing procedure.
 */
static int adf4371_iio_write_reg(struct adf4371_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	return adf4371_write(dev->adf4371_dev, (uint16_t)reg,
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

static int adf4371_iio_read_dev_attr(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct adf4371_iio_dev *iio_adf4371 = (struct adf4371_iio_dev *)dev;
	struct adf4371_dev *adf4371;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int32_t val_b[2];
	char buffer[5];
	int32_t cp_i;
	uint8_t i;
	bool en;
	int ret;

	if (!iio_adf4371)
		return -EINVAL;

	adf4371 = iio_adf4371->adf4371_dev;

	if (!adf4371)
		return -EINVAL;

	switch (priv) {
	case ADF4371_IIO_DEV_ATTR_REF_FREQ:
		ret = adf4371_get_ref_clk(adf4371, &val_64);
		if (ret)
			return ret;
		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;
	case ADF4371_IIO_DEV_ATTR_BLEED_CURRENT:
		ret = adf4371_get_bleed_icp(adf4371, (uint32_t *)&val);
		if (ret)
			return ret;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4371_IIO_DEV_ATTR_CHARGE_PUMP_CURRENT:
		ret = adf4371_get_charge_pump_current(adf4371, (uint32_t *)&cp_i);
		if (ret)
			return ret;

		val_b[0] = ci_table[cp_i][0];
		val_b[1] = ci_table[cp_i][1];
		ret = iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val_b);
		break;
	case ADF4371_IIO_DEV_ATTR_CHARGE_PUMP_AVAILABLE:
		strcpy(buf, "");
		for (i = 0; i < NO_OS_ARRAY_SIZE(ci_table); i++) {
			val_b[0] = ci_table[i][0];
			val_b[1] = ci_table[i][1];
			iio_format_value(buffer, len, IIO_VAL_INT_PLUS_MICRO, 2,
					 val_b);
			strcat(buf, buffer);
			if (i < NO_OS_ARRAY_SIZE(ci_table) - 1)
				strcat(buf, " ");
		}
		ret = strlen(buf);
		break;
	case ADF4371_IIO_DEV_ATTR_R_WORD:
		ret = adf4371_get_r_word(adf4371, (uint32_t *)&val);
		if (ret)
			return ret;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4371_IIO_DEV_ATTR_REF_DIVIDER2_ENABLE:
		ret = adf4371_get_rdiv2_enable(adf4371, &en);
		if (ret)
			return ret;
		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4371_IIO_DEV_ATTR_REF_DOUBLER_EN:
		ret = adf4371_get_ref_doubler(adf4371, &en);
		if (ret)
			return ret;
		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	default:
		return -EINVAL;
	}
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
static int adf4371_iio_write_dev_attr(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct adf4371_iio_dev *iio_adf4371 = (struct adf4371_iio_dev *)dev;
	struct adf4371_dev *adf4371;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int32_t val_b[2];
	bool en;
	int8_t index;
	int ret;

	if (!iio_adf4371)
		return -EINVAL;

	adf4371 = iio_adf4371->adf4371_dev;

	if (!adf4371)
		return -EINVAL;

	switch (priv) {
	case ADF4371_IIO_DEV_ATTR_REF_FREQ:
		sscanf(buf, "%"PRIu64, &val_64);
		ret = adf4371_set_ref_clk(adf4371, val_64);
		break;
	case ADF4371_IIO_DEV_ATTR_BLEED_CURRENT:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		ret = adf4371_set_bleed_icp(adf4371, (uint32_t)val);
		break;
	case ADF4371_IIO_DEV_ATTR_CHARGE_PUMP_CURRENT:
		iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val_b[0], &val_b[1]);
		index = adf4371_iio_find_2d_row(ci_table, NO_OS_ARRAY_SIZE(ci_table),
						val_b[0], val_b[1]);
		if (index < 0)
			return index;

		ret = adf4371_set_charge_pump_current(adf4371, (uint32_t)index);
		break;
	case ADF4371_IIO_DEV_ATTR_R_WORD:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		ret = adf4371_set_r_word(adf4371, (uint32_t)val);
		break;
	case ADF4371_IIO_DEV_ATTR_REF_DIVIDER2_ENABLE:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		en = val;
		ret = adf4371_set_rdiv2_enable(adf4371, en);
		break;
	case ADF4371_IIO_DEV_ATTR_REF_DOUBLER_EN:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		en = val;
		ret = adf4371_set_ref_doubler(adf4371, en);
		break;
	default:
		return -EINVAL;
	}
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
static int adf4371_iio_read_chan_attr(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct adf4371_iio_dev *iio_adf4371 = (struct adf4371_iio_dev *)dev;
	struct adf4371_dev *adf4371;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	bool en;
	int ret;

	if (!iio_adf4371)
		return -EINVAL;
	adf4371 = iio_adf4371->adf4371_dev;
	if (!adf4371)
		return -EINVAL;

	switch (priv) {
	case ADF4371_IIO_CH_ATTR_CHANNEL_POWERDOWN:
		ret = adf4371_get_channel_power_down(adf4371, channel->ch_num, &en);
		if (ret)
			return ret;
		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4371_IIO_CH_ATTR_RFOUT8_FREQ:
		ret = adf4371_get_rfout(adf4371, &val_64);
		if (ret)
			return ret;
		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;
	case ADF4371_IIO_CH_ATTR_RFOUT8AUX_FREQ:
		ret = adf4371_get_rfout8aux(adf4371, &val_64);
		if (ret)
			return ret;
		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;
	case ADF4371_IIO_CH_ATTR_RFOUT16_FREQ:
		ret = adf4371_get_rfout16(adf4371, &val_64);
		if (ret)
			return ret;
		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;
	case ADF4371_IIO_CH_ATTR_RFOUT32_FREQ:
		ret = adf4371_get_rfout32(adf4371, &val_64);
		if (ret)
			return ret;
		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;
	default:
		return -EINVAL;
	}
	return ret;
}

/**
 * @brief Handles the write request for the channel attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the written data in case of success, error code
 * 		    otherwise.
 */
static int adf4371_iio_write_chan_attr(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adf4371_iio_dev *iio_adf4371 = (struct adf4371_iio_dev *)dev;
	struct adf4371_dev *adf4371;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	bool en;
	int ret;

	if (!iio_adf4371)
		return -EINVAL;
	adf4371 = iio_adf4371->adf4371_dev;
	if (!adf4371)
		return -EINVAL;

	switch (priv) {
	case ADF4371_IIO_CH_ATTR_CHANNEL_POWERDOWN:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		en = val;
		ret = adf4371_set_channel_power_down(adf4371, channel->ch_num, en);
		break;
	case ADF4371_IIO_CH_ATTR_RFOUT8_FREQ:
		sscanf(buf, "%"PRIu64, &val_64);
		ret = adf4371_set_rf8_freq(adf4371, val_64);
		break;
	default:
		return -EINVAL;
	}
	return ret;
}

static struct iio_attribute adf4371_iio_dev_attributes[] = {
	{
		.name = "ref_clk",
		.priv = ADF4371_IIO_DEV_ATTR_REF_FREQ,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_dev_attr,
		.store = adf4371_iio_write_dev_attr,
	},
	{
		.name = "bleed_current",
		.priv = ADF4371_IIO_DEV_ATTR_BLEED_CURRENT,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_dev_attr,
		.store = adf4371_iio_write_dev_attr,
	},
	{
		.name = "charge_pump_current",
		.priv = ADF4371_IIO_DEV_ATTR_CHARGE_PUMP_CURRENT,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_dev_attr,
		.store = adf4371_iio_write_dev_attr,
	},
	{
		.name = "charge_pump_available_currents",
		.priv = ADF4371_IIO_DEV_ATTR_CHARGE_PUMP_AVAILABLE,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_dev_attr,
	},
	{
		.name = "r_word",
		.priv = ADF4371_IIO_DEV_ATTR_R_WORD,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_dev_attr,
		.store = adf4371_iio_write_dev_attr,
	},
	{
		.name = "ref_divider2_enable",
		.priv = ADF4371_IIO_DEV_ATTR_REF_DIVIDER2_ENABLE,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_dev_attr,
		.store = adf4371_iio_write_dev_attr,
	},
	{
		.name = "ref_doubler_enable",
		.priv = ADF4371_IIO_DEV_ATTR_REF_DOUBLER_EN,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_dev_attr,
		.store = adf4371_iio_write_dev_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adf4371_iio_ch_attributes[] = {
	{
		.name = "powerdown",
		.priv = ADF4371_IIO_CH_ATTR_CHANNEL_POWERDOWN,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_chan_attr,
		.store = adf4371_iio_write_chan_attr,
	},
	{
		.name = "frequency",
		.priv = ADF4371_IIO_CH_ATTR_RFOUT8_FREQ,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_chan_attr,
		.store = adf4371_iio_write_chan_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adf4371_iio_rf16_chan_attr[] = {
	{
		.name = "powerdown",
		.priv = ADF4371_IIO_CH_ATTR_CHANNEL_POWERDOWN,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_chan_attr,
		.store = adf4371_iio_write_chan_attr,
	},
	{
		.name = "frequency",
		.priv = ADF4371_IIO_CH_ATTR_RFOUT16_FREQ,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_chan_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adf4371_iio_rf32_chan_attr[] = {
	{
		.name = "frequency",
		.priv = ADF4371_IIO_CH_ATTR_RFOUT32_FREQ,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_chan_attr,
	},
	{
		.name = "powerdown",
		.priv = ADF4371_IIO_CH_ATTR_CHANNEL_POWERDOWN,
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4371_iio_read_chan_attr,
		.store = adf4371_iio_write_chan_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adf4371_iio_channels[] = {
	{
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 0,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4371_iio_ch_attributes,
		.scan_type = NULL,
	},
	{
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 1,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4371_iio_ch_attributes,
		.scan_type = NULL,
	},
	{
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 2,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4371_iio_rf16_chan_attr,
		.scan_type = NULL,
	},
	{
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 3,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4371_iio_rf32_chan_attr,
		.scan_type = NULL,
	}
};

static struct iio_device adf4371_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adf4371_iio_channels),
	.channels = adf4371_iio_channels,
	.attributes = adf4371_iio_dev_attributes,
	.debug_reg_read = (int32_t (*)())adf4371_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adf4371_iio_write_reg
};

/**
 * @brief Initializes the ADF4371 IIO driver
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial parameters.
 * @return ret	     - Result of the initialization procedure.
 */
int adf4371_iio_init(struct adf4371_iio_dev **iio_dev,
		     struct adf4371_iio_dev_init_param *init_param)
{
	struct adf4371_iio_dev *dev;
	int ret;

	dev = (struct adf4371_iio_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->iio_dev = &adf4371_iio_dev;

	ret = adf4371_init(&dev->adf4371_dev, init_param->adf4371_dev_init);
	if (ret)
		goto error;

	*iio_dev = dev;

	return ret;
error:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free the resources allocated by adf4371_iio_init().
 * @param dev	- The IIO device structure.
 * @return ret 	- Result of the remove procedure.
*/
int adf4371_iio_remove(struct adf4371_iio_dev *dev)
{
	int ret;

	ret = adf4371_remove(dev->adf4371_dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}