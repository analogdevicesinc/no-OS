/***************************************************************************//**
 *   @file   iio_ltm4700.c
 *   @brief  Source file for the LTM4700 IIO Driver
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"

#include "ltm4700.h"
#include "iio_ltm4700.h"
#include "iio_types.h"

#define LTM4700_IIO_REG_CHAN(reg, chan)			(reg | (chan << 8))
#define LTM4700_IIO_REG(x)				(x & 0xFF)
#define LTM4700_IIO_CHAN(x)				((x >> 8) & 0xFF)

static const char *const ltm4700_operation_avail[] = {
	"off", "on", "margin_high", "margin_low"
};

enum ltm4700_iio_chan_type {
	LTM4700_IIO_VOUT_0_CHAN,
	LTM4700_IIO_IOUT_0_CHAN,
	LTM4700_IIO_VOUT_1_CHAN,
	LTM4700_IIO_IOUT_1_CHAN,
	LTM4700_IIO_VIN_CHAN,
	LTM4700_IIO_IIN_CHAN,
	LTM4700_IIO_TEMP_EXT_CHAN,
	LTM4700_IIO_TEMP_IC_CHAN,
	LTM4700_IIO_FREQ_CHAN,
	LTM4700_IIO_PIN_CHAN,
	LTM4700_IIO_POUT_0_CHAN,
	LTM4700_IIO_POUT_1_CHAN,
};

static struct iio_device ltm4700_iio_dev;

/**
 * @brief Read register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to read.
 * @param readval - Read value.
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
 */
#ifndef TEST
static
#endif
int32_t ltm4700_iio_reg_read(void *dev, uint32_t reg, uint32_t *readval)
{
	struct ltm4700_iio_desc *iio_ltm4700 = dev;
	struct ltm4700_dev *ltm4700 = iio_ltm4700->ltm4700_dev;
	uint8_t block[4];
	int ret;

	switch (reg) {
	case LTM4700_VOUT_COMMAND:
	case LTM4700_VOUT_MAX:
	case LTM4700_VOUT_MARGIN_HIGH:
	case LTM4700_VOUT_MARGIN_LOW:
	case LTM4700_VOUT_OV_FAULT_LIMIT:
	case LTM4700_VOUT_OV_WARN_LIMIT:
	case LTM4700_VOUT_UV_WARN_LIMIT:
	case LTM4700_VOUT_UV_FAULT_LIMIT:
	case LTM4700_IOUT_OC_FAULT_LIMIT:
	case LTM4700_IOUT_OC_WARN_LIMIT:
	case LTM4700_TON_DELAY:
	case LTM4700_TOFF_DELAY:
	case LTM4700_STATUS_WORD:
	case LTM4700_READ_VIN:
	case LTM4700_READ_IIN:
	case LTM4700_READ_VOUT:
	case LTM4700_READ_IOUT:
	case LTM4700_READ_TEMPERATURE_1:
	case LTM4700_READ_TEMPERATURE_2:
	case LTM4700_READ_FREQUENCY:
	case LTM4700_READ_POUT:
	case LTM4700_READ_PIN:
	case LTM4700_MFR_VOUT_MAX:
	case LTM4700_MFR_CHAN_CONFIG:
	case LTM4700_MFR_CONFIG_ALL:
	case LTM4700_MFR_PWM_COMP:
	case LTM4700_MFR_PWM_MODE:
	case LTM4700_MFR_FAULT_RESPONSE:
	case LTM4700_MFR_IOUT_PEAK:
	case LTM4700_MFR_ADC_CONTROL:
	case LTM4700_MFR_RETRY_DELAY:
	case LTM4700_MFR_RESTART_DELAY:
	case LTM4700_MFR_VOUT_PEAK:
	case LTM4700_MFR_VIN_PEAK:
	case LTM4700_MFR_TEMPERATURE_1_PEAK:
	case LTM4700_MFR_READ_IIN_PEAK:
	case LTM4700_MFR_READ_ICHIP:
	case LTM4700_MFR_PADS:
	case LTM4700_MFR_ADDRESS:
	case LTM4700_MFR_SPECIAL_ID:
	case LTM4700_MFR_IIN_CAL_GAIN:
	case LTM4700_MFR_PWM_CONFIG:
	case LTM4700_MFR_IOUT_CAL_GAIN_TC:
	case LTM4700_MFR_RVIN:
	case LTM4700_MFR_TEMP_1_GAIN:
	case LTM4700_MFR_TEMP_1_OFFSET:
	case LTM4700_MFR_RAIL_ADDRESS: {
		uint16_t word;
		ret = ltm4700_read_word(ltm4700, ltm4700->page, (uint8_t)reg, &word);
		if (ret)
			return ret;
		*readval = (uint32_t)word;
		return 0;
	}
	case LTM4700_MFR_ID:
	case LTM4700_MFR_MODEL:
		ret = ltm4700_read_block_data(ltm4700, ltm4700->page,
					      (uint8_t)reg, &block[0], 4);
		if (ret)
			return ret;

		*readval = (uint32_t)block[0] << 24 | (uint32_t)block[1] << 16 |
			   (uint32_t)block[2] << 8 | (uint32_t)block[3];

		return 0;
	default: {
		uint8_t byte;
		ret = ltm4700_read_byte(ltm4700, ltm4700->page, (uint8_t)reg, &byte);
		if (ret)
			return ret;
		*readval = (uint32_t)byte;
		return 0;
	}
	}
}

/**
 * @brief Write register value.
 * @param dev     - The iio device structure.
 * @param reg	  - Register to write.
 * @param writeval - Value to write.
 * @return ret    - Result of the writing procedure.
 */
#ifndef TEST
static
#endif
int32_t ltm4700_iio_reg_write(void *dev, uint32_t reg, uint32_t writeval)
{
	struct ltm4700_iio_desc *iio_ltm4700 = dev;
	struct ltm4700_dev *ltm4700 = iio_ltm4700->ltm4700_dev;

	switch (reg) {
	case LTM4700_CLEAR_FAULTS:
	case LTM4700_STORE_USER_ALL:
	case LTM4700_RESTORE_USER_ALL:
	case LTM4700_MFR_CLEAR_PEAKS:
	case LTM4700_MFR_FAULT_LOG_STORE:
	case LTM4700_MFR_FAULT_LOG_CLEAR:
	case LTM4700_MFR_COMPARE_USER_ALL:
	case LTM4700_MFR_RESET:
		return ltm4700_send_byte(ltm4700, ltm4700->page, (uint8_t)reg);
	case LTM4700_VOUT_COMMAND:
	case LTM4700_VOUT_MAX:
	case LTM4700_VOUT_MARGIN_HIGH:
	case LTM4700_VOUT_MARGIN_LOW:
	case LTM4700_VOUT_OV_FAULT_LIMIT:
	case LTM4700_VOUT_OV_WARN_LIMIT:
	case LTM4700_VOUT_UV_WARN_LIMIT:
	case LTM4700_VOUT_UV_FAULT_LIMIT:
	case LTM4700_IOUT_OC_FAULT_LIMIT:
	case LTM4700_IOUT_OC_WARN_LIMIT:
	case LTM4700_TON_DELAY:
	case LTM4700_TOFF_DELAY:
	case LTM4700_MFR_VOUT_MAX:
	case LTM4700_MFR_RETRY_DELAY:
	case LTM4700_MFR_RESTART_DELAY:
	case LTM4700_MFR_IIN_CAL_GAIN:
	case LTM4700_MFR_IOUT_CAL_GAIN_TC:
	case LTM4700_MFR_RVIN:
	case LTM4700_MFR_TEMP_1_GAIN:
	case LTM4700_MFR_TEMP_1_OFFSET:
		return ltm4700_write_word(ltm4700, ltm4700->page, (uint8_t)reg,
					  (uint16_t)writeval);
	default:
		return ltm4700_write_byte(ltm4700, ltm4700->page, (uint8_t)reg,
					  (uint8_t)writeval);
	}
}

/**
 * @brief Read raw attribute for a specific channel.
 * @param device - The iio device structure.
 * @param buf	 - Buffer to store the read data.
 * @param len	 - Buffer length.
 * @param channel - IIO channel.
 * @param priv   - IIO private data.
 * @return ret   - Result of the reading procedure.
 */
#ifndef TEST
static
#endif
int ltm4700_iio_read_raw(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel,
			 intptr_t priv)
{
	struct ltm4700_iio_desc *iio_desc = device;
	struct ltm4700_dev *dev = iio_desc->ltm4700_dev;
	int val, ret;

	switch (priv) {
	case LTM4700_IIO_VIN:
		ret = ltm4700_read_value(dev, 0, LTM4700_VIN, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case LTM4700_IIO_IIN:
		ret = ltm4700_read_value(dev, 0, LTM4700_IIN, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case LTM4700_IIO_VOUT:
		ret = ltm4700_read_value(dev, channel->ch_num, LTM4700_VOUT, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case LTM4700_IIO_IOUT:
		ret = ltm4700_read_value(dev, channel->ch_num, LTM4700_IOUT, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case LTM4700_IIO_TEMP_EXT:
		ret = ltm4700_read_value(dev, channel->ch_num, LTM4700_TEMP_EXT, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case LTM4700_IIO_TEMP_IC:
		ret = ltm4700_read_value(dev, 0, LTM4700_TEMP_IC, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case LTM4700_IIO_FREQ:
		ret = ltm4700_read_value(dev, channel->ch_num, LTM4700_FREQ, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case LTM4700_IIO_POUT:
		ret = ltm4700_read_value(dev, channel->ch_num, LTM4700_POUT, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	case LTM4700_IIO_PIN:
		ret = ltm4700_read_value(dev, 0, LTM4700_PIN, &val);
		if (ret)
			return ret;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

	default:
		return -EINVAL;
	}
}

/**
 * @brief Read scale attribute for a specific channel.
 * @param device - The iio device structure.
 * @param buf	 - Buffer to store the read data.
 * @param len	 - Buffer length.
 * @param channel - IIO channel.
 * @param priv   - IIO private data.
 * @return ret   - Result of the reading procedure.
 */
#ifndef TEST
static
#endif
int ltm4700_iio_read_scale(void *device, char *buf, uint32_t len,
			   const struct iio_ch_info *channel,
			   intptr_t priv)
{
	int32_t vals[2];

	/* All values are in milli-units, so scale is 0.001 (1/1000) */
	vals[0] = 1;
	vals[1] = (int)MILLI;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
}

/**
 * @brief Read offset attribute for a specific channel.
 * @param device - The iio device structure.
 * @param buf	 - Buffer to store the read data.
 * @param len	 - Buffer length.
 * @param channel - IIO channel.
 * @param priv   - IIO private data.
 * @return ret   - Result of the reading procedure.
 */
#ifndef TEST
static
#endif
int ltm4700_iio_read_offset(void *device, char *buf, uint32_t len,
			    const struct iio_ch_info *channel,
			    intptr_t priv)
{
	int val = 0;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read attribute for a specific channel.
 * @param device - The iio device structure.
 * @param buf	 - Buffer to store the read data.
 * @param len	 - Buffer length.
 * @param channel - IIO channel.
 * @param priv   - IIO private data.
 * @return ret   - Result of the reading procedure.
 */
#ifndef TEST
static
#endif
int ltm4700_iio_read_attr(void *device, char *buf, uint32_t len,
			  const struct iio_ch_info *channel,
			  intptr_t priv)
{
	struct ltm4700_iio_desc *iio_desc = device;
	struct ltm4700_dev *dev = iio_desc->ltm4700_dev;
	int val, ret;
	uint8_t readval;
	uint16_t special_id;

	switch (priv) {
	case LTM4700_IIO_VIN:
	case LTM4700_IIO_IIN:
	case LTM4700_IIO_VOUT:
	case LTM4700_IIO_IOUT:
	case LTM4700_IIO_TEMP_EXT:
	case LTM4700_IIO_TEMP_IC:
	case LTM4700_IIO_FREQ:
	case LTM4700_IIO_POUT:
	case LTM4700_IIO_PIN:
		return ltm4700_iio_read_raw(device, buf, len, channel, priv);

	case LTM4700_IIO_VOUT_COMMAND:
		ret = ltm4700_read_word_data(dev, channel->ch_num,
					     LTM4700_VOUT_COMMAND, &val);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d.%03d", val / 1000, val % 1000);

	case LTM4700_IIO_VOUT_MAX:
		ret = ltm4700_read_word_data(dev, channel->ch_num,
					     LTM4700_VOUT_MAX, &val);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d.%03d", val / 1000, val % 1000);

	case LTM4700_IIO_VOUT_MARGIN_HIGH:
		ret = ltm4700_read_word_data(dev, channel->ch_num,
					     LTM4700_VOUT_MARGIN_HIGH, &val);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d.%03d", val / 1000, val % 1000);

	case LTM4700_IIO_VOUT_MARGIN_LOW:
		ret = ltm4700_read_word_data(dev, channel->ch_num,
					     LTM4700_VOUT_MARGIN_LOW, &val);
		if (ret)
			return ret;
		return snprintf(buf, len, "%d.%03d", val / 1000, val % 1000);

	case LTM4700_IIO_OPERATION:
		ret = ltm4700_read_byte(dev, channel->ch_num,
					LTM4700_OPERATION, &readval);
		if (ret)
			return ret;

		switch (readval) {
		case LTM4700_OPERATION_OFF:
			return snprintf(buf, len, "off");
		case LTM4700_OPERATION_ON:
			return snprintf(buf, len, "on");
		case LTM4700_OPERATION_MARGIN_HIGH:
			return snprintf(buf, len, "margin_high");
		case LTM4700_OPERATION_MARGIN_LOW:
			return snprintf(buf, len, "margin_low");
		default:
			return snprintf(buf, len, "unknown");
		}

	case LTM4700_IIO_DEVICE_ID:
		ret = ltm4700_read_word(dev, 0, LTM4700_MFR_SPECIAL_ID, &special_id);
		if (ret)
			return ret;

		switch (special_id & LTM4700_ID_MSK) {
		case LTM4700_SPECIAL_ID_VALUE:
			return snprintf(buf, len, "LTM4700");
		default:
			return snprintf(buf, len, "Unknown (0x%04X)", special_id);
		}

	default:
		return -EINVAL;
	}
}

/**
 * @brief Parse voltage string to millivolts
 * @param buf - Input string (e.g., "3.300", "3.3", "3", "-1.25")
 * @param val_mv - Pointer to store parsed value in millivolts
 * @return 0 on success, -EINVAL on parse error
 */
static int ltm4700_parse_voltage_mv(const char *buf, int *val_mv)
{
	int int_part, frac_part;
	int sign = 1;
	const char *ptr = buf;

	if (!buf || !val_mv)
		return -EINVAL;

	if (*ptr == '-') {
		sign = -1;
		ptr++;
	}

	if (sscanf(ptr, "%d.%d", &int_part, &frac_part) == 2) {
		int frac_mv;

		if (frac_part < 10)
			frac_mv = frac_part * 100;
		else if (frac_part < 100)
			frac_mv = frac_part * 10;
		else
			frac_mv = frac_part;

		*val_mv = sign * (int_part * 1000 + frac_mv);
		return 0;
	}

	if (sscanf(ptr, "%d", &int_part) == 1) {
		*val_mv = sign * int_part * 1000;
		return 0;
	}

	return -EINVAL;
}

/**
 * @brief Write attribute for a specific channel.
 * @param device - The iio device structure.
 * @param buf	 - Buffer containing data to write.
 * @param len	 - Buffer length.
 * @param channel - IIO channel.
 * @param priv   - IIO private data.
 * @return ret   - Result of the writing procedure.
 */
#ifndef TEST
static
#endif
int ltm4700_iio_write_attr(void *device, const char *buf, uint32_t len,
			   const struct iio_ch_info *channel,
			   intptr_t priv)
{
	struct ltm4700_iio_desc *iio_desc = device;
	struct ltm4700_dev *dev = iio_desc->ltm4700_dev;
	int val;
	int ret;

	switch (priv) {
	case LTM4700_IIO_VOUT_COMMAND:
		ret = ltm4700_parse_voltage_mv(buf, &val);
		if (ret)
			return ret;
		return ltm4700_write_word_data(dev, channel->ch_num,
					       LTM4700_VOUT_COMMAND, val);

	case LTM4700_IIO_VOUT_MAX:
		ret = ltm4700_parse_voltage_mv(buf, &val);
		if (ret)
			return ret;
		return ltm4700_write_word_data(dev, channel->ch_num,
					       LTM4700_VOUT_MAX, val);

	case LTM4700_IIO_VOUT_MARGIN_HIGH:
		ret = ltm4700_parse_voltage_mv(buf, &val);
		if (ret)
			return ret;
		return ltm4700_write_word_data(dev, channel->ch_num,
					       LTM4700_VOUT_MARGIN_HIGH, val);

	case LTM4700_IIO_VOUT_MARGIN_LOW:
		ret = ltm4700_parse_voltage_mv(buf, &val);
		if (ret)
			return ret;
		return ltm4700_write_word_data(dev, channel->ch_num,
					       LTM4700_VOUT_MARGIN_LOW, val);

	case LTM4700_IIO_OPERATION:
		if (!strncmp(buf, "off", 3))
			val = LTM4700_OPERATION_OFF;
		else if (!strncmp(buf, "on", 2))
			val = LTM4700_OPERATION_ON;
		else if (!strncmp(buf, "margin_high", 11))
			val = LTM4700_OPERATION_MARGIN_HIGH;
		else if (!strncmp(buf, "margin_low", 10))
			val = LTM4700_OPERATION_MARGIN_LOW;
		else
			return -EINVAL;

		return ltm4700_write_byte(dev, channel->ch_num,
					  LTM4700_OPERATION, (uint8_t)val);

	case LTM4700_IIO_CLEAR_PEAKS:
		return ltm4700_clear_peaks(dev);

	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles global attributes reading for ltm4700 IIO device.
 * @param device - The iio device structure.
 * @param buf	 - Buffer to be filled with requested attr value.
 * @param len	 - Buffer length.
 * @param channel - IIO channel structure.
 * @param priv	 - Private descriptor.
 * @return ret   - Result of the reading procedure.
 */
#ifndef TEST
static
#endif
int ltm4700_iio_read_avail(void *device, char *buf, uint32_t len,
			   const struct iio_ch_info *channel,
			   intptr_t priv)
{
	switch (priv) {
	case LTM4700_IIO_OPERATION:
		return snprintf(buf, len, "%s %s %s %s",
				ltm4700_operation_avail[0],
				ltm4700_operation_avail[1],
				ltm4700_operation_avail[2],
				ltm4700_operation_avail[3]);
	default:
		return -EINVAL;
	}
}

/* LTM4700 IIO voltage input attributes */
static struct iio_attribute ltm4700_iio_voltage_input_attrs[] = {
	{
		.name = "raw",
		.priv = LTM4700_IIO_VIN,
		.show = ltm4700_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltm4700_iio_read_scale,
	},
	{
		.name = "offset",
		.show = ltm4700_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

/* LTM4700 IIO voltage output attributes */
static struct iio_attribute ltm4700_iio_voltage_output_attrs[] = {
	{
		.name = "raw",
		.priv = LTM4700_IIO_VOUT,
		.show = ltm4700_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltm4700_iio_read_scale,
	},
	{
		.name = "offset",
		.show = ltm4700_iio_read_offset,
	},
	{
		.name = "command",
		.priv = LTM4700_IIO_VOUT_COMMAND,
		.show = ltm4700_iio_read_attr,
		.store = ltm4700_iio_write_attr,
	},
	{
		.name = "max",
		.priv = LTM4700_IIO_VOUT_MAX,
		.show = ltm4700_iio_read_attr,
		.store = ltm4700_iio_write_attr,
	},
	{
		.name = "margin_high",
		.priv = LTM4700_IIO_VOUT_MARGIN_HIGH,
		.show = ltm4700_iio_read_attr,
		.store = ltm4700_iio_write_attr,
	},
	{
		.name = "margin_low",
		.priv = LTM4700_IIO_VOUT_MARGIN_LOW,
		.show = ltm4700_iio_read_attr,
		.store = ltm4700_iio_write_attr,
	},
	{
		.name = "operation",
		.priv = LTM4700_IIO_OPERATION,
		.show = ltm4700_iio_read_attr,
		.store = ltm4700_iio_write_attr,
	},
	{
		.name = "operation_available",
		.priv = LTM4700_IIO_OPERATION,
		.show = ltm4700_iio_read_avail,
	},
	END_ATTRIBUTES_ARRAY
};

/* LTM4700 IIO current input attributes */
static struct iio_attribute ltm4700_iio_current_input_attrs[] = {
	{
		.name = "raw",
		.priv = LTM4700_IIO_IIN,
		.show = ltm4700_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltm4700_iio_read_scale,
	},
	{
		.name = "offset",
		.show = ltm4700_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

/* LTM4700 IIO current output attributes */
static struct iio_attribute ltm4700_iio_current_output_attrs[] = {
	{
		.name = "raw",
		.priv = LTM4700_IIO_IOUT,
		.show = ltm4700_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltm4700_iio_read_scale,
	},
	{
		.name = "offset",
		.show = ltm4700_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

/* LTM4700 IIO power input attributes */
static struct iio_attribute ltm4700_iio_power_input_attrs[] = {
	{
		.name = "raw",
		.priv = LTM4700_IIO_PIN,
		.show = ltm4700_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltm4700_iio_read_scale,
	},
	{
		.name = "offset",
		.show = ltm4700_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

/* LTM4700 IIO power output attributes */
static struct iio_attribute ltm4700_iio_power_output_attrs[] = {
	{
		.name = "raw",
		.priv = LTM4700_IIO_POUT,
		.show = ltm4700_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltm4700_iio_read_scale,
	},
	{
		.name = "offset",
		.show = ltm4700_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

/* LTM4700 IIO temperature attributes */
static struct iio_attribute ltm4700_iio_temp_attrs[] = {
	{
		.name = "raw",
		.priv = LTM4700_IIO_TEMP_EXT,
		.show = ltm4700_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltm4700_iio_read_scale,
	},
	{
		.name = "offset",
		.show = ltm4700_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

/* LTM4700 IIO frequency attributes */
static struct iio_attribute ltm4700_iio_freq_attrs[] = {
	{
		.name = "raw",
		.priv = LTM4700_IIO_FREQ,
		.show = ltm4700_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ltm4700_iio_read_scale,
	},
	{
		.name = "offset",
		.show = ltm4700_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

/* LTM4700 IIO device attributes */
static struct iio_attribute ltm4700_iio_dev_attrs[] = {
	{
		.name = "clear_peaks",
		.priv = LTM4700_IIO_CLEAR_PEAKS,
		.store = ltm4700_iio_write_attr,
	},
	{
		.name = "device_id",
		.priv = LTM4700_IIO_DEVICE_ID,
		.show = ltm4700_iio_read_attr,
	},
	END_ATTRIBUTES_ARRAY
};

/* LTM4700 IIO channels */
static struct iio_channel ltm4700_channels[] = {
	{
		.name = "vin",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.scan_index = 0,
		.indexed = false,
		.ch_out = false,
		.attributes = ltm4700_iio_voltage_input_attrs,
	},
	{
		.name = "vout0",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.scan_index = 1,
		.indexed = true,
		.ch_out = true,
		.attributes = ltm4700_iio_voltage_output_attrs,
	},
	{
		.name = "vout1",
		.ch_type = IIO_VOLTAGE,
		.channel = 1,
		.scan_index = 2,
		.indexed = true,
		.ch_out = true,
		.attributes = ltm4700_iio_voltage_output_attrs,
	},
	{
		.name = "iin",
		.ch_type = IIO_CURRENT,
		.channel = 0,
		.scan_index = 3,
		.indexed = false,
		.ch_out = false,
		.attributes = ltm4700_iio_current_input_attrs,
	},
	{
		.name = "iout0",
		.ch_type = IIO_CURRENT,
		.channel = 0,
		.scan_index = 4,
		.indexed = true,
		.ch_out = true,
		.attributes = ltm4700_iio_current_output_attrs,
	},
	{
		.name = "iout1",
		.ch_type = IIO_CURRENT,
		.channel = 1,
		.scan_index = 5,
		.indexed = true,
		.ch_out = true,
		.attributes = ltm4700_iio_current_output_attrs,
	},
	{
		.name = "pin",
		.ch_type = IIO_POWER,
		.channel = 0,
		.scan_index = 6,
		.indexed = false,
		.ch_out = false,
		.attributes = ltm4700_iio_power_input_attrs,
	},
	{
		.name = "pout0",
		.ch_type = IIO_POWER,
		.channel = 0,
		.scan_index = 7,
		.indexed = true,
		.ch_out = true,
		.attributes = ltm4700_iio_power_output_attrs,
	},
	{
		.name = "pout1",
		.ch_type = IIO_POWER,
		.channel = 1,
		.scan_index = 8,
		.indexed = true,
		.ch_out = true,
		.attributes = ltm4700_iio_power_output_attrs,
	},
	{
		.name = "temp0",
		.ch_type = IIO_TEMP,
		.channel = 0,
		.scan_index = 9,
		.indexed = true,
		.ch_out = false,
		.attributes = ltm4700_iio_temp_attrs,
	},
	{
		.name = "temp1",
		.ch_type = IIO_TEMP,
		.channel = 1,
		.scan_index = 10,
		.indexed = true,
		.ch_out = false,
		.attributes = ltm4700_iio_temp_attrs,
	},
	{
		.name = "frequency0",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 0,
		.scan_index = 11,
		.indexed = true,
		.ch_out = false,
		.attributes = ltm4700_iio_freq_attrs,
	},
	{
		.name = "frequency1",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 1,
		.scan_index = 12,
		.indexed = true,
		.ch_out = false,
		.attributes = ltm4700_iio_freq_attrs,
	},
};

/* LTM4700 IIO device descriptor */
static struct iio_device ltm4700_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ltm4700_channels),
	.channels = ltm4700_channels,
	.attributes = ltm4700_iio_dev_attrs,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = NULL,
	.post_disable = NULL,
	.read_dev = NULL,
	.debug_reg_read = ltm4700_iio_reg_read,
	.debug_reg_write = ltm4700_iio_reg_write,
};

/**
 * @brief Initializes the LTM4700 IIO driver
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int ltm4700_iio_init(struct ltm4700_iio_desc **iio_desc,
		     struct ltm4700_iio_desc_init_param *init_param)
{
	struct ltm4700_iio_desc *descriptor;
	int ret;

	if (!iio_desc || !init_param || !init_param->ltm4700_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = ltm4700_init(&descriptor->ltm4700_dev, init_param->ltm4700_init_param);
	if (ret)
		goto error;

	descriptor->iio_dev = &ltm4700_iio_dev;

	*iio_desc = descriptor;

	return 0;

error:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - The iio device descriptor.
 * @return 0 in case of success, an error code otherwise.
 */
int ltm4700_iio_remove(struct ltm4700_iio_desc *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = ltm4700_remove(desc->ltm4700_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
