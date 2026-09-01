/***************************************************************************//**
 *   @file   iio_ad5933.c
 *   @brief  Implementation of the AD5933 IIO driver.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include "iio_ad5933.h"
#include "ad5933.h"
#include "iio.h"
#include "no_os_alloc.h"
#include "no_os_util.h"
#include <string.h>

/* Temperature scale is 1/32 degC/LSB = 0.031250. */
#define AD5933_TEMP_SCALE_INT				0
#define AD5933_TEMP_SCALE_MICRO				31250

#define AD5933_INIT_EXCITATION_TIME_ms		100

#define DECIMAL_TO_MICROS(x)				((x) * 1e6)
#define MICROS_TO_DECIMAL(x)				((x) / 1e6)

/* Dummy value used to track sweep state in case of multiple submit calls.*/
#define AD5933_STATE_SWEEP_DONE 			0x0C

/** Per-channel selector carried through iio_channel.address. */
enum ad5933_iio_chan {
	AD5933_CH_REAL,
	AD5933_CH_IMAG,
	AD5933_CH_TEMP,
};

/** Device-attribute selector carried through iio_attribute.priv. */
enum ad5933_iio_attr_priv {
	AD5933_ATTR_TEMP_SCALE,
	AD5933_ATTR_PGA_GAIN,
	AD5933_ATTR_PGA_GAIN_AVAILABLE,
	AD5933_ATTR_OUTPUT_RANGE,
	AD5933_ATTR_OUTPUT_RANGE_AVAILABLE,
	AD5933_ATTR_START_FREQ,
	AD5933_ATTR_FREQ_INCREMENT,
	AD5933_ATTR_FREQ_POINTS,
	AD5933_ATTR_SETTLING_CYCLES,
	AD5933_ATTR_SWEEP_INITIALIZED,
	AD5933_ATTR_SWEEP_STARTED,
	AD5933_ATTR_CURRENT_OUTPUT_FREQ,
	AD5933_ATTR_REPEAT_MEASUREMENT,
	AD5933_ATTR_INCREMENTED_MEASUREMENT,
	AD5933_ATTR_CLK_FREQ,
	AD5933_ATTR_MIN_OUTPUT_FREQ,
	AD5933_ATTR_MAX_OUTPUT_FREQ,
};

static char* ad5933_pga_gain_available[] = {
	[AD5933_GAIN_X5] = "0.2",
	[AD5933_GAIN_X1] = "1",
};

static unsigned int ad5933_output_range_available[] = {
	[AD5933_RANGE_2000mVpp] = 1,
	[AD5933_RANGE_200mVpp] = 4,
	[AD5933_RANGE_400mVpp] = 3,
	[AD5933_RANGE_1000mVpp] = 2,
};

static int ad5933_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int ad5933_iio_read_attr(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv);
static int ad5933_iio_write_attr(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);
static int ad5933_iio_pre_enable(void *dev, uint32_t mask);
static int ad5933_iio_post_disable(void *dev);
static int ad5933_iio_submit(struct iio_device_data *dev_data);

static int ad5933_iio_reg_read(void *device, uint32_t reg_addr,
			       uint32_t *reg_data);
static int ad5933_iio_reg_write(void *device, uint32_t reg_addr,
				uint32_t reg_data);

#define AD5933_CH_RW_ATTR(_name, _priv) 	\
	{ 					\
		.name = _name, 			\
		.priv = _priv, 			\
		.show = ad5933_iio_read_attr, 	\
		.store = ad5933_iio_write_attr, 	\
	}

#define AD5933_CH_RO_ATTR(_name, _priv) 	\
	{ 					\
		.name = _name, 			\
		.priv = _priv, 			\
		.show = ad5933_iio_read_attr, 	\
	}

static struct iio_attribute ad5933_temp_attrs[] = {
	{ .name = "raw", .show = ad5933_iio_read_raw },
	AD5933_CH_RO_ATTR("scale", AD5933_ATTR_TEMP_SCALE),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad5933_voltage0_attrs[] = {
	{ .name = "raw", .show = ad5933_iio_read_raw },
	AD5933_CH_RW_ATTR("scale", AD5933_ATTR_PGA_GAIN),
	AD5933_CH_RO_ATTR("scale_available", AD5933_ATTR_PGA_GAIN_AVAILABLE),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad5933_voltage1_attrs[] = {
	{ .name = "raw", .show = ad5933_iio_read_raw },
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad5933_altvoltage0_attrs[] = {
	AD5933_CH_RW_ATTR("raw", AD5933_ATTR_OUTPUT_RANGE),
	AD5933_CH_RO_ATTR("scale_available", AD5933_ATTR_OUTPUT_RANGE_AVAILABLE),
	AD5933_CH_RW_ATTR("frequency_start", AD5933_ATTR_START_FREQ),
	AD5933_CH_RW_ATTR("frequency_increment", AD5933_ATTR_FREQ_INCREMENT),
	AD5933_CH_RW_ATTR("frequency_points", AD5933_ATTR_FREQ_POINTS),
	AD5933_CH_RW_ATTR("settling_cycles", AD5933_ATTR_SETTLING_CYCLES),
	END_ATTRIBUTES_ARRAY
};

static struct scan_type ad5933_sweep_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
};

static struct iio_channel ad5933_channels[] = {

	{
		.ch_type = IIO_VOLTAGE,
		.name = "real",
		.channel = 0,
		.indexed = true,
		.address = AD5933_REG_REAL_DATA,
		.attributes = ad5933_voltage0_attrs,
		.ch_out = false,
		.scan_index = 0,
		.scan_type = &ad5933_sweep_scan_type,
	},
	{
		.ch_type = IIO_VOLTAGE,
		.name = "imag",
		.channel = 1,
		.indexed = true,
		.address = AD5933_REG_IMAG_DATA,
		.attributes = ad5933_voltage1_attrs,
		.ch_out = false,
		.scan_index = 1,
		.scan_type = &ad5933_sweep_scan_type,
	},
	{
		.ch_type = IIO_TEMP,
		.channel = 0,
		.address = AD5933_REG_TEMP_DATA,
		.attributes = ad5933_temp_attrs,
		.ch_out = false,
		.scan_index = -1,
	},
	{
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 0,
		.indexed = true,
		.address = AD5933_REG_CONTROL_HB,
		.attributes = ad5933_altvoltage0_attrs,
		.ch_out = true,
		.scan_index = -1,
	},
};

static struct iio_attribute ad5933_iio_debug_attrs[] = {
	{
		.name = "sweep_initialized",
		.show = ad5933_iio_read_attr,
		.store = ad5933_iio_write_attr,
		.priv = AD5933_ATTR_SWEEP_INITIALIZED,
	},
	{
		.name = "sweep_started",
		.show = ad5933_iio_read_attr,
		.store = ad5933_iio_write_attr,
		.priv = AD5933_ATTR_SWEEP_STARTED,
	},
	{
		.name = "current_output_frequency",
		.show = ad5933_iio_read_attr,
		.priv = AD5933_ATTR_CURRENT_OUTPUT_FREQ,
	},
	{
		.name = "repeat_measurement",
		.store = ad5933_iio_write_attr,
		.show = ad5933_iio_read_attr,
		.priv = AD5933_ATTR_REPEAT_MEASUREMENT,
	},
	{
		.name = "incremented_measurement",
		.store = ad5933_iio_write_attr,
		.show = ad5933_iio_read_attr,
		.priv = AD5933_ATTR_INCREMENTED_MEASUREMENT,
	},
	{
		.name = "clock_frequency",
		.show = ad5933_iio_read_attr,
		.priv = AD5933_ATTR_CLK_FREQ,
	},
	{
		.name = "minimum_output_frequency",
		.show = ad5933_iio_read_attr,
		.priv = AD5933_ATTR_MIN_OUTPUT_FREQ,
	},
	{
		.name = "maximum_output_frequency",
		.show = ad5933_iio_read_attr,
		.priv = AD5933_ATTR_MAX_OUTPUT_FREQ,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device ad5933_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ad5933_channels),
	.channels = ad5933_channels,
	.debug_attributes = ad5933_iio_debug_attrs,
	.pre_enable = ad5933_iio_pre_enable,
	.post_disable = ad5933_iio_post_disable,
	.submit = ad5933_iio_submit,
	.debug_reg_read = ad5933_iio_reg_read,
	.debug_reg_write = ad5933_iio_reg_write,
};

/***************************************************************************//**
 * @brief Wrapper for reading AD5933 register.
 *
 * @param device  		- The iio device structure.
 * @param reg_addr	  	- Address of the register to be read from.
 * @param reg_data 		- Read data.
 *
 * @return ret    		- Result of the reading procedure.
*******************************************************************************/
static int ad5933_iio_reg_read(void *device, uint32_t reg_addr,
			       uint32_t *reg_data)
{
	struct ad5933_iio_dev *iio_ad5933 = device;
	int ret;
	uint8_t reg_val;

	if (!device || !reg_data)
		return -EINVAL;

	if (!reg_addr)
		return 0;

	ret = ad5933_reg_read(iio_ad5933->ad5933_dev, (uint8_t)reg_addr, &reg_val);

	if (ret)
		return ret;

	*reg_data = (uint32_t)reg_val;

	return 0;
}

/***************************************************************************//**
 * @brief Wrapper for writing to AD5933 register.
 *
 * @param device   		- The iio device structure.
 * @param reg_addr	   	- Address of the register to be written to.
 * @param reg_data 		- Data to be written.
 *
 * @return ret    		- Result of the writing procedure.
*******************************************************************************/
static int ad5933_iio_reg_write(void *device, uint32_t reg_addr,
				uint32_t reg_data)
{
	struct ad5933_iio_dev *iio_ad5933 = device;

	if (!device)
		return -EINVAL;

	return ad5933_reg_write(iio_ad5933->ad5933_dev, (uint8_t)reg_addr,
				(uint8_t)reg_data);
}

/***************************************************************************//**
 * @brief Handles the read request for raw attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int ad5933_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad5933_iio_dev *iio_ad5933;
	struct ad5933_dev *ad5933;
	int32_t raw;
	int32_t temp_raw;
	int16_t real;
	int16_t imag;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_ad5933 = dev;
	ad5933 = iio_ad5933->ad5933_dev;

	switch (channel->address) {
	case AD5933_REG_TEMP_DATA:
		ret = ad5933_get_raw_temperature(ad5933, &temp_raw);
		if (ret)
			return ret;
		raw = temp_raw;
		break;
	case AD5933_REG_REAL_DATA:
	case AD5933_REG_IMAG_DATA:
		ret = ad5933_get_current_data(ad5933, &real, &imag);
		if (ret)
			return ret;
		raw = (channel->address == AD5933_REG_REAL_DATA) ? real : imag;
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &raw);
}

/***************************************************************************//**
 * @brief Handles the read request for device attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int ad5933_iio_read_attr(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ad5933_iio_dev *iio_ad5933;
	struct ad5933_dev *ad5933;
	int32_t val;
	int32_t vals[2];
	double intval, decval;

	if (!dev)
		return -EINVAL;

	iio_ad5933 = dev;
	ad5933 = iio_ad5933->ad5933_dev;

	switch (priv) {
	case AD5933_ATTR_TEMP_SCALE:
		vals[0] = AD5933_TEMP_SCALE_INT;
		vals[1] = AD5933_TEMP_SCALE_MICRO;
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
					vals);
	case AD5933_ATTR_PGA_GAIN:
		val = ad5933->pga_gain;
		return snprintf(buf, len, "%s", ad5933_pga_gain_available[val]);
	case AD5933_ATTR_PGA_GAIN_AVAILABLE:
		return snprintf(buf, len, "%s %s", ad5933_pga_gain_available[1],
				ad5933_pga_gain_available[0]);
	case AD5933_ATTR_OUTPUT_RANGE:
		val = ad5933->output_range;
		return snprintf(buf, len, "%u", ad5933_output_range_available[val]);
	case AD5933_ATTR_OUTPUT_RANGE_AVAILABLE:
		return snprintf(buf, len, "%u %u %u %u", ad5933_output_range_available[0],
				ad5933_output_range_available[3], ad5933_output_range_available[2],
				ad5933_output_range_available[1]);
	case AD5933_ATTR_START_FREQ:
		decval = modf(ad5933->start_freq, &intval);
		vals[0] = intval;
		vals[1] = DECIMAL_TO_MICROS(decval);
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	case AD5933_ATTR_FREQ_INCREMENT:
		decval = modf(ad5933->freq_increment, &intval);
		vals[0] = intval;
		vals[1] = DECIMAL_TO_MICROS(decval);
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	case AD5933_ATTR_FREQ_POINTS:
		val = ad5933->num_increments;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_SETTLING_CYCLES:
		val = ad5933->settling_cycles;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_SWEEP_INITIALIZED:
		val = iio_ad5933->state == AD5933_FUNCTION_INIT_START_FREQ;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_SWEEP_STARTED:
		val = iio_ad5933->state == AD5933_FUNCTION_START_SWEEP;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_CURRENT_OUTPUT_FREQ:
		val = ad5933->current_output_freq;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_REPEAT_MEASUREMENT:
	case AD5933_ATTR_INCREMENTED_MEASUREMENT:
		val = ad5933->current_output_freq;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_CLK_FREQ:
		val = ad5933->clock_freq;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	case AD5933_ATTR_MIN_OUTPUT_FREQ:
		decval = modf(AD5933_MIN_OUTPUT_FREQ(ad5933->clock_freq), &intval);
		vals[0] = intval;
		vals[1] = DECIMAL_TO_MICROS(decval);
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	case AD5933_ATTR_MAX_OUTPUT_FREQ:
		decval = modf(AD5933_MAX_OUTPUT_FREQ(ad5933->clock_freq), &intval);
		vals[0] = intval;
		vals[1] = DECIMAL_TO_MICROS(decval);
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the write request for raw attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int ad5933_iio_write_attr(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad5933_iio_dev *iio_ad5933;
	struct ad5933_dev *ad5933;
	int32_t val;
	int32_t vals[2];
	int ret;

	if (!dev)
		return -EINVAL;

	iio_ad5933 = dev;
	ad5933 = iio_ad5933->ad5933_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	if (ret < 0)
		return ret;

	switch (priv) {
	case AD5933_ATTR_PGA_GAIN:
		for (int i = 0; i < NO_OS_ARRAY_SIZE(ad5933_pga_gain_available); i++) {
			if (!strcmp(buf, ad5933_pga_gain_available[i])) {
				ret = ad5933_set_gain(ad5933, i);
				if (ret)
					return ret;
				return len;
			}
		}
		return -EINVAL;
	case AD5933_ATTR_OUTPUT_RANGE:
		for (int i = 0; i < NO_OS_ARRAY_SIZE(ad5933_output_range_available); i++) {
			if (val == ad5933_output_range_available[i]) {
				ret = ad5933_set_range(ad5933, i);
				if (ret)
					return ret;
				return len;
			}
		}
		return -EINVAL;
	case AD5933_ATTR_START_FREQ:
		ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &vals[0], &vals[1]);
		if (ret < 0) {
			/* Input has no decimal point; treat it as a plain integer. */
			vals[0] = val;
			vals[1] = 0;
		}

		ret = ad5933_config_sweep(ad5933,
					  (double) vals[0] + (double) MICROS_TO_DECIMAL(vals[1]),
					  ad5933->freq_increment,
					  ad5933->num_increments);
		if (ret)
			return ret;
		return len;
	case AD5933_ATTR_FREQ_INCREMENT:
		ret = iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &vals[0], &vals[1]);
		if (ret < 0) {
			/* Input has no decimal point; treat it as a plain integer. */
			vals[0] = val;
			vals[1] = 0;
		}

		ret = ad5933_config_sweep(ad5933, ad5933->start_freq,
					  (double) vals[0] + (double) MICROS_TO_DECIMAL(vals[1]),
					  ad5933->num_increments);
		if (ret)
			return ret;
		return len;
	case AD5933_ATTR_FREQ_POINTS:
		ret = ad5933_config_sweep(ad5933, ad5933->start_freq,
					  ad5933->freq_increment,
					  val);
		if (ret)
			return ret;
		return len;
	case AD5933_ATTR_SETTLING_CYCLES:
		ret = ad5933_set_settling_time(ad5933, val);
		if (ret)
			return ret;
		return len;
	case AD5933_ATTR_SWEEP_INITIALIZED:
		if (val) {
			ret = ad5933_initialize_sweep(ad5933);
			if (ret)
				return ret;
			iio_ad5933->state = AD5933_FUNCTION_INIT_START_FREQ;
			iio_ad5933->current_output_freq = ad5933->start_freq;
		}
		return len;
	case AD5933_ATTR_SWEEP_STARTED:
		if (val) {
			ret = ad5933_start_sweep(ad5933);
			if (ret)
				return ret;
			iio_ad5933->state = AD5933_FUNCTION_START_SWEEP;
		}
		return len;
	case AD5933_ATTR_REPEAT_MEASUREMENT:
		if (val) {
			if (iio_ad5933->state != AD5933_FUNCTION_START_SWEEP)
				return -EINVAL;
			ret = ad5933_repeat_freq(ad5933);
			if (ret)
				return ret;
			ret = ad5933_wait_status(ad5933, AD5933_STAT_DATA_VALID, NULL);
			if (ret)
				return ret;
		}
		return len;
	case AD5933_ATTR_INCREMENTED_MEASUREMENT:
		if (val) {
			if (iio_ad5933->state != AD5933_FUNCTION_START_SWEEP)
				return -EINVAL;
			ret = ad5933_increment_freq(ad5933);
			if (ret)
				return ret;
			ret = ad5933_wait_status(ad5933, AD5933_STAT_DATA_VALID, NULL);
			if (ret)
				return ret;
		}
		return len;
	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Buffer pre-enable: record the active scan-channel mask.
 *
 * This pre-enable also initializes the datasheet sweep sequence, resets the
 * channel_data and waits for 100ms before sending the START_SWEEP command
 * to AD5933 which will start the excitation of the unknown impedance.
 * No measurements will be done yet at the end of this function.
 *
 * @param dev  - The AD5933 IIO device descriptor.
 * @param mask - Active-channel mask (bit position == channel array index).
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int ad5933_iio_pre_enable(void *dev, uint32_t mask)
{
	struct ad5933_iio_dev *iio_ad5933 = dev;
	int ret;

	if (!iio_ad5933)
		return -EINVAL;


	iio_ad5933->active_channels = mask;
	iio_ad5933->no_of_active_channels = no_os_hweight32(mask);

	ret = ad5933_initialize_sweep(iio_ad5933->ad5933_dev);

	if (ret)
		return ret;

	memset(iio_ad5933->channel_data, 0, sizeof(iio_ad5933->channel_data));

	no_os_mdelay(AD5933_INIT_EXCITATION_TIME_ms);

	ret = ad5933_start_sweep(iio_ad5933->ad5933_dev);
	if (ret)
		return ret;

	iio_ad5933->state = AD5933_FUNCTION_START_SWEEP;

	return 0;
}

/***************************************************************************//**
 * @brief Buffer post-disable: sends the POWER_DOWN command to the AD5933.
 *
 * @param dev_data - The IIO device data structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int ad5933_iio_post_disable(void *dev)
{
	struct ad5933_iio_dev *iio_ad5933 = dev;
	int ret;

	ret = ad5933_power_down(iio_ad5933->ad5933_dev);
	if (ret)
		return ret;

	iio_ad5933->state = AD5933_FUNCTION_POWER_DOWN;

	return 0;
}

/***************************************************************************//**
 * @brief Buffer submit: copy the collected sweep points into the ring.
 *
 * For each configured sweep point, the corresponding output frequency excites
 * the unknown impedance. After the configured settling cycles, the DATA_VALID
 * register is set and the real and imaginary components can be retrieved from
 * the AD5933. If the requested number of samples is greater than the number
 * of valid points collected, excess sample points are padded with INT16_MAX.
 *
 * @param dev_data - The IIO device data structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
static int ad5933_iio_submit(struct iio_device_data *dev_data)
{
	struct ad5933_iio_dev *iio_ad5933;
	struct ad5933_dev *ad5933;
	struct iio_buffer *buffer;
	uint32_t mask;
	uint32_t points;
	uint32_t max_samples;
	uint32_t i;
	int16_t scan[3];
	uint8_t status;
	int32_t sweep_done;
	int ret;

	if (!dev_data)
		return -EINVAL;

	iio_ad5933 = dev_data->dev;
	ad5933 = iio_ad5933->ad5933_dev;
	buffer = dev_data->buffer;
	mask = buffer->active_mask;
	points = buffer->samples;
	max_samples = ad5933->num_increments + 1;

	for (i = 0; i <= ad5933->num_increments; i++) {

		/* skip remeasuring on subsequent calls to submit. */
		if (iio_ad5933->state == AD5933_STATE_SWEEP_DONE)
			break;

		ret = ad5933_wait_status(ad5933, AD5933_STAT_DATA_VALID, &status);
		if (ret)
			return ret;

		ret = ad5933_get_current_data(iio_ad5933->ad5933_dev,
					      &iio_ad5933->channel_data[i * 2],
					      &iio_ad5933->channel_data[i * 2 + 1]);
		if (ret)
			return ret;

		ret = ad5933_increment_freq(ad5933);
		if (ret)
			return ret;
	}

	iio_ad5933->state = AD5933_STATE_SWEEP_DONE;

	for (i = 0; i < points; i++) {
		uint8_t k = 0;

		if (mask & NO_OS_BIT(AD5933_CH_REAL))
			scan[k++] = i < max_samples ? iio_ad5933->channel_data[i * 2] : INT16_MAX;
		if (mask & NO_OS_BIT(AD5933_CH_IMAG))
			scan[k++] = i < max_samples ? iio_ad5933->channel_data[i * 2 + 1] : INT16_MAX;
		ret = iio_buffer_push_scan(buffer, scan);
		if (ret)
			return ret;
	}

	return 0;
}

/***************************************************************************//**
 * @brief Initialize the AD5933 IIO driver.
 *
 * @param iio_dev    - The AD5933 IIO device descriptor.
 * @param init_param - The AD5933 IIO device initialization parameters.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_iio_init(struct ad5933_iio_dev **iio_dev,
		    struct ad5933_iio_dev_init_param *init_param)
{
	struct ad5933_iio_dev *desc;
	int ret;

	if (!iio_dev || !init_param || !init_param->ad5933_dev_ip)
		return -EINVAL;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &ad5933_iio_dev;

	ret = ad5933_init(&desc->ad5933_dev, init_param->ad5933_dev_ip);

	if (ret) {
		goto error_desc;
	}

	ret = ad5933_config_sweep(desc->ad5933_dev, init_param->start_freq,
				  init_param->freq_increment, init_param->freq_points);

	if (ret)
		goto error_dev;

	desc->state = AD5933_FUNCTION_POWER_DOWN;

	*iio_dev = desc;

	return 0;

error_dev:
	ad5933_remove(desc->ad5933_dev);
error_desc:
	no_os_free(desc);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad5933_iio_init().
 *
 * @param desc - The AD5933 IIO device descriptor.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ad5933_iio_remove(struct ad5933_iio_dev *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = ad5933_remove(desc->ad5933_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
