/***************************************************************************//**
 *   @file   iio_adf5611.c
 *   @brief  Implementation of IIO ADF5611 Driver.
 *   @author JOsemene (jude.osemene@analog.com)
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
#include "iio_adf5611.h"
#include "adf5611.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

/**
 * @brief Supported charge pump currents
 */
static const int ci_table[16][2] = {
	{0, 200000},
	{0, 400000},
	{0, 600000},
	{0, 800000},
	{1, 000000},
	{1, 200000},
	{1, 400000},
	{1, 600000},
	{1, 800000},
	{2, 000000},
	{2, 200000},
	{2, 400000},
	{2, 600000},
	{2, 800000},
	{3, 000000},
	{3, 200000}
};

/**
 * @brief Supported rfoutdiv_divider_values
 */
static const int rfoutdiv_div_table[8] = {
	1,   2,   4,   8,
	16,  32,  64,  128
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
static int adf5611_iio_find_2d_row(const int (*tbl)[2], const int size,
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
static int adf5611_iio_find_element_in_tbl(const int *tbl, const int size,
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
 * @brief IIO Wrapper for reading ADF5611 frequency.
 * @param dev       - The iio device structure.
 * @param reg    - Address of the register to be read from.
 * @param readval   - Read data.
 * @return          - Result of the reading procedure.
*/
static int adf5611_iio_read_reg(struct adf5611_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	uint8_t tmp;
	int ret;
	ret = adf5611_spi_read(dev->adf5611_dev, (uint16_t)reg, &tmp);
	if (ret)
		return ret;
	*readval = tmp;
	return 0;
}

/**
 * @brief IIO Wrapper for writing adf5611 register.
 * @param dev	    - The iio device structure.
 * @param reg    - Address of the register to be written to.
 * @param writeval  - Data to be written.
 * @return	        - Result of the writing procedure.
 */
static int adf5611_iio_write_reg(struct adf5611_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	return adf5611_spi_write(dev->adf5611_dev, (uint16_t)reg,
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
static int adf5611_iio_read_device_attr(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adf5611_iio_dev *iio_adf5611 = (struct adf5611_iio_dev *)dev;
	struct adf5611_dev *adf5611;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int32_t val_b[2];
	char buffer[5];
	int32_t cp_i;
	int8_t div;
	uint8_t i;
	bool en;
	int ret;

	if (!iio_adf5611)
		return -EINVAL;

	adf5611 = iio_adf5611->adf5611_dev;

	if (!adf5611)
		return -EINVAL;

	switch (priv) {
	case ADF5611_IIO_DEV_ATTR_CP_I:
		ret = adf5611_get_cp_i(adf5611, &cp_i);
		if (ret)
			return ret;

		val_b[0] = ci_table[cp_i][0];
		val_b[1] = ci_table[cp_i][1];
		ret = iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, val_b);
		break;
	case ADF5611_IIO_DEV_ATTR_CP_AVAIL:
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
	case ADF5611_IIO_DEV_ATTR_REF_DIV:
		ret = adf5611_get_ref_div(adf5611, &val);
		if (ret)
			return ret;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF5611_IIO_DEV_ATTR_REF_CLK:
		ret = adf5611_get_ref_clk(adf5611, &val_64);
		if (ret)
			return ret;
		ret = snprintf(buf, len, "%"PRIu64, val_64);
		break;
	case ADF5611_IIO_DEV_ATTR_RFOUTDIV_PWR:
		ret = adf5611_get_rfoutdiv_power(adf5611, &val);
		if (ret)
			return ret;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF5611_IIO_DEV_ATTR_EN_RFOUTDIV:
		ret = adf5611_get_en_rfoutdiv(adf5611, &en);
		if (ret)
			return ret;
		val = en;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF5611_IIO_DEV_ATTR_RFOUTDIV_DIV:
		ret = adf5611_get_rfoutdiv_divider(adf5611, &div);
		if (ret)
			return ret;
		val = rfoutdiv_div_table[div];
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		break;
	case ADF5611_IIO_DEV_ATTR_RFOUTDIV_DIV_AVAIL:
		strcpy(buf, "");
		for (i = 0; i < NO_OS_ARRAY_SIZE(rfoutdiv_div_table); i++) {
			val = rfoutdiv_div_table[i];
			iio_format_value(buffer, len, IIO_VAL_INT, 1, &val);
			strcat(buf, buffer);
			if (i < NO_OS_ARRAY_SIZE(rfoutdiv_div_table) - 1)
				strcat(buf, " ");
		}
		ret = strlen(buf);
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
static int adf5611_iio_write_device_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct adf5611_iio_dev *iio_adf5611 = (struct adf5611_iio_dev *)dev;
	struct adf5611_dev *adf5611;
	int32_t val = -EINVAL;
	uint64_t val_64 = 0;
	int32_t val_b[2];
	int8_t index;
	bool en;
	int ret;

	if (!iio_adf5611)
		return -EINVAL;
	adf5611 = iio_adf5611->adf5611_dev;

	if (!adf5611)
		return -EINVAL;

	switch (priv) {
	case ADF5611_IIO_DEV_ATTR_CP_I:
		ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val_b[0],
				      &val_b[1]);
		if (ret)
			return ret;

		index = adf5611_iio_find_2d_row(ci_table, NO_OS_ARRAY_SIZE(ci_table),
						val_b[0], val_b[1]);
		if (index < 0)
			return index;
		val = index;
		ret = adf5611_set_cp_i(adf5611, val);
		break;
	case ADF5611_IIO_DEV_ATTR_REF_DIV:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;
		ret = adf5611_set_ref_div(adf5611, val);
		break;
	case ADF5611_IIO_DEV_ATTR_REF_CLK:
		sscanf(buf, "%"PRIu64, &val_64);
		ret = adf5611_set_ref_clk(adf5611, val_64);
		break;
	case ADF5611_IIO_DEV_ATTR_RFOUTDIV_PWR:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;
		ret = adf5611_set_ref_div(adf5611, val);
		break;
	case ADF5611_IIO_DEV_ATTR_EN_RFOUTDIV:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;
		en = val;
		ret = adf5611_set_en_rfoutdiv(adf5611, en);
		break;
	case ADF5611_IIO_DEV_ATTR_RFOUTDIV_DIV:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret)
			return ret;
		index = adf5611_iio_find_element_in_tbl(rfoutdiv_div_table,
							NO_OS_ARRAY_SIZE
							(rfoutdiv_div_table), val);
		if (index < 0)
			return index;
		ret = adf5611_set_rfoutdiv_divider(adf5611, index);
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
static int adf5611_iio_read_channel_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct adf5611_iio_dev *iio_adf5611 = (struct adf5611_iio_dev *)dev;
	struct adf5611_dev *adf5611;
	uint64_t val = -EINVAL;
	int8_t opwr;
	int32_t val_32;
	int ret;

	if (!iio_adf5611)
		return -EINVAL;

	adf5611 = iio_adf5611->adf5611_dev;

	if (!adf5611)
		return -EINVAL;

	switch (priv) {
	case ADF5611_IIO_CH_ATTR_FREQ:
		ret = adf5611_get_rfout(adf5611, &val);
		if (ret)
			return ret;
		ret = snprintf(buf, len, "%"PRIu64, val);
		break;
	case ADF5611_IIO_CH_ATTR_OPWR:
		ret = adf5611_get_output_power(adf5611, &opwr);
		if (ret)
			return ret;
		val_32 = opwr;
		ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &val_32);
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
static int adf5611_iio_write_channel_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct adf5611_iio_dev *iio_adf5611 = (struct adf5611_iio_dev *)dev;
	struct adf5611_dev *adf5611;
	int64_t val = -EINVAL;
	int32_t val_32;
	int8_t opwr;

	int ret;

	if (!iio_adf5611)
		return -EINVAL;

	adf5611 = iio_adf5611->adf5611_dev;

	if (!adf5611)
		return -EINVAL;

	switch (priv) {
	case ADF5611_IIO_CH_ATTR_FREQ:
		sscanf(buf, "%"PRIu64, &val);
		ret = adf5611_set_rfout(adf5611, val);
		break;
	case ADF5611_IIO_CH_ATTR_OPWR:
		ret = iio_parse_value(buf, IIO_VAL_INT, &val_32, NULL);
		if (ret)
			return ret;
		opwr = val_32;
		ret = adf5611_set_output_power(adf5611, opwr);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	return 0;
}

static struct iio_attribute adf5611_iio_attrs[] = {
	{
		.name = "reference_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF5611_IIO_DEV_ATTR_REF_CLK,
		.show = adf5611_iio_read_device_attr,
		.store = adf5611_iio_write_device_attr,
	},
	{
		.name = "reference_divider",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF5611_IIO_DEV_ATTR_REF_DIV,
		.show = adf5611_iio_read_device_attr,
		.store = adf5611_iio_write_device_attr,
	},
	{
		.name = "charge_pump_current",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF5611_IIO_DEV_ATTR_CP_I,
		.show = adf5611_iio_read_device_attr,
		.store = adf5611_iio_write_device_attr,
	},
	{
		.name = "charge_pump_current_available",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF5611_IIO_DEV_ATTR_CP_AVAIL,
		.show = adf5611_iio_read_device_attr,
	},
	{
		.name = "rfoutdiv_power",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF5611_IIO_DEV_ATTR_RFOUTDIV_PWR,
		.show = adf5611_iio_read_device_attr,
		.store = adf5611_iio_write_device_attr,
	},
	{
		.name = "rfoutdiv_divider",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF5611_IIO_DEV_ATTR_RFOUTDIV_DIV,
		.show = adf5611_iio_read_device_attr,
		.store = adf5611_iio_write_device_attr,
	},
	{
		.name = "rfoutdiv_divider_available",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF5611_IIO_DEV_ATTR_RFOUTDIV_DIV_AVAIL,
		.show = adf5611_iio_read_device_attr,
	},
	{
		.name = "en_rfoutdiv",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF5611_IIO_DEV_ATTR_EN_RFOUTDIV,
		.show = adf5611_iio_read_device_attr,
		.store = adf5611_iio_write_device_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adf5611_iio_ch_attrs[] = {
	{
		.name = "rfout_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.priv = ADF5611_IIO_CH_ATTR_FREQ,
		.show = adf5611_iio_read_channel_attr,
		.store = adf5611_iio_write_channel_attr,
	},
	{
		.name = "rfout_power",
		.shared = IIO_SHARED_BY_TYPE,
		.priv = ADF5611_IIO_CH_ATTR_OPWR,
		.show = adf5611_iio_read_channel_attr,
		.store = adf5611_iio_write_channel_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adf5611_channels[] = {
	{
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 0,
		.ch_out = true,
		.indexed = true,
		.attributes = adf5611_iio_ch_attrs,
		.scan_type = NULL,
	}
};

static struct iio_device adf5611_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adf5611_channels),
	.channels = adf5611_channels,
	.attributes = adf5611_iio_attrs,
	.debug_reg_read = (int32_t (*)())adf5611_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adf5611_iio_write_reg
};


/**
 * @brief Initializes the ADF5611 IIO driver
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial parameters.
 * @return ret	     - Result of the initialization procedure.
 */
int adf5611_iio_init(struct adf5611_iio_dev **iio_dev,
		     struct adf5611_iio_dev_init_param *init_param)
{
	struct adf5611_iio_dev *dev;
	int ret;

	dev = (struct adf5611_iio_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->iio_dev = &adf5611_iio_dev;

	ret = adf5611_init(&dev->adf5611_dev, init_param->adf5611_dev_init);

	if (ret)
		goto error;

	*iio_dev = dev;

	return ret;

error:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free the resources allocated by adf5611_iio_init().
 * @param dev	- The IIO device structure.
 * @return ret 	- Result of the remove procedure.
*/
int adf5611_iio_remove(struct adf5611_iio_dev *dev)
{
	int ret;

	ret = adf5611_remove(dev->adf5611_dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
