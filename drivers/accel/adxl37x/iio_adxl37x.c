/***************************************************************************//**
 *   @file   iio_adxl37x.c
 *   @brief  Implementation of IIO ADXL37x Driver.
 *   @author Raul Georgian (Raul.Georgian@analog.com)
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

#include <iio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_adxl37x.h"
#include "adxl37x.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

#define ACCEL_AXIS_X	0u
#define ACCEL_AXIS_Y	1u
#define ACCEL_AXIS_Z	2u
#define ODR_MODES 5
#define LPF_FREQ 5

/* Scale factor: 100 mg/LSB = 0.980665 m/s²/LSB */
#define ADXL37X_SCALE_MICRO	980665

/*
 * ODR tables (Hz), index matches enum adxl37x_odr (0..4).
 * ADXL371 internal clock is 80% of ADXL372, so ODR values differ per chip
 * even though the same 3-bit field in TIMING[7:5] is used.
 */
static const int32_t adxl37x_iio_odr_table[] = { 320, 640, 1280, 2560, 5120 };
static const int32_t adxl372_iio_odr_table[] = { 400, 800, 1600, 3200, 6400 };

/*
 * Low-pass filter (bandwidth) tables (Hz), index matches enum adxl37x_bandwidth (0..4).
 * Source: ADXL371 datasheet Table 14 / ADXL372 datasheet Table 24, MEASURE[2:0].
 */
static const int32_t adxl37x_iio_lpf_3db_multipliers[] = { 160, 320, 640, 1280, 2560 };
static const int32_t adxl372_iio_lpf_3db_multipliers[] = { 200, 400, 800, 1600, 3200 };

static int adxl37x_iio_read_reg(struct adxl37x_iio_dev *dev, uint32_t reg,
				uint32_t *readval);
static int adxl37x_iio_write_reg(struct adxl37x_iio_dev *dev, uint32_t reg,
				 uint32_t writeval);
static int adxl37x_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_read_calibbias(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_write_calibbias(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_write_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_read_samp_freq_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_read_lpf_freq(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_write_lpf_freq(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_read_lpf_freq_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_update_channels(void *dev, uint32_t mask);
static int adxl37x_iio_read_samples(void *dev, int *buff, uint32_t samples);

static struct iio_attribute adxl37x_iio_accel_attrs[] = {
	{
		.name  = "calibbias",
		.show  = adxl37x_iio_read_calibbias,
		.store = adxl37x_iio_write_calibbias,
	},
	{
		.name   = "filter_low_pass_3db_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl37x_iio_read_lpf_freq,
		.store  = adxl37x_iio_write_lpf_freq,
	},
	{
		.name   = "filter_low_pass_3db_frequency_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl37x_iio_read_lpf_freq_avail,
	},
	{
		.name = "raw",
		.show = adxl37x_iio_read_raw,
	},
	{
		.name   = "sampling_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl37x_iio_read_sampling_freq,
		.store  = adxl37x_iio_write_sampling_freq,
	},
	{
		.name   = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl37x_iio_read_samp_freq_avail,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl37x_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type adxl37x_iio_accel_scan_type = {
	.sign          = 's',
	.realbits      = 12,
	.storagebits   = 32,
	.shift         = 0,
	.is_big_endian = false
};

#define ADXL37X_ACCEL_CHANNEL(index, reg, axis) {	\
	.ch_type    = IIO_ACCEL,			\
	.channel    = index,				\
	.address    = reg,				\
	.modified   = true,				\
	.channel2   = IIO_MOD_##axis,			\
	.scan_type  = &adxl37x_iio_accel_scan_type,	\
	.scan_index = index,				\
	.attributes = adxl37x_iio_accel_attrs,		\
	.ch_out     = false				\
}

static struct iio_channel adxl37x_channels[] = {
	ADXL37X_ACCEL_CHANNEL(0, ACCEL_AXIS_X, X),
	ADXL37X_ACCEL_CHANNEL(1, ACCEL_AXIS_Y, Y),
	ADXL37X_ACCEL_CHANNEL(2, ACCEL_AXIS_Z, Z),
};

static struct iio_device adxl37x_iio_dev = {
	.num_ch          = NO_OS_ARRAY_SIZE(adxl37x_channels),
	.channels        = adxl37x_channels,
	.pre_enable      = (int32_t (*)())adxl37x_iio_update_channels,
	.trigger_handler = NULL,
	.read_dev        = (int32_t (*)())adxl37x_iio_read_samples,
	.debug_reg_read  = (int32_t (*)())adxl37x_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adxl37x_iio_write_reg
};

/***************************************************************************//**
 * @brief Wrapper for reading a device register.
*******************************************************************************/
static int adxl37x_iio_read_reg(struct adxl37x_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	uint8_t val;
	int ret;

	ret = adxl37x_read_reg(dev->adxl37x_dev, (uint8_t)reg, &val);
	if (ret)
		return ret;

	*readval = val;

	return 0;
}

/***************************************************************************//**
 * @brief Wrapper for writing a device register.
*******************************************************************************/
static int adxl37x_iio_write_reg(struct adxl37x_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	return adxl37x_write_reg(dev->adxl37x_dev, (uint8_t)reg,
				 (uint8_t)writeval);
}

/***************************************************************************//**
 * @brief Handles the read request for raw attribute.
*******************************************************************************/
static int adxl37x_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	uint8_t raw_buf[6];
	int32_t val;
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = dev;
	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;
	adxl37x = iio_adxl37x->adxl37x_dev;

	ret = adxl37x->reg_read_multiple(adxl37x, ADXL37X_X_DATA_H, raw_buf, 6);
	if (ret)
		return ret;

	switch (channel->address) {
	case ACCEL_AXIS_X:
		val = no_os_sign_extend32(((uint16_t)raw_buf[0] << 4) | (raw_buf[1] >> 4), 11);
		break;
	case ACCEL_AXIS_Y:
		val = no_os_sign_extend32(((uint16_t)raw_buf[2] << 4) | (raw_buf[3] >> 4), 11);
		break;
	case ACCEL_AXIS_Z:
		val = no_os_sign_extend32(((uint16_t)raw_buf[4] << 4) | (raw_buf[5] >> 4), 11);
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

}

/***************************************************************************//**
 * @brief Handles the read request for scale attribute.
 *        Scale is constant: 100 mg/LSB = 0.980665 m/s²/LSB for both chips.
*******************************************************************************/
static int adxl37x_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];

	if (channel->type == IIO_ACCEL) {
		vals[0] = 0;
		vals[1] = ADXL37X_SCALE_MICRO;
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	} else {
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for calibbias attribute.
 *        Maps to OFFSET_X/Y/Z registers (0x20-0x22), 8-bit signed.
*******************************************************************************/
static int adxl37x_iio_read_calibbias(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	int val;
	int ret;
	struct adxl37x_iio_dev* iio_adxl37x;
	struct adxl37x_dev* adxl37x;
	if (!dev)
		return -EINVAL;
	iio_adxl37x = (struct adxl37x_iio_dev*) dev;
	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;
	adxl37x = iio_adxl37x->adxl37x_dev;
	uint8_t raw_buf[3];
	ret = adxl37x->reg_read_multiple(adxl37x, ADXL37X_OFFSET_X, raw_buf, 3);

	if (ret)
		return ret;
	if (channel->type == IIO_ACCEL) {
		switch (channel->address) {
		case ACCEL_AXIS_X:
			val = no_os_sign_extend32(raw_buf[0] & 0xF, 3);
			return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		case ACCEL_AXIS_Y:
			val = no_os_sign_extend32(raw_buf[1] & 0xF,
						  3);			//use 0xF as mask to get only the lower nibble
			return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		case ACCEL_AXIS_Z:
			val = no_os_sign_extend32(raw_buf[2] & 0xF, 3);
			return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		default:
			pr_err("No axis found!\n\r");
			return -EINVAL;
		}

	} else {
		return -EINVAL;
	}

}

/***************************************************************************//**
 * @brief Handles the write request for calibbias attribute.
*******************************************************************************/
static int adxl37x_iio_write_calibbias(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t calibbias;
	int32_t ret;
	enum adxl37x_op_mode current_op_mode;
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	if (channel->type == IIO_ACCEL) {
		ret = iio_parse_value(buf, IIO_VAL_INT, &calibbias, NULL);
		if (ret != 0)
			return ret;

		if (calibbias < -8 || calibbias > 7) {
			pr_err("Offset value is a signed value on 4 bits.\r\n");
			return -EINVAL;
		}
		calibbias = calibbias & 0xF; //need only the lower nibble

		// The device has to be in standby mode in order to be able to
		// modify the offset.
		// Obtain the current op mode
		ret = adxl37x_get_op_mode(adxl37x, &current_op_mode);
		if (ret)
			return ret;

		ret = adxl37x_set_op_mode(adxl37x, ADXL37x_STANDBY);
		if (ret)
			return ret;

		//set the offset to the requested axis
		switch (channel->address) {
		case ACCEL_AXIS_X:
			ret = adxl37x_write_reg(adxl37x, ADXL37X_OFFSET_X, calibbias);
			break;
		case ACCEL_AXIS_Y:
			ret = adxl37x_write_reg(adxl37x, ADXL37X_OFFSET_Y, calibbias);
			break;
		case ACCEL_AXIS_Z:
			ret = adxl37x_write_reg(adxl37x, ADXL37X_OFFSET_Z, calibbias);
			break;

		default:
			ret = -EINVAL;
			break;
		}

		//set the op_mode to the one used at the beggining
		if (ret) {
			adxl37x_set_op_mode(adxl37x, current_op_mode);
			return ret;
		}

		return adxl37x_set_op_mode(adxl37x, current_op_mode);


	} else {
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for sampling_frequency attribute.
 *        Reads adxl37x_dev->odr and maps it through the per-chip ODR table.
*******************************************************************************/
static int adxl37x_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	if (channel->type == IIO_ACCEL) {
		val = (adxl37x->adxl_type == ADXL371) ? adxl37x_iio_odr_table[adxl37x->odr] :
		      adxl372_iio_odr_table[adxl37x->odr];
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	} else {
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the write request for sampling_frequency attribute.
 *        Searches the per-chip ODR table, calls adxl37x_set_odr on match.
*******************************************************************************/
static int adxl37x_iio_write_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val, ret;
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	int index = -1;
	const int32_t* used_ODR_table = NULL; //used for multiplexing based on adxl type

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	if (channel->type == IIO_ACCEL) {
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret != 0)
			return ret;

		if (adxl37x->adxl_type == ADXL371)
			used_ODR_table = adxl37x_iio_odr_table;
		else
			used_ODR_table = adxl372_iio_odr_table;

		for (int i = 0; i < NO_OS_ARRAY_SIZE(adxl37x_iio_odr_table); i++) {
			if (used_ODR_table[i] == val) {
				index = i; //validate the data with the odr table
				break;
			}
		}

		if (index == -1) //the value doesen't exist
			return -EINVAL;

		return adxl37x_set_odr(adxl37x, index);

	} else {
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for sampling_frequency_available attribute.
 *        Lists all valid ODR values for the active chip variant.
*******************************************************************************/
static int adxl37x_iio_read_samp_freq_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val, ret;
	char buffer[50];
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	const int32_t* used_freq_table = NULL;
	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;
	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;
	adxl37x = iio_adxl37x->adxl37x_dev;
	if (channel->type == IIO_ACCEL) {
		//remove existing data from result buffer
		strcpy(buf, "");
		if (adxl37x->adxl_type == ADXL371) {
			used_freq_table = adxl37x_iio_odr_table;
		} else {
			used_freq_table = adxl372_iio_odr_table;
		}

		//iterate trough the table and get each value in buf,then concatanate
		// with the buffer used by IIO
		for (uint8_t i = 0; i < ODR_MODES; i++) {
			val = used_freq_table[i];
			ret = iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT, 1, &val);
			if (ret != 0)
				return ret;

			strcat(buf, buffer);
			if (i < ODR_MODES - 1)
				strcat(buf, " ");
		}
		return strlen(buf);
	} else {
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for filter_low_pass_3db_frequency attribute.
 *        Reads adxl37x_dev->bw and maps it through the per-chip BW table.
*******************************************************************************/
static int adxl37x_iio_read_lpf_freq(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	if (channel->type == IIO_ACCEL) {
		val = (adxl37x->adxl_type == ADXL371) ?
		      adxl37x_iio_lpf_3db_multipliers[adxl37x->bw] :
		      adxl372_iio_lpf_3db_multipliers[adxl37x->bw];
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	} else {
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the write request for filter_low_pass_3db_frequency attribute.
 *        Searches the per-chip BW table, calls adxl37x_set_bandwidth on match.
*******************************************************************************/
static int adxl37x_iio_write_lpf_freq(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val, ret;
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	int index = -1;
	const int32_t* used_lpf_table = NULL; //used for multiplexing based on adxl type

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	if (channel->type == IIO_ACCEL) {
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret != 0)
			return ret;

		if (adxl37x->adxl_type == ADXL371)
			used_lpf_table = adxl37x_iio_lpf_3db_multipliers;
		else
			used_lpf_table = adxl372_iio_lpf_3db_multipliers;

		for (int i = 0; i < LPF_FREQ; i++) {
			if (used_lpf_table[i] == val) {
				index = i; //validate the data with the odr table
				break;
			}
		}

		if (index == -1) //the value doesen't exist
			return -EINVAL;

		return adxl37x_set_bandwidth(adxl37x, index);

	} else {
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for filter_low_pass_3db_frequency_available.
 *        Lists all valid BW values for the active chip variant.
*******************************************************************************/
static int adxl37x_iio_read_lpf_freq_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val, ret;
	char buffer[50];
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	const int32_t* used_lpf_table = NULL;
	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;
	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;
	adxl37x = iio_adxl37x->adxl37x_dev;
	if (channel->type == IIO_ACCEL) {
		//remove existing data from result buffer
		strcpy(buf, "");
		if (adxl37x->adxl_type == ADXL371) {
			used_lpf_table = adxl37x_iio_lpf_3db_multipliers;
		} else {
			used_lpf_table = adxl372_iio_lpf_3db_multipliers;
		}

		//iterate trough the table and get each value in buf,then concatanate
		// with the buffer used by IIO
		for (uint8_t i = 0; i < LPF_FREQ; i++) {
			val = used_lpf_table[i];
			ret = iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT, 1, &val);
			if (ret < 0)
				return ret;

			strcat(buf, buffer);
			if (i < LPF_FREQ - 1)
				strcat(buf, " ");
		}
		return strlen(buf);
	} else {
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Updates active_channels mask and no_of_active_channels count.
 *        Called by the framework before streaming starts (pre_enable).
*******************************************************************************/
/*
 * Map the IIO active-channel mask (bit0=X, bit1=Y, bit2=Z) to the matching
 * chip FIFO format. Only these 7 combinations are valid; anything else has no
 * FIFO format and is rejected.
 */
static int adxl37x_mask_to_fifo_format(uint32_t mask,
				       enum adxl37x_fifo_format *format)
{
	switch (mask & 0x7) {
	case 0x1: /* X       */
		*format = ADXL37x_X_FIFO;
		return 0;
	case 0x2: /* Y       */
		*format = ADXL37x_Y_FIFO;
		return 0;
	case 0x4: /* Z       */
		*format = ADXL37x_Z_FIFO;
		return 0;
	case 0x3: /* X+Y     */
		*format = ADXL37x_XY_FIFO;
		return 0;
	case 0x5: /* X+Z     */
		*format = ADXL37x_XZ_FIFO;
		return 0;
	case 0x6: /* Y+Z     */
		*format = ADXL37x_YZ_FIFO;
		return 0;
	case 0x7: /* X+Y+Z   */
		*format = ADXL37x_XYZ_FIFO;
		return 0;
	default:
		return -EINVAL;
	}
}

static int adxl37x_iio_update_channels(void *dev, uint32_t mask)
{
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	enum adxl37x_fifo_format format;
	enum adxl37x_op_mode op_mode;
	uint8_t counter = 0;
	uint16_t max_samples;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;
	adxl37x = iio_adxl37x->adxl37x_dev;
	if (!adxl37x)
		return -EINVAL;

	iio_adxl37x->active_channels = mask;

	if (mask & NO_OS_BIT(0))
		counter++;
	if (mask & NO_OS_BIT(1))
		counter++;
	if (mask & NO_OS_BIT(2))
		counter++;

	iio_adxl37x->no_of_active_channels = counter;

	if (!counter)
		return -EINVAL;

	/* Translate the enabled-axis mask into the chip FIFO format. */
	ret = adxl37x_mask_to_fifo_format(mask, &format);
	if (ret)
		return ret;

	/* Keep the device FIFO config coherent with the selected axes */
	adxl37x->fifo_config.bytes_per_sample = 2 * counter;

	switch (counter) {
	case 1:
		max_samples = 512;
		break;
	case 2:
		max_samples = 256;
		break;
	default:
		max_samples = 170;
		break;
	}
	adxl37x->fifo_config.extracted_samples = max_samples - 1;

	/* Preserve the current operating mode across the FIFO reconfig. */
	ret = adxl37x_get_op_mode(adxl37x, &op_mode);
	if (ret)
		return ret;

	/* Reprogram the chip FIFO to the new format (also sets fifo_parse). */
	return adxl37x_configure_fifo(adxl37x, adxl37x->fifo_config.fifo_mode,
				      format, adxl37x->fifo_config.fifo_samples,
				      op_mode);
}

/***************************************************************************//**
 * @brief Reads N samples in polling mode (read_dev path).
 *        Burst-reads X_DATA_H..Z_DATA_L for each sample, fills buff[].
*******************************************************************************/
static int adxl37x_iio_read_samples(void *dev, int *buff, uint32_t samples)
{
	static struct adxl37x_xyz_accel_data data_buffer[ADXL37X_FIFO_MAX_SAMPLES];
	if (samples > ADXL37X_FIFO_MAX_SAMPLES)
		samples = ADXL37X_FIFO_MAX_SAMPLES;
	uint8_t active_channels;
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	int32_t ret;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	int i = 0;
	ret = adxl37x_get_fifo_xyz_data_workaround(adxl37x, data_buffer, samples);
	if (ret) {
		pr_err("Error in getting the samples!\r\n");
		return ret;
	}

	for (int s = 0; s < samples; s++) {
		if (iio_adxl37x->active_channels & NO_OS_BIT(0))
			buff[i++] = no_os_sign_extend32(data_buffer[s].x, 11);
		if (iio_adxl37x->active_channels & NO_OS_BIT(1))
			buff[i++] = no_os_sign_extend32(data_buffer[s].y, 11);
		if (iio_adxl37x->active_channels & NO_OS_BIT(2))
			buff[i++] = no_os_sign_extend32(data_buffer[s].z, 11);
	}

	return samples;
}

/***************************************************************************//**
 * @brief Initializes the ADXL37x IIO driver.
 *
 * @param iio_dev    - The iio device structure.
 * @param init_param - Initialization parameters.
 *
 * @return 0 on success, negative error code otherwise.
*******************************************************************************/
int adxl37x_iio_init(struct adxl37x_iio_dev **iio_dev,
		     struct adxl37x_iio_dev_init_param *init_param)
{
	int ret;
	struct adxl37x_iio_dev *desc;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adxl37x_iio_dev;

	ret = adxl37x_init(&desc->adxl37x_dev, *(init_param->adxl37x_dev_init));
	if (ret)
		goto error_init;

	ret = adxl37x_set_op_mode(desc->adxl37x_dev,
				  init_param->adxl37x_dev_init->op_mode);
	if (ret)
		goto error_config;

	ret = adxl37x_set_internal_sync(desc->adxl37x_dev);
	if (ret)
		goto error_config;

	*iio_dev = desc;

	return 0;

error_config:
	adxl37x_remove(desc->adxl37x_dev);
error_init:
	no_os_free(desc);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adxl37x_iio_init().
 *
 * @param desc - The IIO device structure.
 *
 * @return 0 on success, negative error code otherwise.
*******************************************************************************/
int adxl37x_iio_remove(struct adxl37x_iio_dev *desc)
{
	int ret;

	ret = adxl37x_remove(desc->adxl37x_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
