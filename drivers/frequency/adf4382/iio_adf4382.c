/***************************************************************************//**
 *   @file   iio_adf4382.c
 *   @brief  Implementation of IIO ADF4382 Driver.
 *   @author Ciprian Hegbeli (ciprian.hegbeli@analog.com)
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
#include "iio_adf4382.h"
#include "adf4382.h"
#include "no_os_alloc.h"

/**
 * @brief Supported charge pump currents
 */
static const int ci_table[16][2] = {
	{0, 700000},
	{0, 900000},
	{1, 100000},
	{1, 300000},
	{1, 500000},
	{1, 900000},
	{2, 300000},
	{2, 700000},
	{3, 100000},
	{3, 900000},
	{4, 700000},
	{5, 500000},
	{6, 300000},
	{7, 900000},
	{9, 500000},
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
static int adf4382_iio_find_2d_row(const int (*tbl)[2], const int size,
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
 * @brief Wrapper for reading adf4382 register.
 * @param dev	  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 * @return	  - Result of the reading procedure.
 */
static int adf4382_iio_read_reg(struct adf4382_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	uint8_t tmp;
	int ret;

	ret = adf4382_spi_read(dev->adf4382_dev, (uint16_t)reg, &tmp);
	if (ret)
		return ret;

	*readval = tmp;
	return 0;
}

/**
 * @brief Wrapper for writing adf4382 register.
 * @param dev	   - The iio device structure.
 * @param reg	   - Address of the register to be written to.
 * @param writeval - Data to be written.
 * @return	   - Result of the writing procedure.
 */
static int adf4382_iio_write_reg(struct adf4382_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	return adf4382_spi_write(dev->adf4382_dev, (uint16_t)reg,
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
static int adf4382_iio_read_dev_attr(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct adf4382_iio_dev *iio_adf4382 = (struct adf4382_iio_dev *)dev;
	struct adf4382_dev *adf4382;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int32_t val_b[2];
	char buffer[5];
	int32_t cp_i;
	uint8_t i;
	bool en;
	int ret;

	if (!iio_adf4382)
		return -EINVAL;

	adf4382 = iio_adf4382->adf4382_dev;

	if (!adf4382)
		return -EINVAL;

	switch (priv) {
	case ADF4382_IIO_DEV_ATTR_REF_FREQ:
		ret = adf4382_get_ref_clk(adf4382, &val_64);
		if (ret)
			return ret;

		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;

	case ADF4382_IIO_DEV_ATTR_BLEED_CURRENT:
		ret = adf4382_get_bleed_word(adf4382, &val);
		if (ret)
			return ret;

		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4382_IIO_DEV_ATTR_CHARGE_PUMP_CURRENT:
		ret = adf4382_get_cp_i(adf4382, &cp_i);
		if (ret)
			return ret;

		val_b[0] = ci_table[cp_i][0];
		val_b[1] = ci_table[cp_i][1];
		ret = iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val_b);
		break;

	case ADF4382_IIO_DEV_ATTR_CHARGE_PUMP_AVAILABLE:
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

	case ADF4382_IIO_DEV_ATTR_REF_DIVIDER:
		ret = adf4382_get_ref_div(adf4382, &val);
		if (ret)
			return ret;

		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4382_IIO_DEV_ATTR_REF_DOUBLER_EN:
		ret = adf4382_get_en_ref_doubler(adf4382, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4382_IIO_DEV_ATTR_FASTCAL_EN:
		ret = adf4382_get_en_lut_calibration(adf4382, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4382_IIO_DEV_ATTR_FASTCAL_LUT_EN:
		ret = adf4382_get_en_lut_calibration(adf4382, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4382_IIO_DEV_ATTR_FASTCAL_CHANGE_FREQ:
		ret = adf4382_get_change_rfout(adf4382, &val_64);
		if (ret)
			return ret;

		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;

	case ADF4382_IIO_DEV_ATTR_FASTCAL_START_CALIBRATION:
		ret = adf4382_get_start_calibration(adf4382, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4382_IIO_DEV_ATTR_SW_SYNC:
		ret = adf4382_get_sw_sync(adf4382, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4382_IIO_DEV_ATTR_EZ_SYNC:
		ret = adf4382_get_phase_sync_setup(adf4382, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4382_IIO_DEV_ATTR_TIMED_SYNC:
		ret = adf4382_get_phase_sync_setup(adf4382, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	default:
		return -EINVAL;
	};
	return ret;
};

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
static int adf4382_iio_write_dev_attr(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct adf4382_iio_dev *iio_adf4382 = (struct adf4382_iio_dev *)dev;
	struct adf4382_dev *adf4382;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int32_t val_b[2];
	bool en;
	int8_t index;
	int ret;

	if (!iio_adf4382)
		return -EINVAL;

	adf4382 = iio_adf4382->adf4382_dev;

	if (!adf4382)
		return -EINVAL;

	switch (priv) {
	case ADF4382_IIO_DEV_ATTR_REF_FREQ:
		sscanf(buf, "%"PRIu64, &val_64);
		ret = adf4382_set_ref_clk(adf4382, val_64);
		break;

	case ADF4382_IIO_DEV_ATTR_BLEED_CURRENT:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		ret = adf4382_set_bleed_word(adf4382, val);
		break;

	case ADF4382_IIO_DEV_ATTR_CHARGE_PUMP_CURRENT:
		iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val_b[0], &val_b[1]);

		index = adf4382_iio_find_2d_row(ci_table, NO_OS_ARRAY_SIZE(ci_table),
						val_b[0], val_b[1]);
		if (index < 0)
			return index;

		ret = adf4382_set_cp_i(adf4382, index);
		break;

	case ADF4382_IIO_DEV_ATTR_REF_DIVIDER:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		ret = adf4382_set_ref_div(adf4382, val);
		break;

	case ADF4382_IIO_DEV_ATTR_REF_DOUBLER_EN:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		en = val;
		ret = adf4382_set_en_ref_doubler(adf4382, val);
		break;

	case ADF4382_IIO_DEV_ATTR_FASTCAL_EN:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		en = val;
		ret = adf4382_set_en_fast_calibration(adf4382, en);
		break;

	case ADF4382_IIO_DEV_ATTR_FASTCAL_LUT_EN:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		en = val;
		ret = adf4382_set_en_lut_calibration(adf4382, en);
		break;

	case ADF4382_IIO_DEV_ATTR_FASTCAL_CHANGE_FREQ:
		sscanf(buf, "%"PRIu64, &val_64);
		ret = adf4382_set_change_rfout(adf4382, val_64);
		break;

	case ADF4382_IIO_DEV_ATTR_FASTCAL_START_CALIBRATION:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		if (!val)
			return 0;

		ret = adf4382_set_start_calibration(adf4382);
		break;

	case ADF4382_IIO_DEV_ATTR_SW_SYNC:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		en = val;
		ret = adf4382_set_sw_sync(adf4382, en);
		break;
	case ADF4382_IIO_DEV_ATTR_EZ_SYNC:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		en = val;
		ret = adf4382_set_ezsync_setup(adf4382, en);
		break;

	case ADF4382_IIO_DEV_ATTR_TIMED_SYNC:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		en = val;
		ret = adf4382_set_timed_sync_setup(adf4382, en);
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
static int adf4382_iio_read_chan_attr(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct adf4382_iio_dev *iio_adf4382 = (struct adf4382_iio_dev *)dev;
	struct adf4382_dev *adf4382;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	bool en, pol;
	int ret;

	if (!iio_adf4382)
		return -EINVAL;

	adf4382 = iio_adf4382->adf4382_dev;

	if (!adf4382)
		return -EINVAL;

	switch (priv) {
	case ADF4382_IIO_CH_ATTR_EN:
		ret = adf4382_get_en_chan(adf4382, channel->ch_num, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4382_IIO_CH_ATTR_FREQ:
		ret = adf4382_get_rfout(adf4382, &val_64);
		if (ret)
			return ret;

		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;

	case ADF4382_IIO_CH_ATTR_OPWR:
		ret = adf4382_get_out_power(adf4382, channel->ch_num, &val);
		if (ret)
			return ret;

		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4382_IIO_CH_ATTR_PHASE_ADJ:
		val = adf4382->phase_adj;
		ret = adf4382_get_phase_pol(adf4382, &pol);
		if (ret)
			return ret;

		if (pol)
			val *= -1;

		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
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
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int adf4382_iio_write_chan_attr(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adf4382_iio_dev *iio_adf4382 = (struct adf4382_iio_dev *)dev;
	struct adf4382_dev *adf4382;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	bool en;
	int ret;

	if (!iio_adf4382)
		return -EINVAL;

	adf4382 = iio_adf4382->adf4382_dev;

	if (!adf4382)
		return -EINVAL;

	switch (priv) {
	case ADF4382_IIO_CH_ATTR_EN:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		en = val;
		ret = adf4382_set_en_chan(adf4382, channel->ch_num, en);
		break;

	case ADF4382_IIO_CH_ATTR_FREQ:
		sscanf(buf, "%"PRIu64, &val_64);

		ret = adf4382_set_rfout(adf4382, val_64);
		break;

	case ADF4382_IIO_CH_ATTR_PHASE_ADJ:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (val < 0)
			ret = adf4382_set_phase_pol(adf4382, 1);
		else
			ret = adf4382_set_phase_pol(adf4382, 0);

		if (ret)
			return ret;

		val = abs(val);

		ret = adf4382_set_phase_adjust(adf4382, val);
		break;

	default:
		return -EINVAL;
	}
	return ret;
}

static struct iio_attribute adf4382_iio_attrs[] = {
	{
		.name = "reference_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_REF_FREQ,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "charge_pump_current",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_CHARGE_PUMP_CURRENT,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "charge_pump_current_available",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_CHARGE_PUMP_AVAILABLE,
		.show = adf4382_iio_read_dev_attr,
	},
	{
		.name = "bleed_current",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_BLEED_CURRENT,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "reference_doubler_en",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_REF_DOUBLER_EN,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "reference_divider",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_REF_DIVIDER,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "sw_sync_en",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_SW_SYNC,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "ezsync_setup",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_EZ_SYNC,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "timed_sync_setup",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_TIMED_SYNC,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "fastcal_en",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_FASTCAL_EN,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "fastcal_lut_en",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_FASTCAL_LUT_EN,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "change_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_FASTCAL_CHANGE_FREQ,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	{
		.name = "start_calibration",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4382_IIO_DEV_ATTR_FASTCAL_START_CALIBRATION,
		.show = adf4382_iio_read_dev_attr,
		.store = adf4382_iio_write_dev_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adf4382_iio_ch_attrs[] = {
	{
		.name = "en",
		.shared = IIO_SEPARATE,
		.priv = ADF4382_IIO_CH_ATTR_EN,
		.show = adf4382_iio_read_chan_attr,
		.store = adf4382_iio_write_chan_attr,
	},
	{
		.name = "frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.priv = ADF4382_IIO_CH_ATTR_FREQ,
		.show = adf4382_iio_read_chan_attr,
		.store = adf4382_iio_write_chan_attr,
	},
	{
		.name = "output_power",
		.shared = IIO_SEPARATE,
		.priv = ADF4382_IIO_CH_ATTR_OPWR,
		.show = adf4382_iio_read_chan_attr,
		.store = adf4382_iio_write_chan_attr,
	},
	{
		.name = "phase",
		.shared = IIO_SHARED_BY_TYPE,
		.priv = ADF4382_IIO_CH_ATTR_PHASE_ADJ,
		.show = adf4382_iio_read_chan_attr,
		.store = adf4382_iio_write_chan_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adf4382_channels[] = {
	{
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 0,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4382_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 1,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4382_iio_ch_attrs,
		.scan_type = NULL,
	},
};

static struct iio_device adf4382_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adf4382_channels),
	.channels = adf4382_channels,
	.attributes = adf4382_iio_attrs,
	.debug_reg_read = (int32_t (*)())adf4382_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adf4382_iio_write_reg
};

/**
 * @brief Initializes the ADF4382 IIO driver
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial parameters.
 * @return ret	     - Result of the initialization procedure.
 */
int adf4382_iio_init(struct adf4382_iio_dev **iio_dev,
		     struct adf4382_iio_dev_init_param *init_param)
{
	struct adf4382_iio_dev *dev;
	int ret;

	dev = (struct adf4382_iio_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->iio_dev = &adf4382_iio_dev;

	ret = adf4382_init(&dev->adf4382_dev, init_param->adf4382_dev_init);
	if (ret)
		goto error;

	*iio_dev = dev;

	return ret;

error:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free the resources allocated by adf4382_iio_init().
 * @param dev	- The IIO device structure.
 * @return ret 	- Result of the remove procedure.
*/
int adf4382_iio_remove(struct adf4382_iio_dev *dev)
{
	int ret;

	ret = adf4382_remove(dev->adf4382_dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
