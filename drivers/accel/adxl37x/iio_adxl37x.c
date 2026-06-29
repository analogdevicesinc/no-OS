/***************************************************************************//**
 *   @file   iio_adxl37x.c
 *   @brief  Implementation of IIO ADXL37x Driver.
 *   @author Raul Georgian (Raul.Georgian@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
static int adxl37x_iio_read_hpf_enable(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int adxl37x_iio_write_hpf_enable(void *dev, char *buf, uint32_t len,
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
		.shared = IIO_SHARED_BY_ALL,
		.show   = adxl37x_iio_read_lpf_freq,
		.store  = adxl37x_iio_write_lpf_freq,
	},
	{
		.name   = "filter_low_pass_3db_frequency_available",
		.shared = IIO_SHARED_BY_ALL,
		.show   = adxl37x_iio_read_lpf_freq_avail,
	},
	{
		.name   = "filter_high_pass_enable",
		.shared = IIO_SHARED_BY_ALL,
		.show   = adxl37x_iio_read_hpf_enable,
		.store  = adxl37x_iio_write_hpf_enable,
	},
	{
		.name = "raw",
		.show = adxl37x_iio_read_raw,
	},
	{
		.name   = "sampling_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.show   = adxl37x_iio_read_sampling_freq,
		.store  = adxl37x_iio_write_sampling_freq,
	},
	{
		.name   = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_ALL,
		.show   = adxl37x_iio_read_samp_freq_avail,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_ALL,
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
 *
 * @param dev     - The IIO device structure.
 * @param reg     - Address of the register to read.
 * @param readval - Where the register value is stored.
 * @return 0 on success, negative error code otherwise.
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
 *
 * @param dev      - The IIO device structure.
 * @param reg      - Address of the register to write.
 * @param writeval - Value to write into the register.
 * @return 0 on success, negative error code otherwise.
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
		val = no_os_sign_extend32((raw_buf[0] << 4) | (raw_buf[1] >> 4), 11);
		break;
	case ACCEL_AXIS_Y:
		val = no_os_sign_extend32((raw_buf[2] << 4) | (raw_buf[3] >> 4), 11);
		break;
	case ACCEL_AXIS_Z:
		val = no_os_sign_extend32((raw_buf[4] << 4) | (raw_buf[5] >> 4), 11);
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

}

/***************************************************************************//**
 * @brief Handles the read request for scale attribute.
 *        Scale is constant: 100 mg/LSB = 0.980665 m/s²/LSB for both chips.
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer where the formatted value is written.
 * @param len     - Maximum number of bytes that may be written to buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return Number of bytes written to buf, negative error code otherwise.
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
 *        Maps to OFFSET_X/Y/Z registers (0x20-0x22). Only bits [3:0] are
 *        implemented, holding a 4-bit twos complement offset; bits [7:4]
 *        are reserved.
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer where the formatted value is written.
 * @param len     - Maximum number of bytes that may be written to buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return Number of bytes written to buf, negative error code otherwise.
*******************************************************************************/
static int adxl37x_iio_read_calibbias(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	struct adxl37x_iio_dev* iio_adxl37x;
	struct adxl37x_dev* adxl37x;
	uint8_t raw_buf[3];
	int val;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev*) dev;
	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	ret = adxl37x->reg_read_multiple(adxl37x, ADXL37X_OFFSET_X, raw_buf, 3);
	if (ret)
		return ret;

	if (channel->type != IIO_ACCEL)
		return -EINVAL;

	/* Only the lower nibble holds the offset, which is a 4-bit signed
	 * value. The upper nibble of OFFSET_X/Y/Z is reserved.
	 */
	switch (channel->address) {
	case ACCEL_AXIS_X:
		val = no_os_sign_extend32(raw_buf[0] & 0xF, 3);
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ACCEL_AXIS_Y:
		val = no_os_sign_extend32(raw_buf[1] & 0xF, 3);
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case ACCEL_AXIS_Z:
		val = no_os_sign_extend32(raw_buf[2] & 0xF, 3);
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		pr_err("No axis found!\n\r");
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the write request for calibbias attribute.
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer holding the value to be written.
 * @param len     - Number of bytes available in buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return 0 on success, negative error code otherwise.
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
		if (ret < 0)
			return ret;

		if (calibbias < -8 || calibbias > 7) {
			pr_err("Offset value is a signed value on 4 bits.\r\n");
			return -EINVAL;
		}
		/* Only the lower nibble is needed. */
		calibbias = calibbias & 0xF;

		/* The device has to be in standby mode in order to be able to
		 * modify the offset, so obtain the current op mode first.
		 */
		ret = adxl37x_get_op_mode(adxl37x, &current_op_mode);
		if (ret)
			return ret;

		ret = adxl37x_set_op_mode(adxl37x, ADXL37X_STANDBY);
		if (ret)
			return ret;

		/* Set the offset to the requested axis. */
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

		/* Set the op_mode to the one used at the beginning. */
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
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer where the formatted value is written.
 * @param len     - Maximum number of bytes that may be written to buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return Number of bytes written to buf, negative error code otherwise.
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
 * @brief Handles the read request for filter_high_pass_enable attribute.
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer where the formatted value is written.
 * @param len     - Maximum number of bytes that may be written to buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return Number of bytes written to buf, negative error code otherwise.
*******************************************************************************/
static int adxl37x_iio_read_hpf_enable(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	uint8_t reg_data;
	int32_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	ret = adxl37x_read_reg(adxl37x, ADXL37X_POWER_CTL, &reg_data);
	if (ret)
		return ret;

	val = !(reg_data & ADXL37X_POWER_CTL_HPF_DIS_MSK);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/***************************************************************************//**
 * @brief Handles the write request for filter_high_pass_enable attribute.
 *
 * Accepts 0 (filter off) or 1 (filter on) and writes the inverted value into
 * the HPF_DISABLE bit.
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer holding the value to be written.
 * @param len     - Number of bytes available in buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return 0 on success, negative error code otherwise.
*******************************************************************************/
static int adxl37x_iio_write_hpf_enable(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	int32_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
	if (ret < 0)
		return ret;

	if (val != 0 && val != 1)
		return -EINVAL;

	return adxl37x_write_mask(adxl37x, ADXL37X_POWER_CTL,
				  ADXL37X_POWER_CTL_HPF_DIS_MSK,
				  ADXL37X_POWER_CTL_HPF_DIS_MODE(!val));
}

/***************************************************************************//**
 * @brief Handles the write request for sampling_frequency attribute.
 *        Searches the per-chip ODR table, calls adxl37x_set_odr on match.
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer holding the value to be written.
 * @param len     - Number of bytes available in buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return 0 on success, negative error code otherwise.
*******************************************************************************/
static int adxl37x_iio_write_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val, ret;
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	int index = -1;
	/* Used for multiplexing based on the adxl type. */
	const int32_t* used_ODR_table = NULL;
	uint32_t i;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	if (channel->type == IIO_ACCEL) {
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		if (adxl37x->adxl_type == ADXL371)
			used_ODR_table = adxl37x_iio_odr_table;
		else
			used_ODR_table = adxl372_iio_odr_table;

		/* Validate the data against the odr table. */
		for (i = 0; i < NO_OS_ARRAY_SIZE(adxl37x_iio_odr_table); i++) {
			if (used_ODR_table[i] == val) {
				index = i;
				break;
			}
		}

		/* The value doesn't exist. */
		if (index == -1)
			return -EINVAL;

		return adxl37x_set_odr(adxl37x, index);

	} else {
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for sampling_frequency_available attribute.
 *        Lists all valid ODR values for the active chip variant.
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer where the formatted value is written.
 * @param len     - Maximum number of bytes that may be written to buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return Number of bytes written to buf, negative error code otherwise.
*******************************************************************************/
static int adxl37x_iio_read_samp_freq_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val, ret;
	char buffer[50];
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	const int32_t* used_freq_table = NULL;
	uint8_t i;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;
	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	if (channel->type == IIO_ACCEL) {
		/* Remove existing data from the result buffer. */
		strcpy(buf, "");
		if (adxl37x->adxl_type == ADXL371) {
			used_freq_table = adxl37x_iio_odr_table;
		} else {
			used_freq_table = adxl372_iio_odr_table;
		}

		/* Iterate through the table and get each value in buf, then
		 * concatenate with the buffer used by IIO.
		 */
		for (i = 0; i < ODR_MODES; i++) {
			val = used_freq_table[i];
			ret = iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT, 1, &val);
			if (ret < 0)
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
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer where the formatted value is written.
 * @param len     - Maximum number of bytes that may be written to buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return Number of bytes written to buf, negative error code otherwise.
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
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer holding the value to be written.
 * @param len     - Number of bytes available in buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return 0 on success, negative error code otherwise.
*******************************************************************************/
static int adxl37x_iio_write_lpf_freq(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val, ret;
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	int index = -1;
	/* Used for multiplexing based on the adxl type. */
	const int32_t* used_lpf_table = NULL;
	uint32_t i;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	if (channel->type == IIO_ACCEL) {
		ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		if (ret < 0)
			return ret;

		if (adxl37x->adxl_type == ADXL371)
			used_lpf_table = adxl37x_iio_lpf_3db_multipliers;
		else
			used_lpf_table = adxl372_iio_lpf_3db_multipliers;

		/* Validate the data against the lpf table. */
		for (i = 0; i < LPF_FREQ; i++) {
			if (used_lpf_table[i] == val) {
				index = i;
				break;
			}
		}

		/* The value doesn't exist. */
		if (index == -1)
			return -EINVAL;

		return adxl37x_set_bandwidth(adxl37x, index);

	} else {
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for filter_low_pass_3db_frequency_available.
 *        Lists all valid BW values for the active chip variant.
 *
 * @param dev     - The IIO device structure.
 * @param buf     - Buffer where the formatted value is written.
 * @param len     - Maximum number of bytes that may be written to buf.
 * @param channel - Channel properties, NULL for device-scope attributes.
 * @param priv    - Private attribute data, unused.
 * @return Number of bytes written to buf, negative error code otherwise.
*******************************************************************************/
static int adxl37x_iio_read_lpf_freq_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val, ret;
	char buffer[50];
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	const int32_t* used_lpf_table = NULL;
	uint8_t i;

	if (!dev)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;
	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	if (channel->type == IIO_ACCEL) {
		/* Remove existing data from the result buffer. */
		strcpy(buf, "");
		if (adxl37x->adxl_type == ADXL371) {
			used_lpf_table = adxl37x_iio_lpf_3db_multipliers;
		} else {
			used_lpf_table = adxl372_iio_lpf_3db_multipliers;
		}

		/* Iterate through the table and get each value in buf, then
		 * concatenate with the buffer used by IIO.
		 */
		for (i = 0; i < LPF_FREQ; i++) {
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
 * @brief Maps the IIO active-channel mask to the matching chip FIFO format.
 *
 * The chip has no per-axis enable; FIFO_FORMAT names a whole combination, so
 * the mask has to be translated rather than written through. Only these 7
 * combinations exist; anything else has no FIFO format and is rejected.
 *
 * @param mask   - IIO active-channel mask, bit0 = X, bit1 = Y, bit2 = Z.
 * @param format - Where the matching chip FIFO format is stored.
 * @return 0 on success, -EINVAL if the mask has no FIFO format.
*******************************************************************************/
static int adxl37x_mask_to_fifo_format(uint32_t mask,
				       enum adxl37x_fifo_format *format)
{
	switch (mask & 0x7) {
	case 0x1: /* X       */
		*format = ADXL37X_X_FIFO;
		return 0;
	case 0x2: /* Y       */
		*format = ADXL37X_Y_FIFO;
		return 0;
	case 0x4: /* Z       */
		*format = ADXL37X_Z_FIFO;
		return 0;
	case 0x3: /* X+Y     */
		*format = ADXL37X_XY_FIFO;
		return 0;
	case 0x5: /* X+Z     */
		*format = ADXL37X_XZ_FIFO;
		return 0;
	case 0x6: /* Y+Z     */
		*format = ADXL37X_YZ_FIFO;
		return 0;
	case 0x7: /* X+Y+Z   */
		*format = ADXL37X_XYZ_FIFO;
		return 0;
	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Applies the active-channel selection to the device.
 *        Called by the framework at OPEN, before streaming starts (pre_enable).
 *
 * Caches the mask and axis count, derives the FIFO sample width and the
 * maximum number of sample sets the 512-entry FIFO can hold for that many
 * axes, then reprograms the chip FIFO to the matching format.
 *
 * @param dev  - The IIO device structure.
 * @param mask - Active-channel mask, bit0 = X, bit1 = Y, bit2 = Z.
 * @return 0 on success, negative error code otherwise.
*******************************************************************************/
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

	/* Translate the enabled-axis mask into the chip FIFO format. */
	ret = adxl37x_mask_to_fifo_format(mask, &format);
	if (ret)
		return ret;

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
 *
 * @param dev     - The IIO device structure.
 * @param buff    - Destination buffer, one int32 per enabled axis per sample.
 * @param samples - Number of sample sets to read.
 * @return Number of sample sets read, negative error code otherwise.
*******************************************************************************/
static int adxl37x_iio_read_samples(void *dev, int *buff, uint32_t samples)
{
	static struct adxl37x_xyz_accel_data data_buffer[ADXL37X_FIFO_MAX_SAMPLES];
	struct adxl37x_iio_dev *iio_adxl37x;
	struct adxl37x_dev *adxl37x;
	uint8_t status1;
	uint32_t i = 0;
	uint32_t s;
	int32_t ret;

	if (!dev)
		return -EINVAL;

	if (samples > ADXL37X_FIFO_MAX_SAMPLES)
		return -EINVAL;

	iio_adxl37x = (struct adxl37x_iio_dev *)dev;

	if (!iio_adxl37x->adxl37x_dev)
		return -EINVAL;

	adxl37x = iio_adxl37x->adxl37x_dev;

	ret = adxl37x_get_fifo_xyz_data_workaround(adxl37x, data_buffer, samples);
	if (ret) {
		pr_err("Error in getting the samples!\r\n");
		return ret;
	}

	/* The samples are already sign extended by the driver. */
	for (s = 0; s < samples; s++) {
		if (iio_adxl37x->active_channels & NO_OS_BIT(0))
			buff[i++] = data_buffer[s].x;
		if (iio_adxl37x->active_channels & NO_OS_BIT(1))
			buff[i++] = data_buffer[s].y;
		if (iio_adxl37x->active_channels & NO_OS_BIT(2))
			buff[i++] = data_buffer[s].z;
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
