/***************************************************************************//**
 *   @file   iio_lt7170.c
 *   @brief  Source file for the LT7170 IIO Driver
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
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

#include "lt7170.h"
#include "iio_lt7170.h"

static const char *const lt7170_enable_avail[2] = {
	"Disabled", "Enabled"
};

enum lt7170_iio_enable_type {
	LT7170_IIO_OUTPUT_ENABLE,
	LT7170_IIO_PULSE_ENABLE,
	LT7170_IIO_SYNC_ENABLE,
};

enum lt7170_iio_pwm_params {
	LT7170_IIO_PWM_FREQUENCY,
	LT7170_IIO_PWM_PHASE,
};

enum lt7170_iio_chan_type {
	LT7170_IIO_VOUT_CHAN,
	LT7170_IIO_VIN_CHAN,
	LT7170_IIO_IOUT_CHAN,
	LT7170_IIO_TEMP_CHAN,
	LT7170_IIO_VCC_CHAN,
};

static struct iio_device lt7170_iio_dev;

/**
 * @brief Read register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to read.
 * @param readval - Read value.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int32_t lt7170_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	int ret;
	uint8_t byte_val;
	uint16_t word_val;
	uint8_t block[4] = {0};

	switch (reg) {
	case LT7170_PAGE:
	case LT7170_OPERATION:
	case LT7170_ON_OFF_CONFIG:
	case LT7170_WRITE_PROTECT:
	case LT7170_CAPABILITY:
	case LT7170_VOUT_MODE:
	case LT7170_VOUT_OV_FAULT_RESPONSE:
	case LT7170_VOUT_UV_FAULT_RESPONSE:
	case LT7170_IOUT_OC_FAULT_RESPONSE:
	case LT7170_OT_FAULT_RESPONSE:
	case LT7170_VIN_OV_FAULT_RESPONSE:
	case LT7170_TON_MAX_FAULT_RESPONSE:
	case LT7170_STATUS_BYTE:
	case LT7170_STATUS_VOUT:
	case LT7170_STATUS_IOUT:
	case LT7170_STATUS_INPUT:
	case LT7170_STATUS_TEMPERATURE:
	case LT7170_STATUS_CML:
	case LT7170_STATUS_MFR_SPECIFIC:
	case LT7170_REVISION:
	case LT7170_MFR_FAULT_RESPONSE:
	case LT7170_MFR_ADC_CONTROL:
	case LT7170_MFR_COMMON:
	case LT7170_MFR_CHANNEL_STATE:
	case LT7170_MFR_SYNC_CONFIG:
	case LT7170_MFR_RAIL_ADDRESS:
	case LT7170_MFR_DISABLE_OUTPUT:
		ret = lt7170_read_byte(lt7170, reg, &byte_val);
		*readval = byte_val;
		return ret;
	case LT7170_VOUT_COMMAND:
	case LT7170_VOUT_MAX:
	case LT7170_VOUT_MARGIN_HIGH:
	case LT7170_VOUT_MARGIN_LOW:
	case LT7170_VOUT_TRANSITION_RATE:
	case LT7170_FREQUENCY_SWITCH:
	case LT7170_VIN_ON:
	case LT7170_VIN_OFF:
	case LT7170_IOUT_CAL_OFFSET:
	case LT7170_VOUT_OV_FAULT_LIMIT:
	case LT7170_VOUT_OV_WARN_LIMIT:
	case LT7170_VOUT_UV_WARN_LIMIT:
	case LT7170_VOUT_UV_FAULT_LIMIT:
	case LT7170_IOUT_OC_WARN_LIMIT:
	case LT7170_OT_FAULT_LIMIT:
	case LT7170_OT_WARN_LIMIT:
	case LT7170_VIN_UV_WARN_LIMIT:
	case LT7170_TON_DELAY:
	case LT7170_TON_RISE:
	case LT7170_TON_MAX_FAULT_LIMIT:
	case LT7170_TOFF_DELAY:
	case LT7170_TOFF_FALL:
	case LT7170_TOFF_MAX_WARN_LIMIT:
	case LT7170_STATUS_WORD:
	case LT7170_READ_VIN:
	case LT7170_READ_VOUT:
	case LT7170_READ_IOUT:
	case LT7170_READ_TEMPERATURE_1:
	case LT7170_READ_FREQUENCY:
	case LT7170_MFR_USER_DATA_00:
	case LT7170_MFR_USER_DATA_01:
	case LT7170_MFR_READ_EXTVCC:
	case LT7170_MFR_READ_ITH:
	case LT7170_MFR_CHAN_CONFIG:
	case LT7170_MFR_CONFIG_ALL:
	case LT7170_MFR_FAULT_PROPAGATE:
	case LT7170_MFR_READ_ASEL:
	case LT7170_MFR_PWM_MODE:
	case LT7170_MFR_IOUT_PEAK:
	case LT7170_MFR_RETRY_DELAY:
	case LT7170_MFR_VOUT_PEAK:
	case LT7170_MFR_VIN_PEAK:
	case LT7170_MFR_TEMPERATURE_1_PEAK:
	case LT7170_MFR_DISCHARGE_THRESHOLD:
	case LT7170_MFR_PADS:
	case LT7170_MFR_SPECIAL_ID:
	case LT7170_MFR_PGOOD_DELAY:
	case LT7170_MFR_NOT_PGOOD_DELAY:
	case LT7170_MFR_PWM_PHASE:
		ret = lt7170_read_word(lt7170, reg, &word_val);
		*readval = word_val;
		return ret;
	case LT7170_PAGE_PLUS_READ:
	case LT7170_QUERY:
	case LT7170_SMBALERT_MASK:
	case LT7170_MFR_ID:
	case LT7170_MFR_SERIAL:
	case LT7170_IC_DEVICE_ID:
	case LT7170_IC_DEVICE_REV:
		ret = lt7170_read_block_data(lt7170, reg, &block[0], 4);
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
static int32_t lt7170_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;

	switch (reg) {
	case LT7170_PAGE:
	case LT7170_OPERATION:
	case LT7170_ON_OFF_CONFIG:
	case LT7170_WRITE_PROTECT:
	case LT7170_VOUT_OV_FAULT_RESPONSE:
	case LT7170_VOUT_UV_FAULT_RESPONSE:
	case LT7170_IOUT_OC_FAULT_RESPONSE:
	case LT7170_OT_FAULT_RESPONSE:
	case LT7170_VIN_OV_FAULT_RESPONSE:
	case LT7170_TON_MAX_FAULT_RESPONSE:
	case LT7170_STATUS_BYTE:
	case LT7170_STATUS_VOUT:
	case LT7170_STATUS_IOUT:
	case LT7170_STATUS_INPUT:
	case LT7170_STATUS_TEMPERATURE:
	case LT7170_STATUS_CML:
	case LT7170_STATUS_MFR_SPECIFIC:
	case LT7170_MFR_FAULT_RESPONSE:
	case LT7170_MFR_ADC_CONTROL:
	case LT7170_MFR_SYNC_CONFIG:
	case LT7170_MFR_RAIL_ADDRESS:
	case LT7170_MFR_DISABLE_OUTPUT:
		return lt7170_write_byte(lt7170, reg, (uint8_t)writeval);
	case LT7170_CLEAR_FAULTS:
	case LT7170_STORE_USER_ALL:
	case LT7170_RESTORE_USER_ALL:
	case LT7170_MFR_CLEAR_PEAKS:
	case LT7170_MFR_COMPARE_USER_ALL:
	case LT7170_MFR_RESET:
		return lt7170_send_byte(dev, reg);
	case LT7170_ZONE_CONFIG:
	case LT7170_ZONE_ACTIVE:
	case LT7170_VOUT_COMMAND:
	case LT7170_VOUT_MAX:
	case LT7170_VOUT_MARGIN_HIGH:
	case LT7170_VOUT_MARGIN_LOW:
	case LT7170_VOUT_TRANSITION_RATE:
	case LT7170_FREQUENCY_SWITCH:
	case LT7170_VIN_ON:
	case LT7170_VIN_OFF:
	case LT7170_IOUT_CAL_OFFSET:
	case LT7170_VOUT_OV_FAULT_LIMIT:
	case LT7170_VOUT_OV_WARN_LIMIT:
	case LT7170_VOUT_UV_WARN_LIMIT:
	case LT7170_VOUT_UV_FAULT_LIMIT:
	case LT7170_IOUT_OC_WARN_LIMIT:
	case LT7170_OT_FAULT_LIMIT:
	case LT7170_OT_WARN_LIMIT:
	case LT7170_VIN_UV_WARN_LIMIT:
	case LT7170_TON_DELAY:
	case LT7170_TON_RISE:
	case LT7170_TON_MAX_FAULT_LIMIT:
	case LT7170_TOFF_DELAY:
	case LT7170_TOFF_FALL:
	case LT7170_TOFF_MAX_WARN_LIMIT:
	case LT7170_STATUS_WORD:
	case LT7170_MFR_USER_DATA_00:
	case LT7170_MFR_USER_DATA_01:
	case LT7170_MFR_CHAN_CONFIG:
	case LT7170_MFR_CONFIG_ALL:
	case LT7170_MFR_FAULT_PROPAGATE:
	case LT7170_MFR_PWM_MODE:
	case LT7170_MFR_RETRY_DELAY:
	case LT7170_MFR_VOUT_PEAK:
	case LT7170_MFR_VIN_PEAK:
	case LT7170_MFR_TEMPERATURE_1_PEAK:
	case LT7170_MFR_DISCHARGE_THRESHOLD:
	case LT7170_MFR_PGOOD_DELAY:
	case LT7170_MFR_NOT_PGOOD_DELAY:
	case LT7170_MFR_PWM_PHASE:
		return lt7170_write_word(lt7170, reg, (uint16_t)writeval);
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
static int lt7170_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	int ret;
	uint16_t value;

	switch (channel->address) {
	case LT7170_IIO_VIN_CHAN:
		ret = lt7170_read_word(lt7170, LT7170_READ_VIN, &value);
		break;
	case LT7170_IIO_VOUT_CHAN:
		ret = lt7170_read_word(lt7170, LT7170_READ_VOUT, &value);
		break;
	case LT7170_IIO_IOUT_CHAN:
		ret = lt7170_read_word(lt7170, LT7170_READ_IOUT, &value);
		break;
	case LT7170_IIO_TEMP_CHAN:
		ret = lt7170_read_word(lt7170, LT7170_READ_TEMPERATURE_1,
				       &value);
		break;
	case LT7170_IIO_VCC_CHAN:
		ret = lt7170_read_word(lt7170, LT7170_MFR_READ_EXTVCC, &value);
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
static int lt7170_iio_read_scale(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	int vals[2], ret;
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;

	switch (channel->address) {
	case LT7170_IIO_VIN_CHAN:
		ret = lt7170_read_value(lt7170, LT7170_READ_VIN, &vals[0]);
		break;
	case LT7170_IIO_VOUT_CHAN:
		ret = lt7170_read_value(lt7170, LT7170_READ_VOUT, &vals[0]);
		break;
	case LT7170_IIO_IOUT_CHAN:
		ret = lt7170_read_value(lt7170, LT7170_READ_IOUT, &vals[0]);
		break;
	case LT7170_IIO_TEMP_CHAN:
		ret = lt7170_read_value(lt7170, LT7170_READ_TEMPERATURE_1,
					&vals[0]);
		break;
	case LT7170_IIO_VCC_CHAN:
		ret = lt7170_read_value(lt7170, LT7170_MFR_READ_EXTVCC,
					&vals[0]);
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
static int lt7170_iio_read_enable(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	int ret;
	uint32_t val;
	uint16_t word;

	switch (priv) {
	case LT7170_IIO_OUTPUT_ENABLE:
		ret = lt7170_read_word(lt7170, LT7170_MFR_PADS, &word);
		if (ret)
			return ret;

		val = no_os_field_get(LT7170_PADS_RUN_BIT, word);

		val = no_os_clamp(val, 0, 1);

		return sprintf(buf, "%s ", lt7170_enable_avail[val]);
	case LT7170_IIO_PULSE_ENABLE:
		ret = lt7170_read_word(lt7170, LT7170_MFR_PWM_MODE, &word);
		if (ret)
			return ret;

		val = no_os_clamp(no_os_field_get(LT7170_PWM_OP_MODE_BIT,
						  word),
				  0, 1);

		return sprintf(buf, "%s ", lt7170_enable_avail[val]);
	case LT7170_IIO_SYNC_ENABLE:
		ret = lt7170_read_word(lt7170, LT7170_MFR_SYNC_CONFIG, &word);
		if (ret)
			return ret;

		val = no_os_clamp(no_os_field_get(LT7170_SYNC_CLK_OUTPUT_BIT,
						  word),
				  0, 1);

		return sprintf(buf, "%s ", lt7170_enable_avail[val]);
	default:
		return -EINVAL;
	}
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
static int lt7170_iio_read_enable_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(lt7170_enable_avail); i++)
		length += sprintf(buf + length, "%s ", lt7170_enable_avail[i]);

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
static int lt7170_iio_write_enable(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(lt7170_enable_avail); i++)
		if (!strcmp(buf, lt7170_enable_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(lt7170_enable_avail))
		return -EINVAL;

	switch (priv) {
	case LT7170_IIO_OUTPUT_ENABLE:
		return lt7170_set_channel_state(lt7170, (bool)i);
	case LT7170_IIO_PULSE_ENABLE:
		return lt7170_pwm_mode(lt7170,
				       (enum lt7170_pwm_mode)i);
	case LT7170_IIO_SYNC_ENABLE:
		return lt7170_sync_config(lt7170, !i, i);
	default:
		return -EINVAL;
	}
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
static int lt7170_iio_read_vout(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	int ret, vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_lt7170->lt7170_dev)
		return -EINVAL;

	ret = lt7170_read_word_data(lt7170, (uint8_t)priv, &vals[1]);
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
static int lt7170_iio_write_vout(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	int val1, val2;

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, (int32_t *)&val1,
			(int32_t *)&val2);

	val1 = val1 * (int)MILLI + val2 / (int)MILLI;

	return lt7170_write_word_data(lt7170, priv, val1);
}

/**
 * @brief Handles the read request for pwm attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt7170_iio_read_pwm(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel,
			       intptr_t priv)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	int ret, vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_lt7170->lt7170_dev)
		return -EINVAL;

	switch (priv) {
	case LT7170_IIO_PWM_FREQUENCY:
		ret = lt7170_read_value(lt7170, LT7170_FREQUENCY, &vals[1]);
		break;
	case LT7170_IIO_PWM_PHASE:
		ret = lt7170_read_word_data(lt7170, LT7170_MFR_PWM_PHASE,
					    &vals[1]);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	vals[0] = (int32_t)(vals[1] / (int)MILLI);
	vals[1] = (int32_t)((vals[1] * (int)MILLI) % (int)MICRO);

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
				(int32_t *)vals);
}

/**
 * @brief Handles the write request for pwm attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int lt7170_iio_write_pwm(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	int val1, val2;
	bool negative;

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, (int32_t *)&val1,
			(int32_t *)&val2);

	negative = (val1 < 0 || val2 < 0) ? true : false;
	if (val1 < 0)
		val1 = -val1;
	if (val2 < 0)
		val2 = -val2;

	val1 = val1 * (int)MILLI + val2 / (int)MILLI;

	if (negative)
		val1 = -val1;

	switch (priv) {
	case LT7170_IIO_PWM_FREQUENCY:
		return lt7170_switch_freq(lt7170, val1);
	case LT7170_IIO_PWM_PHASE:
		return lt7170_pwm_phase(lt7170, val1);
	default:
		return -EINVAL;
	}
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
static int lt7170_iio_read_limits(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	int ret, vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_lt7170->lt7170_dev)
		return -EINVAL;

	ret = lt7170_read_word_data(lt7170, priv, &vals[1]);
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
static int lt7170_iio_write_limits(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	int val1, val2;
	bool negative;

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, (int32_t *)&val1,
			(int32_t *)&val2);

	negative = (val1 < 0 || val2 < 0) ? true : false;
	if (val1 < 0)
		val1 = -val1;
	if (val2 < 0)
		val2 = -val2;

	val1 = val1 * (int)MILLI + val2 / (int)MILLI;

	if (negative)
		val1 = -val1;

	return lt7170_set_limit(lt7170,
				(enum lt7170_limit_type)priv, val1);
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
static int lt7170_iio_read_status(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct lt7170_iio_desc *iio_lt7170 = dev;
	struct lt7170_dev *lt7170 = iio_lt7170->lt7170_dev;
	int ret;
	int32_t val;
	uint16_t status_word;
	uint8_t status_byte;

	if (priv == LT7170_STATUS_WORD) {
		ret = lt7170_read_word(lt7170, LT7170_STATUS_WORD,
				       &status_word);
		if (ret)
			return ret;

		val = (int32_t)status_word;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	} else {
		ret = lt7170_read_byte(lt7170, (uint8_t)priv,
				       &status_byte);
		if (ret)
			return ret;

		val = (int32_t)status_byte;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	}
}

/**
 * @brief Initializes the LT7170 IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int lt7170_iio_init(struct lt7170_iio_desc **iio_desc,
		    struct lt7170_iio_desc_init_param *init_param)
{
	struct lt7170_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->lt7170_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = lt7170_init(&descriptor->lt7170_dev,
			  init_param->lt7170_init_param);
	if (ret)
		goto dev_err;

	descriptor->iio_dev = &lt7170_iio_dev;

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
int lt7170_iio_remove(struct lt7170_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	no_os_free(iio_desc->iio_dev->channels);
	lt7170_remove(iio_desc->lt7170_dev);
	no_os_free(iio_desc);

	return 0;
}

static struct iio_attribute lt7170_input_attrs[] = {
	{
		.name = "raw",
		.show = lt7170_iio_read_raw
	},
	{
		.name = "scale",
		.show = lt7170_iio_read_scale
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt7170_output_attrs[] = {
	{
		.name = "raw",
		.show = lt7170_iio_read_raw
	},
	{
		.name = "scale",
		.show = lt7170_iio_read_scale
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt7170_global_attrs[] = {
	{
		.name = "freq_sync",
		.show = lt7170_iio_read_enable,
		.store = lt7170_iio_write_enable,
		.priv = LT7170_IIO_SYNC_ENABLE
	},
	{
		.name = "freq_sync_available",
		.show = lt7170_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "frequency",
		.show = lt7170_iio_read_pwm,
		.store = lt7170_iio_write_pwm,
		.priv = LT7170_IIO_PWM_FREQUENCY
	},
	{
		.name = "vout_ov_fault_limit",
		.show = lt7170_iio_read_limits,
		.store = lt7170_iio_write_limits,
		.priv = LT7170_VOUT_OV_FAULT_LIMIT
	},
	{
		.name = "vout_ov_warn_limit",
		.show = lt7170_iio_read_limits,
		.store = lt7170_iio_write_limits,
		.priv = LT7170_VOUT_OV_WARN_LIMIT
	},
	{
		.name = "vout_uv_fault_limit",
		.show = lt7170_iio_read_limits,
		.store = lt7170_iio_write_limits,
		.priv = LT7170_VOUT_UV_FAULT_LIMIT
	},
	{
		.name = "vout_uv_warn_limit",
		.show = lt7170_iio_read_limits,
		.store = lt7170_iio_write_limits,
		.priv = LT7170_VOUT_UV_WARN_LIMIT
	},
	{
		.name = "iout_oc_warn_limit",
		.show = lt7170_iio_read_limits,
		.store = lt7170_iio_write_limits,
		.priv = LT7170_IOUT_OC_WARN_LIMIT
	},
	{
		.name = "ot_fault_limit",
		.show = lt7170_iio_read_limits,
		.store = lt7170_iio_write_limits,
		.priv = LT7170_OT_FAULT_LIMIT
	},
	{
		.name = "ot_warn_limit",
		.show = lt7170_iio_read_limits,
		.store = lt7170_iio_write_limits,
		.priv = LT7170_OT_WARN_LIMIT
	},
	{
		.name = "vin_uv_warn_limit",
		.show = lt7170_iio_read_limits,
		.store = lt7170_iio_write_limits,
		.priv = LT7170_VIN_UV_WARN_LIMIT
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt7170_debug_attrs[] = {
	{
		.name = "status_vout",
		.show = lt7170_iio_read_status,
		.priv = LT7170_STATUS_VOUT
	},
	{
		.name = "status_iout",
		.show = lt7170_iio_read_status,
		.priv = LT7170_STATUS_IOUT
	},
	{
		.name = "status_input",
		.show = lt7170_iio_read_status,
		.priv = LT7170_STATUS_INPUT
	},
	{
		.name = "status_mfr_specific",
		.show = lt7170_iio_read_status,
		.priv = LT7170_STATUS_MFR_SPECIFIC
	},
	{
		.name = "status_word",
		.show = lt7170_iio_read_status,
		.priv = LT7170_STATUS_WORD
	},
	{
		.name = "status_temperature",
		.show = lt7170_iio_read_status,
		.priv = LT7170_STATUS_TEMPERATURE
	},
	{
		.name = "status_cml",
		.show = lt7170_iio_read_status,
		.priv = LT7170_STATUS_CML
	},
	{
		.name = "pads_enable",
		.show = lt7170_iio_read_enable,
		.store = lt7170_iio_write_enable,
		.priv = LT7170_IIO_OUTPUT_ENABLE,
	},
	{
		.name = "pads_enable_available",
		.show = lt7170_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "vout_command",
		.show = lt7170_iio_read_vout,
		.store = lt7170_iio_write_vout,
		.priv = LT7170_VOUT_COMMAND
	},
	{
		.name = "vout_max",
		.show = lt7170_iio_read_vout,
		.store = lt7170_iio_write_vout,
		.priv = LT7170_VOUT_MAX
	},
	{
		.name = "vout_margin_high",
		.show = lt7170_iio_read_vout,
		.store = lt7170_iio_write_vout,
		.priv = LT7170_VOUT_MARGIN_HIGH
	},
	{
		.name = "vout_margin_low",
		.show = lt7170_iio_read_vout,
		.store = lt7170_iio_write_vout,
		.priv = LT7170_VOUT_MARGIN_LOW
	},
	{
		.name = "phase",
		.show = lt7170_iio_read_pwm,
		.store = lt7170_iio_write_pwm,
		.priv = LT7170_IIO_PWM_PHASE
	},
	{
		.name = "pulse_skipping",
		.show = lt7170_iio_read_enable,
		.store = lt7170_iio_write_enable,
		.priv = LT7170_IIO_PULSE_ENABLE
	},
	{
		.name = "pulse_skipping_available",
		.show = lt7170_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel lt7170_channels[] = {
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.address = LT7170_IIO_VIN_CHAN,
		.attributes = lt7170_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout",
		.ch_type = IIO_CURRENT,
		.address = LT7170_IIO_IOUT_CHAN,
		.attributes = lt7170_input_attrs,
		.ch_out = true
	},
	{
		.name = "vout",
		.ch_type = IIO_VOLTAGE,
		.address = LT7170_IIO_VOUT_CHAN,
		.attributes = lt7170_output_attrs,
		.ch_out = true
	},
	{
		.name = "temperature",
		.ch_type = IIO_TEMP,
		.address = LT7170_IIO_TEMP_CHAN,
		.attributes = lt7170_input_attrs,
		.ch_out = false,
	},
	{
		.name = "vcc",
		.ch_type = IIO_VOLTAGE,
		.address = LT7170_IIO_VCC_CHAN,
		.attributes = lt7170_input_attrs,
		.ch_out = false,
	},
};

static struct iio_device lt7170_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(lt7170_channels),
	.channels = lt7170_channels,
	.attributes = lt7170_global_attrs,
	.debug_attributes = lt7170_debug_attrs,
	.debug_reg_read = lt7170_iio_reg_read,
	.debug_reg_write = lt7170_iio_reg_write,
};
