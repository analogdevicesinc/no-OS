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
static int read_temp_freq_range(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int write_temp_freq_range(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int read_dev_mode_overwrite_def_calib_values(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int write_dev_mode_overwrite_def_calib_values(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int read_temperature_compensation_value(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int write_temperature_compensation_value(void *device, char *buf,
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
		.name = "calib_10MHz_values",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX,  // frequency index 0
	},
	{
		.name = "calib_100MHz_values",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 1,  // frequency index 1
	},
	{
		.name = "calib_1000MHz_values",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 2,  // frequency index 2
	},
	{
		.name = "calib_2000MHz_values",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 3,  // frequency index 3
	},
	{
		.name = "calib_3000MHz_values",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 4,  // frequency index 4
	},
	{
		.name = "calib_4000MHz_values",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 5,  // frequency index 5
	},
	{
		.name = "calib_5000MHz_values",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 6,  // frequency index 6
	},
	{
		.name = "calib_6000MHz_values",
		.show = read_calib_freq_range,
		.store = write_calib_freq_range,
		.priv = MAX_FREQ_RANGE_INDEX,  // frequency index 7
	},
	// New per-frequency range attributes for temperature calibration values
	{
		.name = "calib_temp_10MHz_values",
		.show = read_temp_freq_range,
		.store = write_temp_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX,  // frequency index 0
	},
	{
		.name = "calib_temp_100MHz_values",
		.show = read_temp_freq_range,
		.store = write_temp_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 1,  // frequency index 1
	},
	{
		.name = "calib_temp_1000MHz_values",
		.show = read_temp_freq_range,
		.store = write_temp_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 2,  // frequency index 2
	},
	{
		.name = "calib_temp_2000MHz_values",
		.show = read_temp_freq_range,
		.store = write_temp_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 3,  // frequency index 3
	},
	{
		.name = "calib_temp_3000MHz_values",
		.show = read_temp_freq_range,
		.store = write_temp_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 4,  // frequency index 4
	},
	{
		.name = "calib_temp_4000MHz_values",
		.show = read_temp_freq_range,
		.store = write_temp_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 5,  // frequency index 5
	},
	{
		.name = "calib_temp_5000MHz_values",
		.show = read_temp_freq_range,
		.store = write_temp_freq_range,
		.priv = MIN_FREQ_RANGE_INDEX + 6,  // frequency index 6
	},
	{
		.name = "calib_temp_6000MHz_values",
		.show = read_temp_freq_range,
		.store = write_temp_freq_range,
		.priv = MAX_FREQ_RANGE_INDEX,  // frequency index 7
	},
	{
		.name = "temperature_compensation_value",
		.show = read_temperature_compensation_value,
		.store = write_temperature_compensation_value,
		.priv = IIO_ATTR_SCALE,
	},
	{
		.name = "dev_mode_overwrite_def_calib_values",
		.show = read_dev_mode_overwrite_def_calib_values,
		.store = write_dev_mode_overwrite_def_calib_values,
		.priv = IIO_ATTR_SCALE,
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
		return snprintf(buf, len, "%.7f", (1.0 / ((double)(BUFFER_VAL_PRECISSION))));

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
		int bytes = snprintf(buf + written, remaining, "%.7f", float_value);

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

	if (len >= CALIB_VALUES_BUFFER_SIZE) {
		return -EINVAL;
	}

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

	if (index != FREQ_RANGE_VALUES_PER_RANGE) {
		return -EINVAL;  // Expected exactly 6 values
	}

	// Save to EEPROM (write the entire array since EEPROM functions work with full array)
	ret = powrms_eeprom_write_precision_array(precision_values);
	if (ret != IIO_SUCCESS) {
		// EEPROM write failed - values are still updated in RAM, so continue
		// This allows the system to function even without EEPROM persistence
	}

	return len;
}

static int read_temp_freq_range(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int freq_index = (int)priv;  // frequency range index (0-7)
	int written = 0;
	int remaining = len;
	int start_idx = freq_index *
			TEMP_COEFFS_PER_RANGE;  // Starting index for this frequency range (3 temp coeffs per range)

	// Write 3 temperature values for this frequency range as comma-separated list
	for (int i = 0; i < TEMP_COEFFS_PER_RANGE
	     && remaining > MIN_REMAINING_BUFFER_SIZE; i++) {
		float float_value = (float)temperature_precision_values[start_idx + i] *
				    PRECISION_RESOLUTION;
		int bytes = snprintf(buf + written, remaining, "%.7f", float_value);

		if (bytes < 0 || bytes >= remaining)
			break;

		written += bytes;
		remaining -= bytes;

		// Add comma separator except for last element
		if (i < TEMP_COEFFS_PER_RANGE - 1 && remaining > MIN_COMMA_BUFFER_SIZE) {
			buf[written++] = ',';
			remaining--;
		}
	}

	return written;
}

static int write_temp_freq_range(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	int freq_index = (int)priv;  // frequency range index (0-7)
	char *token;
	char *saveptr;
	char buf_copy[TEMP_VALUES_BUFFER_SIZE];  // Buffer for 3 values
	int index = 0;
	int start_idx = freq_index * TEMP_COEFFS_PER_RANGE;
	int ret;

	if (len >= TEMP_VALUES_BUFFER_SIZE) {
		return -EINVAL;
	}

	// Create a copy of the buffer for tokenization
	strncpy(buf_copy, buf, len);
	buf_copy[len] = '\0';

	// Parse comma-separated values
	token = strtok_r(buf_copy, ",", &saveptr);
	while (token != NULL && index < TEMP_COEFFS_PER_RANGE) {
		float float_value;
		if (sscanf(token, "%f", &float_value) == 1) {
			// Convert float to integer and store
			temperature_precision_values[start_idx + index] = (int32_t)(
						float_value * PRECISION_SCALE_FACTOR);
			index++;
		} else {
			return -EINVAL;
		}
		token = strtok_r(NULL, ",", &saveptr);
	}

	if (index != TEMP_COEFFS_PER_RANGE) {
		return -EINVAL;  // Expected exactly 3 values
	}

	// Save to EEPROM (write the entire array since EEPROM functions work with full array)
	ret = powrms_eeprom_write_temp_corr_array(temperature_precision_values);
	if (ret != IIO_SUCCESS) {
		// EEPROM write failed - values are still updated in RAM, so continue
		// This allows the system to function even without EEPROM persistence
	}

	return len;
}

int powrms_buffers_init(void)
{
	int ret;

	ret = no_os_cb_init(&powrms_vin0_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_vin0;

	ret = no_os_cb_init(&powrms_vin1_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_vin1;

	ret = no_os_cb_init(&powrms_p_fwd_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_p_fwd;

	ret = no_os_cb_init(&powrms_p_rev_buffer, POWRMS_CIRC_BUFF_SIZE);
	if (ret != 0)
		goto error_p_rev;

	return 0;

error_p_rev:
	no_os_cb_remove(powrms_p_rev_buffer);
error_p_fwd:
	no_os_cb_remove(powrms_p_fwd_buffer);
error_vin1:
	no_os_cb_remove(powrms_vin0_buffer);
error_vin0:
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
	return snprintf(buf, len, "%.7f", float_value);
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
		if (ret != IIO_SUCCESS) {
			// EEPROM write failed - values are still updated in RAM, so continue
			// This allows the system to function even without EEPROM persistence
		}

		return len;
	}
	return -EINVAL;  // Invalid input format
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
			if (ret != IIO_SUCCESS) {
				return len;
			}
			ret = powrms_eeprom_write_def_temp_corr_array(temperature_precision_values);
			if (ret != IIO_SUCCESS) {
				return len;
			}
			// Save current temperature compensation value to default position in EEPROM
			ret = powrms_eeprom_write_def_v_temp_comp_val(temperature_compensation_value);
			if (ret != IIO_SUCCESS) {
				return len;
			}
		} else {
			// For any other value (including 0), just acknowledge without action
			return len;
		}
	}
	return -EINVAL;  // Invalid input format
}
