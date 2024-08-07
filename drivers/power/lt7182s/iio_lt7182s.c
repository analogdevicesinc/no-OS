/***************************************************************************//**
 *   @file   iio_lt7182s.c
 *   @brief  Source file for the LT7182S IIO Driver
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

#include "lt7182s.h"
#include "iio_lt7182s.h"

#define LT7182S_IIO_REG_CHAN(reg, chan)			(reg | (chan << 8))
#define LT7182S_IIO_REG(x)				(x & 0xFF)
#define LT7182S_IIO_CHAN(x)				((x >> 8) & 0xFF)

static const char *const lt7182s_enable_avail[2] = {
	"Disabled", "Enabled"
};

enum lt7182s_iio_enable_type {
	LT7182S_IIO_OUTPUT_ENABLE,
	LT7182S_IIO_PULSE_ENABLE,
	LT7182S_IIO_SYNC_ENABLE,
};

enum lt7182s_iio_pwm_params {
	LT7182S_IIO_PWM_FREQUENCY,
	LT7182S_IIO_PWM_PHASE,
};

enum lt7182s_iio_chan_type {
	LT7182S_IIO_VOUT_0_CHAN,
	LT7182S_IIO_VOUT_1_CHAN,
	LT7182S_IIO_VIN_0_CHAN,
	LT7182S_IIO_VIN_1_CHAN,
	LT7182S_IIO_IIN_0_CHAN,
	LT7182S_IIO_IIN_1_CHAN,
	LT7182S_IIO_IOUT_0_CHAN,
	LT7182S_IIO_IOUT_1_CHAN,
	LT7182S_IIO_TEMP_CHAN,
	LT7182S_IIO_VCC_CHAN,
};

static struct iio_device lt7182s_iio_dev;

/**
 * @brief Read register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to read.
 * @param readval - Read value.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*/
static int32_t lt7182s_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int ret;
	uint8_t block[4] = {0};

	switch (reg) {
	case LT7182S_PAGE:
	case LT7182S_OPERATION:
	case LT7182S_ON_OFF_CONFIG:
	case LT7182S_WRITE_PROTECT:
	case LT7182S_CAPABILITY:
	case LT7182S_VOUT_MODE:
	case LT7182S_VOUT_OV_FAULT_RESPONSE:
	case LT7182S_VOUT_UV_FAULT_RESPONSE:
	case LT7182S_IOUT_OC_FAULT_RESPONSE:
	case LT7182S_OT_FAULT_RESPONSE:
	case LT7182S_VIN_OV_FAULT_RESPONSE:
	case LT7182S_TON_MAX_FAULT_RESPONSE:
	case LT7182S_STATUS_BYTE:
	case LT7182S_STATUS_VOUT:
	case LT7182S_STATUS_IOUT:
	case LT7182S_STATUS_INPUT:
	case LT7182S_STATUS_TEMPERATURE:
	case LT7182S_STATUS_CML:
	case LT7182S_STATUS_MFR_SPECIFIC:
	case LT7182S_REVISION:
	case LT7182S_MFR_FAULT_RESPONSE:
	case LT7182S_MFR_ADC_CONTROL:
	case LT7182S_MFR_COMMON:
	case LT7182S_MFR_CHANNEL_STATE:
	case LT7182S_MFR_SYNC_CONFIG:
	case LT7182S_MFR_PIN_CONFIG:
	case LT7182S_MFR_RAIL_ADDRESS:
	case LT7182S_MFR_DISABLE_OUTPUT:
	case LT7182S_MFR_EE_USER_WP:
		return lt7182s_read_byte(lt7182s, lt7182s->page, (uint8_t)reg,
					 (uint8_t *)readval);
	case LT7182S_VOUT_COMMAND:
	case LT7182S_VOUT_MAX:
	case LT7182S_VOUT_MARGIN_HIGH:
	case LT7182S_VOUT_MARGIN_LOW:
	case LT7182S_VOUT_TRANSITION_RATE:
	case LT7182S_FREQUENCY_SWITCH:
	case LT7182S_VIN_ON:
	case LT7182S_VIN_OFF:
	case LT7182S_VOUT_OV_FAULT_LIMIT:
	case LT7182S_VOUT_OV_WARN_LIMIT:
	case LT7182S_VOUT_UV_WARN_LIMIT:
	case LT7182S_VOUT_UV_FAULT_LIMIT:
	case LT7182S_IOUT_OC_WARN_LIMIT:
	case LT7182S_OT_FAULT_LIMIT:
	case LT7182S_OT_WARN_LIMIT:
	case LT7182S_VIN_UV_WARN_LIMIT:
	case LT7182S_IIN_OC_WARN_LIMIT:
	case LT7182S_TON_DELAY:
	case LT7182S_TON_RISE:
	case LT7182S_TON_MAX_FAULT_LIMIT:
	case LT7182S_TOFF_DELAY:
	case LT7182S_TOFF_FALL:
	case LT7182S_TOFF_MAX_WARN_LIMIT:
	case LT7182S_STATUS_WORD:
	case LT7182S_READ_VIN:
	case LT7182S_READ_IIN:
	case LT7182S_READ_VOUT:
	case LT7182S_READ_IOUT:
	case LT7182S_READ_TEMPERATURE_1:
	case LT7182S_READ_FREQUENCY:
	case LT7182S_READ_POUT:
	case LT7182S_MFR_USER_DATA_00:
	case LT7182S_MFR_USER_DATA_01:
	case LT7182S_MFR_USER_DATA_02:
	case LT7182S_MFR_USER_DATA_03:
	case LT7182S_MFR_READ_EXTVCC:
	case LT7182S_MFR_READ_ITH:
	case LT7182S_MFR_CHAN_CONFIG:
	case LT7182S_MFR_CONFIG_ALL:
	case LT7182S_MFR_FAULT_PROPAGATE:
	case LT7182S_MFR_PWM_MODE:
	case LT7182S_MFR_IOUT_PEAK:
	case LT7182S_MFR_RETRY_DELAY:
	case LT7182S_MFR_RESTART_DELAY:
	case LT7182S_MFR_VOUT_PEAK:
	case LT7182S_MFR_VIN_PEAK:
	case LT7182S_MFR_TEMPERATURE_1_PEAK:
	case LT7182S_MFR_DISCHARGE_THRESHOLD:
	case LT7182S_MFR_PADS:
	case LT7182S_MFR_ADDRESS:
	case LT7182S_MFR_SPECIAL_ID:
	case LT7182S_MFR_PGOOD_DELAY:
	case LT7182S_MFR_NOT_PGOOD_DELAY:
	case LT7182S_MFR_PWM_PHASE:
		return lt7182s_read_word(lt7182s, lt7182s->page, (uint8_t)reg,
					 (uint16_t *)readval);
	case LT7182S_PAGE_PLUS_READ:
	case LT7182S_QUERY:
	case LT7182S_SMBALERT_MASK:
	case LT7182S_MFR_ID:
	case LT7182S_MFR_MODEL:
	case LT7182S_MFR_REVISION:
	case LT7182S_MFR_SERIAL:
	case LT7182S_IC_DEVICE_ID:
	case LT7182S_IC_DEVICE_REV:
		ret = lt7182s_read_block_data(lt7182s, lt7182s->page,
					      (uint8_t)reg, &block[0], 4);
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
static int32_t lt7182s_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;

	switch (reg) {
	case LT7182S_PAGE:
	case LT7182S_OPERATION:
	case LT7182S_ON_OFF_CONFIG:
	case LT7182S_WRITE_PROTECT:
	case LT7182S_VOUT_OV_FAULT_RESPONSE:
	case LT7182S_VOUT_UV_FAULT_RESPONSE:
	case LT7182S_IOUT_OC_FAULT_RESPONSE:
	case LT7182S_OT_FAULT_RESPONSE:
	case LT7182S_VIN_OV_FAULT_RESPONSE:
	case LT7182S_TON_MAX_FAULT_RESPONSE:
	case LT7182S_STATUS_BYTE:
	case LT7182S_STATUS_VOUT:
	case LT7182S_STATUS_IOUT:
	case LT7182S_STATUS_INPUT:
	case LT7182S_STATUS_TEMPERATURE:
	case LT7182S_STATUS_CML:
	case LT7182S_STATUS_MFR_SPECIFIC:
	case LT7182S_MFR_FAULT_RESPONSE:
	case LT7182S_MFR_ADC_CONTROL:
	case LT7182S_MFR_SYNC_CONFIG:
	case LT7182S_MFR_RAIL_ADDRESS:
	case LT7182S_MFR_DISABLE_OUTPUT:
	case LT7182S_MFR_EE_USER_WP:
		return lt7182s_write_byte(lt7182s, lt7182s->page, (uint8_t)reg,
					  (uint8_t)writeval);
	case LT7182S_CLEAR_FAULTS:
	case LT7182S_STORE_USER_ALL:
	case LT7182S_RESTORE_USER_ALL:
	case LT7182S_MFR_CLEAR_PEAKS:
	case LT7182S_MFR_FAULT_LOG_STORE:
	case LT7182S_MFR_FAULT_LOG_CLEAR:
	case LT7182S_MFR_COMPARE_USER_ALL:
	case LT7182S_MFR_RESET:
		return lt7182s_send_byte(dev, lt7182s->page, (uint8_t) reg);
	case LT7182S_ZONE_CONFIG:
	case LT7182S_ZONE_ACTIVE:
	case LT7182S_VOUT_COMMAND:
	case LT7182S_VOUT_MAX:
	case LT7182S_VOUT_MARGIN_HIGH:
	case LT7182S_VOUT_MARGIN_LOW:
	case LT7182S_VOUT_TRANSITION_RATE:
	case LT7182S_FREQUENCY_SWITCH:
	case LT7182S_VIN_ON:
	case LT7182S_VIN_OFF:
	case LT7182S_VOUT_OV_FAULT_LIMIT:
	case LT7182S_VOUT_OV_WARN_LIMIT:
	case LT7182S_VOUT_UV_WARN_LIMIT:
	case LT7182S_VOUT_UV_FAULT_LIMIT:
	case LT7182S_IOUT_OC_WARN_LIMIT:
	case LT7182S_OT_FAULT_LIMIT:
	case LT7182S_OT_WARN_LIMIT:
	case LT7182S_VIN_UV_WARN_LIMIT:
	case LT7182S_IIN_OC_WARN_LIMIT:
	case LT7182S_TON_DELAY:
	case LT7182S_TON_RISE:
	case LT7182S_TON_MAX_FAULT_LIMIT:
	case LT7182S_TOFF_DELAY:
	case LT7182S_TOFF_FALL:
	case LT7182S_TOFF_MAX_WARN_LIMIT:
	case LT7182S_STATUS_WORD:
	case LT7182S_MFR_USER_DATA_00:
	case LT7182S_MFR_USER_DATA_01:
	case LT7182S_MFR_USER_DATA_02:
	case LT7182S_MFR_USER_DATA_03:
	case LT7182S_MFR_CHAN_CONFIG:
	case LT7182S_MFR_CONFIG_ALL:
	case LT7182S_MFR_FAULT_PROPAGATE:
	case LT7182S_MFR_PWM_MODE:
	case LT7182S_MFR_RETRY_DELAY:
	case LT7182S_MFR_RESTART_DELAY:
	case LT7182S_MFR_VOUT_PEAK:
	case LT7182S_MFR_VIN_PEAK:
	case LT7182S_MFR_TEMPERATURE_1_PEAK:
	case LT7182S_MFR_DISCHARGE_THRESHOLD:
	case LT7182S_MFR_ADDRESS:
	case LT7182S_MFR_PGOOD_DELAY:
	case LT7182S_MFR_NOT_PGOOD_DELAY:
	case LT7182S_MFR_PWM_PHASE:
		return lt7182s_write_word(lt7182s, lt7182s->page, (uint8_t)reg,
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
static int lt7182s_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int ret;
	uint16_t value;

	switch (channel->address) {
	case LT7182S_IIO_VIN_0_CHAN:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_0,
					LT7182S_READ_VIN, &value);
		break;
	case LT7182S_IIO_VIN_1_CHAN:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_1,
					LT7182S_READ_VIN, &value);
		break;
	case LT7182S_IIO_VOUT_0_CHAN:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_0,
					LT7182S_READ_VOUT, &value);
		break;
	case LT7182S_IIO_VOUT_1_CHAN:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_1,
					LT7182S_READ_VOUT, &value);
		break;
	case LT7182S_IIO_IIN_0_CHAN:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_0,
					LT7182S_READ_IIN, &value);
		break;
	case LT7182S_IIO_IIN_1_CHAN:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_1,
					LT7182S_READ_IIN, &value);
		break;
	case LT7182S_IIO_IOUT_0_CHAN:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_0,
					LT7182S_READ_IOUT, &value);
		break;
	case LT7182S_IIO_IOUT_1_CHAN:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_1,
					LT7182S_READ_IOUT, &value);
		break;
	case LT7182S_IIO_TEMP_CHAN:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_ALL,
					LT7182S_READ_TEMPERATURE_1, &value);
		break;
	case LT7182S_IIO_VCC_CHAN:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_ALL,
					LT7182S_MFR_READ_EXTVCC, &value);
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
static int lt7182s_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	int vals[2], ret;
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;

	switch (channel->address) {
	case LT7182S_IIO_VIN_0_CHAN:
		ret = lt7182s_read_value(lt7182s, LT7182S_CHAN_0,
					 LT7182S_READ_VIN, &vals[0]);
		break;
	case LT7182S_IIO_VIN_1_CHAN:
		ret = lt7182s_read_value(lt7182s, LT7182S_CHAN_1,
					 LT7182S_READ_VIN, &vals[0]);
		break;
	case LT7182S_IIO_VOUT_0_CHAN:
		ret = lt7182s_read_value(lt7182s, LT7182S_CHAN_0,
					 LT7182S_READ_VOUT, &vals[0]);
		break;
	case LT7182S_IIO_VOUT_1_CHAN:
		ret = lt7182s_read_value(lt7182s, LT7182S_CHAN_1,
					 LT7182S_READ_VOUT, &vals[0]);
		break;
	case LT7182S_IIO_IIN_0_CHAN:
		ret = lt7182s_read_value(lt7182s, LT7182S_CHAN_0,
					 LT7182S_READ_IIN, &vals[0]);
		break;
	case LT7182S_IIO_IIN_1_CHAN:
		ret = lt7182s_read_value(lt7182s, LT7182S_CHAN_1,
					 LT7182S_READ_IIN, &vals[0]);
		break;
	case LT7182S_IIO_IOUT_0_CHAN:
		ret = lt7182s_read_value(lt7182s, LT7182S_CHAN_0,
					 LT7182S_READ_IOUT, &vals[0]);
		break;
	case LT7182S_IIO_IOUT_1_CHAN:
		ret = lt7182s_read_value(lt7182s, LT7182S_CHAN_1,
					 LT7182S_READ_IOUT, &vals[0]);
		break;
	case LT7182S_IIO_TEMP_CHAN:
		ret = lt7182s_read_value(lt7182s, LT7182S_CHAN_ALL,
					 LT7182S_READ_TEMPERATURE_1, &vals[0]);
		break;
	case LT7182S_IIO_VCC_CHAN:
		ret = lt7182s_read_value(lt7182s, LT7182S_CHAN_ALL,
					 LT7182S_MFR_READ_EXTVCC, &vals[0]);
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
static int lt7182s_iio_read_enable(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int ret, chan;
	uint32_t val;
	uint16_t word;

	chan = no_os_clamp(channel->address, 0, 1);

	switch (priv) {
	case LT7182S_IIO_OUTPUT_ENABLE:
		ret = lt7182s_read_word(lt7182s, LT7182S_CHAN_ALL,
					LT7182S_MFR_PADS, &word);
		if (ret)
			return ret;

		if (chan)
			val = no_os_field_get(LT7182S_PADS_RUN1_BIT, word);
		else
			val = no_os_field_get(LT7182S_PADS_RUN0_BIT, word);

		val = no_os_clamp(val, 0, 1);

		return sprintf(buf, "%s ", lt7182s_enable_avail[val]);
	case LT7182S_IIO_PULSE_ENABLE:
		ret = lt7182s_read_word(lt7182s, chan,
					LT7182S_MFR_PWM_MODE, &word);
		if (ret)
			return ret;

		val = no_os_clamp(no_os_field_get(LT7182S_PWM_OP_MODE_BIT,
						  word),
				  0, 1);

		return sprintf(buf, "%s ", lt7182s_enable_avail[val]);
	case LT7182S_IIO_SYNC_ENABLE:
		ret = lt7182s_read_word(lt7182s, chan,
					LT7182S_MFR_SYNC_CONFIG, &word);
		if (ret)
			return ret;

		val = no_os_clamp(no_os_field_get(LT7182S_SYNC_CLK_OUTPUT_BIT,
						  word),
				  0, 1);

		return sprintf(buf, "%s ", lt7182s_enable_avail[val]);
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
static int lt7182s_iio_read_enable_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(lt7182s_enable_avail); i++)
		length += sprintf(buf + length, "%s ", lt7182s_enable_avail[i]);

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
static int lt7182s_iio_write_enable(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int chan;
	uint32_t i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(lt7182s_enable_avail); i++)
		if (!strcmp(buf, lt7182s_enable_avail[i]))
			break;

	if (i == NO_OS_ARRAY_SIZE(lt7182s_enable_avail))
		return -EINVAL;

	chan = no_os_clamp(channel->address, 0, 1);

	switch (priv) {
	case LT7182S_IIO_OUTPUT_ENABLE:
		return lt7182s_set_channel_state(lt7182s, chan, (bool)i);
	case LT7182S_IIO_PULSE_ENABLE:
		return lt7182s_pwm_mode(lt7182s, chan,
					(enum lt7182s_pwm_mode)i);
	case LT7182S_IIO_SYNC_ENABLE:
		return lt7182s_sync_config(lt7182s, !i, i);
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
static int lt7182s_iio_read_vout(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int ret, chan, vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_lt7182s->lt7182s_dev)
		return -EINVAL;

	chan = no_os_clamp(channel->address, 0, 1);

	ret = lt7182s_read_word_data(lt7182s, chan, (uint8_t)priv, &vals[1]);
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
static int lt7182s_iio_write_vout(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int val1, val2, chan;

	chan = no_os_clamp(channel->address, 0, 1);

	iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, (int32_t *)&val1,
			(int32_t *)&val2);

	val1 = val1 * (int)MILLI + val2 / (int)MILLI;

	return lt7182s_write_word_data(lt7182s, chan, priv, val1);
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
static int lt7182s_iio_read_pwm(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int ret, chan, vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_lt7182s->lt7182s_dev)
		return -EINVAL;

	chan = no_os_clamp(channel->address, 0, 1);

	switch (priv) {
	case LT7182S_IIO_PWM_FREQUENCY:
		ret = lt7182s_read_value(lt7182s, chan,
					 LT7182S_FREQUENCY, &vals[1]);
		break;
	case LT7182S_IIO_PWM_PHASE:
		ret = lt7182s_read_word_data(lt7182s, chan,
					     LT7182S_MFR_PWM_PHASE, &vals[1]);
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
static int lt7182s_iio_write_pwm(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int val1, val2, chan;
	bool negative;

	chan = no_os_clamp(channel->address, 0, 1);

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
	case LT7182S_IIO_PWM_FREQUENCY:
		return lt7182s_switch_freq(lt7182s, val1);
	case LT7182S_IIO_PWM_PHASE:
		return lt7182s_pwm_phase(lt7182s, chan, val1);
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
static int lt7182s_iio_read_limits(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int ret, chan, vals[2];

	if (!dev)
		return -EINVAL;

	if (!iio_lt7182s->lt7182s_dev)
		return -EINVAL;

	chan = LT7182S_IIO_CHAN(priv);
	priv = LT7182S_IIO_REG(priv);

	ret = lt7182s_read_word_data(lt7182s, chan, priv, &vals[1]);
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
static int lt7182s_iio_write_limits(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int val1, val2, chan;
	bool negative;

	chan = LT7182S_IIO_CHAN(priv);
	priv = LT7182S_IIO_REG(priv);

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

	return lt7182s_set_limit(lt7182s, chan,
				 (enum lt7182s_limit_type)priv, val1);
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
static int lt7182s_iio_read_status(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	struct lt7182s_iio_desc *iio_lt7182s = dev;
	struct lt7182s_dev *lt7182s = iio_lt7182s->lt7182s_dev;
	int ret, chan;
	int32_t val;
	uint16_t status_word;
	uint8_t status_byte;

	chan = LT7182S_IIO_CHAN(priv);
	priv = LT7182S_IIO_REG(priv);

	if (priv == LT7182S_STATUS_WORD) {
		ret = lt7182s_read_word(lt7182s, chan, LT7182S_STATUS_WORD,
					&status_word);
		if (ret)
			return ret;

		val = (int32_t)status_word;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	} else {
		ret = lt7182s_read_byte(lt7182s, chan, (uint8_t)priv,
					&status_byte);
		if (ret)
			return ret;

		val = (int32_t)status_byte;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	}
}

/**
 * @brief Initializes the LT7182S IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int lt7182s_iio_init(struct lt7182s_iio_desc **iio_desc,
		     struct lt7182s_iio_desc_init_param *init_param)
{
	struct lt7182s_iio_desc *descriptor;
	int ret;

	if (!init_param || !init_param->lt7182s_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = lt7182s_init(&descriptor->lt7182s_dev,
			   init_param->lt7182s_init_param);
	if (ret)
		goto dev_err;

	descriptor->iio_dev = &lt7182s_iio_dev;

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
int lt7182s_iio_remove(struct lt7182s_iio_desc *iio_desc)
{
	if (!iio_desc)
		return -ENODEV;

	no_os_free(iio_desc->iio_dev->channels);
	lt7182s_remove(iio_desc->lt7182s_dev);
	no_os_free(iio_desc);

	return 0;
}

static struct iio_attribute lt7182s_input_attrs[] = {
	{
		.name = "raw",
		.show = lt7182s_iio_read_raw
	},
	{
		.name = "scale",
		.show = lt7182s_iio_read_scale
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt7182s_output_attrs[] = {
	{
		.name = "raw",
		.show = lt7182s_iio_read_raw
	},
	{
		.name = "scale",
		.show = lt7182s_iio_read_scale
	},
	{
		.name = "enable",
		.show = lt7182s_iio_read_enable,
		.store = lt7182s_iio_write_enable,
		.priv = LT7182S_IIO_OUTPUT_ENABLE,
	},
	{
		.name = "enable_available",
		.show = lt7182s_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "vout_command",
		.show = lt7182s_iio_read_vout,
		.store = lt7182s_iio_write_vout,
		.priv = LT7182S_VOUT_COMMAND
	},
	{
		.name = "vout_max",
		.show = lt7182s_iio_read_vout,
		.store = lt7182s_iio_write_vout,
		.priv = LT7182S_VOUT_MAX
	},
	{
		.name = "vout_margin_high",
		.show = lt7182s_iio_read_vout,
		.store = lt7182s_iio_write_vout,
		.priv = LT7182S_VOUT_MARGIN_HIGH
	},
	{
		.name = "vout_margin_low",
		.show = lt7182s_iio_read_vout,
		.store = lt7182s_iio_write_vout,
		.priv = LT7182S_VOUT_MARGIN_LOW
	},
	{
		.name = "phase",
		.show = lt7182s_iio_read_pwm,
		.store = lt7182s_iio_write_pwm,
		.priv = LT7182S_IIO_PWM_PHASE
	},
	{
		.name = "pulse_skipping",
		.show = lt7182s_iio_read_enable,
		.store = lt7182s_iio_write_enable,
		.priv = LT7182S_IIO_PULSE_ENABLE
	},
	{
		.name = "pulse_skipping_available",
		.show = lt7182s_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	END_ATTRIBUTES_ARRAY
};


static struct iio_attribute lt7182s_global_attrs[] = {
	{
		.name = "freq_sync",
		.show = lt7182s_iio_read_enable,
		.store = lt7182s_iio_write_enable,
		.priv = LT7182S_IIO_SYNC_ENABLE
	},
	{
		.name = "freq_sync_available",
		.show = lt7182s_iio_read_enable_available,
		.shared = IIO_SHARED_BY_ALL
	},
	{
		.name = "frequency",
		.show = lt7182s_iio_read_pwm,
		.store = lt7182s_iio_write_pwm,
		.priv = LT7182S_IIO_PWM_FREQUENCY
	},
	{
		.name = "vout_ov_fault_limit_0",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_VOUT_OV_FAULT_LIMIT, 0)
	},
	{
		.name = "vout_ov_fault_limit_1",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_VOUT_OV_FAULT_LIMIT, 1)
	},
	{
		.name = "vout_ov_warn_limit_0",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_VOUT_OV_WARN_LIMIT, 0)
	},
	{
		.name = "vout_ov_warn_limit_1",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_VOUT_OV_WARN_LIMIT, 1)
	},
	{
		.name = "vout_uv_fault_limit_0",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_VOUT_UV_FAULT_LIMIT, 0)
	},
	{
		.name = "vout_uv_fault_limit_1",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_VOUT_UV_FAULT_LIMIT, 1)
	},
	{
		.name = "vout_uv_warn_limit_0",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_VOUT_UV_WARN_LIMIT, 0)
	},
	{
		.name = "vout_uv_warn_limit_1",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_VOUT_UV_WARN_LIMIT, 1)
	},
	{
		.name = "iin_oc_warn_limit_0",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_IIN_OC_WARN_LIMIT, 0)
	},
	{
		.name = "iin_oc_warn_limit_1",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_IIN_OC_WARN_LIMIT, 1)
	},
	{
		.name = "iout_oc_warn_limit_0",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_IOUT_OC_WARN_LIMIT, 0)
	},
	{
		.name = "iout_oc_warn_limit_1",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_IOUT_OC_WARN_LIMIT, 1)
	},
	{
		.name = "ot_fault_limit",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_OT_FAULT_LIMIT,
					     LT7182S_CHAN_ALL)
	},
	{
		.name = "ot_warn_limit",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_OT_WARN_LIMIT,
					     LT7182S_CHAN_ALL)
	},
	{
		.name = "vin_uv_warn_limit",
		.show = lt7182s_iio_read_limits,
		.store = lt7182s_iio_write_limits,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_VIN_UV_WARN_LIMIT,
					     LT7182S_CHAN_ALL)
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute lt7182s_debug_attrs[] = {
	{
		.name = "status_vout_0",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_VOUT, 0)
	},
	{
		.name = "status_vout_1",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_VOUT, 1)
	},
	{
		.name = "status_iout_0",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_IOUT, 0)
	},
	{
		.name = "status_iout_1",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_IOUT, 1)
	},
	{
		.name = "status_input_0",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_INPUT, 0)
	},
	{
		.name = "status_input_1",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_INPUT, 1)
	},
	{
		.name = "status_mfr_specific_0",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_MFR_SPECIFIC, 0)
	},
	{
		.name = "status_mfr_specific_1",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_MFR_SPECIFIC, 1)
	},
	{
		.name = "status_word_0",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_WORD, 0)
	},
	{
		.name = "status_word_1",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_WORD, 1)
	},
	{
		.name = "status_temperature",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_TEMPERATURE,
					     LT7182S_CHAN_ALL)
	},
	{
		.name = "status_cml",
		.show = lt7182s_iio_read_status,
		.priv = LT7182S_IIO_REG_CHAN(LT7182S_STATUS_CML,
					     LT7182S_CHAN_ALL)
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel lt7182s_channels[] = {
	{
		.name = "vin0",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LT7182S_CHAN_0,
		.address = LT7182S_IIO_VIN_0_CHAN,
		.attributes = lt7182s_input_attrs,
		.ch_out = false
	},
	{
		.name = "vin1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LT7182S_CHAN_1,
		.address = LT7182S_IIO_VIN_1_CHAN,
		.attributes = lt7182s_input_attrs,
		.ch_out = false
	},
	{
		.name = "iin0",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LT7182S_CHAN_0,
		.address = LT7182S_IIO_IIN_0_CHAN,
		.attributes = lt7182s_input_attrs,
		.ch_out = false
	},
	{
		.name = "iin1",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LT7182S_CHAN_1,
		.address = LT7182S_IIO_IIN_1_CHAN,
		.attributes = lt7182s_input_attrs,
		.ch_out = false
	},
	{
		.name = "iout0",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LT7182S_CHAN_0,
		.address = LT7182S_IIO_IOUT_0_CHAN,
		.attributes = lt7182s_input_attrs,
		.ch_out = true
	},
	{
		.name = "iout1",
		.ch_type = IIO_CURRENT,
		.indexed = 1,
		.channel = LT7182S_CHAN_1,
		.address = LT7182S_IIO_IOUT_1_CHAN,
		.attributes = lt7182s_input_attrs,
		.ch_out = true
	},
	{
		.name = "vout0",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LT7182S_CHAN_0,
		.address = LT7182S_IIO_VOUT_0_CHAN,
		.attributes = lt7182s_output_attrs,
		.ch_out = true
	},
	{
		.name = "vout1",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = LT7182S_CHAN_1,
		.address = LT7182S_IIO_VOUT_1_CHAN,
		.attributes = lt7182s_output_attrs,
		.ch_out = true
	},
	{
		.name = "temperature",
		.ch_type = IIO_TEMP,
		.indexed = 1,
		.channel = LT7182S_CHAN_0,
		.address = LT7182S_IIO_TEMP_CHAN,
		.attributes = lt7182s_input_attrs,
		.ch_out = false,
	},
	{
		.name = "vcc",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = 2,
		.address = LT7182S_IIO_VCC_CHAN,
		.attributes = lt7182s_input_attrs,
		.ch_out = false,
	},
};

static struct iio_device lt7182s_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(lt7182s_channels),
	.channels = lt7182s_channels,
	.attributes = lt7182s_global_attrs,
	.debug_attributes = lt7182s_debug_attrs,
	.debug_reg_read = lt7182s_iio_reg_read,
	.debug_reg_write = lt7182s_iio_reg_write,
};
