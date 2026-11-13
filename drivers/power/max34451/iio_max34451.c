/***************************************************************************//**
 *   @file   iio_max34451.c
 *   @brief  Source file for the MAX34451 IIO Driver
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
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
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "max34451.h"
#include "iio_max34451.h"

#define MAX34451_IIO_REG_CHAN(reg, chan)			((reg) | ((chan) << 8))
#define MAX34451_IIO_REG(x)				((x) & 0xFF)
#define MAX34451_IIO_CHAN(x)				(((x) >> 8) & 0xFF)

#define MAX34451_IIO_VOUT_CHAN_GROUP(inst)	MAX34451_IIO_VOUT_ ##inst## _CHAN, \
					MAX34451_IIO_IOUT_ ##inst## _CHAN

static const char *const max34451_enable_avail[2] = {
	"Disabled", "Enabled"
};

enum max34451_iio_chan_type {
	MAX34451_IIO_VOUT_CHAN_GROUP(0),
	MAX34451_IIO_VOUT_CHAN_GROUP(1),
	MAX34451_IIO_VOUT_CHAN_GROUP(2),
	MAX34451_IIO_VOUT_CHAN_GROUP(3),
	MAX34451_IIO_VOUT_CHAN_GROUP(4),
	MAX34451_IIO_VOUT_CHAN_GROUP(5),
	MAX34451_IIO_VOUT_CHAN_GROUP(6),
	MAX34451_IIO_VOUT_CHAN_GROUP(7),
	MAX34451_IIO_VOUT_CHAN_GROUP(8),
	MAX34451_IIO_VOUT_CHAN_GROUP(9),
	MAX34451_IIO_VOUT_CHAN_GROUP(10),
	MAX34451_IIO_VOUT_CHAN_GROUP(11),
	MAX34451_IIO_VOUT_CHAN_GROUP(12),
	MAX34451_IIO_VOUT_CHAN_GROUP(13),
	MAX34451_IIO_VOUT_CHAN_GROUP(14),
	MAX34451_IIO_VOUT_CHAN_GROUP(15),
	MAX34451_IIO_VIN_CHAN,
	MAX34451_IIO_IIN_CHAN,
	MAX34451_IIO_TEMP_0_CHAN,
	MAX34451_IIO_TEMP_1_CHAN,
	MAX34451_IIO_TEMP_2_CHAN,
	MAX34451_IIO_TEMP_3_CHAN,
	MAX34451_IIO_TEMP_4_CHAN,
};

static struct iio_device max34451_iio_dev;
static struct iio_device max34451na6_iio_dev;
static struct iio_device max34455_iio_dev;

/**
 * @brief Read register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to read.
 * @param readval - Read value.
 * @return ret    - Returns 0 if successful.
*/
static int max34451_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	int ret;
	uint8_t block[8] = {0}, byte;
	uint16_t word;
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
	case MAX34451_MFR_NV_FAULT_LOG:
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
 * @param reg	  - Register to write.
 * @param writeval - Value to write.
 * @return ret    - Returns 0 if successful.
*/
static int max34451_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	int ret;

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
	case MAX34451_MFR_SEQ_CONFIG: {
		uint8_t block4[4];
		no_os_put_unaligned_le32(writeval, block4);
		return max34451_write_block_data(max34451, iio_max34451->page,
						 (uint8_t)reg, block4, 4);
	}
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

	switch (channel->address) {
	case MAX34451_IIO_VOUT_0_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_0,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_1_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_1,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_2_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_2,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_3_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_3,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_4_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_4,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_5_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_5,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_6_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_6,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_7_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_7,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_8_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_8,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_9_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_9,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_10_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_10,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_11_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_11,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_12_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_12,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_13_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_13,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_14_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_14,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_15_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_15,
					 MAX34451_READ_VOUT, &value);
		break;
	case MAX34451_IIO_IOUT_0_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_0,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_1_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_1,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_2_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_2,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_3_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_3,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_4_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_4,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_5_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_5,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_6_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_6,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_7_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_7,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_8_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_8,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_9_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_9,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_10_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_10,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_11_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_11,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_12_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_12,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_13_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_13,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_14_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_14,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_15_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_15,
					 MAX34451_READ_IOUT, &value);
		break;
	case MAX34451_IIO_VIN_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_ALL,
					 MAX34451NA6_READ_VIN, &value);
		break;
	case MAX34451_IIO_IIN_CHAN:
		ret = max34451_read_word(max34451, MAX34451_CHAN_ALL,
					 MAX34451NA6_READ_IIN, &value);
		break;
	case MAX34451_IIO_TEMP_0_CHAN:
		if (max34451->id == ID_MAX34455)
			ret = max34451_read_word(max34451, MAX34455_CHAN_12,
						 MAX34451_READ_TEMPERATURE_1, &value);
		else
			ret = max34451_read_word(max34451, MAX34451_CHAN_16,
						 MAX34451_READ_TEMPERATURE_1, &value);
		break;
	case MAX34451_IIO_TEMP_1_CHAN:
		if (max34451->id == ID_MAX34455)
			ret = max34451_read_word(max34451, MAX34455_CHAN_13,
						 MAX34451_READ_TEMPERATURE_1, &value);
		else
			ret = max34451_read_word(max34451, MAX34451_CHAN_17,
						 MAX34451_READ_TEMPERATURE_1, &value);
		break;
	case MAX34451_IIO_TEMP_2_CHAN:
		if (max34451->id == ID_MAX34455)
			ret = max34451_read_word(max34451, MAX34455_CHAN_14,
						 MAX34451_READ_TEMPERATURE_1, &value);
		else
			ret = max34451_read_word(max34451, MAX34451_CHAN_18,
						 MAX34451_READ_TEMPERATURE_1, &value);
		break;
	case MAX34451_IIO_TEMP_3_CHAN:
		if (max34451->id == ID_MAX34455)
			ret = max34451_read_word(max34451, MAX34455_CHAN_15,
						 MAX34451_READ_TEMPERATURE_1, &value);
		else
			ret = max34451_read_word(max34451, MAX34451_CHAN_19,
						 MAX34451_READ_TEMPERATURE_1, &value);
		break;
	case MAX34451_IIO_TEMP_4_CHAN:
		if (max34451->id == ID_MAX34455)
			ret = max34451_read_word(max34451, MAX34455_CHAN_16,
						 MAX34451_READ_TEMPERATURE_1, &value);
		else
			ret = max34451_read_word(max34451, MAX34451_CHAN_20,
						 MAX34451_READ_TEMPERATURE_1, &value);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	raw = (int16_t)value;
	return iio_format_value(buf, len, IIO_VAL_INT, 1, &raw);
}

/**
 * @brief Handles the read request for converted values.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_conv(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int value;
	int ret;
	int32_t vals[2];
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;

	switch (channel->address) {
	case MAX34451_IIO_VOUT_0_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_0,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_1_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_1,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_2_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_2,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_3_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_3,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_4_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_4,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_5_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_5,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_6_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_6,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_7_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_7,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_8_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_8,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_9_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_9,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_10_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_10,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_11_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_11,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_12_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_12,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_13_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_13,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_14_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_14,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_VOUT_15_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_15,
					  MAX34451_VAL_VOUT, &value);
		break;
	case MAX34451_IIO_IOUT_0_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_0,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_1_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_1,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_2_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_2,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_3_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_3,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_4_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_4,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_5_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_5,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_6_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_6,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_7_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_7,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_8_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_8,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_9_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_9,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_10_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_10,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_11_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_11,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_12_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_12,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_13_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_13,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_14_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_14,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_IOUT_15_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_15,
					  MAX34451_VAL_IOUT, &value);
		break;
	case MAX34451_IIO_VIN_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_ALL,
					  MAX34451NA6_VAL_VIN, &value);
		break;
	case MAX34451_IIO_IIN_CHAN:
		ret = max34451_read_value(max34451, MAX34451_CHAN_ALL,
					  MAX34451NA6_VAL_IIN, &value);
		break;
	case MAX34451_IIO_TEMP_0_CHAN:
		if (max34451->id == ID_MAX34455)
			ret = max34451_read_value(max34451, MAX34455_CHAN_12,
						  MAX34451_VAL_TEMP, &value);
		else
			ret = max34451_read_value(max34451, MAX34451_CHAN_16,
						  MAX34451_VAL_TEMP, &value);
		break;
	case MAX34451_IIO_TEMP_1_CHAN:
		if (max34451->id == ID_MAX34455)
			ret = max34451_read_value(max34451, MAX34455_CHAN_13,
						  MAX34451_VAL_TEMP, &value);
		else
			ret = max34451_read_value(max34451, MAX34451_CHAN_17,
						  MAX34451_VAL_TEMP, &value);
		break;
	case MAX34451_IIO_TEMP_2_CHAN:
		if (max34451->id == ID_MAX34455)
			ret = max34451_read_value(max34451, MAX34455_CHAN_14,
						  MAX34451_VAL_TEMP, &value);
		else
			ret = max34451_read_value(max34451, MAX34451_CHAN_18,
						  MAX34451_VAL_TEMP, &value);
		break;
	case MAX34451_IIO_TEMP_3_CHAN:
		if (max34451->id == ID_MAX34455)
			ret = max34451_read_value(max34451, MAX34455_CHAN_15,
						  MAX34451_VAL_TEMP, &value);
		else
			ret = max34451_read_value(max34451, MAX34451_CHAN_19,
						  MAX34451_VAL_TEMP, &value);
		break;
	case MAX34451_IIO_TEMP_4_CHAN:
		if (max34451->id == ID_MAX34455)
			ret = max34451_read_value(max34451, MAX34455_CHAN_16,
						  MAX34451_VAL_TEMP, &value);
		else
			ret = max34451_read_value(max34451, MAX34451_CHAN_20,
						  MAX34451_VAL_TEMP, &value);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	vals[0] = value;
	vals[1] = (int32_t)MILLI;

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
 * @brief Handles the read request for vout attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_vout(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	int32_t vals[2];
	int ret, mv;

	ret = max34451_read_word_data(max34451, channel->ch_num,
				      (uint8_t)priv, &mv);
	if (ret)
		return ret;

	vals[0] = mv;
	vals[1] = (int32_t)MILLI;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

/**
 * @brief Handles the write request for vout attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - User buffer containing the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Returns 0 if successful.
*/
static int max34451_iio_write_vout(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	int ret, mv;
	int32_t val1 = 0, val2 = 0;

	ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val1, &val2);
	if (ret)
		return ret;

	mv = abs(val1) * (int)MILLI + abs(val2) / (int)MILLI;
	if (val1 < 0 || val2 < 0)
		mv = -mv;

	return max34451_write_word_data(max34451, channel->ch_num, (uint8_t)priv, mv);
}

/**
 * @brief Handles the read request for limits global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_limits(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	int32_t vals[2];
	int ret, chan, mv;

	chan = MAX34451_IIO_CHAN(priv);
	priv = MAX34451_IIO_REG(priv);

	ret = max34451_read_word_data(max34451, chan, (uint8_t)priv, &mv);
	if (ret)
		return ret;

	vals[0] = mv;
	vals[1] = (int32_t)MILLI;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

/**
 * @brief Handles the write request for limits global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - User buffer containing the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Returns 0 if successful.
*/
static int max34451_iio_write_limits(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	int32_t val1 = 0, val2 = 0;
	int ret, chan, data;

	chan = MAX34451_IIO_CHAN(priv);
	priv = MAX34451_IIO_REG(priv);

	ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val1, &val2);
	if (ret)
		return ret;

	data = abs(val1) * (int)MILLI + abs(val2) / (int)MILLI;
	if (val1 < 0 || val2 < 0)
		data = -data;

	return max34451_write_word_data(max34451, chan, (uint8_t)priv, data);
}

/**
 * @brief Handles the read request for status debug attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int max34451_iio_read_status(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct max34451_iio_desc *iio_max34451 = dev;
	struct max34451_dev *max34451 = iio_max34451->max34451_dev;
	int ret, chan;
	int32_t val;
	uint16_t status_word;
	uint8_t status_byte;

	chan = MAX34451_IIO_CHAN(priv);
	priv = MAX34451_IIO_REG(priv);

	if (priv == MAX34451_STATUS_WORD) {
		ret = max34451_read_word(max34451, chan, MAX34451_STATUS_WORD,
					 &status_word);
		if (ret)
			return ret;

		val = (int32_t)status_word;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	} else {
		ret = max34451_read_byte(max34451, chan, (uint8_t)priv,
					 &status_byte);
		if (ret)
			return ret;

		val = (int32_t)status_byte;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	}
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

	if (!init_param || !init_param->max34451_init_param)
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

	max34451_remove(iio_desc->max34451_dev);
	no_os_free(iio_desc);

	return 0;
}

static struct iio_attribute max34451_input_attrs[] = {
	{
		.name = "raw",
		.show = max34451_iio_read_raw
	},
	{
		.name = "converted",
		.show = max34451_iio_read_conv
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max34451_output_attrs[] = {
	{
		.name = "raw",
		.show = max34451_iio_read_raw
	},
	{
		.name = "converted",
		.show = max34451_iio_read_conv
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
	{
		.name = "vout_margin_high",
		.show = max34451_iio_read_vout,
		.store = max34451_iio_write_vout,
		.priv = MAX34451_VOUT_MARGIN_HIGH
	},
	{
		.name = "vout_margin_low",
		.show = max34451_iio_read_vout,
		.store = max34451_iio_write_vout,
		.priv = MAX34451_VOUT_MARGIN_LOW
	},
	END_ATTRIBUTES_ARRAY
};

#define	MAX34451_IIO_LIM(_name, _reg, _chan) { \
	.name = _name, \
	.show = max34451_iio_read_limits, \
	.store = max34451_iio_write_limits, \
	.priv = MAX34451_IIO_REG_CHAN(_reg, _chan) \
}

static struct iio_attribute max34451_global_attrs[] = {
	/* VOUT OV fault limits - channels 0-15 */
	MAX34451_IIO_LIM("vout_ov_fault_limit_0", MAX34451_VOUT_OV_FAULT_LIMIT, 0),
	MAX34451_IIO_LIM("vout_ov_fault_limit_1", MAX34451_VOUT_OV_FAULT_LIMIT, 1),
	MAX34451_IIO_LIM("vout_ov_fault_limit_2", MAX34451_VOUT_OV_FAULT_LIMIT, 2),
	MAX34451_IIO_LIM("vout_ov_fault_limit_3", MAX34451_VOUT_OV_FAULT_LIMIT, 3),
	MAX34451_IIO_LIM("vout_ov_fault_limit_4", MAX34451_VOUT_OV_FAULT_LIMIT, 4),
	MAX34451_IIO_LIM("vout_ov_fault_limit_5", MAX34451_VOUT_OV_FAULT_LIMIT, 5),
	MAX34451_IIO_LIM("vout_ov_fault_limit_6", MAX34451_VOUT_OV_FAULT_LIMIT, 6),
	MAX34451_IIO_LIM("vout_ov_fault_limit_7", MAX34451_VOUT_OV_FAULT_LIMIT, 7),
	MAX34451_IIO_LIM("vout_ov_fault_limit_8", MAX34451_VOUT_OV_FAULT_LIMIT, 8),
	MAX34451_IIO_LIM("vout_ov_fault_limit_9", MAX34451_VOUT_OV_FAULT_LIMIT, 9),
	MAX34451_IIO_LIM("vout_ov_fault_limit_10", MAX34451_VOUT_OV_FAULT_LIMIT, 10),
	MAX34451_IIO_LIM("vout_ov_fault_limit_11", MAX34451_VOUT_OV_FAULT_LIMIT, 11),
	MAX34451_IIO_LIM("vout_ov_fault_limit_12", MAX34451_VOUT_OV_FAULT_LIMIT, 12),
	MAX34451_IIO_LIM("vout_ov_fault_limit_13", MAX34451_VOUT_OV_FAULT_LIMIT, 13),
	MAX34451_IIO_LIM("vout_ov_fault_limit_14", MAX34451_VOUT_OV_FAULT_LIMIT, 14),
	MAX34451_IIO_LIM("vout_ov_fault_limit_15", MAX34451_VOUT_OV_FAULT_LIMIT, 15),
	/* VOUT UV fault limits - channels 0-15 */
	MAX34451_IIO_LIM("vout_uv_fault_limit_0", MAX34451_VOUT_UV_FAULT_LIMIT, 0),
	MAX34451_IIO_LIM("vout_uv_fault_limit_1", MAX34451_VOUT_UV_FAULT_LIMIT, 1),
	MAX34451_IIO_LIM("vout_uv_fault_limit_2", MAX34451_VOUT_UV_FAULT_LIMIT, 2),
	MAX34451_IIO_LIM("vout_uv_fault_limit_3", MAX34451_VOUT_UV_FAULT_LIMIT, 3),
	MAX34451_IIO_LIM("vout_uv_fault_limit_4", MAX34451_VOUT_UV_FAULT_LIMIT, 4),
	MAX34451_IIO_LIM("vout_uv_fault_limit_5", MAX34451_VOUT_UV_FAULT_LIMIT, 5),
	MAX34451_IIO_LIM("vout_uv_fault_limit_6", MAX34451_VOUT_UV_FAULT_LIMIT, 6),
	MAX34451_IIO_LIM("vout_uv_fault_limit_7", MAX34451_VOUT_UV_FAULT_LIMIT, 7),
	MAX34451_IIO_LIM("vout_uv_fault_limit_8", MAX34451_VOUT_UV_FAULT_LIMIT, 8),
	MAX34451_IIO_LIM("vout_uv_fault_limit_9", MAX34451_VOUT_UV_FAULT_LIMIT, 9),
	MAX34451_IIO_LIM("vout_uv_fault_limit_10", MAX34451_VOUT_UV_FAULT_LIMIT, 10),
	MAX34451_IIO_LIM("vout_uv_fault_limit_11", MAX34451_VOUT_UV_FAULT_LIMIT, 11),
	MAX34451_IIO_LIM("vout_uv_fault_limit_12", MAX34451_VOUT_UV_FAULT_LIMIT, 12),
	MAX34451_IIO_LIM("vout_uv_fault_limit_13", MAX34451_VOUT_UV_FAULT_LIMIT, 13),
	MAX34451_IIO_LIM("vout_uv_fault_limit_14", MAX34451_VOUT_UV_FAULT_LIMIT, 14),
	MAX34451_IIO_LIM("vout_uv_fault_limit_15", MAX34451_VOUT_UV_FAULT_LIMIT, 15),
	/* IOUT OC fault limits - channels 0-15 */
	MAX34451_IIO_LIM("iout_oc_fault_limit_0", MAX34451_IOUT_OC_FAULT_LIMIT, 0),
	MAX34451_IIO_LIM("iout_oc_fault_limit_1", MAX34451_IOUT_OC_FAULT_LIMIT, 1),
	MAX34451_IIO_LIM("iout_oc_fault_limit_2", MAX34451_IOUT_OC_FAULT_LIMIT, 2),
	MAX34451_IIO_LIM("iout_oc_fault_limit_3", MAX34451_IOUT_OC_FAULT_LIMIT, 3),
	MAX34451_IIO_LIM("iout_oc_fault_limit_4", MAX34451_IOUT_OC_FAULT_LIMIT, 4),
	MAX34451_IIO_LIM("iout_oc_fault_limit_5", MAX34451_IOUT_OC_FAULT_LIMIT, 5),
	MAX34451_IIO_LIM("iout_oc_fault_limit_6", MAX34451_IOUT_OC_FAULT_LIMIT, 6),
	MAX34451_IIO_LIM("iout_oc_fault_limit_7", MAX34451_IOUT_OC_FAULT_LIMIT, 7),
	MAX34451_IIO_LIM("iout_oc_fault_limit_8", MAX34451_IOUT_OC_FAULT_LIMIT, 8),
	MAX34451_IIO_LIM("iout_oc_fault_limit_9", MAX34451_IOUT_OC_FAULT_LIMIT, 9),
	MAX34451_IIO_LIM("iout_oc_fault_limit_10", MAX34451_IOUT_OC_FAULT_LIMIT, 10),
	MAX34451_IIO_LIM("iout_oc_fault_limit_11", MAX34451_IOUT_OC_FAULT_LIMIT, 11),
	MAX34451_IIO_LIM("iout_oc_fault_limit_12", MAX34451_IOUT_OC_FAULT_LIMIT, 12),
	MAX34451_IIO_LIM("iout_oc_fault_limit_13", MAX34451_IOUT_OC_FAULT_LIMIT, 13),
	MAX34451_IIO_LIM("iout_oc_fault_limit_14", MAX34451_IOUT_OC_FAULT_LIMIT, 14),
	MAX34451_IIO_LIM("iout_oc_fault_limit_15", MAX34451_IOUT_OC_FAULT_LIMIT, 15),
	/* OT fault limits - temperature sensors (pages 16-20) */
	MAX34451_IIO_LIM("ot_fault_limit_0", MAX34451_OT_FAULT_LIMIT, 16),
	MAX34451_IIO_LIM("ot_fault_limit_1", MAX34451_OT_FAULT_LIMIT, 17),
	MAX34451_IIO_LIM("ot_fault_limit_2", MAX34451_OT_FAULT_LIMIT, 18),
	MAX34451_IIO_LIM("ot_fault_limit_3", MAX34451_OT_FAULT_LIMIT, 19),
	MAX34451_IIO_LIM("ot_fault_limit_4", MAX34451_OT_FAULT_LIMIT, 20),
	/* OT warn limits - temperature sensors (pages 16-20) */
	MAX34451_IIO_LIM("ot_warn_limit_0", MAX34451_OT_WARN_LIMIT, 16),
	MAX34451_IIO_LIM("ot_warn_limit_1", MAX34451_OT_WARN_LIMIT, 17),
	MAX34451_IIO_LIM("ot_warn_limit_2", MAX34451_OT_WARN_LIMIT, 18),
	MAX34451_IIO_LIM("ot_warn_limit_3", MAX34451_OT_WARN_LIMIT, 19),
	MAX34451_IIO_LIM("ot_warn_limit_4", MAX34451_OT_WARN_LIMIT, 20),
	/* Power good thresholds - channels 0-15 */
	MAX34451_IIO_LIM("power_good_on_0", MAX34451_POWER_GOOD_ON, 0),
	MAX34451_IIO_LIM("power_good_on_1", MAX34451_POWER_GOOD_ON, 1),
	MAX34451_IIO_LIM("power_good_on_2", MAX34451_POWER_GOOD_ON, 2),
	MAX34451_IIO_LIM("power_good_on_3", MAX34451_POWER_GOOD_ON, 3),
	MAX34451_IIO_LIM("power_good_on_4", MAX34451_POWER_GOOD_ON, 4),
	MAX34451_IIO_LIM("power_good_on_5", MAX34451_POWER_GOOD_ON, 5),
	MAX34451_IIO_LIM("power_good_on_6", MAX34451_POWER_GOOD_ON, 6),
	MAX34451_IIO_LIM("power_good_on_7", MAX34451_POWER_GOOD_ON, 7),
	MAX34451_IIO_LIM("power_good_on_8", MAX34451_POWER_GOOD_ON, 8),
	MAX34451_IIO_LIM("power_good_on_9", MAX34451_POWER_GOOD_ON, 9),
	MAX34451_IIO_LIM("power_good_on_10", MAX34451_POWER_GOOD_ON, 10),
	MAX34451_IIO_LIM("power_good_on_11", MAX34451_POWER_GOOD_ON, 11),
	MAX34451_IIO_LIM("power_good_on_12", MAX34451_POWER_GOOD_ON, 12),
	MAX34451_IIO_LIM("power_good_on_13", MAX34451_POWER_GOOD_ON, 13),
	MAX34451_IIO_LIM("power_good_on_14", MAX34451_POWER_GOOD_ON, 14),
	MAX34451_IIO_LIM("power_good_on_15", MAX34451_POWER_GOOD_ON, 15),
	MAX34451_IIO_LIM("power_good_off_0", MAX34451_POWER_GOOD_OFF, 0),
	MAX34451_IIO_LIM("power_good_off_1", MAX34451_POWER_GOOD_OFF, 1),
	MAX34451_IIO_LIM("power_good_off_2", MAX34451_POWER_GOOD_OFF, 2),
	MAX34451_IIO_LIM("power_good_off_3", MAX34451_POWER_GOOD_OFF, 3),
	MAX34451_IIO_LIM("power_good_off_4", MAX34451_POWER_GOOD_OFF, 4),
	MAX34451_IIO_LIM("power_good_off_5", MAX34451_POWER_GOOD_OFF, 5),
	MAX34451_IIO_LIM("power_good_off_6", MAX34451_POWER_GOOD_OFF, 6),
	MAX34451_IIO_LIM("power_good_off_7", MAX34451_POWER_GOOD_OFF, 7),
	MAX34451_IIO_LIM("power_good_off_8", MAX34451_POWER_GOOD_OFF, 8),
	MAX34451_IIO_LIM("power_good_off_9", MAX34451_POWER_GOOD_OFF, 9),
	MAX34451_IIO_LIM("power_good_off_10", MAX34451_POWER_GOOD_OFF, 10),
	MAX34451_IIO_LIM("power_good_off_11", MAX34451_POWER_GOOD_OFF, 11),
	MAX34451_IIO_LIM("power_good_off_12", MAX34451_POWER_GOOD_OFF, 12),
	MAX34451_IIO_LIM("power_good_off_13", MAX34451_POWER_GOOD_OFF, 13),
	MAX34451_IIO_LIM("power_good_off_14", MAX34451_POWER_GOOD_OFF, 14),
	MAX34451_IIO_LIM("power_good_off_15", MAX34451_POWER_GOOD_OFF, 15),
	/* TON/TOFF delays - channels 0-11 (sequencing channels) */
	MAX34451_IIO_LIM("ton_delay_0", MAX34451_TON_DELAY, 0),
	MAX34451_IIO_LIM("ton_delay_1", MAX34451_TON_DELAY, 1),
	MAX34451_IIO_LIM("ton_delay_2", MAX34451_TON_DELAY, 2),
	MAX34451_IIO_LIM("ton_delay_3", MAX34451_TON_DELAY, 3),
	MAX34451_IIO_LIM("ton_delay_4", MAX34451_TON_DELAY, 4),
	MAX34451_IIO_LIM("ton_delay_5", MAX34451_TON_DELAY, 5),
	MAX34451_IIO_LIM("ton_delay_6", MAX34451_TON_DELAY, 6),
	MAX34451_IIO_LIM("ton_delay_7", MAX34451_TON_DELAY, 7),
	MAX34451_IIO_LIM("ton_delay_8", MAX34451_TON_DELAY, 8),
	MAX34451_IIO_LIM("ton_delay_9", MAX34451_TON_DELAY, 9),
	MAX34451_IIO_LIM("ton_delay_10", MAX34451_TON_DELAY, 10),
	MAX34451_IIO_LIM("ton_delay_11", MAX34451_TON_DELAY, 11),
	MAX34451_IIO_LIM("toff_delay_0", MAX34451_TOFF_DELAY, 0),
	MAX34451_IIO_LIM("toff_delay_1", MAX34451_TOFF_DELAY, 1),
	MAX34451_IIO_LIM("toff_delay_2", MAX34451_TOFF_DELAY, 2),
	MAX34451_IIO_LIM("toff_delay_3", MAX34451_TOFF_DELAY, 3),
	MAX34451_IIO_LIM("toff_delay_4", MAX34451_TOFF_DELAY, 4),
	MAX34451_IIO_LIM("toff_delay_5", MAX34451_TOFF_DELAY, 5),
	MAX34451_IIO_LIM("toff_delay_6", MAX34451_TOFF_DELAY, 6),
	MAX34451_IIO_LIM("toff_delay_7", MAX34451_TOFF_DELAY, 7),
	MAX34451_IIO_LIM("toff_delay_8", MAX34451_TOFF_DELAY, 8),
	MAX34451_IIO_LIM("toff_delay_9", MAX34451_TOFF_DELAY, 9),
	MAX34451_IIO_LIM("toff_delay_10", MAX34451_TOFF_DELAY, 10),
	MAX34451_IIO_LIM("toff_delay_11", MAX34451_TOFF_DELAY, 11),
	/* Global fault retry */
	MAX34451_IIO_LIM("fault_retry", MAX34451_MFR_FAULT_RETRY, MAX34451_CHAN_ALL),
	/* Manufacturer information */
	{
		.name = "mfr_location",
		.show = max34451_iio_read_mfr_str,
		.store = max34451_iio_write_mfr_str,
		.priv = MAX34451_MFR_LOCATION,
	},
	{
		.name = "mfr_date",
		.show = max34451_iio_read_mfr_str,
		.store = max34451_iio_write_mfr_str,
		.priv = MAX34451_MFR_DATE,
	},
	{
		.name = "mfr_serial",
		.show = max34451_iio_read_mfr_str,
		.store = max34451_iio_write_mfr_str,
		.priv = MAX34451_MFR_SERIAL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max34455_global_attrs[] = {
	/* VOUT OV fault limits - channels 0-11 */
	MAX34451_IIO_LIM("vout_ov_fault_limit_0", MAX34451_VOUT_OV_FAULT_LIMIT, 0),
	MAX34451_IIO_LIM("vout_ov_fault_limit_1", MAX34451_VOUT_OV_FAULT_LIMIT, 1),
	MAX34451_IIO_LIM("vout_ov_fault_limit_2", MAX34451_VOUT_OV_FAULT_LIMIT, 2),
	MAX34451_IIO_LIM("vout_ov_fault_limit_3", MAX34451_VOUT_OV_FAULT_LIMIT, 3),
	MAX34451_IIO_LIM("vout_ov_fault_limit_4", MAX34451_VOUT_OV_FAULT_LIMIT, 4),
	MAX34451_IIO_LIM("vout_ov_fault_limit_5", MAX34451_VOUT_OV_FAULT_LIMIT, 5),
	MAX34451_IIO_LIM("vout_ov_fault_limit_6", MAX34451_VOUT_OV_FAULT_LIMIT, 6),
	MAX34451_IIO_LIM("vout_ov_fault_limit_7", MAX34451_VOUT_OV_FAULT_LIMIT, 7),
	MAX34451_IIO_LIM("vout_ov_fault_limit_8", MAX34451_VOUT_OV_FAULT_LIMIT, 8),
	MAX34451_IIO_LIM("vout_ov_fault_limit_9", MAX34451_VOUT_OV_FAULT_LIMIT, 9),
	MAX34451_IIO_LIM("vout_ov_fault_limit_10", MAX34451_VOUT_OV_FAULT_LIMIT, 10),
	MAX34451_IIO_LIM("vout_ov_fault_limit_11", MAX34451_VOUT_OV_FAULT_LIMIT, 11),
	/* VOUT UV fault limits - channels 0-11 */
	MAX34451_IIO_LIM("vout_uv_fault_limit_0", MAX34451_VOUT_UV_FAULT_LIMIT, 0),
	MAX34451_IIO_LIM("vout_uv_fault_limit_1", MAX34451_VOUT_UV_FAULT_LIMIT, 1),
	MAX34451_IIO_LIM("vout_uv_fault_limit_2", MAX34451_VOUT_UV_FAULT_LIMIT, 2),
	MAX34451_IIO_LIM("vout_uv_fault_limit_3", MAX34451_VOUT_UV_FAULT_LIMIT, 3),
	MAX34451_IIO_LIM("vout_uv_fault_limit_4", MAX34451_VOUT_UV_FAULT_LIMIT, 4),
	MAX34451_IIO_LIM("vout_uv_fault_limit_5", MAX34451_VOUT_UV_FAULT_LIMIT, 5),
	MAX34451_IIO_LIM("vout_uv_fault_limit_6", MAX34451_VOUT_UV_FAULT_LIMIT, 6),
	MAX34451_IIO_LIM("vout_uv_fault_limit_7", MAX34451_VOUT_UV_FAULT_LIMIT, 7),
	MAX34451_IIO_LIM("vout_uv_fault_limit_8", MAX34451_VOUT_UV_FAULT_LIMIT, 8),
	MAX34451_IIO_LIM("vout_uv_fault_limit_9", MAX34451_VOUT_UV_FAULT_LIMIT, 9),
	MAX34451_IIO_LIM("vout_uv_fault_limit_10", MAX34451_VOUT_UV_FAULT_LIMIT, 10),
	MAX34451_IIO_LIM("vout_uv_fault_limit_11", MAX34451_VOUT_UV_FAULT_LIMIT, 11),
	/* IOUT OC fault limits - channels 0-11 (sequencing channels) */
	MAX34451_IIO_LIM("iout_oc_fault_limit_0", MAX34451_IOUT_OC_FAULT_LIMIT, 0),
	MAX34451_IIO_LIM("iout_oc_fault_limit_1", MAX34451_IOUT_OC_FAULT_LIMIT, 1),
	MAX34451_IIO_LIM("iout_oc_fault_limit_2", MAX34451_IOUT_OC_FAULT_LIMIT, 2),
	MAX34451_IIO_LIM("iout_oc_fault_limit_3", MAX34451_IOUT_OC_FAULT_LIMIT, 3),
	MAX34451_IIO_LIM("iout_oc_fault_limit_4", MAX34451_IOUT_OC_FAULT_LIMIT, 4),
	MAX34451_IIO_LIM("iout_oc_fault_limit_5", MAX34451_IOUT_OC_FAULT_LIMIT, 5),
	MAX34451_IIO_LIM("iout_oc_fault_limit_6", MAX34451_IOUT_OC_FAULT_LIMIT, 6),
	MAX34451_IIO_LIM("iout_oc_fault_limit_7", MAX34451_IOUT_OC_FAULT_LIMIT, 7),
	MAX34451_IIO_LIM("iout_oc_fault_limit_8", MAX34451_IOUT_OC_FAULT_LIMIT, 8),
	MAX34451_IIO_LIM("iout_oc_fault_limit_9", MAX34451_IOUT_OC_FAULT_LIMIT, 9),
	MAX34451_IIO_LIM("iout_oc_fault_limit_10", MAX34451_IOUT_OC_FAULT_LIMIT, 10),
	MAX34451_IIO_LIM("iout_oc_fault_limit_11", MAX34451_IOUT_OC_FAULT_LIMIT, 11),
	/* OT fault limits - temperature sensors (pages 12-16) */
	MAX34451_IIO_LIM("ot_fault_limit_0", MAX34451_OT_FAULT_LIMIT, 12),
	MAX34451_IIO_LIM("ot_fault_limit_1", MAX34451_OT_FAULT_LIMIT, 13),
	MAX34451_IIO_LIM("ot_fault_limit_2", MAX34451_OT_FAULT_LIMIT, 14),
	MAX34451_IIO_LIM("ot_fault_limit_3", MAX34451_OT_FAULT_LIMIT, 15),
	MAX34451_IIO_LIM("ot_fault_limit_4", MAX34451_OT_FAULT_LIMIT, 16),
	/* OT warn limits - temperature sensors (pages 12-16) */
	MAX34451_IIO_LIM("ot_warn_limit_0", MAX34451_OT_WARN_LIMIT, 12),
	MAX34451_IIO_LIM("ot_warn_limit_1", MAX34451_OT_WARN_LIMIT, 13),
	MAX34451_IIO_LIM("ot_warn_limit_2", MAX34451_OT_WARN_LIMIT, 14),
	MAX34451_IIO_LIM("ot_warn_limit_3", MAX34451_OT_WARN_LIMIT, 15),
	MAX34451_IIO_LIM("ot_warn_limit_4", MAX34451_OT_WARN_LIMIT, 16),
	/* Power good thresholds - channels 0-11 */
	MAX34451_IIO_LIM("power_good_on_0", MAX34451_POWER_GOOD_ON, 0),
	MAX34451_IIO_LIM("power_good_on_1", MAX34451_POWER_GOOD_ON, 1),
	MAX34451_IIO_LIM("power_good_on_2", MAX34451_POWER_GOOD_ON, 2),
	MAX34451_IIO_LIM("power_good_on_3", MAX34451_POWER_GOOD_ON, 3),
	MAX34451_IIO_LIM("power_good_on_4", MAX34451_POWER_GOOD_ON, 4),
	MAX34451_IIO_LIM("power_good_on_5", MAX34451_POWER_GOOD_ON, 5),
	MAX34451_IIO_LIM("power_good_on_6", MAX34451_POWER_GOOD_ON, 6),
	MAX34451_IIO_LIM("power_good_on_7", MAX34451_POWER_GOOD_ON, 7),
	MAX34451_IIO_LIM("power_good_on_8", MAX34451_POWER_GOOD_ON, 8),
	MAX34451_IIO_LIM("power_good_on_9", MAX34451_POWER_GOOD_ON, 9),
	MAX34451_IIO_LIM("power_good_on_10", MAX34451_POWER_GOOD_ON, 10),
	MAX34451_IIO_LIM("power_good_on_11", MAX34451_POWER_GOOD_ON, 11),
	MAX34451_IIO_LIM("power_good_off_0", MAX34451_POWER_GOOD_OFF, 0),
	MAX34451_IIO_LIM("power_good_off_1", MAX34451_POWER_GOOD_OFF, 1),
	MAX34451_IIO_LIM("power_good_off_2", MAX34451_POWER_GOOD_OFF, 2),
	MAX34451_IIO_LIM("power_good_off_3", MAX34451_POWER_GOOD_OFF, 3),
	MAX34451_IIO_LIM("power_good_off_4", MAX34451_POWER_GOOD_OFF, 4),
	MAX34451_IIO_LIM("power_good_off_5", MAX34451_POWER_GOOD_OFF, 5),
	MAX34451_IIO_LIM("power_good_off_6", MAX34451_POWER_GOOD_OFF, 6),
	MAX34451_IIO_LIM("power_good_off_7", MAX34451_POWER_GOOD_OFF, 7),
	MAX34451_IIO_LIM("power_good_off_8", MAX34451_POWER_GOOD_OFF, 8),
	MAX34451_IIO_LIM("power_good_off_9", MAX34451_POWER_GOOD_OFF, 9),
	MAX34451_IIO_LIM("power_good_off_10", MAX34451_POWER_GOOD_OFF, 10),
	MAX34451_IIO_LIM("power_good_off_11", MAX34451_POWER_GOOD_OFF, 11),
	/* TON/TOFF delays - channels 0-7 (sequencing channels) */
	MAX34451_IIO_LIM("ton_delay_0", MAX34451_TON_DELAY, 0),
	MAX34451_IIO_LIM("ton_delay_1", MAX34451_TON_DELAY, 1),
	MAX34451_IIO_LIM("ton_delay_2", MAX34451_TON_DELAY, 2),
	MAX34451_IIO_LIM("ton_delay_3", MAX34451_TON_DELAY, 3),
	MAX34451_IIO_LIM("ton_delay_4", MAX34451_TON_DELAY, 4),
	MAX34451_IIO_LIM("ton_delay_5", MAX34451_TON_DELAY, 5),
	MAX34451_IIO_LIM("ton_delay_6", MAX34451_TON_DELAY, 6),
	MAX34451_IIO_LIM("ton_delay_7", MAX34451_TON_DELAY, 7),
	MAX34451_IIO_LIM("toff_delay_0", MAX34451_TOFF_DELAY, 0),
	MAX34451_IIO_LIM("toff_delay_1", MAX34451_TOFF_DELAY, 1),
	MAX34451_IIO_LIM("toff_delay_2", MAX34451_TOFF_DELAY, 2),
	MAX34451_IIO_LIM("toff_delay_3", MAX34451_TOFF_DELAY, 3),
	MAX34451_IIO_LIM("toff_delay_4", MAX34451_TOFF_DELAY, 4),
	MAX34451_IIO_LIM("toff_delay_5", MAX34451_TOFF_DELAY, 5),
	MAX34451_IIO_LIM("toff_delay_6", MAX34451_TOFF_DELAY, 6),
	MAX34451_IIO_LIM("toff_delay_7", MAX34451_TOFF_DELAY, 7),
	/* Global fault retry */
	MAX34451_IIO_LIM("fault_retry", MAX34451_MFR_FAULT_RETRY, MAX34451_CHAN_ALL),
	/* Manufacturer information */
	{
		.name = "mfr_location",
		.show = max34451_iio_read_mfr_str,
		.store = max34451_iio_write_mfr_str,
		.priv = MAX34451_MFR_LOCATION,
	},
	{
		.name = "mfr_date",
		.show = max34451_iio_read_mfr_str,
		.store = max34451_iio_write_mfr_str,
		.priv = MAX34451_MFR_DATE,
	},
	{
		.name = "mfr_serial",
		.show = max34451_iio_read_mfr_str,
		.store = max34451_iio_write_mfr_str,
		.priv = MAX34451_MFR_SERIAL,
	},
	END_ATTRIBUTES_ARRAY
};

#define MAX34451_IIO_STATUS(_name, _reg, _chan) { \
	.name = _name, \
	.show = max34451_iio_read_status, \
	.priv = MAX34451_IIO_REG_CHAN(_reg, _chan) \
}

static struct iio_attribute max34451_debug_attrs[] = {
	/* STATUS_VOUT - channels 0-15 */
	MAX34451_IIO_STATUS("status_vout_0", MAX34451_STATUS_VOUT, 0),
	MAX34451_IIO_STATUS("status_vout_1", MAX34451_STATUS_VOUT, 1),
	MAX34451_IIO_STATUS("status_vout_2", MAX34451_STATUS_VOUT, 2),
	MAX34451_IIO_STATUS("status_vout_3", MAX34451_STATUS_VOUT, 3),
	MAX34451_IIO_STATUS("status_vout_4", MAX34451_STATUS_VOUT, 4),
	MAX34451_IIO_STATUS("status_vout_5", MAX34451_STATUS_VOUT, 5),
	MAX34451_IIO_STATUS("status_vout_6", MAX34451_STATUS_VOUT, 6),
	MAX34451_IIO_STATUS("status_vout_7", MAX34451_STATUS_VOUT, 7),
	MAX34451_IIO_STATUS("status_vout_8", MAX34451_STATUS_VOUT, 8),
	MAX34451_IIO_STATUS("status_vout_9", MAX34451_STATUS_VOUT, 9),
	MAX34451_IIO_STATUS("status_vout_10", MAX34451_STATUS_VOUT, 10),
	MAX34451_IIO_STATUS("status_vout_11", MAX34451_STATUS_VOUT, 11),
	MAX34451_IIO_STATUS("status_vout_12", MAX34451_STATUS_VOUT, 12),
	MAX34451_IIO_STATUS("status_vout_13", MAX34451_STATUS_VOUT, 13),
	MAX34451_IIO_STATUS("status_vout_14", MAX34451_STATUS_VOUT, 14),
	MAX34451_IIO_STATUS("status_vout_15", MAX34451_STATUS_VOUT, 15),
	/* STATUS_IOUT - channels 0-15 */
	MAX34451_IIO_STATUS("status_iout_0", MAX34451_STATUS_IOUT, 0),
	MAX34451_IIO_STATUS("status_iout_1", MAX34451_STATUS_IOUT, 1),
	MAX34451_IIO_STATUS("status_iout_2", MAX34451_STATUS_IOUT, 2),
	MAX34451_IIO_STATUS("status_iout_3", MAX34451_STATUS_IOUT, 3),
	MAX34451_IIO_STATUS("status_iout_4", MAX34451_STATUS_IOUT, 4),
	MAX34451_IIO_STATUS("status_iout_5", MAX34451_STATUS_IOUT, 5),
	MAX34451_IIO_STATUS("status_iout_6", MAX34451_STATUS_IOUT, 6),
	MAX34451_IIO_STATUS("status_iout_7", MAX34451_STATUS_IOUT, 7),
	MAX34451_IIO_STATUS("status_iout_8", MAX34451_STATUS_IOUT, 8),
	MAX34451_IIO_STATUS("status_iout_9", MAX34451_STATUS_IOUT, 9),
	MAX34451_IIO_STATUS("status_iout_10", MAX34451_STATUS_IOUT, 10),
	MAX34451_IIO_STATUS("status_iout_11", MAX34451_STATUS_IOUT, 11),
	MAX34451_IIO_STATUS("status_iout_12", MAX34451_STATUS_IOUT, 12),
	MAX34451_IIO_STATUS("status_iout_13", MAX34451_STATUS_IOUT, 13),
	MAX34451_IIO_STATUS("status_iout_14", MAX34451_STATUS_IOUT, 14),
	MAX34451_IIO_STATUS("status_iout_15", MAX34451_STATUS_IOUT, 15),
	/* STATUS_TEMPERATURE - channels 16-20 */
	MAX34451_IIO_STATUS("status_temperature_0", MAX34451_STATUS_TEMPERATURE, 16),
	MAX34451_IIO_STATUS("status_temperature_1", MAX34451_STATUS_TEMPERATURE, 17),
	MAX34451_IIO_STATUS("status_temperature_2", MAX34451_STATUS_TEMPERATURE, 18),
	MAX34451_IIO_STATUS("status_temperature_3", MAX34451_STATUS_TEMPERATURE, 19),
	MAX34451_IIO_STATUS("status_temperature_4", MAX34451_STATUS_TEMPERATURE, 20),
	/* STATUS_MFR_SPECIFIC - channels 0-15 */
	MAX34451_IIO_STATUS("status_mfr_specific_0", MAX34451_STATUS_MFR_SPECIFIC, 0),
	MAX34451_IIO_STATUS("status_mfr_specific_1", MAX34451_STATUS_MFR_SPECIFIC, 1),
	MAX34451_IIO_STATUS("status_mfr_specific_2", MAX34451_STATUS_MFR_SPECIFIC, 2),
	MAX34451_IIO_STATUS("status_mfr_specific_3", MAX34451_STATUS_MFR_SPECIFIC, 3),
	MAX34451_IIO_STATUS("status_mfr_specific_4", MAX34451_STATUS_MFR_SPECIFIC, 4),
	MAX34451_IIO_STATUS("status_mfr_specific_5", MAX34451_STATUS_MFR_SPECIFIC, 5),
	MAX34451_IIO_STATUS("status_mfr_specific_6", MAX34451_STATUS_MFR_SPECIFIC, 6),
	MAX34451_IIO_STATUS("status_mfr_specific_7", MAX34451_STATUS_MFR_SPECIFIC, 7),
	MAX34451_IIO_STATUS("status_mfr_specific_8", MAX34451_STATUS_MFR_SPECIFIC, 8),
	MAX34451_IIO_STATUS("status_mfr_specific_9", MAX34451_STATUS_MFR_SPECIFIC, 9),
	MAX34451_IIO_STATUS("status_mfr_specific_10", MAX34451_STATUS_MFR_SPECIFIC, 10),
	MAX34451_IIO_STATUS("status_mfr_specific_11", MAX34451_STATUS_MFR_SPECIFIC, 11),
	MAX34451_IIO_STATUS("status_mfr_specific_12", MAX34451_STATUS_MFR_SPECIFIC, 12),
	MAX34451_IIO_STATUS("status_mfr_specific_13", MAX34451_STATUS_MFR_SPECIFIC, 13),
	MAX34451_IIO_STATUS("status_mfr_specific_14", MAX34451_STATUS_MFR_SPECIFIC, 14),
	MAX34451_IIO_STATUS("status_mfr_specific_15", MAX34451_STATUS_MFR_SPECIFIC, 15),
	/* STATUS_WORD - channels 0-15 */
	MAX34451_IIO_STATUS("status_word_0", MAX34451_STATUS_WORD, 0),
	MAX34451_IIO_STATUS("status_word_1", MAX34451_STATUS_WORD, 1),
	MAX34451_IIO_STATUS("status_word_2", MAX34451_STATUS_WORD, 2),
	MAX34451_IIO_STATUS("status_word_3", MAX34451_STATUS_WORD, 3),
	MAX34451_IIO_STATUS("status_word_4", MAX34451_STATUS_WORD, 4),
	MAX34451_IIO_STATUS("status_word_5", MAX34451_STATUS_WORD, 5),
	MAX34451_IIO_STATUS("status_word_6", MAX34451_STATUS_WORD, 6),
	MAX34451_IIO_STATUS("status_word_7", MAX34451_STATUS_WORD, 7),
	MAX34451_IIO_STATUS("status_word_8", MAX34451_STATUS_WORD, 8),
	MAX34451_IIO_STATUS("status_word_9", MAX34451_STATUS_WORD, 9),
	MAX34451_IIO_STATUS("status_word_10", MAX34451_STATUS_WORD, 10),
	MAX34451_IIO_STATUS("status_word_11", MAX34451_STATUS_WORD, 11),
	MAX34451_IIO_STATUS("status_word_12", MAX34451_STATUS_WORD, 12),
	MAX34451_IIO_STATUS("status_word_13", MAX34451_STATUS_WORD, 13),
	MAX34451_IIO_STATUS("status_word_14", MAX34451_STATUS_WORD, 14),
	MAX34451_IIO_STATUS("status_word_15", MAX34451_STATUS_WORD, 15),
	/* STATUS_CML - global */
	MAX34451_IIO_STATUS("status_cml", MAX34451_STATUS_CML, MAX34451_CHAN_ALL),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute max34455_debug_attrs[] = {
	/* STATUS_VOUT - channels 0-11 */
	MAX34451_IIO_STATUS("status_vout_0", MAX34451_STATUS_VOUT, 0),
	MAX34451_IIO_STATUS("status_vout_1", MAX34451_STATUS_VOUT, 1),
	MAX34451_IIO_STATUS("status_vout_2", MAX34451_STATUS_VOUT, 2),
	MAX34451_IIO_STATUS("status_vout_3", MAX34451_STATUS_VOUT, 3),
	MAX34451_IIO_STATUS("status_vout_4", MAX34451_STATUS_VOUT, 4),
	MAX34451_IIO_STATUS("status_vout_5", MAX34451_STATUS_VOUT, 5),
	MAX34451_IIO_STATUS("status_vout_6", MAX34451_STATUS_VOUT, 6),
	MAX34451_IIO_STATUS("status_vout_7", MAX34451_STATUS_VOUT, 7),
	MAX34451_IIO_STATUS("status_vout_8", MAX34451_STATUS_VOUT, 8),
	MAX34451_IIO_STATUS("status_vout_9", MAX34451_STATUS_VOUT, 9),
	MAX34451_IIO_STATUS("status_vout_10", MAX34451_STATUS_VOUT, 10),
	MAX34451_IIO_STATUS("status_vout_11", MAX34451_STATUS_VOUT, 11),
	/* STATUS_IOUT - channels 0-11 */
	MAX34451_IIO_STATUS("status_iout_0", MAX34451_STATUS_IOUT, 0),
	MAX34451_IIO_STATUS("status_iout_1", MAX34451_STATUS_IOUT, 1),
	MAX34451_IIO_STATUS("status_iout_2", MAX34451_STATUS_IOUT, 2),
	MAX34451_IIO_STATUS("status_iout_3", MAX34451_STATUS_IOUT, 3),
	MAX34451_IIO_STATUS("status_iout_4", MAX34451_STATUS_IOUT, 4),
	MAX34451_IIO_STATUS("status_iout_5", MAX34451_STATUS_IOUT, 5),
	MAX34451_IIO_STATUS("status_iout_6", MAX34451_STATUS_IOUT, 6),
	MAX34451_IIO_STATUS("status_iout_7", MAX34451_STATUS_IOUT, 7),
	MAX34451_IIO_STATUS("status_iout_8", MAX34451_STATUS_IOUT, 8),
	MAX34451_IIO_STATUS("status_iout_9", MAX34451_STATUS_IOUT, 9),
	MAX34451_IIO_STATUS("status_iout_10", MAX34451_STATUS_IOUT, 10),
	MAX34451_IIO_STATUS("status_iout_11", MAX34451_STATUS_IOUT, 11),
	/* STATUS_TEMPERATURE - channels 12-16 */
	MAX34451_IIO_STATUS("status_temperature_0", MAX34451_STATUS_TEMPERATURE, 12),
	MAX34451_IIO_STATUS("status_temperature_1", MAX34451_STATUS_TEMPERATURE, 13),
	MAX34451_IIO_STATUS("status_temperature_2", MAX34451_STATUS_TEMPERATURE, 14),
	MAX34451_IIO_STATUS("status_temperature_3", MAX34451_STATUS_TEMPERATURE, 15),
	MAX34451_IIO_STATUS("status_temperature_4", MAX34451_STATUS_TEMPERATURE, 16),
	/* STATUS_MFR_SPECIFIC - channels 0-11 */
	MAX34451_IIO_STATUS("status_mfr_specific_0", MAX34451_STATUS_MFR_SPECIFIC, 0),
	MAX34451_IIO_STATUS("status_mfr_specific_1", MAX34451_STATUS_MFR_SPECIFIC, 1),
	MAX34451_IIO_STATUS("status_mfr_specific_2", MAX34451_STATUS_MFR_SPECIFIC, 2),
	MAX34451_IIO_STATUS("status_mfr_specific_3", MAX34451_STATUS_MFR_SPECIFIC, 3),
	MAX34451_IIO_STATUS("status_mfr_specific_4", MAX34451_STATUS_MFR_SPECIFIC, 4),
	MAX34451_IIO_STATUS("status_mfr_specific_5", MAX34451_STATUS_MFR_SPECIFIC, 5),
	MAX34451_IIO_STATUS("status_mfr_specific_6", MAX34451_STATUS_MFR_SPECIFIC, 6),
	MAX34451_IIO_STATUS("status_mfr_specific_7", MAX34451_STATUS_MFR_SPECIFIC, 7),
	MAX34451_IIO_STATUS("status_mfr_specific_8", MAX34451_STATUS_MFR_SPECIFIC, 8),
	MAX34451_IIO_STATUS("status_mfr_specific_9", MAX34451_STATUS_MFR_SPECIFIC, 9),
	MAX34451_IIO_STATUS("status_mfr_specific_10", MAX34451_STATUS_MFR_SPECIFIC, 10),
	MAX34451_IIO_STATUS("status_mfr_specific_11", MAX34451_STATUS_MFR_SPECIFIC, 11),
	/* STATUS_WORD - channels 0-11 */
	MAX34451_IIO_STATUS("status_word_0", MAX34451_STATUS_WORD, 0),
	MAX34451_IIO_STATUS("status_word_1", MAX34451_STATUS_WORD, 1),
	MAX34451_IIO_STATUS("status_word_2", MAX34451_STATUS_WORD, 2),
	MAX34451_IIO_STATUS("status_word_3", MAX34451_STATUS_WORD, 3),
	MAX34451_IIO_STATUS("status_word_4", MAX34451_STATUS_WORD, 4),
	MAX34451_IIO_STATUS("status_word_5", MAX34451_STATUS_WORD, 5),
	MAX34451_IIO_STATUS("status_word_6", MAX34451_STATUS_WORD, 6),
	MAX34451_IIO_STATUS("status_word_7", MAX34451_STATUS_WORD, 7),
	MAX34451_IIO_STATUS("status_word_8", MAX34451_STATUS_WORD, 8),
	MAX34451_IIO_STATUS("status_word_9", MAX34451_STATUS_WORD, 9),
	MAX34451_IIO_STATUS("status_word_10", MAX34451_STATUS_WORD, 10),
	MAX34451_IIO_STATUS("status_word_11", MAX34451_STATUS_WORD, 11),
	/* STATUS_CML - global */
	MAX34451_IIO_STATUS("status_cml", MAX34451_STATUS_CML, MAX34451_CHAN_ALL),
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel max34451_channels[] = {
	/* Sequencing voltage outputs (0-11) */
	{
		.name = "vout0",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_0,
		.address = MAX34451_IIO_VOUT_0_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_1,
		.address = MAX34451_IIO_VOUT_1_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout2",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_2,
		.address = MAX34451_IIO_VOUT_2_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout3",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_3,
		.address = MAX34451_IIO_VOUT_3_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout4",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_4,
		.address = MAX34451_IIO_VOUT_4_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout5",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_5,
		.address = MAX34451_IIO_VOUT_5_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout6",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_6,
		.address = MAX34451_IIO_VOUT_6_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout7",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_7,
		.address = MAX34451_IIO_VOUT_7_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout8",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_8,
		.address = MAX34451_IIO_VOUT_8_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout9",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_9,
		.address = MAX34451_IIO_VOUT_9_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout10",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_10,
		.address = MAX34451_IIO_VOUT_10_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout11",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_11,
		.address = MAX34451_IIO_VOUT_11_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	/* Monitor-only voltage inputs (12-15) */
	{
		.name = "vout12",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_12,
		.address = MAX34451_IIO_VOUT_12_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "vout13",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_13,
		.address = MAX34451_IIO_VOUT_13_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "vout14",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_14,
		.address = MAX34451_IIO_VOUT_14_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "vout15",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34451_CHAN_15,
		.address = MAX34451_IIO_VOUT_15_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	/* Current outputs (0-15) */
	{
		.name = "iout0",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_0,
		.address = MAX34451_IIO_IOUT_0_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout1",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_1,
		.address = MAX34451_IIO_IOUT_1_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout2",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_2,
		.address = MAX34451_IIO_IOUT_2_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout3",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_3,
		.address = MAX34451_IIO_IOUT_3_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout4",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_4,
		.address = MAX34451_IIO_IOUT_4_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout5",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_5,
		.address = MAX34451_IIO_IOUT_5_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout6",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_6,
		.address = MAX34451_IIO_IOUT_6_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout7",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_7,
		.address = MAX34451_IIO_IOUT_7_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout8",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_8,
		.address = MAX34451_IIO_IOUT_8_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout9",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_9,
		.address = MAX34451_IIO_IOUT_9_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout10",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_10,
		.address = MAX34451_IIO_IOUT_10_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout11",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_11,
		.address = MAX34451_IIO_IOUT_11_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout12",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_12,
		.address = MAX34451_IIO_IOUT_12_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout13",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_13,
		.address = MAX34451_IIO_IOUT_13_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout14",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_14,
		.address = MAX34451_IIO_IOUT_14_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout15",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_15,
		.address = MAX34451_IIO_IOUT_15_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	/* Temperature sensors (pages 16-20) */
	{
		.name = "temp0",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34451_CHAN_16,
		.address = MAX34451_IIO_TEMP_0_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "temp1",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34451_CHAN_17,
		.address = MAX34451_IIO_TEMP_1_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "temp2",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34451_CHAN_18,
		.address = MAX34451_IIO_TEMP_2_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "temp3",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34451_CHAN_19,
		.address = MAX34451_IIO_TEMP_3_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "temp4",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34451_CHAN_20,
		.address = MAX34451_IIO_TEMP_4_CHAN,
		.attributes = max34451_input_attrs,
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
		.address = MAX34451_IIO_VIN_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iin",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34451_CHAN_ALL,
		.address = MAX34451_IIO_IIN_CHAN,
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
		.address = MAX34451_IIO_VOUT_0_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_1,
		.address = MAX34451_IIO_VOUT_1_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout2",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_2,
		.address = MAX34451_IIO_VOUT_2_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout3",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_3,
		.address = MAX34451_IIO_VOUT_3_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout4",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_4,
		.address = MAX34451_IIO_VOUT_4_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout5",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_5,
		.address = MAX34451_IIO_VOUT_5_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout6",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_6,
		.address = MAX34451_IIO_VOUT_6_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout7",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_7,
		.address = MAX34451_IIO_VOUT_7_CHAN,
		.attributes = max34451_output_attrs,
		.ch_out = true
	},
	/* Monitor-only voltage inputs (8-11) */
	{
		.name = "vout8",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_8,
		.address = MAX34451_IIO_VOUT_8_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "vout9",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_9,
		.address = MAX34451_IIO_VOUT_9_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "vout10",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_10,
		.address = MAX34451_IIO_VOUT_10_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "vout11",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = MAX34455_CHAN_11,
		.address = MAX34451_IIO_VOUT_11_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	/* Current outputs (0-11) */
	{
		.name = "iout0",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_0,
		.address = MAX34451_IIO_IOUT_0_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout1",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_1,
		.address = MAX34451_IIO_IOUT_1_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout2",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_2,
		.address = MAX34451_IIO_IOUT_2_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout3",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_3,
		.address = MAX34451_IIO_IOUT_3_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout4",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_4,
		.address = MAX34451_IIO_IOUT_4_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout5",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_5,
		.address = MAX34451_IIO_IOUT_5_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout6",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_6,
		.address = MAX34451_IIO_IOUT_6_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout7",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_7,
		.address = MAX34451_IIO_IOUT_7_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout8",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_8,
		.address = MAX34451_IIO_IOUT_8_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout9",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_9,
		.address = MAX34451_IIO_IOUT_9_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout10",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_10,
		.address = MAX34451_IIO_IOUT_10_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout11",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = MAX34455_CHAN_11,
		.address = MAX34451_IIO_IOUT_11_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	/* Temperature sensors (pages 12-16) */
	{
		.name = "temp0",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34455_CHAN_12,
		.address = MAX34451_IIO_TEMP_0_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "temp1",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34455_CHAN_13,
		.address = MAX34451_IIO_TEMP_1_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "temp2",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34455_CHAN_14,
		.address = MAX34451_IIO_TEMP_2_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "temp3",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34455_CHAN_15,
		.address = MAX34451_IIO_TEMP_3_CHAN,
		.attributes = max34451_input_attrs,
		.ch_out = false
	},
	{
		.name = "temp4",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = MAX34455_CHAN_16,
		.address = MAX34451_IIO_TEMP_4_CHAN,
		.attributes = max34451_input_attrs,
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
	.attributes = max34455_global_attrs,
	.debug_attributes = max34455_debug_attrs,
	.debug_reg_read = max34451_iio_reg_read,
	.debug_reg_write = max34451_iio_reg_write
};
