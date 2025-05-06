/***************************************************************************//**
 *   @file   powrms_data_processing.c
 *   @brief  POWRMS data processing source file for POWRMS project
 *   @author Robert Budai (robert.budai@analog.com)
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

#include "powrms_data_processing.h"

int32_t use_eeprom_calibration_data = 0;

// Storage for frequency ranges for callibration
uint32_t frequency_MHz_ranges[FREQUENCY_RANGE_NR] = {
	10,
	100,
	1000,
	2000,
	3000,
	4000,
	5000,
	6000,
};

// Define the global ADC data variable
volatile struct adc_data adc_data_input = {
	.adc_p_fwd_raw = 0.0,
	.adc_p_rev_raw = 0.0,
	.adc_vin0_voltage = 0.0,
	.adc_vin1_voltage = 0.0,
	.adc_vin2_voltage = 0.0,
};

struct powrms_variables input_variables[VARIABLE_NUMBER] = {
	{
		.name = SIGNAL_FREQUENCY, // MHz
		.value = 1000.0,
		.min_possible_value = 10.0,
		.max_possible_value = 7000.0,
		.nr_of_int_digits = INTEGER_NUMERIC_LENGTH,
		.nr_of_float_digits = FLOATING_POINT_LENGTH,
		.digits = {'1', '0', '0', '0', '0', '0'},
		.unit = " MHz"
	},
	{
		.name = INPUT_IMPEDANCE,
		.value = 50.0,
		.min_possible_value = 0.0,
		.max_possible_value = 1000.0,
		.nr_of_int_digits = INTEGER_NUMERIC_LENGTH,
		.nr_of_float_digits = FLOATING_POINT_LENGTH,
		.digits = {'0', '0', '5', '0', '0', '0'},
		.unit = " Ohm"
	},
	{
		.name = OUTPUT_IMPEDANCE,
		.value = 50.0,
		.min_possible_value = 0.0,
		.max_possible_value = 1000.0,
		.nr_of_int_digits = INTEGER_NUMERIC_LENGTH,
		.nr_of_float_digits = FLOATING_POINT_LENGTH,
		.digits = {'0', '0', '5', '0', '0', '0'},
		.unit = " Ohm"

	},
};

// Storage for the 48 precision values as 32-bit integers
// Order X1, Y1, X2, Y2, X3, Y3 ....
int32_t precision_values[PRECISION_ARRAY_SIZE];

// Storage for the 24 temperature correction coefficients as 32-bit integers
// Order A1, A2, A3 for each frequency range
int32_t temperature_precision_values[TEMPERATURE_CORRECTION_COEFFS *
								   FREQUENCY_RANGE_NR];

// Storage for the temperature compensation value as 32-bit integer
int32_t temperature_compensation_value;

/**
 * @brief Update the value of a specific digit in the input variable
 */
void update_values_of_digit(struct powrms_variables *input_var,
			    uint8_t increment_pos)
{
	double new_value = 0;
	double divisor = 1.0 / 10.0;

	char temp_digits[NUMERIC_LENGTH + 1];
	memcpy(temp_digits, input_var->digits, NUMERIC_LENGTH);
	temp_digits[NUMERIC_LENGTH] = '\0';

	if (increment_pos < NUMERIC_LENGTH) {
		if (temp_digits[increment_pos] == '9') {
			temp_digits[increment_pos] = '0';
		} else {
			temp_digits[increment_pos]++;
		}
	}

	for (int i = 0; i < INTEGER_NUMERIC_LENGTH; i++) {
		new_value = new_value * 10 + (temp_digits[i] - '0');
	}
	for (int i = 0; i < FLOATING_POINT_LENGTH; i++) {
		new_value = new_value + (temp_digits[input_var->nr_of_int_digits + i] - '0') *
			    divisor;
		divisor = divisor / 10.0;
	}

	if (new_value >= input_var->min_possible_value
	    && new_value <= input_var->max_possible_value) {
		input_var->value = new_value;
		memcpy(input_var->digits, temp_digits, NUMERIC_LENGTH);
	} else {
		input_var->value = input_var->min_possible_value;
		powrms_float_to_str(input_var->value, input_var->digits,
				    input_var->nr_of_int_digits, input_var->nr_of_float_digits, true);
	}
}

void update_values_from_new_value(struct powrms_variables *input_var,
				  double new_value)
{
	// Validate the new value is within acceptable range
	if (new_value < input_var->min_possible_value) {
		new_value = input_var->min_possible_value;
	} else if (new_value > input_var->max_possible_value) {
		new_value = input_var->max_possible_value;
	}

	// Update the value
	input_var->value = new_value;

	// Convert the new value to digits array
	powrms_float_to_str((float)new_value, input_var->digits,
			    input_var->nr_of_int_digits, input_var->nr_of_float_digits, true);

}

void sync_all_digits_from_values(void)
{
	// Synchronize all input variables' digits arrays with their current values
	for (int i = 0; i < VARIABLE_NUMBER; i++) {
		update_values_from_new_value(&input_variables[i], input_variables[i].value);
	}
}

int _calc_interpolating_values(double *x_values, double *y_values,
			       uint8_t freq_index, uint32_t local_frequency_MHz, float *position)
{
	if (freq_index < FREQUENCY_RANGE_NR - 1) {
		// Interpolation between frequency ranges
		// Find position factor for interpolation between frequency ranges
		*position = (float)(local_frequency_MHz - frequency_MHz_ranges[freq_index]) /
			    (float)(frequency_MHz_ranges[freq_index + 1] -
				    frequency_MHz_ranges[freq_index]);

		// Calculate x1, x2, x3, y1, y2, y3 using floating-point interpolation
		for (int i = 0; i < 3; i++) {
			x_values[i] = (double)((float)precision_values[6 * freq_index + i * 2] +
					       ((float)precision_values[6 * (freq_index + 1) + i * 2] -
						(float)precision_values[6 * freq_index + i * 2]) * (*position)) /
				      PRECISION_SCALE_FACTOR;
			y_values[i] = (double)((float)precision_values[6 * freq_index + i * 2 + 1] +
					       ((float)precision_values[6 * (freq_index + 1) + i * 2 + 1] -
						(float)precision_values[6 * freq_index + i * 2 + 1]) * (*position)) /
				      PRECISION_SCALE_FACTOR;
		}
		return 0;
	} else if (freq_index == (FREQUENCY_RANGE_NR - 1)) {
		// Extrapolation beyond the highest frequency range using the last two frequency ranges (5GHz and 6GHz)
		// Use linear extrapolation based on the trend between 5GHz and 6GHz ranges
		int prev_range_idx = FREQUENCY_RANGE_NR - 2;  // Index for 5GHz range (6)
		int curr_range_idx = FREQUENCY_RANGE_NR - 1;  // Index for 6GHz range (7)

		// Calculate extrapolation factor based on how far beyond 6GHz we are
		float extrap_factor = (float)(local_frequency_MHz -
					      frequency_MHz_ranges[curr_range_idx]) /
				      (float)(frequency_MHz_ranges[curr_range_idx] -
					      frequency_MHz_ranges[prev_range_idx]);

		// Calculate x1, x2, x3, y1, y2, y3 using linear extrapolation
		for (int i = 0; i < 3; i++) {
			int32_t prev_x = precision_values[6 * prev_range_idx + i * 2];
			int32_t curr_x = precision_values[6 * curr_range_idx + i * 2];
			int32_t prev_y = precision_values[6 * prev_range_idx + i * 2 + 1];
			int32_t curr_y = precision_values[6 * curr_range_idx + i * 2 + 1];

			// Linear extrapolation: value = curr + (curr - prev) * extrap_factor
			x_values[i] = (double)((float)curr_x + (float)(curr_x - prev_x) *
					       extrap_factor) / PRECISION_SCALE_FACTOR;
			y_values[i] = (double)((float)curr_y + (float)(curr_y - prev_y) *
					       extrap_factor) / PRECISION_SCALE_FACTOR;
		}
		return 0;
	} else
		return -EINVAL;
}

int _calc_temp_coefficients(float *temp_coeffs, uint8_t freq_index)
{
	if (freq_index < FREQUENCY_RANGE_NR - 1) {
		// Calculate temperature coefficients using floating-point arithmetic
		for (int i = 0; i < TEMPERATURE_CORRECTION_COEFFS * 2; i++)
			temp_coeffs[i] = (float)temperature_precision_values[freq_index *
					 TEMPERATURE_CORRECTION_COEFFS + i] / (float)PRECISION_SCALE_FACTOR;
		return 0;
	} else {
		return -EINVAL;
	}
}

int32_t _cubic_function(float v_tmp, float a1, float a2, float a3)
{
	return (int32_t)((a1 * v_tmp + a2 * v_tmp * v_tmp + a3 * v_tmp * v_tmp * v_tmp)
			 * PRECISION_SCALE_FACTOR);
}

int calculate_power()
{
	double v_temp_calib = (double)temperature_compensation_value /
			      (double)PRECISION_SCALE_FACTOR;
	double VIN_TEMP_float = adc_data_input.adc_vin2_voltage;
	double VIN0_float = adc_data_input.adc_vin0_voltage;
	double VIN1_float = adc_data_input.adc_vin1_voltage;
	static float
	position_between_freq_ranges; // Percentage proportion of measured frequency compared to available intervals: (F_meas - F_prev) / (F_next - F_prev)
	static uint32_t local_frequency_MHz; // Cached frequency to detect changes
	static float
	temp_corr_coeffs[6]; // Both bottom and top limit coeffs, A1, A2, A3, B1, B2, B3
	static double
	y_values[3];  // Values used for POWER calculations, X axis points, input voltage
	static double
	x_values[3];  // Values used for POWER calculations, Y axis points, output power
	static uint8_t freq_index; // Cached frequency index
	int ret;

	// Use floating-point arithmetic throughout to preserve precision
	// Scale input voltages to match precision values (microvolts)
	int32_t VIN0 = (int32_t)(VIN0_float * PRECISION_SCALE_FACTOR);
	int32_t VIN1 = (int32_t)(VIN1_float * PRECISION_SCALE_FACTOR);

	// Check if frequency is updated since last callback


	if (local_frequency_MHz != (uint32_t)input_variables[SIGNAL_FREQUENCY].value) {
		local_frequency_MHz = (uint32_t)input_variables[SIGNAL_FREQUENCY].value;

		// Find the closest frequency range index
		freq_index = 0;
		while (freq_index < FREQUENCY_RANGE_NR - 1
		       && local_frequency_MHz >= frequency_MHz_ranges[freq_index + 1])
			freq_index++;

		// Update interpolating reference points
		ret = _calc_interpolating_values(x_values, y_values, freq_index,
						 local_frequency_MHz, &position_between_freq_ranges);
		if (ret) {
			return -EINVAL;
		}

		// Update temperature correction coefficients
		ret = _calc_temp_coefficients(temp_corr_coeffs, freq_index);
		if (ret) {
			return -EINVAL;
		}
	}

	// Correct V_IN values bottom limit
	VIN0 = VIN0 - _cubic_function(VIN_TEMP_float, temp_corr_coeffs[0],
				      temp_corr_coeffs[1], temp_corr_coeffs[2]) + _cubic_function(v_temp_calib,
						      temp_corr_coeffs[0], temp_corr_coeffs[1], temp_corr_coeffs[2]);
	VIN1 = VIN1 - _cubic_function(VIN_TEMP_float, temp_corr_coeffs[0],
				      temp_corr_coeffs[1], temp_corr_coeffs[2]) + _cubic_function(v_temp_calib,
						      temp_corr_coeffs[0], temp_corr_coeffs[1], temp_corr_coeffs[2]);

	double VIN0_next = VIN0 - _cubic_function(VIN_TEMP_float, temp_corr_coeffs[3],
			   temp_corr_coeffs[4], temp_corr_coeffs[5]) + _cubic_function(v_temp_calib,
					   temp_corr_coeffs[3], temp_corr_coeffs[4], temp_corr_coeffs[5]);
	double VIN1_next = VIN1 - _cubic_function(VIN_TEMP_float, temp_corr_coeffs[3],
			   temp_corr_coeffs[4], temp_corr_coeffs[5]) + _cubic_function(v_temp_calib,
					   temp_corr_coeffs[3], temp_corr_coeffs[4], temp_corr_coeffs[5]);

	// Correct V_IN values top limit
	VIN0 = (int32_t)((double)VIN0 + (double)(VIN0_next - VIN0) *
			 position_between_freq_ranges);
	VIN1 = (int32_t)((double)VIN1 + (double)(VIN1_next - VIN1) *
			 position_between_freq_ranges);

	VIN0_float = (float)VIN0 / (float)PRECISION_SCALE_FACTOR;
	VIN1_float = (float)VIN1 / (float)PRECISION_SCALE_FACTOR;

	adc_data_input.adc_vin0_voltage_corrected = VIN0_float;
	adc_data_input.adc_vin1_voltage_corrected = VIN1_float;

	// 3-point Lagrange interpolation
	double y_0 = (VIN0_float - x_values[1]) * (VIN0_float - x_values[2]) / ((
				x_values[0] - x_values[1]) * (x_values[0] - x_values[2])) * y_values[0]
		     + (VIN0_float - x_values[0]) * (VIN0_float - x_values[2]) / ((
					     x_values[1] - x_values[0]) * (x_values[1] - x_values[2])) * y_values[1]
		     + (VIN0_float - x_values[0]) * (VIN0_float - x_values[1]) / ((
					     x_values[2] - x_values[0]) * (x_values[2] - x_values[1])) * y_values[2];

	double y_1 = (VIN1_float - x_values[1]) * (VIN1_float - x_values[2]) / ((
				x_values[0] - x_values[1]) * (x_values[0] - x_values[2])) * y_values[0]
		     + (VIN1_float - x_values[0]) * (VIN1_float - x_values[2]) / ((
					     x_values[1] - x_values[0]) * (x_values[1] - x_values[2])) * y_values[1]
		     + (VIN1_float - x_values[0]) * (VIN1_float - x_values[1]) / ((
					     x_values[2] - x_values[0]) * (x_values[2] - x_values[1])) * y_values[2];

	// Convert results back to volts using consistent scale factor
	// precision_values are stored with 10^7 scale factor (PRECISION_SCALE_FACTOR)
	adc_data_input.adc_p_fwd = y_0;
	adc_data_input.adc_p_rev = y_1;

	return 0;
}