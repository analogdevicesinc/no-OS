/***************************************************************************//**
 *   @file   iio_ltp8800.c
 *   @brief  Source file for the LTP8800 IIO Driver
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include "ltp8800.h"
#include "iio_ltp8800.h"

static const char *const ltp8800_enable_avail[2] = {
	"disable", "enable"
};

static const char *const ltp8800_store_avail[2] = {
	"store", "restore"
};

enum ltp8800_iio_chan_type {
	LTP8800_IIO_VOUT_CHAN,
	LTP8800_IIO_VIN_CHAN,
	LTP8800_IIO_IOUT_CHAN,
	LTP8800_IIO_IIN_CHAN,
	LTP8800_IIO_TEMP_CHAN,
};

enum ltp8800_iio_user_settings {
	LTP8800_IIO_STORE_USER_ALL,
	LTP8800_IIO_RESTORE_USER_ALL,
};

static struct iio_device ltp8800_iio_dev;

/**
 * @brief Read register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to read.
 * @param readval - Read value.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int32_t ltp8800_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int ret;
	uint8_t block[4] = {0};

	switch (reg) {
	case LTP8800_PAGE:
		reg = LTP8800_OPERATION;
	/* intentional fall through */
	case LTP8800_OPERATION:
	case LTP8800_WRITE_PROTECT:
	case LTP8800_VOUT_MODE:
	case LTP8800_STATUS_BYTE:
	case LTP8800_STATUS_VOUT:
	case LTP8800_STATUS_IOUT:
	case LTP8800_STATUS_INPUT:
	case LTP8800_STATUS_TEMPERATURE:
	case LTP8800_STATUS_CML:
	case LTP8800_STATUS_OTHER:
	case LTP8800_STATUS_MFR_SPECIFIC:
	case LTP8800_REVISION:
	case LTP8800_NM_DIGFILT_POLE:
	case LTP8800_NM_DIGFILT_ZERO:
	case LTP8800_NM_DIGFILT_HF_GAIN:
	case LTP8800_NM_DIGFILT_LF_GAIN:
	case LTP8800_SYNC:
		return ltp8800_read_byte(ltp8800, (uint16_t)reg,
					 (uint8_t *)readval);
	case LTP8800_VOUT_COMMAND:
	case LTP8800_VOUT_SCALE_LOOP:
	case LTP8800_VOUT_SCALE_MONITOR:
	case LTP8800_FREQUENCY_SWITCH:
	case LTP8800_VIN_ON:
	case LTP8800_VIN_OFF:
	case LTP8800_INTERLEAVE:
	case LTP8800_VIN_OV_FAULT_LIMIT:
	case LTP8800_VIN_UV_FAULT_LIMIT:
	case LTP8800_IIN_OC_FAULT_LIMIT:
	case LTP8800_POUT_OP_FAULT_LIMIT:
	case LTP8800_STATUS_WORD:
	case LTP8800_READ_VIN:
	case LTP8800_READ_IIN:
	case LTP8800_READ_VOUT:
	case LTP8800_READ_IOUT:
	case LTP8800_READ_TEMPERATURE_2:
	case LTP8800_READ_TEMPERATURE_3:
	case LTP8800_READ_DUTY_CYCLE:
	case LTP8800_READ_FREQUENCY:
	case LTP8800_READ_POUT:
		return ltp8800_read_word(ltp8800, (uint16_t)reg,
					 (uint16_t *)readval);
	case LTP8800_MFR_ID:
	case LTP8800_MFR_MODEL:
	case LTP8800_MFR_REVISION:
	case LTP8800_MFR_SERIAL:
	case LTP8800_IC_DEVICE_ID:
	case LTP8800_IC_DEVICE_REV:
		ret = ltp8800_read_block_data(ltp8800, (uint16_t)reg, block, 4);
		if (ret)
			return ret;

		*readval = no_os_get_unaligned_be32(block);

		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Write register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to write.
 * @param writeval - Value to write.
 * @return ret    - Result of the writing procedure.
*/
static int32_t ltp8800_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;

	switch (reg) {
	case LTP8800_STORE_USER_ALL:
	case LTP8800_RESTORE_USER_ALL:
		return ltp8800_send_byte(ltp8800, (uint16_t)reg);
	case LTP8800_WRITE_PROTECT:
	case LTP8800_STATUS_BYTE:
	case LTP8800_STATUS_VOUT:
	case LTP8800_STATUS_IOUT:
	case LTP8800_STATUS_INPUT:
	case LTP8800_STATUS_TEMPERATURE:
	case LTP8800_STATUS_CML:
	case LTP8800_STATUS_OTHER:
	case LTP8800_STATUS_MFR_SPECIFIC:
	case LTP8800_REVISION:
	case LTP8800_GO_CMD:
	case LTP8800_NM_DIGFILT_POLE:
	case LTP8800_NM_DIGFILT_ZERO:
	case LTP8800_NM_DIGFILT_HF_GAIN:
	case LTP8800_NM_DIGFILT_LF_GAIN:
	case LTP8800_SYNC:
	case LTP8800_EEPROM_PASSWORD:
		return ltp8800_write_byte(ltp8800, (uint16_t)reg,
					  (uint8_t)writeval);
	case LTP8800_VOUT_COMMAND:
	case LTP8800_VOUT_SCALE_LOOP:
	case LTP8800_VOUT_SCALE_MONITOR:
	case LTP8800_FREQUENCY_SWITCH:
	case LTP8800_VIN_ON:
	case LTP8800_VIN_OFF:
	case LTP8800_INTERLEAVE:
	case LTP8800_VIN_OV_FAULT_LIMIT:
	case LTP8800_VIN_UV_FAULT_LIMIT:
	case LTP8800_IIN_OC_FAULT_LIMIT:
	case LTP8800_POUT_OP_FAULT_LIMIT:
	case LTP8800_STATUS_WORD:
	case LTP8800_READ_VIN:
	case LTP8800_READ_IIN:
	case LTP8800_READ_VOUT:
	case LTP8800_READ_IOUT:
	case LTP8800_READ_TEMPERATURE_2:
	case LTP8800_READ_TEMPERATURE_3:
	case LTP8800_READ_DUTY_CYCLE:
	case LTP8800_READ_FREQUENCY:
	case LTP8800_READ_POUT:
		return ltp8800_write_word(ltp8800, (uint8_t)reg,
					  (uint16_t)writeval);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int ret;
	int32_t value;
	uint16_t temp;

	switch (channel->address) {
	case LTP8800_IIO_VIN_CHAN:
		ret = ltp8800_read_word(ltp8800, LTP8800_READ_VIN, &temp);
		break;
	case LTP8800_IIO_VOUT_CHAN:
		ret = ltp8800_read_word(ltp8800, LTP8800_READ_VOUT, &temp);
		break;
	case LTP8800_IIO_IIN_CHAN:
		ret = ltp8800_read_word(ltp8800, LTP8800_READ_IIN, &temp);
		break;
	case LTP8800_IIO_IOUT_CHAN:
		ret = ltp8800_read_word(ltp8800, LTP8800_READ_IOUT, &temp);
		break;
	case LTP8800_IIO_TEMP_CHAN:
		ret = ltp8800_read_word(ltp8800, LTP8800_READ_TEMPERATURE_2,
					&temp);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	value = (int32_t)temp;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &value);
}

/**
 * @brief Handles the read request for scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int ret, temp;
	int32_t vals[2];

	switch (channel->address) {
	case LTP8800_IIO_VIN_CHAN:
		ret = ltp8800_read_value(ltp8800, LTP8800_VIN, &temp);
		break;
	case LTP8800_IIO_VOUT_CHAN:
		ret = ltp8800_read_value(ltp8800, LTP8800_VOUT, &temp);
		break;
	case LTP8800_IIO_IIN_CHAN:
		ret = ltp8800_read_value(ltp8800, LTP8800_IIN, &temp);
		break;
	case LTP8800_IIO_IOUT_CHAN:
		ret = ltp8800_read_value(ltp8800, LTP8800_IOUT, &temp);
		break;
	case LTP8800_IIO_TEMP_CHAN:
		ret = ltp8800_read_value(ltp8800, LTP8800_FORWARD_DIODE_TEMP,
					 &temp);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	vals[0] = (int32_t)temp;
	vals[1] = (int32_t)MILLI;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

/**
 * @brief Handles the read request for sync_enable attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_read_sync_enable(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int ret;
	uint32_t val;
	uint8_t sync_en;

	ret = ltp8800_read_byte(ltp8800, LTP8800_SYNC, &sync_en);
	if (ret)
		return ret;

	val = !no_os_field_get(LTP8800_SYNC_ENABLE_BIT, sync_en);

	return sprintf(buf, "%s ", ltp8800_enable_avail[val]);
}

/**
 * @brief Handles the read request for sync_enable_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_read_sync_enable_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ltp8800_enable_avail); i++)
		length += sprintf(buf + length, "%s ", ltp8800_enable_avail[i]);

	return length;
}

/**
 * @brief Handles the write request for sync_enable attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_write_sync_enable(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ltp8800_enable_avail); i++)
		if (!strcmp(buf, ltp8800_enable_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(ltp8800_enable_avail))
		return -EINVAL;

	return ltp8800_sync_config(ltp8800, (bool)!i);
}

/**
 * @brief Handles the read request for vout-related attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_read_vout(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int ret, temp;
	int32_t vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_ltp8800->ltp8800_dev)
		return -EINVAL;

	ret = ltp8800_read_word_data(ltp8800, (uint16_t)priv, &temp);
	if (ret)
		return ret;

	vals[0] = (temp / (int32_t)MILLI);
	vals[1] = ((temp * (int32_t)MILLI) % (int32_t)MICRO);

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
}

/**
 * @brief Handles the write request for vou-related attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_write_vout(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int32_t val1, val2;

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val1, &val2);

	val1 = val1 * (int32_t)MILLI + val2 / (int32_t)MILLI;

	return ltp8800_write_word_data(ltp8800, priv, val1);
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
static int ltp8800_iio_read_limits(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int ret, temp;
	int32_t vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_ltp8800->ltp8800_dev)
		return -EINVAL;

	ret = ltp8800_read_word_data(ltp8800, priv, &temp);
	if (ret)
		return ret;

	if (priv == LTP8800_POUT_OP_FAULT_LIMIT) {
		vals[0] = temp / (int32_t)MICRO;
		vals[1] = (temp % (int32_t)MICRO);
	} else {
		vals[0] = (temp / (int32_t)MILLI);
		vals[1] = ((temp * (int32_t)MILLI) % (int32_t)MICRO);
	}

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
}

/**
 * @brief Handles the write request for limits global attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_write_limits(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int32_t val1, val2;
	bool negative;

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &val1, &val2);

	negative = (val1 < 0 || val2 < 0) ? true : false;
	if (val1 < 0)
		val1 = -val1;
	if (val2 < 0)
		val2 = -val2;

	if (priv == LTP8800_POUT_OP_FAULT_LIMIT) {
		val1 = val1 * (int32_t)MICRO + val2;
	} else {
		val1 = val1 * (int32_t)MILLI + val2 / (int32_t)MILLI;
	}

	if (negative)
		val1 = -val1;

	return ltp8800_set_fault_limit(ltp8800, (enum ltp8800_limit_type)priv,
				       val1);
}

/**
 * @brief Handles the read request for interleave attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_read_interleave(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int ret;
	int32_t val;
	uint16_t temp;

	if (!dev)
		return -EINVAL;

	if (!iio_ltp8800->ltp8800_dev)
		return -EINVAL;

	ret = ltp8800_read_word(ltp8800, LTP8800_INTERLEAVE, &temp);
	if (ret)
		return ret;

	val = no_os_field_get(LTP8800_INTERLEAVE_ORDER_MSK, temp);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the write request for interleave attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_write_interleave(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int32_t val;

	if (!dev)
		return -EINVAL;

	if (!iio_ltp8800->ltp8800_dev)
		return -EINVAL;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return ltp8800_interleave_order(ltp8800, (uint8_t)val);
}

/**
 * @brief Handles the read request for compensation-related attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_read_loop(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int ret;
	int32_t val;
	uint8_t temp;

	if (!dev)
		return -EINVAL;

	if (!iio_ltp8800->ltp8800_dev)
		return -EINVAL;

	ret = ltp8800_read_byte(ltp8800, (uint16_t)priv, &temp);
	if (ret)
		return ret;

	val = (int32_t)temp;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the write request for compensation-related attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_write_loop(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int32_t val;

	if (!dev)
		return -EINVAL;

	if (!iio_ltp8800->ltp8800_dev)
		return -EINVAL;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return ltp8800_write_byte(ltp8800, (uint16_t)priv, (uint8_t)val);
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
static int ltp8800_iio_read_status(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;
	int ret;
	int32_t val;
	uint16_t status_word;
	uint8_t status_byte;

	if (priv == LTP8800_STATUS_WORD) {
		ret = ltp8800_read_word(ltp8800, LTP8800_STATUS_WORD,
					&status_word);
		if (ret)
			return ret;

		val = (int32_t)status_word;
	} else {
		ret = ltp8800_read_byte(ltp8800, (uint16_t)priv,
					&status_byte);
		if (ret)
			return ret;

		val = (int32_t)status_byte;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the read request for user_settings_available attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_user_settings_available(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	switch (priv) {
	case LTP8800_IIO_STORE_USER_ALL:
		return sprintf(buf, "%s ", ltp8800_store_avail[0]);
	case LTP8800_IIO_RESTORE_USER_ALL:
		return sprintf(buf, "%s ", ltp8800_store_avail[1]);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the EEPROM user settings attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltp8800_iio_user_settings(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ltp8800_iio_desc *iio_ltp8800 = dev;
	struct ltp8800_dev *ltp8800 = iio_ltp8800->ltp8800_dev;

	if (!dev)
		return -EINVAL;

	if (!iio_ltp8800->ltp8800_dev)
		return -EINVAL;

	switch (priv) {
	case LTP8800_IIO_STORE_USER_ALL:
		if (!strcmp(buf, ltp8800_store_avail[0]))
			return ltp8800_store_user_settings(ltp8800);
		break;
	case LTP8800_IIO_RESTORE_USER_ALL:
		if (!strcmp(buf, ltp8800_store_avail[1]))
			return ltp8800_restore_user_settings(ltp8800);
		break;
	default:
		break;
	}

	return -EINVAL;
}

/**
 * @brief Initializes the LTP8800 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int ltp8800_iio_init(struct ltp8800_iio_desc **iio_desc,
		     struct ltp8800_iio_desc_init_param *init_param)
{
	struct ltp8800_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->ltp8800_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = ltp8800_init(&descriptor->ltp8800_dev,
			   init_param->ltp8800_init_param);
	if (ret)
		goto dev_err;

	descriptor->iio_dev = &ltp8800_iio_dev;

	*iio_desc = descriptor;

	return 0;

dev_err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function.
 * @param iio_desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int ltp8800_iio_remove(struct ltp8800_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	no_os_free(iio_desc->iio_dev->channels);
	ltp8800_remove(iio_desc->ltp8800_dev);
	no_os_free(iio_desc);

	return 0;
}

static struct iio_attribute ltp8800_chan_attrs[] = {
	{
		.name = "raw",
		.show = ltp8800_iio_read_raw
	},
	{
		.name = "scale",
		.show = ltp8800_iio_read_scale
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltp8800_global_attrs[] = {
	{
		.name = "sync",
		.show = ltp8800_iio_read_sync_enable,
		.store = ltp8800_iio_write_sync_enable,
	},
	{
		.name = "sync_available",
		.show = ltp8800_iio_read_sync_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "vout_command",
		.show = ltp8800_iio_read_vout,
		.store = ltp8800_iio_write_vout,
		.priv = LTP8800_VOUT_COMMAND
	},
	{
		.name = "vout_scale_loop",
		.show = ltp8800_iio_read_vout,
		.store = ltp8800_iio_write_vout,
		.priv = LTP8800_VOUT_SCALE_LOOP
	},
	{
		.name = "vout_scale_monitor",
		.show = ltp8800_iio_read_vout,
		.store = ltp8800_iio_write_vout,
		.priv = LTP8800_VOUT_SCALE_MONITOR
	},
	{
		.name = "vin_ov_fault_limit",
		.show = ltp8800_iio_read_limits,
		.store = ltp8800_iio_write_limits,
		.priv = LTP8800_VIN_OV_FAULT_LIMIT
	},
	{
		.name = "vin_uv_fault_limit",
		.show = ltp8800_iio_read_limits,
		.store = ltp8800_iio_write_limits,
		.priv = LTP8800_VIN_UV_FAULT_LIMIT
	},
	{
		.name = "iin_oc_fault_limit",
		.show = ltp8800_iio_read_limits,
		.store = ltp8800_iio_write_limits,
		.priv = LTP8800_IIN_OC_FAULT_LIMIT
	},
	{
		.name = "pout_op_fault_limit",
		.show = ltp8800_iio_read_limits,
		.store = ltp8800_iio_write_limits,
		.priv = LTP8800_POUT_OP_FAULT_LIMIT
	},
	{
		.name = "interleave_order",
		.show = ltp8800_iio_read_interleave,
		.store = ltp8800_iio_write_interleave,
	},
	{
		.name = "loop_pole",
		.show = ltp8800_iio_read_loop,
		.store = ltp8800_iio_write_loop,
		.priv = LTP8800_NM_DIGFILT_POLE
	},
	{
		.name = "loop_zero",
		.show = ltp8800_iio_read_loop,
		.store = ltp8800_iio_write_loop,
		.priv = LTP8800_NM_DIGFILT_ZERO
	},
	{
		.name = "loop_hf_gain",
		.show = ltp8800_iio_read_loop,
		.store = ltp8800_iio_write_loop,
		.priv = LTP8800_NM_DIGFILT_HF_GAIN
	},
	{
		.name = "loop_lf_gain",
		.show = ltp8800_iio_read_loop,
		.store = ltp8800_iio_write_loop,
		.priv = LTP8800_NM_DIGFILT_LF_GAIN
	},
	{
		.name = "store_user_settings",
		.show = ltp8800_iio_user_settings_available,
		.store = ltp8800_iio_user_settings,
		.priv = LTP8800_IIO_STORE_USER_ALL
	},
	{
		.name = "store_user_settings_available",
		.show = ltp8800_iio_user_settings_available,
		.priv = LTP8800_IIO_STORE_USER_ALL,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "restore_user_settings",
		.show = ltp8800_iio_user_settings_available,
		.store = ltp8800_iio_user_settings,
		.priv = LTP8800_IIO_RESTORE_USER_ALL
	},
	{
		.name = "restore_user_settings_available",
		.show = ltp8800_iio_user_settings_available,
		.priv = LTP8800_IIO_RESTORE_USER_ALL,
		.shared = IIO_SHARED_BY_ALL
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltp8800_debug_attrs[] = {
	{
		.name = "status_vout",
		.show = ltp8800_iio_read_status,
		.priv = LTP8800_STATUS_VOUT
	},
	{
		.name = "status_iout",
		.show = ltp8800_iio_read_status,
		.priv = LTP8800_STATUS_IOUT
	},
	{
		.name = "status_input",
		.show = ltp8800_iio_read_status,
		.priv = LTP8800_STATUS_INPUT
	},
	{
		.name = "status_mfr_specific",
		.show = ltp8800_iio_read_status,
		.priv = LTP8800_STATUS_MFR_SPECIFIC
	},
	{
		.name = "status_word",
		.show = ltp8800_iio_read_status,
		.priv = LTP8800_STATUS_WORD
	},
	{
		.name = "status_temperature",
		.show = ltp8800_iio_read_status,
		.priv = LTP8800_STATUS_TEMPERATURE
	},
	{
		.name = "status_cml",
		.show = ltp8800_iio_read_status,
		.priv = LTP8800_STATUS_CML
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ltp8800_channels[] = {
	{
		.name = "vout",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 0,
		.address = LTP8800_IIO_VOUT_CHAN,
		.attributes = ltp8800_chan_attrs,
		.ch_out = true
	},
	{
		.name = "iout",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = 0,
		.address = LTP8800_IIO_IOUT_CHAN,
		.attributes = ltp8800_chan_attrs,
		.ch_out = true
	},
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 0,
		.address = LTP8800_IIO_VIN_CHAN,
		.attributes = ltp8800_chan_attrs,
		.ch_out = false
	},
	{
		.name = "iin",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = 0,
		.address = LTP8800_IIO_IIN_CHAN,
		.attributes = ltp8800_chan_attrs,
		.ch_out = false
	},
	{
		.name = "temperature",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = 0,
		.address = LTP8800_IIO_TEMP_CHAN,
		.attributes = ltp8800_chan_attrs,
		.ch_out = false
	},
};

static struct iio_device ltp8800_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ltp8800_channels),
	.channels = ltp8800_channels,
	.attributes = ltp8800_global_attrs,
	.debug_attributes = ltp8800_debug_attrs,
	.debug_reg_read = ltp8800_iio_reg_read,
	.debug_reg_write = ltp8800_iio_reg_write,
};
