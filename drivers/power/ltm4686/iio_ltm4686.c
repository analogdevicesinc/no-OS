/***************************************************************************//**
 *   @file   iio_ltm4686.c
 *   @brief  Source file for the LTM4686 IIO Driver
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
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
#include <string.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "ltm4686.h"
#include "iio_ltm4686.h"

#define LTM4686_IIO_REG_CHAN(reg, chan)			(reg | (chan << 8))
#define LTM4686_IIO_REG(x)				(x & 0xFF)
#define LTM4686_IIO_CHAN(x)				((x >> 8) & 0xFF)

#define LTM4686_IIO_VOUT_CHAN_GROUP(inst)	LTM4686_IIO_VOUT_ ##inst## _CHAN, \
					LTM4686_IIO_IOUT_ ##inst## _CHAN

static const char *const ltm4686_enable_avail[2] = {
	"Disabled", "Enabled"
};

enum lt7182s_iio_enable_type {
	LT7182S_IIO_OUTPUT_ENABLE,
	LT7182S_IIO_PULSE_ENABLE,
	LT7182S_IIO_SYNC_ENABLE,
};

enum ltm4686_iio_chan_type {
	LTM4686_IIO_VOUT_CHAN_GROUP(0),
	LTM4686_IIO_VOUT_CHAN_GROUP(1),
	LTM4686_IIO_VOUT_CHAN_GROUP(2),
	LTM4686_IIO_VOUT_CHAN_GROUP(3),
	LTM4686_IIO_VIN_CHAN,
	LTM4686_IIO_IIN_CHAN,
	LTM4686_IIO_TEMP_CHAN,
};

static struct iio_device ltm4686_iio_dev;
static struct iio_device ltm4673_iio_dev;

/**
 * @brief Read register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to read.
 * @param readval - Read value.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int32_t ltm4686_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;
	int ret;
	uint8_t block[4] = {0};

	switch (reg) {
	case LTM4686_MFR_GPIO_PROPAGATE:
		if (ltm4686->id == ID_LTM4673)
			return ltm4686_read_byte(ltm4686, ltm4686->page,
						 (uint8_t)reg,
						 (uint8_t *)readval);
		else
			return ltm4686_read_word(ltm4686, ltm4686->page,
						 (uint8_t)reg,
						 (uint16_t *)readval);
	case LTM4686_MFR_PWM_MODE:
	case LTM4686_MFR_ADC_CONTROL:
		if (ltm4686->id == ID_LTM4673)
			return ltm4686_read_word(ltm4686, ltm4686->page,
						 (uint8_t)reg,
						 (uint16_t *)readval);
		else
			return ltm4686_read_byte(ltm4686, ltm4686->page,
						 (uint8_t)reg,
						 (uint8_t *)readval);

	case LTM4686_VOUT_COMMAND:
	case LTM4686_VOUT_MAX:
	case LTM4686_VOUT_MARGIN_HIGH:
	case LTM4686_VOUT_MARGIN_LOW:
	case LTM4686_VOUT_TRANSITION_RATE:
	case LTM4686_FREQUENCY_SWITCH:
	case LTM4686_VIN_ON:
	case LTM4686_VIN_OFF:
	case LTM4686_VOUT_OV_FAULT_LIMIT:
	case LTM4686_VOUT_OV_WARN_LIMIT:
	case LTM4686_VOUT_UV_WARN_LIMIT:
	case LTM4686_VOUT_UV_FAULT_LIMIT:
	case LTM4686_IOUT_OC_WARN_LIMIT:
	case LTM4686_OT_FAULT_LIMIT:
	case LTM4686_OT_WARN_LIMIT:
	case LTM4686_VIN_UV_WARN_LIMIT:
	case LTM4686_IIN_OC_WARN_LIMIT:
	case LTM4686_TON_DELAY:
	case LTM4686_TON_RISE:
	case LTM4686_TON_MAX_FAULT_LIMIT:
	case LTM4686_TOFF_DELAY:
	case LTM4686_TOFF_FALL:
	case LTM4686_TOFF_MAX_WARN_LIMIT:
	case LTM4686_STATUS_WORD:
	case LTM4686_READ_VIN:
	case LTM4686_READ_IIN:
	case LTM4686_READ_VOUT:
	case LTM4686_READ_IOUT:
	case LTM4686_READ_TEMPERATURE_1:
	case LTM4686_READ_POUT:
	case LTM4686_MFR_USER_DATA_00:
	case LTM4686_MFR_USER_DATA_01:
	case LTM4686_MFR_USER_DATA_02:
	case LTM4686_MFR_USER_DATA_03:
	case LTM4686_MFR_CHAN_CONFIG:
	case LTM4686_MFR_CONFIG_ALL:
	case LTM4686_MFR_IOUT_PEAK:
	case LTM4686_MFR_RETRY_DELAY:
	case LTM4686_MFR_RESTART_DELAY:
	case LTM4686_MFR_VOUT_PEAK:
	case LTM4686_MFR_VIN_PEAK:
	case LTM4686_MFR_TEMPERATURE_1_PEAK:
	case LTM4686_MFR_PADS:
	case LTM4686_MFR_ADDRESS:
	case LTM4686_MFR_SPECIAL_ID:
	case LTM4686_MFR_IIN_OFFSET:
	case LTM4673_MFR_DAC:
	case LTM4673_MFR_PGD_ASSERTION_DELAY:
	case LTM4673_MFR_WATCHDOG_T_FIRST:
	case LTM4673_MFR_WATCHDOG_T:
	case LTM4673_MFR_IIN_CAL_GAIN:
		return ltm4686_read_word(ltm4686, ltm4686->page, (uint8_t)reg,
					 (uint16_t *)readval);
	case LTM4686_PAGE_PLUS_READ:
	case LTM4686_SMBALERT_MASK:
	case LTM4686_MFR_ID:
	case LTM4686_MFR_MODEL:
	case LTM4686_MFR_SERIAL:
		ret = ltm4686_read_block_data(ltm4686, ltm4686->page,
					      (uint8_t)reg, &block[0], 4);
		if (ret)
			return ret;

		*readval = no_os_get_unaligned_be32(block);

		return 0;
	default:
		return ltm4686_read_byte(ltm4686, ltm4686->page, (uint8_t)reg,
					 (uint8_t *)readval);
	}
}

/**
 * @brief Write register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to write.
 * @param writeval - Value to write.
 * @return ret    - Result of the writing procedure.
*/
static int32_t ltm4686_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;

	switch (reg) {
	case LTM4686_CLEAR_FAULTS:
	case LTM4686_STORE_USER_ALL:
	case LTM4686_RESTORE_USER_ALL:
	case LTM4686_MFR_CLEAR_PEAKS:
	case LTM4686_MFR_FAULT_LOG_STORE:
	case LTM4673_MFR_FAULT_LOG_RESTORE:
	case LTM4686_MFR_FAULT_LOG_CLEAR:
	case LTM4686_MFR_COMPARE_USER_ALL:
	case LTM4686_MFR_RESET:
		return ltm4686_send_byte(dev, ltm4686->page, (uint8_t) reg);
	case LTM4686_VOUT_COMMAND:
	case LTM4686_VOUT_MAX:
	case LTM4686_VOUT_MARGIN_HIGH:
	case LTM4686_VOUT_MARGIN_LOW:
	case LTM4686_VOUT_TRANSITION_RATE:
	case LTM4686_FREQUENCY_SWITCH:
	case LTM4686_VIN_ON:
	case LTM4686_VIN_OFF:
	case LTM4686_VOUT_OV_FAULT_LIMIT:
	case LTM4686_VOUT_OV_WARN_LIMIT:
	case LTM4686_VOUT_UV_WARN_LIMIT:
	case LTM4686_VOUT_UV_FAULT_LIMIT:
	case LTM4686_IOUT_OC_WARN_LIMIT:
	case LTM4686_OT_FAULT_LIMIT:
	case LTM4686_OT_WARN_LIMIT:
	case LTM4686_VIN_UV_WARN_LIMIT:
	case LTM4686_IIN_OC_WARN_LIMIT:
	case LTM4686_TON_DELAY:
	case LTM4686_TON_RISE:
	case LTM4686_TON_MAX_FAULT_LIMIT:
	case LTM4686_TOFF_DELAY:
	case LTM4686_TOFF_FALL:
	case LTM4686_TOFF_MAX_WARN_LIMIT:
	case LTM4686_STATUS_WORD:
	case LTM4686_MFR_USER_DATA_00:
	case LTM4686_MFR_USER_DATA_01:
	case LTM4686_MFR_USER_DATA_02:
	case LTM4686_MFR_USER_DATA_03:
	case LTM4686_MFR_CHAN_CONFIG:
	case LTM4686_MFR_CONFIG_ALL:
	case LTM4686_MFR_PWM_MODE:
	case LTM4686_MFR_RETRY_DELAY:
	case LTM4686_MFR_RESTART_DELAY:
	case LTM4686_MFR_VOUT_PEAK:
	case LTM4686_MFR_VIN_PEAK:
	case LTM4686_MFR_TEMPERATURE_1_PEAK:
	case LTM4686_MFR_ADDRESS:
		return ltm4686_write_word(ltm4686, ltm4686->page, (uint8_t)reg,
					  (uint16_t)writeval);
	default:
		return ltm4686_write_byte(ltm4686, ltm4686->page, (uint8_t)reg,
					  (uint8_t)writeval);
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
static int ltm4686_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;
	int ret;
	uint16_t value;

	switch (channel->address) {
	case LTM4686_IIO_VIN_CHAN:
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_ALL,
					LTM4686_READ_VIN, &value);
		break;
	case LTM4686_IIO_VOUT_0_CHAN:
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_0,
					LTM4686_READ_VOUT, &value);
		break;
	case LTM4686_IIO_VOUT_1_CHAN:
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_1,
					LTM4686_READ_VOUT, &value);
		break;
	case LTM4686_IIO_VOUT_2_CHAN:
		if (ltm4686->id != ID_LTM4673)
			return -ENOTSUP;
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_2,
					LTM4686_READ_VOUT, &value);
		break;
	case LTM4686_IIO_VOUT_3_CHAN:
		if (ltm4686->id != ID_LTM4673)
			return -ENOTSUP;
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_3,
					LTM4686_READ_VOUT, &value);
		break;
	case LTM4686_IIO_IIN_CHAN:
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_ALL,
					LTM4686_READ_IIN, &value);
		break;
	case LTM4686_IIO_IOUT_0_CHAN:
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_0,
					LTM4686_READ_IOUT, &value);
		break;
	case LTM4686_IIO_IOUT_1_CHAN:
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_1,
					LTM4686_READ_IOUT, &value);
		break;
	case LTM4686_IIO_IOUT_2_CHAN:
		if (ltm4686->id != ID_LTM4673)
			return -ENOTSUP;
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_2,
					LTM4686_READ_IOUT, &value);
		break;
	case LTM4686_IIO_IOUT_3_CHAN:
		if (ltm4686->id != ID_LTM4673)
			return -ENOTSUP;
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_3,
					LTM4686_READ_IOUT, &value);
		break;
	case LTM4686_IIO_TEMP_CHAN:
		ret = ltm4686_read_word(ltm4686, LTM4686_CHAN_ALL,
					LTM4686_READ_TEMPERATURE_2, &value);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&value);
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
static int ltm4686_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int vals[2], ret;
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;

	switch (channel->address) {
	case LTM4686_IIO_VIN_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_ALL,
					 LTM4686_READ_VIN, &vals[0]);
		break;
	case LTM4686_IIO_VOUT_0_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_0,
					 LTM4686_READ_VOUT, &vals[0]);
		break;
	case LTM4686_IIO_VOUT_1_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_1,
					 LTM4686_READ_VOUT, &vals[0]);
		break;
	case LTM4686_IIO_VOUT_2_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_2,
					 LTM4686_READ_VOUT, &vals[0]);
		break;
	case LTM4686_IIO_VOUT_3_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_3,
					 LTM4686_READ_VOUT, &vals[0]);
		break;
	case LTM4686_IIO_IIN_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_ALL,
					 LTM4686_READ_IIN, &vals[0]);
		break;
	case LTM4686_IIO_IOUT_0_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_0,
					 LTM4686_READ_IOUT, &vals[0]);
		break;
	case LTM4686_IIO_IOUT_1_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_1,
					 LTM4686_READ_IOUT, &vals[0]);
		break;
	case LTM4686_IIO_IOUT_2_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_2,
					 LTM4686_READ_IOUT, &vals[0]);
		break;
	case LTM4686_IIO_IOUT_3_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_3,
					 LTM4686_READ_IOUT, &vals[0]);
		break;
	case LTM4686_IIO_TEMP_CHAN:
		ret = ltm4686_read_value(ltm4686, LTM4686_CHAN_ALL,
					 LTM4686_READ_TEMPERATURE_2, &vals[0]);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	vals[1] = (int)MILLI;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL,
				2, (int32_t *)vals);
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
static int ltm4686_iio_read_enable(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;
	int ret;
	uint8_t val;

	ret = ltm4686_read_byte(ltm4686, LTM4686_CHAN_ALL,
				LTM4686_OPERATION, &val);
	if (ret)
		return ret;

	val = (val == LTM4686_OPERATION_OFF) ? 0 : 1;

	return sprintf(buf, "%s ", ltm4686_enable_avail[val]);
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
static int ltm4686_iio_read_enable_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ltm4686_enable_avail); i++)
		length += sprintf(buf + length, "%s ", ltm4686_enable_avail[i]);

	return length;
}

/**
 * @brief Handles the write request for enable attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltm4686_iio_write_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;
	int chan;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ltm4686_enable_avail); i++)
		if (!strcmp(buf, ltm4686_enable_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(ltm4686_enable_avail))
		return -EINVAL;

	chan = no_os_clamp(channel->address, 0U,
			   (uint32_t)ltm4686->num_channels - 1);

	return ltm4686_set_operation(ltm4686, chan, i ? LTM4686_OPERATION_ON :
				     LTM4686_OPERATION_OFF);
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
static int ltm4686_iio_read_vout(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;
	int ret, chan, vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_ltm4686->ltm4686_dev)
		return -EINVAL;

	chan = no_os_clamp(channel->address, 0U,
			   (uint32_t)ltm4686->num_channels - 1);

	ret = ltm4686_read_word_data(ltm4686, chan, (uint8_t)priv, &vals[1]);
	if (ret)
		return ret;

	vals[0] = (int32_t)(vals[1] / (int)MILLI);
	vals[1] = (int32_t)((vals[1] * (int)MILLI) % (int)MICRO);

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
				(int32_t *)vals);
}

/**
 * @brief Handles the write request for vout attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int ltm4686_iio_write_vout(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;
	int val1, val2, chan;

	chan = no_os_clamp(channel->address, 0U,
			   (uint32_t)ltm4686->num_channels - 1);

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, (int32_t *)&val1,
			(int32_t *)&val2);

	val1 = val1 * (int)MILLI + val2 / (int)MILLI;

	return ltm4686_write_word_data(ltm4686, chan, priv, val1);
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
static int ltm4686_iio_read_limits(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;
	int ret, chan, vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_ltm4686->ltm4686_dev)
		return -EINVAL;

	chan = LTM4686_IIO_CHAN(priv);
	priv = LTM4686_IIO_REG(priv);

	ret = ltm4686_read_word_data(ltm4686, chan, priv, &vals[1]);
	if (ret)
		return ret;

	vals[0] = (int32_t)(vals[1] / (int)MILLI);
	vals[1] = (int32_t)((vals[1] * (int)MILLI) % (int)MICRO);

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
				(int32_t *)vals);
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
static int ltm4686_iio_write_limits(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;
	int val1, val2, chan, data;

	chan = LTM4686_IIO_CHAN(priv);
	priv = LTM4686_IIO_REG(priv);

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, (int32_t *)&val1,
			(int32_t *)&val2);

	data = (val1 < 0) ? -val1 * (int)MILLI : val1 * (int)MILLI;
	data += (val2 < 0) ? -val2 / (int)MILLI : val2 / (int)MILLI;

	if (val1 < 0 || val2 < 0)
		data = -data;

	return ltm4686_write_word_data(ltm4686, chan, priv, data);
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
static int ltm4686_iio_read_status(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct ltm4686_iio_desc *iio_ltm4686 = dev;
	struct ltm4686_dev *ltm4686 = iio_ltm4686->ltm4686_dev;
	int ret, chan;
	int32_t val;
	uint16_t status_word;
	uint8_t status_byte;

	chan = LTM4686_IIO_CHAN(priv);
	priv = LTM4686_IIO_REG(priv);

	if (priv == LTM4686_STATUS_WORD) {
		ret = ltm4686_read_word(ltm4686, chan, LTM4686_STATUS_WORD,
					&status_word);
		if (ret)
			return ret;

		val = (int32_t)status_word;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	} else {
		ret = ltm4686_read_byte(ltm4686, chan, (uint8_t)priv,
					&status_byte);
		if (ret)
			return ret;

		val = (int32_t)status_byte;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	}
}

/**
 * @brief Initializes the LTM4686 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int ltm4686_iio_init(struct ltm4686_iio_desc **iio_desc,
		     struct ltm4686_iio_desc_init_param *init_param)
{
	struct ltm4686_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->ltm4686_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = ltm4686_init(&descriptor->ltm4686_dev,
			   init_param->ltm4686_init_param);
	if (ret)
		goto dev_err;

	if (descriptor->ltm4686_dev->id == ID_LTM4673)
		descriptor->iio_dev = &ltm4673_iio_dev;
	else
		descriptor->iio_dev = &ltm4686_iio_dev;

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
int ltm4686_iio_remove(struct ltm4686_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	no_os_free(iio_desc->iio_dev->channels);
	ltm4686_remove(iio_desc->ltm4686_dev);
	no_os_free(iio_desc);

	return 0;
}

static struct iio_attribute ltm4686_input_attrs[] = {
	{
		.name = "raw",
		.show = ltm4686_iio_read_raw
	},
	{
		.name = "scale",
		.show = ltm4686_iio_read_scale
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltm4686_output_attrs[] = {
	{
		.name = "raw",
		.show = ltm4686_iio_read_raw
	},
	{
		.name = "scale",
		.show = ltm4686_iio_read_scale
	},
	{
		.name = "enable",
		.show = ltm4686_iio_read_enable,
		.store = ltm4686_iio_write_enable,
	},
	{
		.name = "enable_available",
		.show = ltm4686_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "vout_command",
		.show = ltm4686_iio_read_vout,
		.store = ltm4686_iio_write_vout,
		.priv = LTM4686_VOUT_COMMAND
	},
	{
		.name = "vout_max",
		.show = ltm4686_iio_read_vout,
		.store = ltm4686_iio_write_vout,
		.priv = LTM4686_VOUT_MAX
	},
	{
		.name = "vout_margin_high",
		.show = ltm4686_iio_read_vout,
		.store = ltm4686_iio_write_vout,
		.priv = LTM4686_VOUT_MARGIN_HIGH
	},
	{
		.name = "vout_margin_low",
		.show = ltm4686_iio_read_vout,
		.store = ltm4686_iio_write_vout,
		.priv = LTM4686_VOUT_MARGIN_LOW
	},
	END_ATTRIBUTES_ARRAY
};

#define LTM4686_IIO_LIM(_name, _reg, _chan) { \
	.name = _name, \
	.show = ltm4686_iio_read_limits, \
	.store = ltm4686_iio_write_limits, \
	.priv = LTM4686_IIO_REG_CHAN(_reg, _chan) \
}

static struct iio_attribute ltm4686_global_attrs[] = {
	LTM4686_IIO_LIM("vout_ov_fault_limit_0", LTM4686_VOUT_OV_FAULT_LIMIT, 0),
	LTM4686_IIO_LIM("vout_ov_fault_limit_1", LTM4686_VOUT_OV_FAULT_LIMIT, 1),
	LTM4686_IIO_LIM("vout_uv_fault_limit_0", LTM4686_VOUT_UV_FAULT_LIMIT, 0),
	LTM4686_IIO_LIM("vout_uv_fault_limit_1", LTM4686_VOUT_UV_FAULT_LIMIT, 1),
	LTM4686_IIO_LIM("iout_oc_fault_limit_0", LTM4686_IOUT_OC_FAULT_LIMIT, 0),
	LTM4686_IIO_LIM("iout_oc_fault_limit_1", LTM4686_IOUT_OC_FAULT_LIMIT, 1),
	LTM4686_IIO_LIM("ot_fault_limit", LTM4686_OT_FAULT_LIMIT, LTM4686_CHAN_ALL),
	LTM4686_IIO_LIM("vin_ov_fault_limit", LTM4686_VIN_OV_FAULT_LIMIT, LTM4686_CHAN_ALL),
	LTM4686_IIO_LIM("vin_uv_warn_limit", LTM4686_VIN_UV_WARN_LIMIT, LTM4686_CHAN_ALL),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltm4673_global_attrs[] = {
	LTM4686_IIO_LIM("vout_ov_fault_limit_0", LTM4686_VOUT_OV_FAULT_LIMIT, 0),
	LTM4686_IIO_LIM("vout_ov_fault_limit_1", LTM4686_VOUT_OV_FAULT_LIMIT, 1),
	LTM4686_IIO_LIM("vout_ov_fault_limit_2", LTM4686_VOUT_OV_FAULT_LIMIT, 2),
	LTM4686_IIO_LIM("vout_ov_fault_limit_3", LTM4686_VOUT_OV_FAULT_LIMIT, 3),
	LTM4686_IIO_LIM("vout_uv_fault_limit_0", LTM4686_VOUT_UV_FAULT_LIMIT, 0),
	LTM4686_IIO_LIM("vout_uv_fault_limit_1", LTM4686_VOUT_UV_FAULT_LIMIT, 1),
	LTM4686_IIO_LIM("vout_uv_fault_limit_2", LTM4686_VOUT_UV_FAULT_LIMIT, 2),
	LTM4686_IIO_LIM("vout_uv_fault_limit_3", LTM4686_VOUT_UV_FAULT_LIMIT, 3),
	LTM4686_IIO_LIM("iout_oc_fault_limit_0", LTM4686_IOUT_OC_FAULT_LIMIT, 0),
	LTM4686_IIO_LIM("iout_oc_fault_limit_1", LTM4686_IOUT_OC_FAULT_LIMIT, 1),
	LTM4686_IIO_LIM("iout_oc_fault_limit_2", LTM4686_IOUT_OC_FAULT_LIMIT, 2),
	LTM4686_IIO_LIM("iout_oc_fault_limit_3", LTM4686_IOUT_OC_FAULT_LIMIT, 3),
	LTM4686_IIO_LIM("ot_fault_limit", LTM4686_OT_FAULT_LIMIT, LTM4686_CHAN_ALL),
	LTM4686_IIO_LIM("vin_ov_fault_limit", LTM4686_VIN_OV_FAULT_LIMIT, LTM4686_CHAN_ALL),
	LTM4686_IIO_LIM("vin_uv_warn_limit", LTM4686_VIN_UV_WARN_LIMIT, LTM4686_CHAN_ALL),
	END_ATTRIBUTES_ARRAY
};

#define LTM4686_IIO_STATUS(_name, _reg, _chan) { \
	.name = _name, \
	.show = ltm4686_iio_read_status, \
	.priv = LTM4686_IIO_REG_CHAN(_reg, _chan) \
}

static struct iio_attribute ltm4686_debug_attrs[] = {
	LTM4686_IIO_STATUS("status_vout_0", LTM4686_STATUS_VOUT, 0),
	LTM4686_IIO_STATUS("status_vout_1", LTM4686_STATUS_VOUT, 1),
	LTM4686_IIO_STATUS("status_iout_0", LTM4686_STATUS_IOUT, 0),
	LTM4686_IIO_STATUS("status_iout_1", LTM4686_STATUS_IOUT, 1),
	LTM4686_IIO_STATUS("status_mfr_specific_0", LTM4686_STATUS_MFR_SPECIFIC, 0),
	LTM4686_IIO_STATUS("status_mfr_specific_1", LTM4686_STATUS_MFR_SPECIFIC, 1),
	LTM4686_IIO_STATUS("status_word_0", LTM4686_STATUS_WORD, 0),
	LTM4686_IIO_STATUS("status_word_1", LTM4686_STATUS_WORD, 1),
	LTM4686_IIO_STATUS("status_input", LTM4686_STATUS_INPUT, LTM4686_CHAN_ALL),
	LTM4686_IIO_STATUS("status_temperature", LTM4686_STATUS_TEMPERATURE, LTM4686_CHAN_ALL),
	LTM4686_IIO_STATUS("status_cml", LTM4686_STATUS_CML, LTM4686_CHAN_ALL),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ltm4673_debug_attrs[] = {
	LTM4686_IIO_STATUS("status_vout_0", LTM4686_STATUS_VOUT, 0),
	LTM4686_IIO_STATUS("status_vout_1", LTM4686_STATUS_VOUT, 1),
	LTM4686_IIO_STATUS("status_vout_2", LTM4686_STATUS_VOUT, 2),
	LTM4686_IIO_STATUS("status_vout_3", LTM4686_STATUS_VOUT, 3),
	LTM4686_IIO_STATUS("status_iout_0", LTM4686_STATUS_IOUT, 0),
	LTM4686_IIO_STATUS("status_iout_1", LTM4686_STATUS_IOUT, 1),
	LTM4686_IIO_STATUS("status_iout_2", LTM4686_STATUS_IOUT, 2),
	LTM4686_IIO_STATUS("status_iout_3", LTM4686_STATUS_IOUT, 3),
	LTM4686_IIO_STATUS("status_mfr_specific_0", LTM4686_STATUS_MFR_SPECIFIC, 0),
	LTM4686_IIO_STATUS("status_mfr_specific_1", LTM4686_STATUS_MFR_SPECIFIC, 1),
	LTM4686_IIO_STATUS("status_mfr_specific_2", LTM4686_STATUS_MFR_SPECIFIC, 2),
	LTM4686_IIO_STATUS("status_mfr_specific_3", LTM4686_STATUS_MFR_SPECIFIC, 3),
	LTM4686_IIO_STATUS("status_word_0", LTM4686_STATUS_WORD, 0),
	LTM4686_IIO_STATUS("status_word_1", LTM4686_STATUS_WORD, 1),
	LTM4686_IIO_STATUS("status_word_2", LTM4686_STATUS_WORD, 2),
	LTM4686_IIO_STATUS("status_word_3", LTM4686_STATUS_WORD, 3),
	LTM4686_IIO_STATUS("status_input", LTM4686_STATUS_INPUT, LTM4686_CHAN_ALL),
	LTM4686_IIO_STATUS("status_temperature", LTM4686_STATUS_TEMPERATURE, LTM4686_CHAN_ALL),
	LTM4686_IIO_STATUS("status_cml", LTM4686_STATUS_CML, LTM4686_CHAN_ALL),
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ltm4686_channels[] = {
	{
		.name = "vin0",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LTM4686_CHAN_0,
		.address = LTM4686_IIO_VIN_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = false
	},
	{
		.name = "iin0",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LTM4686_CHAN_0,
		.address = LTM4686_IIO_IIN_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout0",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LTM4686_CHAN_0,
		.address = LTM4686_IIO_IOUT_0_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = true
	},
	{
		.name = "iout1",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LTM4686_CHAN_1,
		.address = LTM4686_IIO_IOUT_1_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = true
	},
	{
		.name = "vout0",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LTM4686_CHAN_0,
		.address = LTM4686_IIO_VOUT_0_CHAN,
		.attributes = ltm4686_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LTM4686_CHAN_1,
		.address = LTM4686_IIO_VOUT_1_CHAN,
		.attributes = ltm4686_output_attrs,
		.ch_out = true
	},
	{
		.name = "temperature",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = LTM4686_CHAN_0,
		.address = LTM4686_IIO_TEMP_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = false,
	},
};

static struct iio_channel ltm4673_channels[] = {
	{
		.name = "vin0",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LTM4686_CHAN_0,
		.address = LTM4686_IIO_VIN_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = false
	},
	{
		.name = "iin0",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LTM4686_CHAN_0,
		.address = LTM4686_IIO_IIN_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout0",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LTM4686_CHAN_0,
		.address = LTM4686_IIO_IOUT_0_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = true
	},
	{
		.name = "iout1",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LTM4686_CHAN_1,
		.address = LTM4686_IIO_IOUT_1_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = true
	},
	{
		.name = "iout2",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LTM4686_CHAN_2,
		.address = LTM4686_IIO_IOUT_2_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = true
	},
	{
		.name = "iout3",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LTM4686_CHAN_3,
		.address = LTM4686_IIO_IOUT_3_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = true
	},
	{
		.name = "vout0",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LTM4686_CHAN_0,
		.address = LTM4686_IIO_VOUT_0_CHAN,
		.attributes = ltm4686_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LTM4686_CHAN_1,
		.address = LTM4686_IIO_VOUT_1_CHAN,
		.attributes = ltm4686_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout2",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LTM4686_CHAN_2,
		.address = LTM4686_IIO_VOUT_2_CHAN,
		.attributes = ltm4686_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout3",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LTM4686_CHAN_3,
		.address = LTM4686_IIO_VOUT_3_CHAN,
		.attributes = ltm4686_output_attrs,
		.ch_out = true
	},
	{
		.name = "temperature",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = LTM4686_CHAN_0,
		.address = LTM4686_IIO_TEMP_CHAN,
		.attributes = ltm4686_input_attrs,
		.ch_out = false,
	},
};

static struct iio_device ltm4686_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ltm4686_channels),
	.channels = ltm4686_channels,
	.attributes = ltm4686_global_attrs,
	.debug_attributes = ltm4686_debug_attrs,
	.debug_reg_read = ltm4686_iio_reg_read,
	.debug_reg_write = ltm4686_iio_reg_write,
};

static struct iio_device ltm4673_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ltm4673_channels),
	.channels = ltm4673_channels,
	.attributes = ltm4673_global_attrs,
	.debug_attributes = ltm4673_debug_attrs,
	.debug_reg_read = ltm4686_iio_reg_read,
	.debug_reg_write = ltm4686_iio_reg_write,
};
