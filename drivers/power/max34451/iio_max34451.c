/***************************************************************************//**
 *   @file   iio_max34451.c
 *   @brief  Source file for the MAX34451 IIO Driver
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "max34451.h"
#include "iio_max34451.h"

/* Global attributes enumeration */
enum max34451_iio_global_attrs {
	MAX34451_IIO_FAULT_RETRY_ATTR,
	MAX34451_IIO_MFR_LOCATION_ATTR,
	MAX34451_IIO_MFR_DATE_ATTR,
	MAX34451_IIO_MFR_SERIAL_ATTR,
};

static const char *const max34451_enable_avail[2] = {
	"Disabled", "Enabled"
};

static struct iio_device max34451_iio_dev;
static struct iio_device max34451na6_iio_dev;
static struct iio_device max34455_iio_dev;

/**
 * @brief Read register value.
 * @param dev     - The iio device structure.
 * @param reg     - Register to read.
 * @param readval - Read value.
 * @return ret    - Returns 0 if successful.
*/
static int max34451_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451;
	int ret;
	uint8_t block[8] = {0}, byte;
	uint16_t word;

	if (!iio_max34451 || !iio_max34451->max34451_dev || !readval)
		return -EINVAL;

	max34451 = iio_max34451->max34451_dev;
	*readval = 0;

	switch (reg) {
	case MAX34451_PAGE:
	case MAX34451_OPERATION:
	case MAX34451_ON_OFF_CONFIG:
	case MAX34451_WRITE_PROTECT:
	case MAX34451_CAPABILITY:
	case MAX34451_VOUT_MODE:
	case MAX34451_STATUS_VOUT:
	case MAX34451_STATUS_IOUT:
	case MAX34451NA6_STATUS_INPUT:
	case MAX34451_STATUS_TEMPERATURE:
	case MAX34451_STATUS_CML:
	case MAX34451_STATUS_MFR_SPECIFIC:
	case MAX34451_PMBUS_REVISION:
	case MAX34451_MFR_ID:
	case MAX34451_MFR_MODEL:
	case MAX34451NA6_MFR_WP_CONTROL:
		ret = max34451_read_byte(max34451, iio_max34451->page, (uint8_t)reg,
					 &byte);
		if (ret)
			return ret;

		*readval = byte;

		return 0;
	case MAX34451_VOUT_MARGIN_HIGH:
	case MAX34451_VOUT_MARGIN_LOW:
	case MAX34451_VOUT_SCALE_MONITOR:
	case MAX34451_IOUT_CAL_GAIN:
	case MAX34451_VOUT_OV_FAULT_LIMIT:
	case MAX34451_VOUT_OV_WARN_LIMIT:
	case MAX34451_VOUT_UV_WARN_LIMIT:
	case MAX34451_VOUT_UV_FAULT_LIMIT:
	case MAX34451_IOUT_OC_WARN_LIMIT:
	case MAX34451_IOUT_OC_FAULT_LIMIT:
	case MAX34451_OT_FAULT_LIMIT:
	case MAX34451_OT_WARN_LIMIT:
	case MAX34451_POWER_GOOD_ON:
	case MAX34451_POWER_GOOD_OFF:
	case MAX34451_TON_DELAY:
	case MAX34451_TON_MAX_FAULT_LIMIT:
	case MAX34451_TOFF_DELAY:
	case MAX34451_STATUS_WORD:
	case MAX34451_READ_VOUT:
	case MAX34451_READ_IOUT:
	case MAX34451_READ_TEMPERATURE_1:
	case MAX34451_MFR_MODE:
	case MAX34451_MFR_REVISION:
	case MAX34451_MFR_VOUT_PEAK:
	case MAX34451_MFR_IOUT_PEAK:
	case MAX34451_MFR_TEMPERATURE_PEAK:
	case MAX34451_MFR_NV_LOG_CONFIG:
	case MAX34451_MFR_VOUT_MIN:
	case MAX34451_MFR_FAULT_RETRY:
	case MAX34451_MFR_MARGIN_CONFIG:
	case MAX34451_MFR_FW_SERIAL:
	case MAX34451_MFR_IOUT_AVG:
	case MAX34451_MFR_CHANNEL_CONFIG:
	case MAX34451_MFR_TON_SEQ_MAX:
	case MAX34451_MFR_TEMP_SENSOR_CONFIG:
	case MAX34451NA6_MFR_CONFIG_VERSION:
	case MAX34451NA6_READ_VIN:
	case MAX34451NA6_READ_IIN:
	case MAX34451_MFR_CRC:
		ret = max34451_read_word(max34451, iio_max34451->page, (uint8_t)reg,
					 &word);

		if (ret)
			return ret;

		*readval = word;

		return 0;
	case MAX34451_MFR_PSEN_CONFIG:
	case MAX34451_MFR_FAULT_RESPONSE:
	case MAX34451_MFR_TIME_COUNT:
	case MAX34451_MFR_PWM_CONFIG:
	case MAX34451_MFR_SEQ_CONFIG:
		ret = max34451_read_block_data(max34451, iio_max34451->page,
					       (uint8_t)reg, &block[0], 4);
		if (ret)
			return ret;

		*readval = no_os_get_unaligned_le32(block);

		return 0;
	case MAX34451_MFR_LOCATION:
	case MAX34451_MFR_DATE:
	case MAX34451_MFR_SERIAL:
		return -ENOTSUP;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Write register value.
 * @param dev     - The iio device structure.
 * @param reg     - Register to write.
 * @param writeval - Value to write.
 * @return ret    - Returns 0 if successful.
*/
static int max34451_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451;
	int ret;
	uint8_t block4[4];

	if (!iio_max34451 || !iio_max34451->max34451_dev)
		return -EINVAL;

	max34451 = iio_max34451->max34451_dev;

	switch (reg) {
	case MAX34451_PAGE:
		ret = max34451_set_page(max34451, (int)writeval);
		if (ret)
			return ret;

		iio_max34451->page = (int)writeval;

		return 0;
	case MAX34451_OPERATION:
	case MAX34451_ON_OFF_CONFIG:
	case MAX34451_WRITE_PROTECT:
	case MAX34451NA6_MFR_WP_CONTROL:
		return max34451_write_byte(max34451, iio_max34451->page, (uint8_t)reg,
					   (uint8_t)writeval);
	case MAX34451_CLEAR_FAULTS:
	case MAX34451_STORE_DEFAULT_ALL:
	case MAX34451_RESTORE_DEFAULT_ALL:
	case MAX34451_MFR_STORE_ALL:
	case MAX34451_MFR_RESTORE_ALL:
		return max34451_send_byte(max34451, iio_max34451->page, (uint8_t)reg);
	case MAX34451_VOUT_MARGIN_HIGH:
	case MAX34451_VOUT_MARGIN_LOW:
	case MAX34451_VOUT_SCALE_MONITOR:
	case MAX34451_IOUT_CAL_GAIN:
	case MAX34451_VOUT_OV_FAULT_LIMIT:
	case MAX34451_VOUT_OV_WARN_LIMIT:
	case MAX34451_VOUT_UV_WARN_LIMIT:
	case MAX34451_VOUT_UV_FAULT_LIMIT:
	case MAX34451_IOUT_OC_WARN_LIMIT:
	case MAX34451_IOUT_OC_FAULT_LIMIT:
	case MAX34451_OT_FAULT_LIMIT:
	case MAX34451_OT_WARN_LIMIT:
	case MAX34451_POWER_GOOD_ON:
	case MAX34451_POWER_GOOD_OFF:
	case MAX34451_TON_DELAY:
	case MAX34451_TON_MAX_FAULT_LIMIT:
	case MAX34451_TOFF_DELAY:
	case MAX34451_MFR_MODE:
	case MAX34451_MFR_VOUT_PEAK:
	case MAX34451_MFR_IOUT_PEAK:
	case MAX34451_MFR_TEMPERATURE_PEAK:
	case MAX34451_MFR_VOUT_MIN:
	case MAX34451_MFR_NV_LOG_CONFIG:
	case MAX34451_MFR_FAULT_RETRY:
	case MAX34451_MFR_MARGIN_CONFIG:
	case MAX34451_MFR_CHANNEL_CONFIG:
	case MAX34451_MFR_TON_SEQ_MAX:
	case MAX34451NA6_MFR_CONFIG_VERSION:
	case MAX34451_MFR_TEMP_SENSOR_CONFIG:
	case MAX34451_MFR_STORE_SINGLE:
	case MAX34451_MFR_CRC:
		return max34451_write_word(max34451, iio_max34451->page, (uint8_t)reg,
					   (uint16_t)writeval);
	case MAX34451_MFR_PSEN_CONFIG:
	case MAX34451_MFR_FAULT_RESPONSE:
	case MAX34451_MFR_TIME_COUNT:
	case MAX34451_MFR_PWM_CONFIG:
	case MAX34451_MFR_SEQ_CONFIG:
		no_os_put_unaligned_le32(writeval, block4);
		return max34451_write_block_data(max34451, iio_max34451->page,
						 (uint8_t)reg, block4, 4);
	case MAX34451_MFR_LOCATION:
	case MAX34451_MFR_DATE:
	case MAX34451_MFR_SERIAL:
		return -ENOTSUP;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for manufacturer string attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_mfr_str(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	char str[9] = {0};
	int ret;

	ret = max34451_read_block_data(max34451, MAX34451_CHAN_ALL, (uint8_t)priv,
				       (uint8_t *)str, sizeof(str) - 1);
	if (ret)
		return ret;

	return sprintf(buf, "%s", str);
}

/**
 * @brief Handles the write request for manufacturer string attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - User buffer containing the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Returns 0 if successful.
*/
static int max34451_iio_write_mfr_str(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	uint8_t str[9] = {0};
	size_t n;

	n = len;
	if (n && buf[n - 1] == '\0')
		n--;

	if (n > 8)
		return -EMSGSIZE;

	memcpy(str, buf, n);
	return max34451_write_block_data(max34451, MAX34451_CHAN_ALL, (uint8_t)priv,
					 str, sizeof(str) - 1);
}

/**
 * @brief Handles the read request for raw values.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	int ret;
	uint16_t value;
	int32_t raw;

	ret = max34451_read_word(max34451, channel->ch_num,
				 (uint8_t)channel->address, &value);
	if (ret)
		return ret;

	raw = (int16_t)value;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &raw);
}

/**
 * @brief Handles the read request for the scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	int ret;
	int32_t vals[2];

	ret = max34451_get_scale(max34451, (uint8_t)channel->address, &vals[0],
				 &vals[1]);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

/**
 * @brief Handles the read request for enable attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	int ret;
	uint8_t val;

	ret = max34451_read_byte(max34451, channel->ch_num,
				 MAX34451_OPERATION, &val);
	if (ret)
		return ret;

	val = (val & MAX34451_OPERATION_ON_MSK) ? 1 : 0;

	return sprintf(buf, "%s", max34451_enable_avail[val]);
}

/**
 * @brief Handles the read request for enable_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_enable_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(max34451_enable_avail); i++)
		length += sprintf(buf + length, "%s ", max34451_enable_avail[i]);

	return length;
}

/**
 * @brief Handles the write request for enable attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - User buffer containing the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Returns 0 if successful.
*/
static int max34451_iio_write_enable(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	uint32_t i;
	uint8_t op;
	int ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(max34451_enable_avail); i++)
		if (!strcmp(buf, max34451_enable_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(max34451_enable_avail))
		return -EINVAL;

	ret = max34451_read_byte(max34451, channel->ch_num, MAX34451_OPERATION, &op);
	if (ret)
		return ret;

	/* Bits [7:6] are one field, not two flags: 01b is a sequenced turn-off
	 * and 10b is on, so 11b has no entry in enum max34451_operation_type.
	 * Carry TOFF_DELAY across a disable only; an enable keeps the sequence
	 * bits alone. */
	if (i)
		op = MAX34451_OPERATION_ON_MSK | (op & MAX34451_OPERATION_SEQ_MSK);
	else
		op &= MAX34451_OPERATION_TOFF_DELAY_MSK |
		      MAX34451_OPERATION_SEQ_MSK;

	return max34451_set_operation(max34451, channel->ch_num,
				      (enum max34451_operation_type)op);
}

/**
 * @brief Read a word-data PMBus command for a page and format it in base units.
 * @param max34451 - The MAX34451 device structure.
 * @param buf      - Command buffer to be filled with requested data.
 * @param len      - Length of the received command buffer in bytes.
 * @param page     - PMBus page to read from.
 * @param cmd      - PMBus command code.
 * @return ret     - Result of the reading procedure.
 * 		     In case of success, the size of the read data is returned.
*/
static int max34451_iio_show_word_data(struct max34451_dev *max34451,
				       char *buf, uint32_t len, int page,
				       uint8_t cmd)
{
	int32_t vals[2];
	int ret, data;

	ret = max34451_read_word_data(max34451, page, cmd, &data);
	if (ret)
		return ret;

	vals[0] = data;
	vals[1] = (int32_t)MILLI;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

/**
 * @brief Parse a value in base units and write it to a word-data PMBus command.
 * @param max34451 - The MAX34451 device structure.
 * @param buf      - User buffer containing the data to be written.
 * @param page     - PMBus page to write to.
 * @param cmd      - PMBus command code.
 * @return ret     - Returns 0 if successful.
*/
static int max34451_iio_store_word_data(struct max34451_dev *max34451,
					char *buf, int page, uint8_t cmd)
{
	int32_t val1 = 0, val2 = 0;
	int ret, data;

	ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val1, &val2);
	if (ret)
		return ret;

	data = abs(val1) * (int)MILLI + abs(val2) / (int)MILLI;
	if (val1 < 0 || val2 < 0)
		data = -data;

	return max34451_write_word_data(max34451, page, cmd, data);
}

/**
 * @brief Handles the read request for limit channel attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - PMBus command code of the limit.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_limits(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;

	return max34451_iio_show_word_data(iio_max34451->max34451_dev, buf, len,
					   channel->ch_num, (uint8_t)priv);
}

/**
 * @brief Handles the write request for limit channel attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - User buffer containing the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - PMBus command code of the limit.
 * @return ret    - Returns 0 if successful.
*/
static int max34451_iio_write_limits(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;

	return max34451_iio_store_word_data(iio_max34451->max34451_dev, buf,
					    channel->ch_num, (uint8_t)priv);
}

/**
 * @brief Handles the read request for global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Global attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_global_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;

	switch (priv) {
	case MAX34451_IIO_FAULT_RETRY_ATTR:
		return max34451_iio_show_word_data(iio_max34451->max34451_dev,
						   buf, len, MAX34451_CHAN_ALL,
						   MAX34451_MFR_FAULT_RETRY);
	case MAX34451_IIO_MFR_LOCATION_ATTR:
		return max34451_iio_read_mfr_str(dev, buf, len, channel,
						 MAX34451_MFR_LOCATION);
	case MAX34451_IIO_MFR_DATE_ATTR:
		return max34451_iio_read_mfr_str(dev, buf, len, channel,
						 MAX34451_MFR_DATE);
	case MAX34451_IIO_MFR_SERIAL_ATTR:
		return max34451_iio_read_mfr_str(dev, buf, len, channel,
						 MAX34451_MFR_SERIAL);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the write request for global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - User buffer containing the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Global attribute id.
 * @return ret    - Returns 0 if successful.
*/
static int max34451_iio_write_global_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;

	switch (priv) {
	case MAX34451_IIO_FAULT_RETRY_ATTR:
		return max34451_iio_store_word_data(iio_max34451->max34451_dev,
						    buf, MAX34451_CHAN_ALL,
						    MAX34451_MFR_FAULT_RETRY);
	case MAX34451_IIO_MFR_LOCATION_ATTR:
		return max34451_iio_write_mfr_str(dev, buf, len, channel,
						  MAX34451_MFR_LOCATION);
	case MAX34451_IIO_MFR_DATE_ATTR:
		return max34451_iio_write_mfr_str(dev, buf, len, channel,
						  MAX34451_MFR_DATE);
	case MAX34451_IIO_MFR_SERIAL_ATTR:
		return max34451_iio_write_mfr_str(dev, buf, len, channel,
						  MAX34451_MFR_SERIAL);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read a status PMBus command for a page and format it.
 * @param max34451 - The MAX34451 device structure.
 * @param buf      - Command buffer to be filled with requested data.
 * @param len      - Length of the received command buffer in bytes.
 * @param page     - PMBus page to read from.
 * @param cmd      - PMBus status command code.
 * @return ret     - Result of the reading procedure.
 * 		     In case of success, the size of the read data is returned.
*/
static int max34451_iio_show_status(struct max34451_dev *max34451, char *buf,
				    uint32_t len, int page, uint8_t cmd)
{
	int ret;
	int32_t val;
	uint16_t status_word;
	uint8_t status_byte;

	if (cmd == MAX34451_STATUS_WORD) {
		ret = max34451_read_word(max34451, page, cmd, &status_word);
		if (ret)
			return ret;

		val = (int32_t)status_word;
	} else {
		ret = max34451_read_byte(max34451, page, cmd, &status_byte);
		if (ret)
			return ret;

		val = (int32_t)status_byte;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the read request for status channel attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - PMBus status command code.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_status(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;

	return max34451_iio_show_status(iio_max34451->max34451_dev, buf, len,
					channel->ch_num, (uint8_t)priv);
}

/**
 * @brief Handles the read request for device-wide status debug attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - PMBus status command code.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_debug_status(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;

	return max34451_iio_show_status(iio_max34451->max34451_dev, buf, len,
					MAX34451_CHAN_ALL, (uint8_t)priv);
}

/**
 * @brief Initializes the MAX34451 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int max34451_iio_init(struct max34451_iio_desc **iio_desc,
		      struct max34451_iio_desc_init_param *init_param)
{
	struct max34451_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->max34451_init_param || !iio_desc)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = max34451_init(&descriptor->max34451_dev,
			    init_param->max34451_init_param);
	if (ret)
		goto dev_err;

	descriptor->page = MAX34451_CHAN_ALL;

	switch (descriptor->max34451_dev->id) {
	case ID_MAX34455:
		descriptor->iio_dev = &max34455_iio_dev;
		break;
	case ID_MAX34451:
		descriptor->iio_dev = &max34451_iio_dev;
		break;
	case ID_MAX34451NA6:
		descriptor->iio_dev = &max34451na6_iio_dev;
		break;
	default:
		ret = -EINVAL;
		goto dev_err;
	}

	*iio_desc = descriptor;

	return 0;

dev_err:
	if (descriptor->max34451_dev)
		max34451_remove(descriptor->max34451_dev);
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int max34451_iio_remove(struct max34451_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	if (iio_desc->max34451_dev)
		max34451_remove(iio_desc->max34451_dev);

	no_os_free(iio_desc);

	return 0;
}

#define MAX34451_IIO_LIM(_name, _reg) { \
	.name = _name, \
	.show = max34451_iio_read_limits, \
	.store = max34451_iio_write_limits, \
	.priv = _reg \
}

#define MAX34451_IIO_STATUS(_name, _reg) { \
	.name = _name, \
	.show = max34451_iio_read_status, \
	.priv = _reg \
}

/* VIN and IIN (MAX34451NA6 only) */
static struct iio_attribute max34451_input_attrs[] = {
	{
		.name = "raw",
		.show = max34451_iio_read_raw
	},
	{
		.name = "scale",
		.show = max34451_iio_read_scale
	},
	END_ATTRIBUTES_ARRAY
};

/* Sequenced VOUT channels */
static struct iio_attribute max34451_output_attrs[] = {
	{
		.name = "raw",
		.show = max34451_iio_read_raw
	},
	{
		.name = "scale",
		.show = max34451_iio_read_scale
	},
	{
		.name = "enable",
		.show = max34451_iio_read_enable,
		.store = max34451_iio_write_enable,
	},
	{
		.name = "enable_available",
		.show = max34451_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	MAX34451_IIO_LIM("vout_margin_high", MAX34451_VOUT_MARGIN_HIGH),
	MAX34451_IIO_LIM("vout_margin_low", MAX34451_VOUT_MARGIN_LOW),
	MAX34451_IIO_LIM("vout_ov_fault_limit", MAX34451_VOUT_OV_FAULT_LIMIT),
	MAX34451_IIO_LIM("vout_uv_fault_limit", MAX34451_VOUT_UV_FAULT_LIMIT),
	MAX34451_IIO_LIM("power_good_on", MAX34451_POWER_GOOD_ON),
	MAX34451_IIO_LIM("power_good_off", MAX34451_POWER_GOOD_OFF),
	MAX34451_IIO_LIM("ton_delay", MAX34451_TON_DELAY),
	MAX34451_IIO_LIM("toff_delay", MAX34451_TOFF_DELAY),
	MAX34451_IIO_STATUS("status_vout", MAX34451_STATUS_VOUT),
	MAX34451_IIO_STATUS("status_mfr_specific", MAX34451_STATUS_MFR_SPECIFIC),
	MAX34451_IIO_STATUS("status_word", MAX34451_STATUS_WORD),
	END_ATTRIBUTES_ARRAY
};

/* Monitor-only VOUT channels */
static struct iio_attribute max34451_vout_attrs[] = {
	{
		.name = "raw",
		.show = max34451_iio_read_raw
	},
	{
		.name = "scale",
		.show = max34451_iio_read_scale
	},
	MAX34451_IIO_LIM("vout_ov_fault_limit", MAX34451_VOUT_OV_FAULT_LIMIT),
	MAX34451_IIO_LIM("vout_uv_fault_limit", MAX34451_VOUT_UV_FAULT_LIMIT),
	MAX34451_IIO_LIM("power_good_on", MAX34451_POWER_GOOD_ON),
	MAX34451_IIO_LIM("power_good_off", MAX34451_POWER_GOOD_OFF),
	MAX34451_IIO_STATUS("status_vout", MAX34451_STATUS_VOUT),
	MAX34451_IIO_STATUS("status_mfr_specific", MAX34451_STATUS_MFR_SPECIFIC),
	MAX34451_IIO_STATUS("status_word", MAX34451_STATUS_WORD),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max34451_iout_attrs[] = {
	{
		.name = "raw",
		.show = max34451_iio_read_raw
	},
	{
		.name = "scale",
		.show = max34451_iio_read_scale
	},
	MAX34451_IIO_LIM("iout_oc_fault_limit", MAX34451_IOUT_OC_FAULT_LIMIT),
	MAX34451_IIO_STATUS("status_iout", MAX34451_STATUS_IOUT),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max34451_temp_attrs[] = {
	{
		.name = "raw",
		.show = max34451_iio_read_raw
	},
	{
		.name = "scale",
		.show = max34451_iio_read_scale
	},
	MAX34451_IIO_LIM("ot_fault_limit", MAX34451_OT_FAULT_LIMIT),
	MAX34451_IIO_LIM("ot_warn_limit", MAX34451_OT_WARN_LIMIT),
	MAX34451_IIO_STATUS("status_temperature", MAX34451_STATUS_TEMPERATURE),
	END_ATTRIBUTES_ARRAY
};

/* Device global attributes */
static struct iio_attribute max34451_global_attrs[] = {
	{
		.name = "fault_retry",
		.priv = MAX34451_IIO_FAULT_RETRY_ATTR,
		.show = max34451_iio_read_global_attr,
		.store = max34451_iio_write_global_attr,
	},
	{
		.name = "mfr_location",
		.priv = MAX34451_IIO_MFR_LOCATION_ATTR,
		.show = max34451_iio_read_global_attr,
		.store = max34451_iio_write_global_attr,
	},
	{
		.name = "mfr_date",
		.priv = MAX34451_IIO_MFR_DATE_ATTR,
		.show = max34451_iio_read_global_attr,
		.store = max34451_iio_write_global_attr,
	},
	{
		.name = "mfr_serial",
		.priv = MAX34451_IIO_MFR_SERIAL_ATTR,
		.show = max34451_iio_read_global_attr,
		.store = max34451_iio_write_global_attr,
	},
	END_ATTRIBUTES_ARRAY
};

/* Device debug attributes */
static struct iio_attribute max34451_debug_attrs[] = {
	{
		.name = "status_cml",
		.priv = MAX34451_STATUS_CML,
		.show = max34451_iio_read_debug_status,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel max34451_channels[] = {
	/* Sequencing voltage outputs (0-11) */
	{
		.name = "vout0",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_0,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_1,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout2",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_2,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout3",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_3,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout4",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_4,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout5",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_5,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout6",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_6,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout7",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_7,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout8",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_8,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout9",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_9,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout10",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_10,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout11",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_11,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	/* Monitor-only voltage inputs (12-15) */
	{
		.name = "vout12",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_12,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_vout_attrs,
		.ch_out = false
	},
	{
		.name = "vout13",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_13,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_vout_attrs,
		.ch_out = false
	},
	{
		.name = "vout14",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_14,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_vout_attrs,
		.ch_out = false
	},
	{
		.name = "vout15",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_15,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_vout_attrs,
		.ch_out = false
	},
	/* Current monitors (0-15) */
	{
		.name = "iout0",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_0,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout1",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_1,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout2",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_2,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout3",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_3,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout4",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_4,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout5",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_5,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout6",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_6,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout7",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_7,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout8",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_8,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout9",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_9,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout10",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_10,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout11",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_11,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout12",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_12,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout13",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_13,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout14",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_14,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout15",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_15,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	/* Temperature sensors (pages 16-20) */
	{
		.name = "temp0",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34451_CHAN_16,
		.address = MAX34451_VAL_TEMP,
		.attributes = max34451_temp_attrs,
		.ch_out = false
	},
	{
		.name = "temp1",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34451_CHAN_17,
		.address = MAX34451_VAL_TEMP,
		.attributes = max34451_temp_attrs,
		.ch_out = false
	},
	{
		.name = "temp2",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34451_CHAN_18,
		.address = MAX34451_VAL_TEMP,
		.attributes = max34451_temp_attrs,
		.ch_out = false
	},
	{
		.name = "temp3",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34451_CHAN_19,
		.address = MAX34451_VAL_TEMP,
		.attributes = max34451_temp_attrs,
		.ch_out = false
	},
	{
		.name = "temp4",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34451_CHAN_20,
		.address = MAX34451_VAL_TEMP,
		.attributes = max34451_temp_attrs,
		.ch_out = false
	},

	/* VIN/IIN (NA6+ only). These MUST remain the last two entries:
	 * max34451_iio_dev subtracts MAX34451_NA6_ONLY_CHANNELS from num_ch to
	 * hide them from the base part. Append new channels above this block.
	 */
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_ALL,
		.address = MAX34451NA6_VAL_VIN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iin",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_ALL,
		.address = MAX34451NA6_VAL_IIN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
};

static struct iio_channel max34455_channels[] = {
	/* Sequencing voltage outputs (0-7) */
	{
		.name = "vout0",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_0,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_1,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout2",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_2,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout3",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_3,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout4",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_4,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout5",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_5,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout6",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_6,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout7",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_7,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	/* Monitor-only voltage inputs (8-11) */
	{
		.name = "vout8",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_8,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_vout_attrs,
		.ch_out = false
	},
	{
		.name = "vout9",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_9,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_vout_attrs,
		.ch_out = false
	},
	{
		.name = "vout10",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_10,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_vout_attrs,
		.ch_out = false
	},
	{
		.name = "vout11",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_11,
		.address = MAX34451_VAL_VOUT,
		.attributes = max34451_vout_attrs,
		.ch_out = false
	},
	/* Current monitors (0-11) */
	{
		.name = "iout0",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_0,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout1",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_1,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout2",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_2,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout3",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_3,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout4",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_4,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout5",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_5,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout6",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_6,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout7",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_7,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout8",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_8,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout9",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_9,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout10",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_10,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	{
		.name = "iout11",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_11,
		.address = MAX34451_VAL_IOUT,
		.attributes = max34451_iout_attrs,
		.ch_out = false
	},
	/* Temperature sensors (pages 12-16) */
	{
		.name = "temp0",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34455_CHAN_12,
		.address = MAX34451_VAL_TEMP,
		.attributes = max34451_temp_attrs,
		.ch_out = false
	},
	{
		.name = "temp1",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34455_CHAN_13,
		.address = MAX34451_VAL_TEMP,
		.attributes = max34451_temp_attrs,
		.ch_out = false
	},
	{
		.name = "temp2",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34455_CHAN_14,
		.address = MAX34451_VAL_TEMP,
		.attributes = max34451_temp_attrs,
		.ch_out = false
	},
	{
		.name = "temp3",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34455_CHAN_15,
		.address = MAX34451_VAL_TEMP,
		.attributes = max34451_temp_attrs,
		.ch_out = false
	},
	{
		.name = "temp4",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34455_CHAN_16,
		.address = MAX34451_VAL_TEMP,
		.attributes = max34451_temp_attrs,
		.ch_out = false
	}
};

#define MAX34451_NA6_ONLY_CHANNELS 2

static struct iio_device max34451_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(max34451_channels) - MAX34451_NA6_ONLY_CHANNELS,
	.channels = max34451_channels,
	.attributes = max34451_global_attrs,
	.debug_attributes = max34451_debug_attrs,
	.debug_reg_read = max34451_iio_reg_read,
	.debug_reg_write = max34451_iio_reg_write
};

static struct iio_device max34451na6_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(max34451_channels),
	.channels = max34451_channels,
	.attributes = max34451_global_attrs,
	.debug_attributes = max34451_debug_attrs,
	.debug_reg_read = max34451_iio_reg_read,
	.debug_reg_write = max34451_iio_reg_write
};

static struct iio_device max34455_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(max34455_channels),
	.channels = max34455_channels,
	.attributes = max34451_global_attrs,
	.debug_attributes = max34451_debug_attrs,
	.debug_reg_read = max34451_iio_reg_read,
	.debug_reg_write = max34451_iio_reg_write
};
