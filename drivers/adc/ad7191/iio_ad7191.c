/***************************************************************************//**
 *   @file   iio_ad7191.c
 *   @brief  Implementation of AD7191 IIO driver.
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ad7191.h"
#include "iio.h"
#include "iio_ad7191.h"
#include "no_os_error.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
/*
 * Temperature transfer function (see datasheet):
 *   T[degC] = (code - AD7191_MIDSCALE) / AD7191_TEMP_CODES_PER_DEGREE - 273
 * Rewritten as the IIO processed form (raw + offset) * scale, in degrees C:
 *   scale  = 1 / AD7191_TEMP_CODES_PER_DEGREE
 *   offset = -(AD7191_MIDSCALE + 273 * AD7191_TEMP_CODES_PER_DEGREE)
 */
#define AD7191_TEMP_OFFSET	(-(AD7191_MIDSCALE + \
				   273 * AD7191_TEMP_CODES_PER_DEGREE))
/* Voltage channels use offset-binary coding: midscale maps to 0 V. */
#define AD7191_VOLTAGE_OFFSET	(-AD7191_MIDSCALE)

/******************************************************************************/
/************************ Variable Definitions ********************************/
/******************************************************************************/
/* IIO scan index -> input channel selection. Order matches ad7191_channels. */
static const enum ad7191_channel iio_ad7191_ch_map[] = {
	AD7191_CH_AIN1_AIN2,
	AD7191_CH_AIN3_AIN4,
	AD7191_CH_TEMP,
};

/* Scan-index bitmask of channels enabled for the current buffer capture. */
static uint32_t iio_ad7191_active_mask;

/* AD7191 output is 24-bit offset binary, transmitted MSB first. */
static struct scan_type ad7191_iio_scan_type = {
	.sign = 'u',
	.realbits = AD7191_RESOLUTION,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = true,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/**
 * @brief Compute the voltage scale in nano-units (1e-9 mV/LSB).
 * @param vref_mv - Reference voltage in millivolts.
 * @param gain - PGA gain value.
 * @return Scale expressed in nano millivolts per LSB.
 */
static uint64_t ad7191_compute_scale_nano(uint32_t vref_mv, uint32_t gain)
{
	/* scale = vref_mv / (gain * 2^23); expressed in nano-units. */
	return (uint64_t)vref_mv * 1000000000ULL /
	       ((uint64_t)gain << (AD7191_RESOLUTION - 1));
}

/**
 * @brief Read the raw conversion result for a channel.
 */
static int ad7191_iio_read_raw(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad7191_dev *dev = device;
	uint32_t data;
	int ret;

	ret = ad7191_single_conversion(dev, (enum ad7191_channel)channel->address,
				       &data);
	if (ret)
		return ret;

	return snprintf(buf, len, "%"PRIu32"", data);
}

/**
 * @brief Read the scale for a voltage channel.
 */
static int ad7191_iio_read_scale(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad7191_dev *dev = device;
	uint64_t nano;

	nano = ad7191_compute_scale_nano(dev->vref_mv,
					 ad7191_gain_table[dev->gain]);

	return snprintf(buf, len, "%"PRIu64".%09"PRIu64"",
			(uint64_t)(nano / 1000000000ULL),
			(uint64_t)(nano % 1000000000ULL));
}

/**
 * @brief Read the list of available voltage scales (one per PGA gain).
 */
static int ad7191_iio_read_scale_available(void *device, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad7191_dev *dev = device;
	int n = 0;
	int w;
	uint8_t i;

	for (i = 0; i < AD7191_PGA_GAIN_NR; i++) {
		uint64_t nano = ad7191_compute_scale_nano(dev->vref_mv,
				ad7191_gain_table[i]);

		w = snprintf(buf + n, len - n, "%"PRIu64".%09"PRIu64"",
			     (uint64_t)(nano / 1000000000ULL),
			     (uint64_t)(nano % 1000000000ULL));
		if (w < 0)
			return -EINVAL;
		if ((uint32_t)w >= len - (uint32_t)n)
			break;
		n += w;
		if (i < AD7191_PGA_GAIN_NR - 1) {
			if ((uint32_t)n + 1 >= len)
				break;
			buf[n++] = ' ';
		}
	}

	return n;
}

/**
 * @brief Write the voltage scale by selecting the closest PGA gain.
 */
static int ad7191_iio_write_scale(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	struct ad7191_dev *dev = device;
	uint64_t input_nano, nano, best_diff, diff;
	enum ad7191_pga_gain best_gain = AD7191_PGA_GAIN_1;
	uint8_t i;
	int ret;

	/* Parse a decimal like "0.000298023" into nano-units. */
	{
		char *endptr;
		uint64_t int_part, frac_part = 0;
		int frac_digits = 0;

		int_part = (uint64_t)strtoul(buf, &endptr, 10);
		if (endptr == buf)
			return -EINVAL;
		if (*endptr == '.') {
			const char *frac_start = endptr + 1;

			frac_part = (uint64_t)strtoul(frac_start, &endptr, 10);
			frac_digits = (int)(endptr - frac_start);
		}
		for (i = frac_digits; i < 9; i++)
			frac_part *= 10;
		for (i = 9; i < frac_digits; i++)
			frac_part /= 10;
		input_nano = int_part * 1000000000ULL + frac_part;
	}

	best_diff = UINT64_MAX;
	for (i = 0; i < AD7191_PGA_GAIN_NR; i++) {
		nano = ad7191_compute_scale_nano(dev->vref_mv,
						 ad7191_gain_table[i]);
		diff = (nano > input_nano) ? (nano - input_nano) :
		       (input_nano - nano);
		if (diff < best_diff) {
			best_diff = diff;
			best_gain = (enum ad7191_pga_gain)i;
		}
	}

	ret = ad7191_set_gain(dev, best_gain);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Read the offset for a channel.
 */
static int ad7191_iio_read_offset(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv)
{
	if (channel->type == IIO_TEMP)
		return snprintf(buf, len, "%d", AD7191_TEMP_OFFSET);

	return snprintf(buf, len, "%d", AD7191_VOLTAGE_OFFSET);
}

/**
 * @brief Read the temperature scale (1 / codes-per-degree, in degrees C/LSB).
 */
static int ad7191_iio_read_temp_scale(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	uint64_t nano = 1000000000ULL / AD7191_TEMP_CODES_PER_DEGREE;

	return snprintf(buf, len, "%"PRIu64".%09"PRIu64"",
			(uint64_t)(nano / 1000000000ULL),
			(uint64_t)(nano % 1000000000ULL));
}

/**
 * @brief Read the sampling frequency.
 */
static int ad7191_iio_read_samp_freq(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad7191_dev *dev = device;

	return snprintf(buf, len, "%"PRIu32"", ad7191_odr_table[dev->odr]);
}

/**
 * @brief Read the list of available sampling frequencies.
 */
static int ad7191_iio_read_samp_freq_available(void *device, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int n = 0;
	int w;
	uint8_t i;

	for (i = 0; i < AD7191_ODR_NR; i++) {
		w = snprintf(buf + n, len - n, "%"PRIu32"", ad7191_odr_table[i]);
		if (w < 0)
			return -EINVAL;
		if ((uint32_t)w >= len - (uint32_t)n)
			break;
		n += w;
		if (i < AD7191_ODR_NR - 1) {
			if ((uint32_t)n + 1 >= len)
				break;
			buf[n++] = ' ';
		}
	}

	return n;
}

/**
 * @brief Write the sampling frequency by matching one of the available rates.
 */
static int ad7191_iio_write_samp_freq(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad7191_dev *dev = device;
	uint32_t freq;
	uint8_t i;
	int ret;

	freq = (uint32_t)strtoul(buf, NULL, 10);

	for (i = 0; i < AD7191_ODR_NR; i++) {
		if (freq != ad7191_odr_table[i])
			continue;

		ret = ad7191_set_odr(dev, (enum ad7191_odr)i);
		if (ret)
			return ret;

		return len;
	}

	return -EINVAL;
}

/**
 * @brief Store the active-channel mask before a buffer capture.
 */
static int ad7191_iio_update_channels(void *device, uint32_t mask)
{
	iio_ad7191_active_mask = mask;

	return 0;
}

/**
 * @brief Read nb_samples samples from all active channels into buff.
 *
 * The AD7191 is register-less and converts a single channel selected on its
 * CHAN/TEMP pins, so each active channel is sampled in turn with a full
 * single-conversion cycle (configure, power up, discard settling, read).
 */
static int ad7191_iio_read_samples(void *device, void *buff,
				   uint32_t nb_samples)
{
	struct ad7191_dev *dev = device;
	int32_t *samples = buff;
	uint32_t data;
	uint32_t k, ch;
	int i = 0;
	int ret;

	for (k = 0; k < nb_samples; k++) {
		for (ch = 0; ch < AD7191_CH_NR; ch++) {
			if (!(iio_ad7191_active_mask & NO_OS_BIT(ch)))
				continue;

			ret = ad7191_single_conversion(dev, iio_ad7191_ch_map[ch],
						       &data);
			if (ret)
				return ret;

			samples[i++] = (int32_t)data;
		}
	}

	return nb_samples;
}

static struct iio_attribute ad7191_voltage_attributes[] = {
	{ .name = "raw", .show = ad7191_iio_read_raw },
	{ .name = "scale", .show = ad7191_iio_read_scale, .store = ad7191_iio_write_scale },
	{ .name = "scale_available", .show = ad7191_iio_read_scale_available },
	{ .name = "offset", .show = ad7191_iio_read_offset },
	{
		.name = "sampling_frequency",
		.show = ad7191_iio_read_samp_freq,
		.store = ad7191_iio_write_samp_freq
	},
	{
		.name = "sampling_frequency_available",
		.show = ad7191_iio_read_samp_freq_available
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad7191_temp_attributes[] = {
	{ .name = "raw", .show = ad7191_iio_read_raw },
	{ .name = "scale", .show = ad7191_iio_read_temp_scale },
	{ .name = "offset", .show = ad7191_iio_read_offset },
	{
		.name = "sampling_frequency",
		.show = ad7191_iio_read_samp_freq,
		.store = ad7191_iio_write_samp_freq
	},
	{
		.name = "sampling_frequency_available",
		.show = ad7191_iio_read_samp_freq_available
	},
	END_ATTRIBUTES_ARRAY
};

#define AD7191_IIO_VOLTAGE_CHAN(nm, ch1, ch2, addr, sidx) \
	{ \
		.name = nm, \
		.ch_type = IIO_VOLTAGE, \
		.channel = ch1, \
		.channel2 = ch2, \
		.address = addr, \
		.scan_index = sidx, \
		.scan_type = &ad7191_iio_scan_type, \
		.attributes = ad7191_voltage_attributes, \
		.ch_out = 0, \
		.indexed = 1, \
		.diferential = true, \
	}

static struct iio_channel ad7191_channels[] = {
	AD7191_IIO_VOLTAGE_CHAN("voltage0-voltage1", 0, 1, AD7191_CH_AIN1_AIN2, 0),
	AD7191_IIO_VOLTAGE_CHAN("voltage2-voltage3", 2, 3, AD7191_CH_AIN3_AIN4, 1),
	{
		.name = NULL,
		.ch_type = IIO_TEMP,
		.channel = 0,
		.address = AD7191_CH_TEMP,
		.scan_index = 2,
		.scan_type = &ad7191_iio_scan_type,
		.attributes = ad7191_temp_attributes,
		.ch_out = 0,
		.indexed = 1,
		.diferential = false,
	},
};

struct iio_device iio_ad7191_device = {
	.num_ch = NO_OS_ARRAY_SIZE(ad7191_channels),
	.channels = ad7191_channels,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = ad7191_iio_update_channels,
	.read_dev = ad7191_iio_read_samples,
};
