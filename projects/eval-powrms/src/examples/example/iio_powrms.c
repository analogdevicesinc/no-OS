/***************************************************************************//**
 *   @file   iio_powrms.c
 *   @brief  Source code file for IIO POWRMS example
 *   @author robert.budai@analog.com
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

#include "iio_powrms.h"
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "common_data.h"
#include "powrms_data_processing.h"
#include "powrms_utils.h"

// Global variables for indexed upload state
static uint32_t precision_chunk_index =
	0;    // Current chunk start index for precision array
static uint32_t precision_chunk_size =
	0;     // Number of values in current chunk
static uint32_t temp_chunk_index =
	0;         // Current chunk start index for temperature array
static uint32_t temp_chunk_size =
	0;          // Number of values in current chunk

extern struct powrms_variables input_variables[VARIABLE_NUMBER];

static int read_ch_attr(void *device, char *buf, uint32_t len,
			const struct iio_ch_info *channel, intptr_t priv);
static int read_ch_raw(void *device, char *buf, uint32_t len,
		       const struct iio_ch_info *channel, intptr_t priv);
static int get_input_impedance(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int set_input_impedance(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int get_output_impedance(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int set_output_impedance(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int get_frequency(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv);
static int set_frequency(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv);
static int get_use_default_calibration(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int set_use_default_calibration(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int get_use_default_temp_calibration(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int set_use_default_temp_calibration(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int get_adc_averaging_nr(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int set_adc_averaging_nr(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int read_precision_raw(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv);
static int write_precision_raw(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int read_precision_factor(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int read_frequency_ranges(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int write_frequency_ranges(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int read_calib_freq_range(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int write_calib_freq_range(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int read_calib_freq_range_reverse(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int write_calib_freq_range_reverse(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int read_temp_freq_range(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int write_temp_freq_range(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int read_temperature_compensation_value(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int write_temperature_compensation_value(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int read_dev_mode_overwrite_def_calib_values(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int write_dev_mode_overwrite_def_calib_values(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int read_poly_5000MHz_coeff(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);
static int write_poly_5000MHz_coeff(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv);
static int read_poly_5000MHz_coeff_reverse(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int write_poly_5000MHz_coeff_reverse(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

#define POWRMS_VOLTAGE_CHANNEL(_idx, _name)                                 \
{                                                                           \
.name = _name, .ch_type = IIO_VOLTAGE, .channel = _idx,                     \
.scan_index = -1, .indexed = true, .scan_type = NULL,                      \
.attributes = powrms_voltage_channel_attributes, .ch_out = false            \
}

#define POWRMS_POWER_CHANNEL(_idx, _name)                                 \
{                                                                           \
.name = _name, .ch_type = IIO_POWER, .channel = _idx,                     \
.scan_index = -1, .indexed = true, .scan_type = NULL,                      \
.attributes = powrms_voltage_channel_attributes, .ch_out = false            \
}

#define POWRMS_PRECISION_CHANNEL(_idx, _name)                               \
{                                                                           \
.name = _name, .ch_type = IIO_VOLTAGE, .channel = _idx,                     \
.scan_index = -1, .indexed = true, .scan_type = NULL,                      \
.attributes = powrms_precision_attributes, .ch_out = false                 \
}

#define POWRMS_POLY_CALIB_CHANNEL(_idx, _name)                              \
{                                                                           \
.name = _name, .ch_type = IIO_VOLTAGE, .channel = _idx,                     \
.scan_index = -1, .indexed = true, .scan_type = NULL,                      \
.attributes = powrms_poly_calib_attributes, .ch_out = false                \
}

#define POWRMS_POLY_CALIB_REVERSE_CHANNEL(_idx, _name)                      \
{                                                                           \
.name = _name, .ch_type = IIO_VOLTAGE, .channel = _idx,                     \
.scan_index = -1, .indexed = true, .scan_type = NULL,                      \
.attributes = powrms_poly_calib_reverse_attributes, .ch_out = false        \
}

struct iio_attribute powrms_voltage_channel_attributes[] = {
	{
		.name = "raw",
		.show = read_ch_raw,
		.priv = IIO_ATTR_SCALE,
	},
	{
		.name = "scale",
		.show = read_ch_attr,
		.priv = IIO_ATTR_SCALE,
	},
	{
		.name = "offset",
		.show = read_ch_attr,
		.priv = IIO_ATTR_OFFSET,
	},
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute powrms_precision_attributes[] = {
	// New per-frequency range attributes for correction values
	{
		.name = "calib_10MHz",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX,  // frequency index 0
	},
	{
		.name = "calib_100MHz",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 1,  // frequency index 1
	},
	{
		.name = "calib_1000MHz",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 2,  // frequency index 2
	},
	{
		.name = "calib_2000MHz",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 3,  // frequency index 3
	},
	{
		.name = "calib_3000MHz",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 4,  // frequency index 4
	},
	{
		.name = "calib_4000MHz",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 5,  // frequency index 5
	},
	{
		.name = "calib_5000MHz",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 6,  // frequency index 6
	},
	{
		.name = "calib_6000MHz",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MAX_FREQ_RANGE_INDEX,  // frequency index 7
	},
	// New per-frequency range attributes for reverse correction values
	{
		.name = "calib_10MHz_reverse",
		.show = read_calib_freq_range_reverse,
		.store = write_calib_freq_range_reverse,
		.priv = MIN_FREQ_RANGE_INDEX,  // frequency index 0
	},
	{
		.name = "calib_100MHz_reverse",
		.show = read_calib_freq_range_reverse,
		.store = write_calib_freq_range_reverse,
		.priv = MIN_FREQ_RANGE_INDEX + 1,  // frequency index 1
	},
	{
		.name = "calib_1000MHz_reverse",
		.show = read_calib_freq_range_reverse,
		.store = write_calib_freq_range_reverse,
		.priv = MIN_FREQ_RANGE_INDEX + 2,  // frequency index 2
	},
	{
		.name = "calib_2000MHz_reverse",
		.show = read_calib_freq_range_reverse,
		.store = write_calib_freq_range_reverse,
		.priv = MIN_FREQ_RANGE_INDEX + 3,  // frequency index 3
	},
	{
		.name = "calib_3000MHz_reverse",
		.show = read_calib_freq_range_reverse,
		.store = write_calib_freq_range_reverse,
		.priv = MIN_FREQ_RANGE_INDEX + 4,  // frequency index 4
	},
	{
		.name = "calib_4000MHz_reverse",
		.show = read_calib_freq_range_reverse,
		.store = write_calib_freq_range_reverse,
		.priv = MIN_FREQ_RANGE_INDEX + 5,  // frequency index 5
	},
	{
		.name = "calib_5000MHz_reverse",
		.show = read_calib_freq_range_reverse,
		.store = write_calib_freq_range_reverse,
		.priv = MIN_FREQ_RANGE_INDEX + 6,  // frequency index 6
	},
	{
		.name = "calib_6000MHz_reverse",
		.show = read_calib_freq_range_reverse,
		.store = write_calib_freq_range_reverse,
		.priv = MAX_FREQ_RANGE_INDEX,  // frequency index 7
	},
	{
		.name = "temperature_compensation_value",
		.show = read_temperature_compensation_value,
		.store = write_temperature_compensation_value,
		.priv = IIO_ATTR_SCALE,
	},
	{
		.name = "dev_mode_overwrite_def_calib",
		.show = read_dev_mode_overwrite_def_calib_values,
		.store = write_dev_mode_overwrite_def_calib_values,
		.priv = IIO_ATTR_SCALE,
	},
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute powrms_poly_calib_attributes[] = {
	// Polynomial calibration coefficients for 5000MHz
	{
		.name = "intercept",
		.show = read_poly_5000MHz_coeff,
		.store = write_poly_5000MHz_coeff,
		.priv = 0,  // coefficient index
	},
	{
		.name = "c_x",
		.show = read_poly_5000MHz_coeff,
		.store = write_poly_5000MHz_coeff,
		.priv = 1,
	},
	{
		.name = "c_f",
		.show = read_poly_5000MHz_coeff,
		.store = write_poly_5000MHz_coeff,
		.priv = 2,
	},
	{
		.name = "c_x2",
		.show = read_poly_5000MHz_coeff,
		.store = write_poly_5000MHz_coeff,
		.priv = 3,
	},
	{
		.name = "c_xf",
		.show = read_poly_5000MHz_coeff,
		.store = write_poly_5000MHz_coeff,
		.priv = 4,
	},
	{
		.name = "c_f2",
		.show = read_poly_5000MHz_coeff,
		.store = write_poly_5000MHz_coeff,
		.priv = 5,
	},
	{
		.name = "c_x3",
		.show = read_poly_5000MHz_coeff,
		.store = write_poly_5000MHz_coeff,
		.priv = 6,
	},
	{
		.name = "c_x2f",
		.show = read_poly_5000MHz_coeff,
		.store = write_poly_5000MHz_coeff,
		.priv = 7,
	},
	{
		.name = "c_xf2",
		.show = read_poly_5000MHz_coeff,
		.store = write_poly_5000MHz_coeff,
		.priv = 8,
	},
	{
		.name = "c_f3",
		.show = read_poly_5000MHz_coeff,
		.store = write_poly_5000MHz_coeff,
		.priv = 9,
	},
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute powrms_poly_calib_reverse_attributes[] = {
	// Polynomial calibration coefficients for 5000MHz reverse
	{
		.name = "intercept",
		.show = read_poly_5000MHz_coeff_reverse,
		.store = write_poly_5000MHz_coeff_reverse,
		.priv = 0,  // coefficient index
	},
	{
		.name = "c_x",
		.show = read_poly_5000MHz_coeff_reverse,
		.store = write_poly_5000MHz_coeff_reverse,
		.priv = 1,
	},
	{
		.name = "c_f",
		.show = read_poly_5000MHz_coeff_reverse,
		.store = write_poly_5000MHz_coeff_reverse,
		.priv = 2,
	},
	{
		.name = "c_x2",
		.show = read_poly_5000MHz_coeff_reverse,
		.store = write_poly_5000MHz_coeff_reverse,
		.priv = 3,
	},
	{
		.name = "c_xf",
		.show = read_poly_5000MHz_coeff_reverse,
		.store = write_poly_5000MHz_coeff_reverse,
		.priv = 4,
	},
	{
		.name = "c_f2",
		.show = read_poly_5000MHz_coeff_reverse,
		.store = write_poly_5000MHz_coeff_reverse,
		.priv = 5,
	},
	{
		.name = "c_x3",
		.show = read_poly_5000MHz_coeff_reverse,
		.store = write_poly_5000MHz_coeff_reverse,
		.priv = 6,
	},
	{
		.name = "c_x2f",
		.show = read_poly_5000MHz_coeff_reverse,
		.store = write_poly_5000MHz_coeff_reverse,
		.priv = 7,
	},
	{
		.name = "c_xf2",
		.show = read_poly_5000MHz_coeff_reverse,
		.store = write_poly_5000MHz_coeff_reverse,
		.priv = 8,
	},
	{
		.name = "c_f3",
		.show = read_poly_5000MHz_coeff_reverse,
		.store = write_poly_5000MHz_coeff_reverse,
		.priv = 9,
	},
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute powrms_global_attributes[] = {
	{
		.name = "frequency_MHz",
		.show = get_frequency,
		.store = set_frequency,
		.priv = IIO_ATTR_SCALE,
	},
	{
		.name = "input_impedance",
		.show = get_input_impedance,
		.store = set_input_impedance,
		.priv = IIO_ATTR_OFFSET,
	},
	{
		.name = "output_impedance",
		.show = get_output_impedance,
		.store = set_output_impedance,
		.priv = IIO_ATTR_PRECISION_FACTOR,
	},
	{
		.name = "use_default_calibration",
		.show = get_use_default_calibration,
		.store = set_use_default_calibration,
		.priv = IIO_ATTR_ARRAY_SIZE,
	},
	{
		.name = "adc_averaging_nr",
		.show = get_adc_averaging_nr,
		.store = set_adc_averaging_nr,
		.priv = IIO_ATTR_SCALE,
	},
	END_ATTRIBUTES_ARRAY,
};

struct scan_type powrms_scan_type = {
	.sign = 's',
	.realbits = IIO_SCAN_REALBITS,
	.storagebits = IIO_SCAN_STORAGEBITS,
	.shift = IIO_SCAN_SHIFT,
	.is_big_endian = false
}; // scan type for 32-bit signed integer values (scaled raw values)

static struct iio_channel iio_powrms_channels[] = {
	POWRMS_VOLTAGE_CHANNEL(IIO_CH_V_IN_CORRECTED, "v_in_cor"),
	POWRMS_VOLTAGE_CHANNEL(IIO_CH_V_OUT_CORRECTED, "v_out_cor"),
	POWRMS_VOLTAGE_CHANNEL(IIO_CH_TEMPERATURE, "tmp"),
	POWRMS_VOLTAGE_CHANNEL(IIO_CH_V_IN_RAW, "v_in"),
	POWRMS_VOLTAGE_CHANNEL(IIO_CH_V_OUT_RAW, "v_out"),
	POWRMS_POWER_CHANNEL(IIO_CH_P_FORWARD, "p_fwd"),
	POWRMS_POWER_CHANNEL(IIO_CH_P_REVERSE, "p_rev"),
	POWRMS_PRECISION_CHANNEL(IIO_CH_PRECISION_ARRAY, "precision_array"),
	POWRMS_POLY_CALIB_CHANNEL(IIO_CH_POLY_CALIB_5000MHZ, "poly_calib_5000MHz"),
	POWRMS_POLY_CALIB_REVERSE_CHANNEL(IIO_CH_POLY_CALIB_5000MHZ_REVERSE, "poly_calib_5000MHz_reverse"),
}; // channel definitions for device

struct iio_device powrms_iio_descriptor = {
	.num_ch = IIO_POWRMS_NUM_CHANNELS,
	.channels = iio_powrms_channels,
	.attributes = powrms_global_attributes,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = NULL,
	.post_disable = NULL,
	.trigger_handler = (int32_t(*)())NULL,
	.submit = NULL,
}; // PowerRMS IIO descriptor

IIO_BUFF_TYPE iio_data_buffer_loc[MAX_SIZE_BASE_ADDR] = {0};

struct no_os_circular_buffer *powrms_vin0_buffer = NULL;
struct no_os_circular_buffer *powrms_vin1_buffer = NULL;
struct no_os_circular_buffer *powrms_p_fwd_buffer = NULL;
struct no_os_circular_buffer *powrms_p_rev_buffer = NULL;

/**
 * @brief Read channel raw values for non-scannable voltage channels
 *
 * @param device - IIO device instance
 * @param buf - Buffer to store the raw value
 * @param len - Buffer length
 * @param channel - Channel information
 * @param priv - Private data (not used)
 * @return Number of bytes written to buffer, or negative error code
 */
static int read_ch_raw(void *device, char *buf, uint32_t len,
		       const struct iio_ch_info *channel, intptr_t priv)
{
	extern volatile struct adc_data adc_data_input;

	switch (channel->ch_num) {
	case IIO_CH_V_IN_CORRECTED: // v_in_corrected
		return snprintf(buf, len, "%d",
				(int32_t)(adc_data_input.adc_vin0_voltage_corrected * BUFFER_VAL_PRECISSION *
					  VOLTAGE_TO_MILLIVOLT_SCALE));
	case IIO_CH_V_OUT_CORRECTED: // v_out_corrected
		return snprintf(buf, len, "%d",
				(int32_t)(adc_data_input.adc_vin1_voltage_corrected * BUFFER_VAL_PRECISSION *
					  VOLTAGE_TO_MILLIVOLT_SCALE));
	case IIO_CH_TEMPERATURE: // tmp
		return snprintf(buf, len, "%d",
				(int32_t)(adc_data_input.adc_vin2_voltage * BUFFER_VAL_PRECISSION *
					  VOLTAGE_TO_MILLIVOLT_SCALE));
	case IIO_CH_V_IN_RAW: // v_in_raw
		return snprintf(buf, len, "%d",
				(int32_t)(adc_data_input.adc_vin0_voltage * BUFFER_VAL_PRECISSION *
					  VOLTAGE_TO_MILLIVOLT_SCALE));
	case IIO_CH_V_OUT_RAW: // v_out_raw
		return snprintf(buf, len, "%d",
				(int32_t)(adc_data_input.adc_vin1_voltage * BUFFER_VAL_PRECISSION *
					  VOLTAGE_TO_MILLIVOLT_SCALE));
	case IIO_CH_P_FORWARD: // p_fwd
		return snprintf(buf, len, "%d",
				(int32_t)(adc_data_input.adc_p_fwd * BUFFER_VAL_PRECISSION));
	case IIO_CH_P_REVERSE: // p_rev
		return snprintf(buf, len, "%d",
				(int32_t)(adc_data_input.adc_p_rev * BUFFER_VAL_PRECISSION));
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read channel attributes (scale and offset)
 *
 * @param device - IIO device instance
 * @param buf - Buffer to store the attribute value
 * @param len - Buffer length
 * @param channel - Channel information
 * @param priv - Private data (0 = scale, 1 = offset)
 * @return Number of bytes written to buffer, or negative error code
 */
static int read_ch_attr(void *device, char *buf, uint32_t len,
			const struct iio_ch_info *channel, intptr_t priv)
{
	switch (priv) {
	case IIO_ATTR_SCALE: // scale
		// Scale factor for voltage channels (mV to V conversion)
		// For ADC readings, typically scale = (vref / (2^resolution)) * gain
		// Here we assume 1:1 scale (voltage values are already in volts)
		return snprintf(buf, len, "%.3f", (1.0 / ((double)(BUFFER_VAL_PRECISSION))));

	case IIO_ATTR_OFFSET: // offset
		// Offset for voltage channels (typically 0 for unipolar ADCs)
		return snprintf(buf, len, "%.1f", ADC_OFFSET_VALUE);

	case IIO_ATTR_ARRAY_SIZE: // array_size for precision channel
		return snprintf(buf, len, "%d", PRECISION_ARRAY_SIZE);

	default:
		return -EINVAL;
	}
}

static int get_input_impedance(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	return snprintf(buf, len, "%f", (float)input_variables[INPUT_IMPEDANCE].value);
}

static int get_output_impedance(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	return snprintf(buf, len, "%f", (float)input_variables[OUTPUT_IMPEDANCE].value);
}

static int get_frequency(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	return snprintf(buf, len, "%f", (float)input_variables[SIGNAL_FREQUENCY].value);
}

static int set_input_impedance(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	float val;
	if (sscanf(buf, "%f", &val) == 1) {
		// Update both value and digits array
		update_values_from_new_value(&input_variables[INPUT_IMPEDANCE], (double)val);
		return len;
	}
	return IIO_ERROR_INVALID_INPUT;
}

static int set_output_impedance(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	float val;
	if (sscanf(buf, "%f", &val) == 1) {
		// Update both value and digits array
		update_values_from_new_value(&input_variables[OUTPUT_IMPEDANCE], (double)val);
		return len;
	}
	return IIO_ERROR_INVALID_INPUT;
}

static int set_frequency(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	float val;
	if (sscanf(buf, "%f", &val) == 1) {
		// Update both value and digits array
		update_values_from_new_value(&input_variables[SIGNAL_FREQUENCY], (double)val);
		return len;
	}
	return IIO_ERROR_INVALID_INPUT;
}

static int get_use_default_calibration(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t use_eeprom_calibration_data;
	int ret;

	ret = powrms_eeprom_get_use_def_cal_data(&use_eeprom_calibration_data);
	if (ret) {
		// EEPROM not available - return default value (enabled)
		use_eeprom_calibration_data = CALIBRATION_ENABLED;
	}
	return snprintf(buf, len, "%d", (int)use_eeprom_calibration_data);
}

static int set_use_default_calibration(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int val;
	if (sscanf(buf, "%d", &val) == 1) {
		// Ensure the value is either 0 or 1
		int32_t eeprom_val = (val != CALIBRATION_DISABLED) ? CALIBRATION_ENABLED :
				     CALIBRATION_DISABLED;

		// Write to EEPROM and update global variable
		int ret = powrms_eeprom_set_use_def_cal_data(eeprom_val);
		if (ret == IIO_SUCCESS) {
			return len;  // Success
		} else {
			// EEPROM write failed - still accept the command but don't update EEPROM
			// This allows the system to continue functioning even without EEPROM
			return len;
		}
	}
	return -EINVAL;  // Invalid input format
}

static int get_adc_averaging_nr(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	extern volatile struct adc_data adc_data_input;
	return snprintf(buf, len, "%d", (int)adc_data_input.adc_averaging_nr);
}

static int set_adc_averaging_nr(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	extern volatile struct adc_data adc_data_input;
	int val;
	if (sscanf(buf, "%d", &val) == 1) {
		// Validate the value is reasonable (e.g., 1-255 for uint8_t)
		if (val >= 1 && val <= 255) {
			adc_data_input.adc_averaging_nr = (uint8_t)val;
			return len;
		}
	}
	return -EINVAL;  // Invalid input format
}

// Implementation of precision channel functions
static int read_precision_raw(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	// Return the first value as raw integer (no conversion)
	return snprintf(buf, len, "%"PRId32"", precision_values[0]);
}

static int write_precision_raw(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	// Set the first value from raw integer (no conversion)
	int32_t raw_value;
	if (sscanf(buf, "%"PRId32"", &raw_value) == 1) {
		precision_values[0] = raw_value;
		return len;
	}
	return -EINVAL;
}

/**
 * @brief Read voltage temperature compensation value from EEPROM
 *
 * This function reads the voltage temperature compensation value using single-byte reads to avoid
 * I2C bus blocking issues that can occur with large transfers. The int32_t value is read byte by byte
 * and reconstructed in little-endian format.
 */
int powrms_eeprom_read_v_temp_comp_val(int32_t *value)
{
	int ret;
	uint16_t base_address =
		MEM_V_TEMP_COMP_VAL_POZ; // Address for voltage temperature compensation value
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of int32_t

	if (!m24512_desc)
		return -ENODEV; // EEPROM not initialized

	if (!value)
		return -EINVAL; // Invalid parameter

	// Read 4 bytes for the int32_t value
	for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
		ret = no_os_eeprom_read(m24512_desc, base_address + byte_idx,
					&byte_buffer[byte_idx], 1);
		if (ret)
			return ret; // Read failed

		// Small delay between byte reads to prevent I2C bus saturation
		no_os_udelay(50); // 50 microseconds delay
	}

	// Reconstruct int32_t from bytes (little-endian format)
	*value = (int32_t)((uint32_t)byte_buffer[0] |
			   ((uint32_t)byte_buffer[1] << 8) |
			   ((uint32_t)byte_buffer[2] << 16) |
			   ((uint32_t)byte_buffer[3] << 24));

	return 0; // Success
}

/**
 * @brief Read default voltage temperature compensation value from EEPROM
 *
 * This function reads the default voltage temperature compensation value using single-byte reads to avoid
 * I2C bus blocking issues that can occur with large transfers. The int32_t value is read byte by byte
 * and reconstructed in little-endian format.
 */
int powrms_eeprom_read_def_v_temp_comp_val(int32_t *value)
{
	int ret;
	uint16_t base_address =
		MEM_DEF_V_TEMP_COMP_VAL_POZ; // Address for default voltage temperature compensation value
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of int32_t

	if (!m24512_desc)
		return -ENODEV; // EEPROM not initialized

	if (!value)
		return -EINVAL; // Invalid parameter

	// Read 4 bytes for the int32_t value
	for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
		ret = no_os_eeprom_read(m24512_desc, base_address + byte_idx,
					&byte_buffer[byte_idx], 1);
		if (ret)
			return ret; // Read failed

		// Small delay between byte reads to prevent I2C bus saturation
		no_os_udelay(50); // 50 microseconds delay
	}

	// Reconstruct int32_t from bytes (little-endian format)
	*value = (int32_t)((uint32_t)byte_buffer[0] |
			   ((uint32_t)byte_buffer[1] << 8) |
			   ((uint32_t)byte_buffer[2] << 16) |
			   ((uint32_t)byte_buffer[3] << 24));

	return 0; // Success
}

/**
 * @brief Write voltage temperature compensation value to EEPROM
 *
 * This function writes the voltage temperature compensation value using single-byte writes to avoid
 * I2C bus blocking issues that can occur with large transfers. The int32_t value is broken down
 * into bytes and written individually in little-endian format.
 */
int powrms_eeprom_write_v_temp_comp_val(int32_t value)
{
	int ret;
	uint16_t base_address =
		MEM_V_TEMP_COMP_VAL_POZ; // Address for voltage temperature compensation value
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of int32_t

	if (!m24512_desc)
		return -ENODEV;

	// Convert int32_t to bytes (little-endian format)
	byte_buffer[0] = (uint8_t)(value & 0xFF);
	byte_buffer[1] = (uint8_t)((value >> 8) & 0xFF);
	byte_buffer[2] = (uint8_t)((value >> 16) & 0xFF);
	byte_buffer[3] = (uint8_t)((value >> 24) & 0xFF);

	// Write 4 bytes for the int32_t value
	for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
		ret = no_os_eeprom_write(m24512_desc, base_address + byte_idx,
					 &byte_buffer[byte_idx], 1);
		if (ret)
			return ret; // Write failed

		// Small delay between byte writes to prevent I2C bus saturation
		no_os_udelay(50); // 50 microseconds delay
	}

	// Optimized delay for M24512 - typical write cycle is 2-3ms
	no_os_mdelay(10); // 10ms delay to allow EEPROM internal write cycle

	return 0; // Success
}

/**
 * @brief Write default voltage temperature compensation value to EEPROM
 *
 * This function writes the default voltage temperature compensation value using single-byte writes to avoid
 * I2C bus blocking issues that can occur with large transfers. The int32_t value is broken down
 * into bytes and written individually in little-endian format.
 */
int powrms_eeprom_write_def_v_temp_comp_val(int32_t value)
{
	int ret;
	uint16_t base_address =
		MEM_DEF_V_TEMP_COMP_VAL_POZ; // Address for default voltage temperature compensation value
	uint8_t byte_buffer[4];       // Buffer for 4 bytes of int32_t

	if (!m24512_desc)
		return -ENODEV;

	// Convert int32_t to bytes (little-endian format)
	byte_buffer[0] = (uint8_t)(value & 0xFF);
	byte_buffer[1] = (uint8_t)((value >> 8) & 0xFF);
	byte_buffer[2] = (uint8_t)((value >> 16) & 0xFF);
	byte_buffer[3] = (uint8_t)((value >> 24) & 0xFF);

	// Write 4 bytes for the int32_t value
	for (int byte_idx = 0; byte_idx < 4; byte_idx++) {
		ret = no_os_eeprom_write(m24512_desc, base_address + byte_idx,
					 &byte_buffer[byte_idx], 1);
		if (ret)
			return ret; // Write failed

		// Small delay between byte writes to prevent I2C bus saturation
		no_os_udelay(50); // 50 microseconds delay
	}

	// Optimized delay for M24512 - typical write cycle is 2-3ms
	no_os_mdelay(10); // 10ms delay to allow EEPROM internal write cycle

	return 0; // Success
}


static int read_calib_freq_range(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	int freq_index = (int)priv;  // frequency range index (0-7)
	int written = 0;
	int remaining = len;
	int start_idx = freq_index *
			FREQ_RANGE_VALUES_PER_RANGE;  // Starting index for this frequency range

	// Write 6 values for this frequency range as comma-separated list
	for (int i = 0; i < FREQ_RANGE_VALUES_PER_RANGE
	     && remaining > MIN_REMAINING_BUFFER_SIZE; i++) {
		float float_value = (float)precision_values[start_idx + i] *
				    PRECISION_RESOLUTION;
		int bytes = snprintf(buf + written, remaining, "%.3f", float_value);

		if (bytes < 0 || bytes >= remaining)
			break;

		written += bytes;
		remaining -= bytes;

		// Add comma separator except for last element
		if (i < FREQ_RANGE_VALUES_PER_RANGE - 1 && remaining > MIN_COMMA_BUFFER_SIZE) {
			buf[written++] = ',';
			remaining--;
		}
	}

	return written;
}

static int write_calib_freq_range(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	int freq_index = (int)priv;  // frequency range index (0-7)
	char *token;
	char *saveptr;
	char buf_copy[CALIB_VALUES_BUFFER_SIZE];  // Buffer for 6 values
	int index = 0;
	int start_idx = freq_index * FREQ_RANGE_VALUES_PER_RANGE;
	int ret;

	if (len >= CALIB_VALUES_BUFFER_SIZE)
		return -EINVAL;

	// Create a copy of the buffer for tokenization
	strncpy(buf_copy, buf, len);
	buf_copy[len] = '\0';

	// Parse comma-separated values
	token = strtok_r(buf_copy, ",", &saveptr);
	while (token != NULL && index < FREQ_RANGE_VALUES_PER_RANGE) {
		float float_value;
		if (sscanf(token, "%f", &float_value) == 1) {
			// Convert float to integer and store
			precision_values[start_idx + index] = (int32_t)(float_value *
							      PRECISION_SCALE_FACTOR);
			index++;
		} else {
			return -EINVAL;
		}
		token = strtok_r(NULL, ",", &saveptr);
	}

	if (index != FREQ_RANGE_VALUES_PER_RANGE)
		return -EINVAL;  // Expected exactly 6 values

	// Save to EEPROM (write the entire array since EEPROM functions work with full array)
	ret = powrms_eeprom_write_precision_array(precision_values);
	if (ret != IIO_SUCCESS)
		// EEPROM write failed - values are still updated in RAM, so continue
		// This allows the system to function even without EEPROM persistence
		;

	return len;
}

static int read_calib_freq_range_reverse(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int freq_index = (int)priv;  // frequency range index (0-7)
	int written = 0;
	int remaining = len;
	int start_idx = freq_index *
			FREQ_RANGE_VALUES_PER_RANGE;  // Starting index for this frequency range

	// Write 30 values for this frequency range as comma-separated list
	for (int i = 0; i < FREQ_RANGE_VALUES_PER_RANGE
	     && remaining > MIN_REMAINING_BUFFER_SIZE; i++) {
		float float_value = (float)precision_values_reverse[start_idx + i] *
				    PRECISION_RESOLUTION;
		int bytes = snprintf(buf + written, remaining, "%.3f", float_value);

		if (bytes < 0 || bytes >= remaining)
			break;

		written += bytes;
		remaining -= bytes;

		// Add comma separator except for last element
		if (i < FREQ_RANGE_VALUES_PER_RANGE - 1 && remaining > MIN_COMMA_BUFFER_SIZE) {
			buf[written++] = ',';
			remaining--;
		}
	}

	return written;
}

static int write_calib_freq_range_reverse(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int freq_index = (int)priv;  // frequency range index (0-7)
	char *token;
	char *saveptr;
	char buf_copy[CALIB_VALUES_BUFFER_SIZE];  // Buffer for 14 values
	int index = 0;
	int start_idx = freq_index * FREQ_RANGE_VALUES_PER_RANGE;
	int ret;

	if (len >= CALIB_VALUES_BUFFER_SIZE)
		return -EINVAL;

	// Create a copy of the buffer for tokenization
	strncpy(buf_copy, buf, len);
	buf_copy[len] = '\0';

	// Parse comma-separated values
	token = strtok_r(buf_copy, ",", &saveptr);
	while (token != NULL && index < FREQ_RANGE_VALUES_PER_RANGE) {
		float float_value;
		if (sscanf(token, "%f", &float_value) == 1) {
			// Convert float to integer and store
			precision_values_reverse[start_idx + index] = (int32_t)(float_value *
					PRECISION_SCALE_FACTOR);
			index++;
		} else {
			return -EINVAL;
		}
		token = strtok_r(NULL, ",", &saveptr);
	}

	if (index != FREQ_RANGE_VALUES_PER_RANGE)
		return -EINVAL;  // Expected exactly 14 values

	// Save to EEPROM (write the entire array since EEPROM functions work with full array)
	ret = powrms_eeprom_write_precision_array_reverse(precision_values_reverse);
	if (ret != IIO_SUCCESS)
		// EEPROM write failed - values are still updated in RAM, so continue
		// This allows the system to function even without EEPROM persistence
		;

	return len;
}


// Temperature compensation value utility functions for external access
int powrms_set_temperature_compensation_value(float value)
{
	// Convert float to integer for internal storage
	temperature_compensation_value = (int32_t)(value * PRECISION_SCALE_FACTOR);
	return 0;
}

float powrms_get_temperature_compensation_value(void)
{
	// Convert integer to float for external use
	return (float)temperature_compensation_value * PRECISION_RESOLUTION;
}

int powrms_set_temperature_compensation_value_raw(int32_t raw_value)
{
	temperature_compensation_value = raw_value;
	return 0;
}

int32_t powrms_get_temperature_compensation_value_raw(void)
{
	return temperature_compensation_value;
}

/**
 * @brief Read temperature compensation value attribute
 *
 * This function reads the current temperature compensation value and returns it as a float.
 * The value is converted from the internal int32_t representation using the precision scale factor.
 *
 * @param device - IIO device instance (unused)
 * @param buf - Buffer to store the attribute value
 * @param len - Buffer length
 * @param channel - Channel information (unused)
 * @param priv - Private data (unused)
 * @return Number of bytes written to buffer
 */
static int read_temperature_compensation_value(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	// Convert from internal int32_t representation to float
	float float_value = (float)temperature_compensation_value *
			    PRECISION_RESOLUTION;
	return snprintf(buf, len, "%.3f", float_value);
}

/**
 * @brief Write temperature compensation value attribute
 *
 * This function parses a float value from the input buffer, converts it to the internal
 * int32_t representation, and stores it in both RAM and EEPROM. It updates both the
 * user-configurable position and can optionally update the default position depending
 * on the current calibration mode.
 *
 * @param device - IIO device instance (unused)
 * @param buf - Buffer containing the value to write
 * @param len - Buffer length
 * @param channel - Channel information (unused)
 * @param priv - Private data (unused)
 * @return Number of bytes processed on success, negative error code on failure
 */
static int write_temperature_compensation_value(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	float float_value;
	int ret;

	// Parse the input value
	if (sscanf(buf, "%f", &float_value) == 1) {
		// Convert float to integer for internal storage
		temperature_compensation_value = (int32_t)(float_value *
						 PRECISION_SCALE_FACTOR);

		// Write to EEPROM at user configurable position
		ret = powrms_eeprom_write_v_temp_comp_val(temperature_compensation_value);
		if (ret != IIO_SUCCESS)
			// EEPROM write failed - values are still updated in RAM, so continue
			// This allows the system to function even without EEPROM persistence
			;

		return len;
	}
	return -EINVAL;  // Invalid input format
}

/**
 * @brief Read polynomial calibration coefficient for 5000MHz
 *
 * This function reads a specific polynomial coefficient based on the priv index.
 * Coefficients: 0=intercept, 1=c_x, 2=c_f, 3=c_x2, 4=c_xf, 5=c_f2, 6=c_x3, 7=c_x2f, 8=c_xf2, 9=c_f3
 *
 * @param device - IIO device instance (unused)
 * @param buf - Buffer to store the attribute value
 * @param len - Buffer length
 * @param channel - Channel information (unused)
 * @param priv - Coefficient index (0-9)
 * @return Number of bytes written to buffer
 */
static int read_poly_5000MHz_coeff(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	double *coeff_ptr;

	// Select the appropriate coefficient based on priv index
	switch (priv) {
	case 0:
		coeff_ptr = &poly_5000MHz_intercept;
		break;
	case 1:
		coeff_ptr = &poly_5000MHz_c_x;
		break;
	case 2:
		coeff_ptr = &poly_5000MHz_c_f;
		break;
	case 3:
		coeff_ptr = &poly_5000MHz_c_x2;
		break;
	case 4:
		coeff_ptr = &poly_5000MHz_c_xf;
		break;
	case 5:
		coeff_ptr = &poly_5000MHz_c_f2;
		break;
	case 6:
		coeff_ptr = &poly_5000MHz_c_x3;
		break;
	case 7:
		coeff_ptr = &poly_5000MHz_c_x2f;
		break;
	case 8:
		coeff_ptr = &poly_5000MHz_c_xf2;
		break;
	case 9:
		coeff_ptr = &poly_5000MHz_c_f3;
		break;
	default:
		return -EINVAL;
	}

	// Return value with 20 decimal precision using scientific notation
	return snprintf(buf, len, "%.20e", *coeff_ptr);
}

/**
 * @brief Write polynomial calibration coefficient for 5000MHz
 *
 * This function writes a specific polynomial coefficient based on the priv index.
 * The value is stored in EEPROM for persistence.
 *
 * @param device - IIO device instance (unused)
 * @param buf - Buffer containing the value to write
 * @param len - Buffer length
 * @param channel - Channel information (unused)
 * @param priv - Coefficient index (0-9)
 * @return Number of bytes processed on success, negative error code on failure
 */
static int write_poly_5000MHz_coeff(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv)
{
	double double_value;
	double *coeff_ptr;
	int ret;

	// Parse the input value (supports both standard and scientific notation)
	if (sscanf(buf, "%lf", &double_value) != 1)
		return -EINVAL;  // Invalid input format

	// Select the appropriate coefficient based on priv index
	switch (priv) {
	case 0:
		coeff_ptr = &poly_5000MHz_intercept;
		break;
	case 1:
		coeff_ptr = &poly_5000MHz_c_x;
		break;
	case 2:
		coeff_ptr = &poly_5000MHz_c_f;
		break;
	case 3:
		coeff_ptr = &poly_5000MHz_c_x2;
		break;
	case 4:
		coeff_ptr = &poly_5000MHz_c_xf;
		break;
	case 5:
		coeff_ptr = &poly_5000MHz_c_f2;
		break;
	case 6:
		coeff_ptr = &poly_5000MHz_c_x3;
		break;
	case 7:
		coeff_ptr = &poly_5000MHz_c_x2f;
		break;
	case 8:
		coeff_ptr = &poly_5000MHz_c_xf2;
		break;
	case 9:
		coeff_ptr = &poly_5000MHz_c_f3;
		break;
	default:
		return -EINVAL;
	}

	// Store the double value directly (no conversion needed)
	*coeff_ptr = double_value;

	// Write to EEPROM
	ret = powrms_eeprom_write_poly_5000MHz_coeffs(&poly_5000MHz_intercept);
	if (ret != IIO_SUCCESS)
		// EEPROM write failed - values are still updated in RAM, so continue
		// This allows the system to function even without EEPROM persistence
		;

	return len;
}

/**
 * @brief Read polynomial calibration coefficient for 5000MHz reverse
 *
 * This function reads a specific polynomial coefficient for reverse power based on the priv index.
 * Coefficients: 0=intercept, 1=c_x, 2=c_f, 3=c_x2, 4=c_xf, 5=c_f2, 6=c_x3, 7=c_x2f, 8=c_xf2, 9=c_f3
 *
 * @param device - IIO device instance (unused)
 * @param buf - Buffer to store the attribute value
 * @param len - Buffer length
 * @param channel - Channel information (unused)
 * @param priv - Coefficient index (0-9)
 * @return Number of bytes written to buffer
 */
static int read_poly_5000MHz_coeff_reverse(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	double *coeff_ptr;

	// Select the appropriate coefficient based on priv index
	switch (priv) {
	case 0:
		coeff_ptr = &poly_5000MHz_intercept_reverse;
		break;
	case 1:
		coeff_ptr = &poly_5000MHz_c_x_reverse;
		break;
	case 2:
		coeff_ptr = &poly_5000MHz_c_f_reverse;
		break;
	case 3:
		coeff_ptr = &poly_5000MHz_c_x2_reverse;
		break;
	case 4:
		coeff_ptr = &poly_5000MHz_c_xf_reverse;
		break;
	case 5:
		coeff_ptr = &poly_5000MHz_c_f2_reverse;
		break;
	case 6:
		coeff_ptr = &poly_5000MHz_c_x3_reverse;
		break;
	case 7:
		coeff_ptr = &poly_5000MHz_c_x2f_reverse;
		break;
	case 8:
		coeff_ptr = &poly_5000MHz_c_xf2_reverse;
		break;
	case 9:
		coeff_ptr = &poly_5000MHz_c_f3_reverse;
		break;
	default:
		return -EINVAL;
	}

	// Return value with 20 decimal precision using scientific notation
	return snprintf(buf, len, "%.20e", *coeff_ptr);
}

/**
 * @brief Write polynomial calibration coefficient for 5000MHz reverse
 *
 * This function writes a specific polynomial coefficient for reverse power based on the priv index.
 * The value is stored in EEPROM for persistence.
 *
 * @param device - IIO device instance (unused)
 * @param buf - Buffer containing the value to write
 * @param len - Buffer length
 * @param channel - Channel information (unused)
 * @param priv - Coefficient index (0-9)
 * @return Number of bytes processed on success, negative error code on failure
 */
static int write_poly_5000MHz_coeff_reverse(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	double double_value;
	double *coeff_ptr;
	int ret;

	// Parse the input value (supports both standard and scientific notation)
	if (sscanf(buf, "%lf", &double_value) != 1)
		return -EINVAL;  // Invalid input format

	// Select the appropriate coefficient based on priv index
	switch (priv) {
	case 0:
		coeff_ptr = &poly_5000MHz_intercept_reverse;
		break;
	case 1:
		coeff_ptr = &poly_5000MHz_c_x_reverse;
		break;
	case 2:
		coeff_ptr = &poly_5000MHz_c_f_reverse;
		break;
	case 3:
		coeff_ptr = &poly_5000MHz_c_x2_reverse;
		break;
	case 4:
		coeff_ptr = &poly_5000MHz_c_xf_reverse;
		break;
	case 5:
		coeff_ptr = &poly_5000MHz_c_f2_reverse;
		break;
	case 6:
		coeff_ptr = &poly_5000MHz_c_x3_reverse;
		break;
	case 7:
		coeff_ptr = &poly_5000MHz_c_x2f_reverse;
		break;
	case 8:
		coeff_ptr = &poly_5000MHz_c_xf2_reverse;
		break;
	case 9:
		coeff_ptr = &poly_5000MHz_c_f3_reverse;
		break;
	default:
		return -EINVAL;
	}

	// Store the double value directly (no conversion needed)
	*coeff_ptr = double_value;

	// Write to EEPROM
	ret = powrms_eeprom_write_poly_5000MHz_reverse_coeffs(
		      &poly_5000MHz_intercept_reverse);
	if (ret != IIO_SUCCESS)
		;

	return len;
}


int powrms_buffers_init(void)
{
	int ret;

	ret = no_os_cb_init(&powrms_vin0_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error;

	ret = no_os_cb_init(&powrms_vin1_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_vin0;

	ret = no_os_cb_init(&powrms_p_fwd_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_vin1;

	ret = no_os_cb_init(&powrms_p_rev_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_p_fwd;

	return 0;

error_p_fwd:
	no_os_cb_remove(powrms_p_fwd_buffer);
error_vin1:
	no_os_cb_remove(powrms_vin1_buffer);
error_vin0:
	no_os_cb_remove(powrms_vin0_buffer);
error:
	return ret;
}

void powrms_buffers_cleanup(void)
{
	if (powrms_vin0_buffer)
		no_os_cb_remove(powrms_vin0_buffer);
	if (powrms_vin1_buffer)
		no_os_cb_remove(powrms_vin1_buffer);
	if (powrms_p_fwd_buffer)
		no_os_cb_remove(powrms_p_fwd_buffer);
	if (powrms_p_rev_buffer)
		no_os_cb_remove(powrms_p_rev_buffer);
}

int powrms_push_vin0(float value)
{
	if (!powrms_vin0_buffer)
		return -ENODEV;
	return no_os_cb_write(powrms_vin0_buffer, &value, sizeof(float));
}

int powrms_push_vin1(float value)
{
	if (!powrms_vin1_buffer)
		return -ENODEV;
	return no_os_cb_write(powrms_vin1_buffer, &value, sizeof(float));
}

int powrms_push_p_fwd(float value)
{
	if (!powrms_p_fwd_buffer)
		return -ENODEV;
	return no_os_cb_write(powrms_p_fwd_buffer, &value, sizeof(float));
}

int powrms_push_p_rev(float value)
{
	if (!powrms_p_rev_buffer)
		return -ENODEV;
	return no_os_cb_write(powrms_p_rev_buffer, &value, sizeof(float));
}

int powrms_get_latest_vin0(float *value)
{
	if (!powrms_vin0_buffer || !value)
		return -ENODEV;
	return no_os_cb_read(powrms_vin0_buffer, value, sizeof(float));
}

int powrms_get_latest_vin1(float *value)
{
	if (!powrms_vin1_buffer || !value)
		return -ENODEV;
	return no_os_cb_read(powrms_vin1_buffer, value, sizeof(float));
}

int powrms_get_latest_p_fwd(float *value)
{
	if (!powrms_p_fwd_buffer || !value)
		return -ENODEV;
	return no_os_cb_read(powrms_p_fwd_buffer, value, sizeof(float));
}

int powrms_get_latest_p_rev(float *value)
{
	if (!powrms_p_rev_buffer || !value)
		return -ENODEV;
	return no_os_cb_read(powrms_p_rev_buffer, value, sizeof(float));
}

// Utility functions for external access to precision values
int powrms_set_precision_value(int index, float value)
{
	if (index < 0 || index >= PRECISION_ARRAY_SIZE)
		return -EINVAL;
	// Convert float to integer for internal storage
	precision_values[index] = (int32_t)(value * PRECISION_SCALE_FACTOR);
	return 0;
}

float powrms_get_precision_value(int index)
{
	if (index < 0 || index >= PRECISION_ARRAY_SIZE)
		return 0.0f;
	// Convert integer to float for external use
	return (float)precision_values[index] * PRECISION_RESOLUTION;
}

int powrms_set_precision_array(const float *values, int count)
{
	if (!values || count > PRECISION_ARRAY_SIZE)
		return -EINVAL;

	for (int i = 0; i < count; i++) {
		// Convert float to integer for internal storage
		precision_values[i] = (int32_t)(values[i] * PRECISION_SCALE_FACTOR);
	}
	return 0;
}

int powrms_get_precision_array(float *values, int max_count)
{
	if (!values)
		return -EINVAL;

	int count = (max_count < PRECISION_ARRAY_SIZE) ? max_count :
		    PRECISION_ARRAY_SIZE;
	for (int i = 0; i < count; i++) {
		// Convert integer to float for external use
		values[i] = (float)precision_values[i] * PRECISION_RESOLUTION;
	}
	return count;
}

// Direct integer access functions for MCU internal use (no conversion overhead)
int powrms_set_precision_value_raw(int index, int32_t raw_value)
{
	if (index < 0 || index >= PRECISION_ARRAY_SIZE)
		return -EINVAL;
	precision_values[index] = raw_value;
	return 0;
}

int32_t powrms_get_precision_value_raw(int index)
{
	if (index < 0 || index >= PRECISION_ARRAY_SIZE)
		return 0;
	return precision_values[index];
}

int powrms_set_precision_array_raw(const int32_t *raw_values, int count)
{
	if (!raw_values || count > PRECISION_ARRAY_SIZE)
		return -EINVAL;

	for (int i = 0; i < count; i++) {
		precision_values[i] = raw_values[i];
	}
	return 0;
}

int powrms_get_precision_array_raw(int32_t *raw_values, int max_count)
{
	if (!raw_values)
		return -EINVAL;

	int count = (max_count < PRECISION_ARRAY_SIZE) ? max_count :
		    PRECISION_ARRAY_SIZE;
	for (int i = 0; i < count; i++) {
		raw_values[i] = precision_values[i];
	}
	return count;
}

// Reverse precision array utility functions for external access
int powrms_set_precision_value_reverse(int index, float value)
{
	if (index < 0 || index >= PRECISION_ARRAY_SIZE)
		return -EINVAL;
	// Convert float to integer for internal storage
	precision_values_reverse[index] = (int32_t)(value * PRECISION_SCALE_FACTOR);
	return 0;
}

float powrms_get_precision_value_reverse(int index)
{
	if (index < 0 || index >= PRECISION_ARRAY_SIZE)
		return 0.0f;
	// Convert integer to float for external use
	return (float)precision_values_reverse[index] * PRECISION_RESOLUTION;
}

int powrms_set_precision_array_reverse(const float *values, int count)
{
	if (!values || count > PRECISION_ARRAY_SIZE)
		return -EINVAL;

	for (int i = 0; i < count; i++) {
		// Convert float to integer for internal storage
		precision_values_reverse[i] = (int32_t)(values[i] * PRECISION_SCALE_FACTOR);
	}
	return 0;
}

int powrms_get_precision_array_reverse(float *values, int max_count)
{
	if (!values)
		return -EINVAL;

	int count = (max_count < PRECISION_ARRAY_SIZE) ? max_count :
		    PRECISION_ARRAY_SIZE;
	for (int i = 0; i < count; i++) {
		// Convert integer to float for external use
		values[i] = (float)precision_values_reverse[i] * PRECISION_RESOLUTION;
	}
	return count;
}

// Direct integer access functions for MCU internal use (no conversion overhead) - reverse
int powrms_set_precision_value_raw_reverse(int index, int32_t raw_value)
{
	if (index < 0 || index >= PRECISION_ARRAY_SIZE)
		return -EINVAL;
	precision_values_reverse[index] = raw_value;
	return 0;
}

int32_t powrms_get_precision_value_raw_reverse(int index)
{
	if (index < 0 || index >= PRECISION_ARRAY_SIZE)
		return 0;
	return precision_values_reverse[index];
}

int powrms_set_precision_array_raw_reverse(const int32_t *raw_values, int count)
{
	if (!raw_values || count > PRECISION_ARRAY_SIZE)
		return -EINVAL;

	for (int i = 0; i < count; i++) {
		precision_values_reverse[i] = raw_values[i];
	}
	return 0;
}

int powrms_get_precision_array_raw_reverse(int32_t *raw_values, int max_count)
{
	if (!raw_values)
		return -EINVAL;

	int count = (max_count < PRECISION_ARRAY_SIZE) ? max_count :
		    PRECISION_ARRAY_SIZE;
	for (int i = 0; i < count; i++) {
		raw_values[i] = precision_values_reverse[i];
	}
	return count;
}

// Frequency ranges utility functions for external access
int powrms_set_frequency_range(int index, uint32_t frequency_MHz)
{
	if (index < 0 || index >= FREQUENCY_RANGE_NR)
		return -EINVAL;
	frequency_MHz_ranges[index] = frequency_MHz;
	return 0;
}

uint32_t powrms_get_frequency_range(int index)
{
	if (index < 0 || index >= FREQUENCY_RANGE_NR)
		return 0;
	return frequency_MHz_ranges[index];
}

int powrms_set_frequency_ranges(const uint32_t *frequencies, int count)
{
	if (!frequencies || count > FREQUENCY_RANGE_NR)
		return -EINVAL;

	for (int i = 0; i < count; i++) {
		frequency_MHz_ranges[i] = frequencies[i];
	}
	return 0;
}

int powrms_get_frequency_ranges(uint32_t *frequencies, int max_count)
{
	if (!frequencies)
		return -EINVAL;

	int count = (max_count < FREQUENCY_RANGE_NR) ? max_count : FREQUENCY_RANGE_NR;
	for (int i = 0; i < count; i++) {
		frequencies[i] = frequency_MHz_ranges[i];
	}
	return count;
}

/**
 * @brief Read dev_mode_overwrite_def_calib_values attribute
 *
 * This attribute always reads as 0 since it's a write-only trigger.
 * When written with 1, it saves the current precision values to the default calibration position.
 *
 * @param device - IIO device instance (unused)
 * @param buf - Buffer to store the attribute value
 * @param len - Buffer length
 * @param channel - Channel information (unused)
 * @param priv - Private data (unused)
 * @return Number of bytes written to buffer
 */
static int read_dev_mode_overwrite_def_calib_values(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	// Always return 0 as this is a write-trigger attribute
	return snprintf(buf, len, "%d", CALIBRATION_DISABLED);
}

/**
 * @brief Write dev_mode_overwrite_def_calib_values attribute
 *
 * When this attribute is written with value 1, it saves the current precision_values
 * array to the MEM_DEF_PRECISION_ARRAY_POZ position in EEPROM. Any other value is ignored.
 *
 * @param device - IIO device instance (unused)
 * @param buf - Buffer containing the value to write
 * @param len - Buffer length
 * @param channel - Channel information (unused)
 * @param priv - Private data (unused)
 * @return Number of bytes processed on success, negative error code on failure
 */
static int write_dev_mode_overwrite_def_calib_values(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;

	// Parse the input value
	if (sscanf(buf, "%"PRId32"", &val) == 1) {
		// Only act when value is 1
		if (val == DEV_MODE_TRIGGER_VALUE) {
			// Save current precision values to default calibration position in EEPROM
			int ret = powrms_eeprom_write_def_precision_array(precision_values);
			if (ret != IIO_SUCCESS)
				return len;
			powrms_watchdog_reset();
			ret = powrms_eeprom_write_def_precision_array_reverse(precision_values_reverse);
			if (ret != IIO_SUCCESS)
				return len;
			powrms_watchdog_reset();
			ret = powrms_eeprom_write_def_v_temp_comp_val(temperature_compensation_value);
			if (ret != IIO_SUCCESS)
				return len;
			powrms_watchdog_reset();

			// Save polynomial calibration coefficients for 5000MHz to factory defaults
			double poly_coeffs[10] = {
				poly_5000MHz_intercept,
				poly_5000MHz_c_x,
				poly_5000MHz_c_f,
				poly_5000MHz_c_x2,
				poly_5000MHz_c_xf,
				poly_5000MHz_c_f2,
				poly_5000MHz_c_x3,
				poly_5000MHz_c_x2f,
				poly_5000MHz_c_xf2,
				poly_5000MHz_c_f3
			};
			ret = powrms_eeprom_write_def_poly_5000MHz_coeffs(poly_coeffs);
			if (ret != IIO_SUCCESS)
				return len;
			powrms_watchdog_reset();

			// Save polynomial calibration coefficients for 5000MHz reverse to factory defaults
			double poly_coeffs_reverse[10] = {
				poly_5000MHz_intercept_reverse,
				poly_5000MHz_c_x_reverse,
				poly_5000MHz_c_f_reverse,
				poly_5000MHz_c_x2_reverse,
				poly_5000MHz_c_xf_reverse,
				poly_5000MHz_c_f2_reverse,
				poly_5000MHz_c_x3_reverse,
				poly_5000MHz_c_x2f_reverse,
				poly_5000MHz_c_xf2_reverse,
				poly_5000MHz_c_f3_reverse
			};
			ret = powrms_eeprom_write_def_poly_5000MHz_reverse_coeffs(poly_coeffs_reverse);
			if (ret != IIO_SUCCESS)
				return len;
			powrms_watchdog_reset();

			// All EEPROM writes successful
			return len;
		} else {
			// For any other value (including 0), just acknowledge without action
			return len;
		}
	}
	return -EINVAL;  // Invalid input format
}
