/***************************************************************************//**
 *   @file   iio_adf4377.c
 *   @brief  Implementation of ADF4377 IIO Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
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
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_adf4377.h"
#include "adf4377.h"
#include "no_os_alloc.h"

/**
 * @brief Supported charge pump currents for ADF4377.
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
 * @brief Supported rfout_divider_values
 */
static const int rfout_div_table[4] = {
	1,   2,   4,   8
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
static int adf4377_iio_find_2d_row(const int (*tbl)[2], const int size,
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
 * @brief Helper function to read an element in a table .
 * @param tbl	- Input table.
 * @param size	- Size of the table.
 * @param val	- The value to be found.
 * @return i	- The index of element if found or an error if element is not
 * found in the table.
 */
static int adf4377_iio_find_element_in_tbl(const int *tbl, const int size,
		const int val)
{
	int i;
	for (i = 0; i < size; i++) {
		if (tbl[i] == val)
			return i;
	}
	return -EINVAL;
}

/**
 * @brief Wrapper for reading adf4377 register.
 * @param dev	  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 * @return	  - Result of the reading procedure.
 */
static int adf4377_iio_read_reg(struct adf4377_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	uint8_t tmp;
	int ret;

	ret = adf4377_spi_read(dev->adf4377_dev, (uint16_t)reg, &tmp);
	if (ret)
		return ret;

	*readval = tmp;
	return 0;
}

/**
* @brief Wrapper for writing adf4377 register.
* @param dev	   - The iio device structure.
* @param reg	   - Address of the register to be written to.
* @param writeval - Data to be written.
* @return	   - Result of the writing procedure.
*/
static int adf4377_iio_write_reg(struct adf4377_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	return adf4377_spi_write(dev->adf4377_dev, (uint16_t)reg,
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
static int adf4377_iio_read_dev_attr(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct adf4377_iio_dev *iio_adf4377 = (struct adf4377_iio_dev *)dev;
	struct adf4377_dev *adf4377;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int32_t val_b[2];
	char buffer[5];
	int32_t cp_i;
	int8_t div;
	uint8_t i;
	bool en;
	int ret;

	if (!iio_adf4377)
		return -EINVAL;

	adf4377 = iio_adf4377->adf4377_dev;

	if (!adf4377)
		return -EINVAL;

	switch (priv) {
	case ADF4377_IIO_DEV_ATTR_REF_CLK:
		ret = adf4377_get_ref_clk(adf4377, &val_64);
		if (ret)
			return ret;

		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;
	case ADF4377_IIO_DEV_ATTR_REF_DIV:
		ret = adf4377_get_ref_div(adf4377, &val);
		if (ret)
			return ret;

		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4377_IIO_DEV_ATTR_REF_DOUBLER_EN:
		ret = adf4377_get_en_ref_doubler(adf4377, &en);
		if (ret)
			return ret;

		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4377_IIO_DEV_ATTR_CP_I:
		ret = adf4377_get_cp_i(adf4377, &cp_i);
		if (ret)
			return ret;

		val_b[0] = ci_table[cp_i][0];
		val_b[1] = ci_table[cp_i][1];
		ret = iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val_b);
		break;
	case ADF4377_IIO_DEV_ATTR_CP_AVAIL:
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
	case ADF4377_IIO_DEV_ATTR_BLEED_CURRENT:
		ret = adf4377_get_bleed_word(adf4377, &val);
		if (ret)
			return ret;

		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4377_IIO_DEV_ATTR_RFOUT_DIV:
		ret = adf4377_get_rfout_divider(adf4377, &div);
		if (ret)
			return ret;
		val = rfout_div_table[div];
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4377_IIO_DEV_ATTR_RFOUT_DIV_AVAIL:
		strcpy(buf, "");
		for (i = 0; i < NO_OS_ARRAY_SIZE(rfout_div_table); i++) {
			val = rfout_div_table[i];
			iio_format_value(buffer, len, IIO_VAL_INT, 1, &val);
			strcat(buf, buffer);
			if (i < NO_OS_ARRAY_SIZE(rfout_div_table) - 1)
				strcat(buf, " ");
		}
		ret = strlen(buf);
		break;
	case ADF4377_IIO_DEV_ATTR_NDEL:
		ret = adf4377_get_ndel(adf4377, &val);
		if (ret)
			return ret;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4377_IIO_DEV_ATTR_RDEL:
		ret = adf4377_get_rdel(adf4377, &val);
		if (ret)
			return ret;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4377_IIO_DEV_ATTR_SR_DEL_ADJ:
		ret = adf4377_get_sr_del_adj(adf4377, &val);
		if (ret)
			return ret;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4377_IIO_DEV_ATTR_SR_INV_ADJ:
		ret = adf4377_get_en_sr_inv_adj(adf4377, &en);
		if (ret)
			return ret;
		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4377_IIO_DEV_ATTR_SR_MONITORING:
		ret = adf4377_get_en_sysref_monitor(adf4377, &en);
		if (ret)
			return ret;
		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF4377_IIO_DEV_ATTR_DEFAULT_REG:
		val = 0; // dummy value
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
static int adf4377_iio_write_dev_attr(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct adf4377_iio_dev *iio_adf4377 = (struct adf4377_iio_dev *)dev;
	struct adf4377_dev *adf4377;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int32_t val_b[2];
	bool en;
	int8_t index;
	int ret;

	if (!iio_adf4377)
		return -EINVAL;

	adf4377 = iio_adf4377->adf4377_dev;

	if (!adf4377)
		return -EINVAL;
	switch (priv) {
	case ADF4377_IIO_DEV_ATTR_REF_CLK:
		sscanf(buf, "%"PRIu64, &val_64);
		ret = adf4377_set_ref_clk(adf4377, val_64);
		break;
	case ADF4377_IIO_DEV_ATTR_REF_DIV:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		ret = adf4377_set_ref_div(adf4377, val);
		break;
	case ADF4377_IIO_DEV_ATTR_REF_DOUBLER_EN:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		en = val;
		ret = adf4377_set_en_ref_doubler(adf4377, en);
		break;
	case ADF4377_IIO_DEV_ATTR_CP_I:
		iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val_b[0], &val_b[1]);

		index = adf4377_iio_find_2d_row(ci_table,
						NO_OS_ARRAY_SIZE(ci_table),
						val_b[0], val_b[1]);
		if (index < 0)
			return index;

		ret = adf4377_set_cp_i(adf4377, index);
		break;
	case ADF4377_IIO_DEV_ATTR_BLEED_CURRENT:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		ret = adf4377_set_bleed_word(adf4377, val);
		break;
	case ADF4377_IIO_DEV_ATTR_RFOUT_DIV:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;

		index = adf4377_iio_find_element_in_tbl(rfout_div_table,
							NO_OS_ARRAY_SIZE
							(rfout_div_table), val);
		if (index < 0)
			return index;
		ret = adf4377_set_rfout_divider(adf4377, index);
		break;
	case ADF4377_IIO_DEV_ATTR_NDEL:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		ret = adf4377_set_ndel(adf4377, val);
		break;
	case ADF4377_IIO_DEV_ATTR_RDEL:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		ret = adf4377_set_rdel(adf4377, val);
		break;
	case ADF4377_IIO_DEV_ATTR_SR_DEL_ADJ:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		ret = adf4377_set_sr_del_adj(adf4377, val);
		break;
	case ADF4377_IIO_DEV_ATTR_SR_INV_ADJ:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		en = val;
		ret = adf4377_set_en_sr_inv_adj(adf4377, en);
		break;
	case ADF4377_IIO_DEV_ATTR_SR_MONITORING:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		en = val;
		ret = adf4377_set_en_sysref_monitor(adf4377, en);
		break;
	case ADF4377_IIO_DEV_ATTR_DEFAULT_REG:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		en = val;
		ret = adf4377_soft_reset(adf4377, en);
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
static int adf4377_iio_read_chan_attr(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct adf4377_iio_dev *iio_adf4377 = (struct adf4377_iio_dev *)dev;
	struct adf4377_dev *adf4377;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int8_t opwr;
	bool en;
	int ret;

	if (!iio_adf4377)
		return -EINVAL;

	adf4377 = iio_adf4377->adf4377_dev;

	if (!adf4377)
		return -EINVAL;

	switch (priv) {
	case ADF4377_IIO_CH_ATTR_FREQ:
		ret = adf4377_get_rfout(adf4377, &val_64);
		if (ret)
			return ret;

		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;

	case ADF4377_IIO_CH_ATTR_OPWR:
		ret = adf4377_get_out_power(adf4377, channel->ch_num, &opwr);
		if (ret)
			return ret;

		val = opwr;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;

	case ADF4377_IIO_CH_ATTR_EN:
		ret = adf4377_get_en_chan(adf4377, channel->ch_num, &en);
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
 * @brief Handles the write request for the channel attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int adf4377_iio_write_chan_attr(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adf4377_iio_dev *iio_adf4377 = (struct adf4377_iio_dev *)dev;
	struct adf4377_dev *adf4377;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int8_t opwr;
	bool en;
	int ret;

	if (!iio_adf4377)
		return -EINVAL;

	adf4377 = iio_adf4377->adf4377_dev;

	if (!adf4377)
		return -EINVAL;

	switch (priv) {
	case ADF4377_IIO_CH_ATTR_FREQ:
		sscanf(buf, "%"PRIu64, &val_64);
		ret = adf4377_set_rfout(adf4377, val_64);
		break;
	case ADF4377_IIO_CH_ATTR_OPWR:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		opwr = val;
		ret = adf4377_set_out_power(adf4377, channel->ch_num, opwr);
		break;
	case ADF4377_IIO_CH_ATTR_EN:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		en = val;
		ret = adf4377_set_en_chan(adf4377, channel->ch_num, en);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	return 0;
}

static struct iio_attribute adf4377_iio_dev_attributes[] = {
	{
		.name = "reference_clock",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_REF_CLK,
	},
	{
		.name = "reference_divider",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_REF_DIV,
	},
	{
		.name = "reference_doubler_enable",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_REF_DOUBLER_EN,
	},
	{
		.name = "charge_pump_current",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_CP_I,
	},
	{
		.name = "charge_pump_available",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_CP_AVAIL,
	},
	{
		.name = "bleed_current",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_BLEED_CURRENT,
	},
	{
		.name = "rfout_divider",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_RFOUT_DIV,
	},
	{
		.name = "rfout_divider_available",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_RFOUT_DIV_AVAIL,
	},
	{
		.name = "ndel",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_NDEL,
	},
	{
		.name = "rdel",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_RDEL,
	},
	{
		.name = "sysref_delay_adjust",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_SR_DEL_ADJ,
	},
	{
		.name = "sysref_invert_adjust",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_SR_INV_ADJ,
	},
	{
		.name = "sysref_monitoring",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_SR_MONITORING,
	},
	{
		.name = "default_register",
		.shared = IIO_SHARED_BY_ALL,
		.show = adf4377_iio_read_dev_attr,
		.store = adf4377_iio_write_dev_attr,
		.priv = ADF4377_IIO_DEV_ATTR_DEFAULT_REG,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adf4377_iio_ch_attributes[] = {
	{
		.name = "frequency",
		.shared = IIO_SEPARATE,
		.show = adf4377_iio_read_chan_attr,
		.store = adf4377_iio_write_chan_attr,
		.priv = ADF4377_IIO_CH_ATTR_FREQ,
	},
	{
		.name = "output_power",
		.shared = IIO_SEPARATE,
		.show = adf4377_iio_read_chan_attr,
		.store = adf4377_iio_write_chan_attr,
		.priv = ADF4377_IIO_CH_ATTR_OPWR,
	},
	{
		.name = "enable",
		.shared = IIO_SEPARATE,
		.show = adf4377_iio_read_chan_attr,
		.store = adf4377_iio_write_chan_attr,
		.priv = ADF4377_IIO_CH_ATTR_EN,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adf4377_iio_channels[] = {
	{
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.attributes = adf4377_iio_ch_attributes,
		.ch_out = true,
		.indexed = true,
		.scan_type = NULL,
	},
	{
		.ch_type = IIO_VOLTAGE,
		.channel = 1,
		.attributes = adf4377_iio_ch_attributes,
		.ch_out = true,
		.indexed = true,
		.scan_type = NULL,
	},
};

static struct iio_device adf4377_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adf4377_iio_channels),
	.channels = adf4377_iio_channels,
	.attributes = adf4377_iio_dev_attributes,
	.debug_reg_read = (int32_t (*)())adf4377_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adf4377_iio_write_reg
};

/**
 * @brief Initialize the ADF4377 IIO device.
 * @param iio_dev 	- The IIO device structure.
 * @param init_param 	- The structure containing the initialization parameters.
 * @return ret       	- Result of the initialization procedure.
 */
int adf4377_iio_init(struct adf4377_iio_dev **iio_dev,
		     struct adf4377_iio_dev_init_param *init_param)
{
	struct adf4377_iio_dev *dev;
	int32_t ret;

	dev = (struct adf4377_iio_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->iio_dev = &adf4377_iio_dev;
	ret = adf4377_init(&dev->adf4377_dev, init_param->adf4377_dev_init);
	if (ret)
		goto error;

	*iio_dev = dev;

	return ret;
error:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free the resources allocated by adf4377_iio_init().
 * @param dev	- The IIO device structure.
 * @return ret 	- Result of the remove procedure.
*/
int adf4377_iio_remove(struct adf4377_iio_dev *dev)
{
	int ret;

	ret = adf4377_remove(dev->adf4377_dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
