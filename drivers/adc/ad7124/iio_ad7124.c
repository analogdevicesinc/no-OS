/***************************************************************************//**
 *   @file   iio_ad7124.c
 *   @brief  Implementation of iio_ad7124.c.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2020, 2026(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "no_os_error.h"
#include "iio.h"
#include "iio_ad7124.h"
#include "no_os_util.h"
#include "ad7124.h"

/*
 * IIO scan index → hardware channel register mapping.
 * Voltage channels use every other hw channel register (0,2,4,...,14)
 * to match the board's AINP/AINM differential pairs.
 * Index 8 is the temperature channel mapped to hw channel 15.
 */
static const uint8_t iio_to_hw_ch[] = { 0, 2, 4, 6, 8, 10, 12, 14, 15 };

#define AD7124_IIO_NUM_CH	9	/* 8 voltage + 1 temp */

/*
 * Calibration mode state per voltage channel (0 = zero_scale, 1 = full_scale).
 * Indexed by channel->address / 2 (address is hw ch 0,2,4,...,14 → index 0-7).
 * The temp channel (hw ch 15) has no sys_calibration attribute, so index 7
 * is never aliased by the temp channel address (15/2 = 7).
 */
static uint8_t calib_mode[8];

/* IIO scan-index bitmask of channels enabled for the current buffer capture.
 * Set by iio_ad7124_update_active_channels, consumed by iio_ad7124_read_samples.
 * Avoids re-reading hw channel registers (which can trigger SPI_IGNORE_ERR)
 * inside the hot read path. */
static uint32_t iio_active_mask;

/* Filter type name table, ordered by enum ad7124_filter_type */
static const char * const ad7124_filter_names[] = {
	[AD7124_FILTER_TYPE_SINC3]       = "sinc3",
	[AD7124_FILTER_TYPE_SINC3_PF1]   = "sinc3_pf1",
	[AD7124_FILTER_TYPE_SINC3_PF2]   = "sinc3_pf2",
	[AD7124_FILTER_TYPE_SINC3_PF3]   = "sinc3_pf3",
	[AD7124_FILTER_TYPE_SINC3_PF4]   = "sinc3_pf4",
	[AD7124_FILTER_TYPE_SINC3_REJ60] = "sinc3_rej60",
	[AD7124_FILTER_TYPE_SINC3_SINC1] = "sinc3_sinc1",
	[AD7124_FILTER_TYPE_SINC4]       = "sinc4",
	[AD7124_FILTER_TYPE_SINC4_REJ60] = "sinc4_rej60",
	[AD7124_FILTER_TYPE_SINC4_SINC1] = "sinc4_sinc1",
};

/* -------------------------------------------------------------------------
 * Scan type: AD7124 output is offset binary (unsigned).
 * -------------------------------------------------------------------------
 */
struct scan_type ad7124_iio_scan_type = {
	.sign        = 'u',
	.realbits    = 24,
	.storagebits = 32,
	.shift       = 0,
	.is_big_endian = true,
};

/* -------------------------------------------------------------------------
 * Helper: iterate over set bits in ch_mask starting after last_idx.
 * -------------------------------------------------------------------------
 */
static bool get_next_ch_idx(uint32_t ch_mask, uint32_t last_idx,
			    uint32_t *new_idx)
{
	last_idx++;
	if (last_idx >= 32)
		return 0;
	ch_mask >>= last_idx;
	if (!ch_mask) {
		*new_idx = -1;
		return 0;
	}
	while (!(ch_mask & 1)) {
		last_idx++;
		ch_mask >>= 1;
	}
	*new_idx = last_idx;

	return 1;
}

/* =========================================================================
 * Voltage channel attributes
 * =========================================================================
 */

/**
 * @brief Read raw ADC value for a voltage channel.
 */
static int ad7124_iio_read_raw_chan(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	int32_t value;
	int32_t ret;

	ret = ad7124_set_channel_status(desc, hw_ch, true);
	if (ret)
		return ret;

	ret = ad7124_set_adc_mode(desc, AD7124_SINGLE);
	if (ret)
		goto disable;

	ret = ad7124_wait_for_conv_ready(desc, 10000);
	if (ret)
		goto disable;

	ret = ad7124_read_data(desc, &value);
	if (ret)
		goto disable;

	ret = snprintf(buf, len, "%"PRIu32"", (uint32_t)value);

disable:
	ad7124_set_adc_mode(desc, AD7124_IDLE);
	ad7124_set_channel_status(desc, hw_ch, false);
	return ret;
}

/**
 * @brief Read scale (LSB voltage in mV) for a voltage channel.
 *        bipolar:  vref_mv / 2^(23 + pga_bits)
 *        unipolar: vref_mv / 2^(24 + pga_bits)
 */
static int ad7124_iio_read_scale_chan(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	uint32_t vref_mv;
	uint8_t pga_bits;
	bool bipolar;
	float scale;
	int32_t ret;

	ret = ad7124_get_vref_mv(desc, hw_ch, &vref_mv);
	if (ret)
		return ret;

	ret = ad7124_get_pga(desc, hw_ch, &pga_bits);
	if (ret)
		return ret;

	ret = ad7124_get_bipolar(desc, hw_ch, &bipolar);
	if (ret)
		return ret;

	if (bipolar)
		scale = (float)vref_mv / (float)(1u << (23 + pga_bits));
	else
		scale = (float)vref_mv / (float)(1u << (24 + pga_bits));

	return snprintf(buf, len, "%f", scale);
}

/**
 * @brief Write scale for a voltage channel: derive PGA from desired LSB value.
 */
static int ad7124_iio_write_scale_chan(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	uint8_t setup_id;
	uint32_t vref_mv;
	bool bipolar;
	float new_scale, lsb_gain1;
	uint32_t new_gain;
	uint8_t new_pga;
	uint32_t best_diff, diff;
	uint32_t i;
	int32_t ret;

	ret = ad7124_get_ch_setup(desc, hw_ch, &setup_id);
	if (ret)
		return ret;

	ret = ad7124_get_vref_mv(desc, hw_ch, &vref_mv);
	if (ret)
		return ret;

	ret = ad7124_get_bipolar(desc, hw_ch, &bipolar);
	if (ret)
		return ret;

	sscanf(buf, "%f", &new_scale);

	/* LSB at gain=1 */
	if (bipolar)
		lsb_gain1 = (float)vref_mv / (float)(1u << 23);
	else
		lsb_gain1 = (float)vref_mv / (float)(1u << 24);

	/* Desired gain = lsb_gain1 / new_scale */
	new_gain = (uint32_t)(lsb_gain1 / new_scale + 0.5f);

	/* Find closest power-of-2 gain (PGA bits 0-7, gain 1-128) */
	new_pga = 0;
	best_diff = 0xFFFFFFFF;
	for (i = 0; i < 8; i++) {
		diff = (uint32_t)abs((int)(1u << i) - (int)new_gain);
		if (diff < best_diff) {
			best_diff = diff;
			new_pga = (uint8_t)i;
		}
	}

	return ad7124_set_pga(desc, (enum ad7124_pga)new_pga, setup_id);
}

/**
 * @brief Read all available scale values (one per PGA gain setting).
 */
static int ad7124_iio_read_scale_available(void *device, char *buf,
					   uint32_t len,
					   const struct iio_ch_info *channel,
					   intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	uint32_t vref_mv;
	bool bipolar;
	float scale;
	int32_t ret;
	int n = 0;
	uint8_t i;

	ret = ad7124_get_vref_mv(desc, hw_ch, &vref_mv);
	if (ret)
		return ret;

	ret = ad7124_get_bipolar(desc, hw_ch, &bipolar);
	if (ret)
		return ret;

	for (i = 0; i < 8; i++) {
		if (bipolar)
			scale = (float)vref_mv / (float)(1u << (23 + i));
		else
			scale = (float)vref_mv / (float)(1u << (24 + i));
		n += snprintf(buf + n, len - n, "%f", scale);
		if (i < 7 && (uint32_t)n < len)
			buf[n++] = ' ';
	}

	return n;
}

/**
 * @brief Read offset for a voltage channel.
 *        bipolar → -(1<<23), unipolar → 0
 */
static int ad7124_iio_read_offset_chan(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	bool bipolar;
	int32_t ret;

	ret = ad7124_get_bipolar(desc, hw_ch, &bipolar);
	if (ret)
		return ret;

	if (bipolar)
		return snprintf(buf, len, "%"PRId32"", -(1 << 23));
	else
		return snprintf(buf, len, "0");
}

/**
 * @brief Read sampling frequency (ODR) for a voltage channel.
 */
static int ad7124_iio_read_odr_chan(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	int32_t odr_hz;

	odr_hz = ad7124_get_odr_ch(desc, hw_ch);
	if (odr_hz < 0)
		return odr_hz;

	return snprintf(buf, len, "%"PRId32"", odr_hz);
}

/**
 * @brief Write sampling frequency (ODR) for a voltage channel.
 */
static int ad7124_iio_write_odr_chan(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	int32_t new_odr_hz;
	int32_t ret;

	new_odr_hz = (int32_t)strtol(buf, NULL, 10);

	ret = ad7124_set_odr_ch(desc, new_odr_hz, hw_ch);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Read filter 3dB cutoff frequency for a voltage channel.
 *        f_3db = factor * odr / 1000
 */
static int ad7124_iio_read_filter_3db(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	uint8_t setup_id;
	enum ad7124_filter_type ftype;
	uint32_t factor;
	int32_t odr_hz;
	int32_t ret;

	ret = ad7124_get_ch_setup(desc, hw_ch, &setup_id);
	if (ret)
		return ret;

	ret = ad7124_get_filter_type(desc, setup_id, &ftype);
	if (ret)
		return ret;

	factor = ad7124_get_3db_filter_factor(ftype);

	odr_hz = ad7124_get_odr_ch(desc, hw_ch);
	if (odr_hz < 0)
		return odr_hz;

	return snprintf(buf, len, "%"PRIu32"", factor * (uint32_t)odr_hz / 1000);
}

/**
 * @brief Read filter type for a voltage channel.
 */
static int ad7124_iio_read_filter_type(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	uint8_t setup_id;
	enum ad7124_filter_type ftype;
	int32_t ret;

	ret = ad7124_get_ch_setup(desc, hw_ch, &setup_id);
	if (ret)
		return ret;

	ret = ad7124_get_filter_type(desc, setup_id, &ftype);
	if (ret)
		return ret;

	return snprintf(buf, len, "%s", ad7124_filter_names[ftype]);
}

/**
 * @brief Write filter type for a voltage channel.
 */
static int ad7124_iio_write_filter_type(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	uint8_t setup_id;
	enum ad7124_filter_type ftype;
	uint8_t i;
	int32_t ret;

	ret = ad7124_get_ch_setup(desc, hw_ch, &setup_id);
	if (ret)
		return ret;

	for (i = 0; i < AD7124_FILTER_TYPE_NUM; i++) {
		if (strcmp(buf, ad7124_filter_names[i]) == 0) {
			ftype = (enum ad7124_filter_type)i;
			ret = ad7124_set_filter_type(desc, setup_id, ftype);
			if (ret)
				return ret;
			return len;
		}
	}

	return -EINVAL;
}

/**
 * @brief Read all available filter type names.
 */
static int ad7124_iio_read_filter_type_available(void *device, char *buf,
						  uint32_t len,
						  const struct iio_ch_info *channel,
						  intptr_t priv)
{
	int n = 0;
	uint8_t i;

	for (i = 0; i < AD7124_FILTER_TYPE_NUM; i++) {
		n += snprintf(buf + n, len - n, "%s", ad7124_filter_names[i]);
		if (i < AD7124_FILTER_TYPE_NUM - 1 && (uint32_t)n < len)
			buf[n++] = ' ';
	}

	return n;
}

/**
 * @brief Read the current calibration mode for a voltage channel.
 */
static int ad7124_iio_read_calib_mode(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	/* address is hw ch 0,2,...,14; divide by 2 to get index 0-7 */
	uint8_t iio_idx = (uint8_t)(channel->address / 2);

	if (calib_mode[iio_idx])
		return snprintf(buf, len, "full_scale");
	else
		return snprintf(buf, len, "zero_scale");
}

/**
 * @brief Write (set) the calibration mode for a voltage channel.
 */
static int ad7124_iio_write_calib_mode(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	uint8_t iio_idx = (uint8_t)(channel->address / 2);

	if (strcmp(buf, "full_scale") == 0)
		calib_mode[iio_idx] = 1;
	else if (strcmp(buf, "zero_scale") == 0)
		calib_mode[iio_idx] = 0;
	else
		return -EINVAL;

	return len;
}

/**
 * @brief Read available calibration modes.
 */
static int ad7124_iio_read_calib_mode_available(void *device, char *buf,
						 uint32_t len,
						 const struct iio_ch_info *channel,
						 intptr_t priv)
{
	return snprintf(buf, len, "zero_scale full_scale");
}

/**
 * @brief Trigger system calibration for a voltage channel.
 */
static int ad7124_iio_write_calibration(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	uint8_t hw_ch = (uint8_t)channel->address;
	uint8_t iio_idx = hw_ch / 2;  /* hw ch 0,2,...,14 → index 0-7 */
	bool full_scale;
	int32_t ret;

	full_scale = (calib_mode[iio_idx] == 1);

	ret = ad7124_do_calibration(desc, hw_ch, full_scale);
	if (ret)
		return ret;

	return len;
}

static struct iio_attribute voltage_channel_attributes[] = {
	{
		.name = "raw",
		.show = ad7124_iio_read_raw_chan,
		.store = NULL,
	},
	{
		.name = "scale",
		.show = ad7124_iio_read_scale_chan,
		.store = ad7124_iio_write_scale_chan,
	},
	{
		.name = "scale_available",
		.show = ad7124_iio_read_scale_available,
		.store = NULL,
	},
	{
		.name = "offset",
		.show = ad7124_iio_read_offset_chan,
		.store = NULL,
	},
	{
		.name = "sampling_frequency",
		.show = ad7124_iio_read_odr_chan,
		.store = ad7124_iio_write_odr_chan,
	},
	{
		.name = "filter_low_pass_3db_frequency",
		.show = ad7124_iio_read_filter_3db,
		.store = NULL,
	},
	{
		.name = "filter_type",
		.show = ad7124_iio_read_filter_type,
		.store = ad7124_iio_write_filter_type,
	},
	{
		.name = "filter_type_available",
		.show = ad7124_iio_read_filter_type_available,
		.store = NULL,
	},
	{
		.name = "sys_calibration",
		.show = NULL,
		.store = ad7124_iio_write_calibration,
	},
	{
		.name = "sys_calibration_mode",
		.show = ad7124_iio_read_calib_mode,
		.store = ad7124_iio_write_calib_mode,
	},
	{
		.name = "sys_calibration_mode_available",
		.show = ad7124_iio_read_calib_mode_available,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY
};

/* =========================================================================
 * Temperature channel attributes
 * =========================================================================
 */

/**
 * @brief Read raw value from the internal temperature sensor.
 */
static int ad7124_iio_read_temp_raw(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	int32_t val;
	int32_t ret;

	ret = ad7124_read_temp_raw(desc, &val);
	if (ret)
		return ret;

	return snprintf(buf, len, "%"PRId32"", val);
}

/**
 * @brief Read temperature scale (hardcoded: ≈125/1698 °C/LSB = 0.073242).
 */
static int ad7124_iio_read_temp_scale(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	return snprintf(buf, len, "0.073242");
}

/**
 * @brief Read temperature offset (hardcoded: -12090248 LSB ≈ -25°C offset).
 */
static int ad7124_iio_read_temp_offset(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	return snprintf(buf, len, "-12090248");
}

/**
 * @brief Read sampling frequency for the temperature channel (hw ch 15).
 */
static int ad7124_iio_read_temp_odr(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	int32_t odr_hz;

	/* hw channel 15 */
	odr_hz = ad7124_get_odr_ch(desc, 15);
	if (odr_hz < 0)
		return odr_hz;

	return snprintf(buf, len, "%"PRId32"", odr_hz);
}

/**
 * @brief Write sampling frequency for the temperature channel (hw ch 15).
 */
static int ad7124_iio_write_temp_odr(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)device;
	int32_t new_odr_hz;
	int32_t ret;

	new_odr_hz = (int32_t)strtol(buf, NULL, 10);

	ret = ad7124_set_odr_ch(desc, new_odr_hz, 15);
	if (ret)
		return ret;

	return len;
}

static struct iio_attribute temp_channel_attributes[] = {
	{
		.name = "raw",
		.show = ad7124_iio_read_temp_raw,
		.store = NULL,
	},
	{
		.name = "scale",
		.show = ad7124_iio_read_temp_scale,
		.store = NULL,
	},
	{
		.name = "offset",
		.show = ad7124_iio_read_temp_offset,
		.store = NULL,
	},
	{
		.name = "sampling_frequency",
		.show = ad7124_iio_read_temp_odr,
		.store = ad7124_iio_write_temp_odr,
	},
	END_ATTRIBUTES_ARRAY
};

/* =========================================================================
 * Channel definitions
 * =========================================================================
 */

/*
 * Voltage channels: hw channels 0-7 mapped 1:1 to IIO scan indices 0-7.
 * Each hw channel is configured as a differential pair (AINP, AINM) in
 * hardware. The address field stores the hw channel number.
 */
#define AD7124_IIO_CHANN_DEF(nm, ch1, ch2, hw_ch, sidx) \
	{ \
		.name = nm, \
		.ch_type = IIO_VOLTAGE, \
		.channel = ch1, \
		.channel2 = ch2, \
		.address = hw_ch, \
		.scan_index = sidx, \
		.scan_type = &ad7124_iio_scan_type, \
		.attributes = voltage_channel_attributes, \
		.ch_out = 0, \
		.indexed = 1, \
		.diferential = true, \
	}

static struct iio_channel ad7124_channels[] = {
	AD7124_IIO_CHANN_DEF("voltage0-voltage1",    0,  1,  0, 0),
	AD7124_IIO_CHANN_DEF("voltage2-voltage3",    2,  3,  2, 1),
	AD7124_IIO_CHANN_DEF("voltage4-voltage5",    4,  5,  4, 2),
	AD7124_IIO_CHANN_DEF("voltage6-voltage7",    6,  7,  6, 3),
	AD7124_IIO_CHANN_DEF("voltage8-voltage9",    8,  9,  8, 4),
	AD7124_IIO_CHANN_DEF("voltage10-voltage11", 10, 11, 10, 5),
	AD7124_IIO_CHANN_DEF("voltage12-voltage13", 12, 13, 12, 6),
	AD7124_IIO_CHANN_DEF("voltage14-voltage15", 14, 15, 14, 7),
	/* Temperature channel: hw ch 15, scan index 8 */
	{
		.name = NULL,
		.ch_type = IIO_TEMP,
		.channel = 0,
		.address = 15,
		.scan_index = 8,
		.scan_type = &ad7124_iio_scan_type,
		.attributes = temp_channel_attributes,
		.ch_out = 0,
		.indexed = 1,
		.diferential = false,
	},
};

/* =========================================================================
 * Buffer operations
 * =========================================================================
 */

/**
 * @brief Enable active channels before a buffer capture.
 */
static int32_t iio_ad7124_update_active_channels(void *dev, uint32_t mask)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)dev;
	uint32_t ch_idx = -1;
	int32_t ret;

	/* Stop the ADC before touching channel registers. */
	ret = ad7124_set_adc_mode(desc, AD7124_IDLE);
	if (ret)
		return ret;

	while (get_next_ch_idx(mask, ch_idx, &ch_idx)) {
		ret = ad7124_set_channel_status(desc, iio_to_hw_ch[ch_idx],
						true);
		if (ret)
			return ret;
	}

	iio_active_mask = mask;

	return ad7124_set_adc_mode(desc, AD7124_CONTINUOUS);
}

/**
 * @brief Disable all hardware channels after a buffer capture.
 */
static int32_t iio_ad7124_close_channels(void *dev)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)dev;
	uint32_t ch_idx;
	int32_t ret;

	/* Stop the ADC before disabling channels to avoid undefined sequencer state. */
	ret = ad7124_set_adc_mode(desc, AD7124_IDLE);
	if (ret)
		return ret;

	for (ch_idx = 0; ch_idx < AD7124_MAX_CHANNELS; ch_idx++) {
		ret = ad7124_set_channel_status(desc, ch_idx, false);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Read nb_samples samples from all active channels into buff.
 *
 * The ADC is already running in CONTINUOUS mode (started by pre_enable /
 * iio_ad7124_update_active_channels) and cycles through the enabled hw
 * channels in order. We use the cached iio_active_mask to know which IIO
 * scan indices are active, then collect one conversion per channel per sample
 * without any extra register reads between DATA reads (which would trigger
 * the SPI_IGNORE_ERR sticky flag).
 */
static int32_t iio_ad7124_read_samples(void *dev, int32_t *buff,
				       uint32_t nb_samples)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)dev;
	uint32_t mask = iio_active_mask;
	int32_t ret, i = 0, k;
	int32_t value;
	uint32_t ch_id;

	for (k = 0; k < (int32_t)nb_samples; k++) {
		ch_id = -1;
		while (get_next_ch_idx(mask, ch_id, &ch_id)) {
			ret = ad7124_wait_for_conv_ready(desc, 10000);
			if (ret)
				return ret;
			ret = ad7124_read_data(desc, &value);
			if (ret)
				return ret;
			buff[i++] = value;
		}
	}

	return nb_samples;
}

/* =========================================================================
 * IIO device descriptor
 * =========================================================================
 */

struct iio_device iio_ad7124_device = {
	.num_ch           = NO_OS_ARRAY_SIZE(ad7124_channels),
	.channels         = ad7124_channels,
	.attributes       = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable       = iio_ad7124_update_active_channels,
	.post_disable     = iio_ad7124_close_channels,
	.read_dev         = (int32_t (*)())iio_ad7124_read_samples,
	.debug_reg_read   = (int32_t (*)())ad7124_read_register2,
	.debug_reg_write  = (int32_t (*)())ad7124_write_register2,
};
