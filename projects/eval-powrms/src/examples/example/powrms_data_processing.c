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
#include <math.h>

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
	.adc_averaging_nr = 1,
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

// Storage for the 112 precision values as 32-bit integers
// Order 10MHz: X1, Y1, X2, Y2, X3, Y3 , X4, Y4, X5, Y5, X6, Y6, X7, Y7, 100MHz:...
int32_t precision_values[PRECISION_ARRAY_SIZE];
int32_t precision_values_reverse[PRECISION_ARRAY_SIZE];

// Polynomial calibration coefficients for 5000MHz (stored as double for 20 decimal precision)
double poly_5000MHz_intercept = 0;
double poly_5000MHz_c_x = 0;
double poly_5000MHz_c_f = 0;
double poly_5000MHz_c_x2 = 0;
double poly_5000MHz_c_xf = 0;
double poly_5000MHz_c_f2 = 0;
double poly_5000MHz_c_x3 = 0;
double poly_5000MHz_c_x2f = 0;
double poly_5000MHz_c_xf2 = 0;
double poly_5000MHz_c_f3 = 0;

// Polynomial calibration coefficients for 5000MHz reverse (stored as double for 20 decimal precision)
double poly_5000MHz_intercept_reverse = 0;
double poly_5000MHz_c_x_reverse = 0;
double poly_5000MHz_c_f_reverse = 0;
double poly_5000MHz_c_x2_reverse = 0;
double poly_5000MHz_c_xf_reverse = 0;
double poly_5000MHz_c_f2_reverse = 0;
double poly_5000MHz_c_x3_reverse = 0;
double poly_5000MHz_c_x2f_reverse = 0;
double poly_5000MHz_c_xf2_reverse = 0;
double poly_5000MHz_c_f3_reverse = 0;

// Storage for the 24 temperature correction coefficients as 32-bit integers
// Order A1, A2, A3 for each frequency range
double temperature_precision_values[TEMPERATURE_CORRECTION_COEFFS *
								  FREQUENCY_RANGE_NR] = {
	0.09907037, -0.006107942, -0.0176689,
	0.5951664, -0.409783, 0.08516236,
	-0.9938435, 0.6969325, -0.168762,
	0.08790697, -0.09423475, 0.01784584,
	1.005991, -0.5792234, 0.08180364,
	-6.526894, 4.923146, -1.258186,
	6.637077, -4.713627, 1.078375,
	-3.673048, 2.75258, -0.7360349,
};



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
			       double *x_values_rev, double *y_values_rev,
			       uint8_t freq_index, uint32_t local_frequency_MHz, float *position)
{
	if (freq_index < FREQUENCY_RANGE_NR - 1) {
		// Interpolation between frequency ranges
		// Find position factor for interpolation between frequency ranges
		*position = (float)(local_frequency_MHz - frequency_MHz_ranges[freq_index]) /
			    (float)(frequency_MHz_ranges[freq_index + 1] -
				    frequency_MHz_ranges[freq_index]);

		// Calculate x1, x2, x3, y1, y2, y3 using floating-point interpolation
		for (int i = 0; i < PRECISION_POINTS_FREQ; i++) {
			x_values[i] = (double)((float)precision_values[PRECISION_POINTS_FREQ_ALL *
					       freq_index + i * 2] +
					       ((float)precision_values[PRECISION_POINTS_FREQ_ALL * (freq_index + 1) + i * 2] -
						(float)precision_values[PRECISION_POINTS_FREQ_ALL * freq_index + i * 2]) *
					       (*position)) /
				      PRECISION_SCALE_FACTOR;
			y_values[i] = (double)((float)precision_values[PRECISION_POINTS_FREQ_ALL *
					       freq_index + i * 2 + 1] +
					       ((float)precision_values[PRECISION_POINTS_FREQ_ALL * (freq_index + 1) + i * 2 +
											 1] -
						(float)precision_values[PRECISION_POINTS_FREQ_ALL * freq_index + i * 2 + 1]) *
					       (*position)) /
				      PRECISION_SCALE_FACTOR;
			x_values_rev[i] = (double)((float)
						   precision_values_reverse[PRECISION_POINTS_FREQ_ALL *
											     freq_index + i * 2] +
						   ((float)precision_values_reverse[PRECISION_POINTS_FREQ_ALL *
											     (freq_index + 1) + i * 2] -
						    (float)precision_values_reverse[PRECISION_POINTS_FREQ_ALL * freq_index + i * 2])
						   *
						   (*position)) /
					  PRECISION_SCALE_FACTOR;
			y_values_rev[i] = (double)((float)
						   precision_values_reverse[PRECISION_POINTS_FREQ_ALL *
											     freq_index + i * 2 + 1] +
						   ((float)precision_values_reverse[PRECISION_POINTS_FREQ_ALL *
											     (freq_index + 1) + i * 2 +
											     1] -
						    (float)precision_values[PRECISION_POINTS_FREQ_ALL * freq_index + i * 2 + 1]) *
						   (*position)) /
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
		for (int i = 0; i < PRECISION_POINTS_FREQ; i++) {
			int32_t prev_x = precision_values[PRECISION_POINTS_FREQ_ALL * prev_range_idx + i
										    * 2];
			int32_t curr_x = precision_values[PRECISION_POINTS_FREQ_ALL * curr_range_idx + i
										    * 2];
			int32_t prev_y = precision_values[PRECISION_POINTS_FREQ_ALL * prev_range_idx + i
										    * 2 + 1];
			int32_t curr_y = precision_values[PRECISION_POINTS_FREQ_ALL * curr_range_idx + i
										    * 2 + 1];

			int32_t prev_x_rev = precision_values_reverse[PRECISION_POINTS_FREQ_ALL *
								       prev_range_idx + i
								       * 2];
			int32_t curr_x_rev = precision_values_reverse[PRECISION_POINTS_FREQ_ALL *
								       curr_range_idx + i
								       * 2];
			int32_t prev_y_rev = precision_values_reverse[PRECISION_POINTS_FREQ_ALL *
								       prev_range_idx + i
								       * 2 + 1];
			int32_t curr_y_rev = precision_values_reverse[PRECISION_POINTS_FREQ_ALL *
								       curr_range_idx + i
								       * 2 + 1];

			// Linear extrapolation: value = curr + (curr - prev) * extrap_factor
			x_values[i] = (double)((float)curr_x + (float)(curr_x - prev_x) *
					       extrap_factor) / PRECISION_SCALE_FACTOR;
			y_values[i] = (double)((float)curr_y + (float)(curr_y - prev_y) *
					       extrap_factor) / PRECISION_SCALE_FACTOR;
			x_values_rev[i] = (double)((float)curr_x_rev + (float)(curr_x_rev - prev_x_rev)
						   *
						   extrap_factor) / PRECISION_SCALE_FACTOR;
			y_values_rev[i] = (double)((float)curr_y_rev + (float)(curr_y_rev - prev_y_rev)
						   *
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
					 TEMPERATURE_CORRECTION_COEFFS + i];
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

double _Lagrange_interpolaiton(double *x_values, double *y_values,
			       uint8_t points_number, double x_input)
{
	double y_output = 0.0;
	for (int i = 0; i < points_number; i++) {
		double term = y_values[i];
		for (int j = 0; j < points_number; j++) {
			if (j != i) {
				term *= (x_input - x_values[j]) / (x_values[i] - x_values[j]);
			}
		}
		y_output += term;
	}
	return y_output;
}

// Helper function to compute PCHIP derivatives (monotonicity-preserving)
static void _pchip_compute_derivatives(double *x_values, double *y_values,
				       uint8_t points_number, double *derivatives)
{
	if (points_number < 2) {
		return;
	}

	// Handle special case of only 2 points (linear interpolation)
	if (points_number == 2) {
		double slope = (y_values[1] - y_values[0]) / (x_values[1] - x_values[0]);
		derivatives[0] = slope;
		derivatives[1] = slope;
		return;
	}

	// Calculate segment lengths and slopes
	double hk[points_number - 1];  // segment lengths
	double mk[points_number - 1];  // segment slopes

	for (int i = 0; i < points_number - 1; i++) {
		hk[i] = x_values[i + 1] - x_values[i];
		mk[i] = (y_values[i + 1] - y_values[i]) / hk[i];
	}

	// Initialize derivatives array
	for (int i = 0; i < points_number; i++) {
		derivatives[i] = 0.0;
	}

	// Calculate derivatives for internal points using PCHIP algorithm
	for (int i = 1; i < points_number - 1; i++) {
		double mk_prev = mk[i - 1];  // slope of previous segment
		double mk_curr = mk[i];      // slope of current segment

		// Check monotonicity condition:
		// If signs differ or either slope is zero, set derivative to zero
		if ((mk_prev > 0.0 && mk_curr < 0.0) ||
		    (mk_prev < 0.0 && mk_curr > 0.0) ||
		    mk_prev == 0.0 || mk_curr == 0.0) {
			derivatives[i] = 0.0;
		} else {
			// Use weighted harmonic mean
			double w1 = 2.0 * hk[i] + hk[i - 1];      // 2*h_k + h_{k-1}
			double w2 = hk[i] + 2.0 * hk[i - 1];      // h_k + 2*h_{k-1}

			// Weighted harmonic mean: 1/d_k = (w1/m_{k-1} + w2/m_k) / (w1 + w2)
			double denominator = (w1 / mk_prev + w2 / mk_curr) / (w1 + w2);
			derivatives[i] = 1.0 / denominator;
		}
	}

	// Calculate endpoint derivatives using one-sided three-point estimate
	// Left endpoint (i=0)
	double h0 = hk[0];
	double h1 = hk[1];
	double m0 = mk[0];
	double m1 = mk[1];

	double d_left = ((2.0 * h0 + h1) * m0 - h0 * m1) / (h0 + h1);

	// Preserve shape at left endpoint
	if ((d_left > 0.0 && m0 < 0.0) || (d_left < 0.0 && m0 > 0.0)) {
		derivatives[0] = 0.0;
	} else if ((m0 > 0.0 && m1 < 0.0) || (m0 < 0.0 && m1 > 0.0)) {
		if (fabs(d_left) > 3.0 * fabs(m0)) {
			derivatives[0] = 3.0 * m0;
		} else {
			derivatives[0] = d_left;
		}
	} else {
		derivatives[0] = d_left;
	}

	// Right endpoint (i=points_number-1)
	double h_n2 = hk[points_number - 2];
	double h_n3 = hk[points_number - 3];
	double m_n2 = mk[points_number - 2];
	double m_n3 = mk[points_number - 3];

	double d_right = ((2.0 * h_n2 + h_n3) * m_n2 - h_n2 * m_n3) / (h_n2 + h_n3);

	// Preserve shape at right endpoint
	if ((d_right > 0.0 && m_n2 < 0.0) || (d_right < 0.0 && m_n2 > 0.0)) {
		derivatives[points_number - 1] = 0.0;
	} else if ((m_n2 > 0.0 && m_n3 < 0.0) || (m_n2 < 0.0 && m_n3 > 0.0)) {
		if (fabs(d_right) > 3.0 * fabs(m_n2)) {
			derivatives[points_number - 1] = 3.0 * m_n2;
		} else {
			derivatives[points_number - 1] = d_right;
		}
	} else {
		derivatives[points_number - 1] = d_right;
	}
}

static double _piecewise_cubic_Hermite_interpolation(double *x_values,
		double *y_values,
		uint8_t points_number, double x_input)
{
	// Compute PCHIP derivatives
	double derivatives[points_number];
	_pchip_compute_derivatives(x_values, y_values, points_number, derivatives);

	// Find the interval where x_input lies
	int interval = -1;
	for (int i = 0; i < points_number - 1; i++) {
		if (x_input >= x_values[i] && x_input <= x_values[i + 1]) {
			interval = i;
			break;
		}
	}

	// If x_input is outside the range, return the closest endpoint value
	if (interval == -1) {
		if (x_input < x_values[0]) {
			return y_values[0];
		} else {
			return y_values[points_number - 1];
		}
	}

	// Get interval data
	double x0 = x_values[interval];
	double x1 = x_values[interval + 1];
	double y0 = y_values[interval];
	double y1 = y_values[interval + 1];
	double d0 = derivatives[interval];
	double d1 = derivatives[interval + 1];

	// Normalize the input to [0,1] within the interval
	double h = x1 - x0;
	double t = (x_input - x0) / h;

	// Calculate Hermite basis functions
	double h00 = 2.0 * t * t * t - 3.0 * t * t + 1.0; // (1 + 2t)(1-t)²
	double h10 = t * t * t - 2.0 * t * t + t;  // t(1-t)²
	double h01 = -2.0 * t * t * t + 3.0 * t * t; // t²(3-2t)
	double h11 = t * t * t - t * t;            // t²(t-1)

	// Calculate the interpolated value using cubic Hermite interpolation
	// Note: derivatives need to be scaled by interval width for proper Hermite interpolation
	double y_output = h00 * y0 + h10 * (d0 * h) + h01 * y1 + h11 * (d1 * h);

	return y_output;
}

double _correct_newtone(double newtone_meas, int32_t freq_MHz)
{
	double f = (double)freq_MHz / 1000.0; // Convert MHz to GHz
	return (201.7376
		+ 0.9386 * newtone_meas
		- 72.9199 * f
		- 0.0003276 * (newtone_meas * newtone_meas)
		+ 0.0104111 * newtone_meas * f
		+ 6.52708 * (f * f));
}

double _correct_newtone_3rd_order(double newtone_meas, int32_t freq_MHz,
				  bool reverse)
{
	double x = newtone_meas;
	double f = (double)freq_MHz / 1000.0; // Convert MHz to GHz

	if (reverse) {
		return (poly_5000MHz_intercept_reverse
			+ poly_5000MHz_c_x_reverse * x
			+ poly_5000MHz_c_f_reverse * f
			+ poly_5000MHz_c_x2_reverse * (x * x)
			+ poly_5000MHz_c_xf_reverse * (x * f)
			+ poly_5000MHz_c_f2_reverse * (f * f)
			+ poly_5000MHz_c_x3_reverse * (x * x * x)
			+ poly_5000MHz_c_x2f_reverse * (x * x * f)
			+ poly_5000MHz_c_xf2_reverse * (x * f * f)
			+ poly_5000MHz_c_f3_reverse * (f * f * f));
	}

	return (poly_5000MHz_intercept
		+ poly_5000MHz_c_x * x
		+ poly_5000MHz_c_f * f
		+ poly_5000MHz_c_x2 * (x * x)
		+ poly_5000MHz_c_xf * (x * f)
		+ poly_5000MHz_c_f2 * (f * f)
		+ poly_5000MHz_c_x3 * (x * x * x)
		+ poly_5000MHz_c_x2f * (x * x * f)
		+ poly_5000MHz_c_xf2 * (x * f * f)
		+ poly_5000MHz_c_f3 * (f * f * f));
}


int calculate_power()
{
	double temp_compensation = (double)temperature_compensation_value /
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
	x_values[PRECISION_POINTS_FREQ];  // Values used for POWER calculations, Y axis points, output power
	static double
	y_values[PRECISION_POINTS_FREQ];  // Values used for POWER calculations, X axis points, input voltage
	static double
	x_values_rev[PRECISION_POINTS_FREQ];  // Values used for POWER calculations, Y axis points, output power
	static double
	y_values_rev[PRECISION_POINTS_FREQ];  // Values used for POWER calculations, X axis points, input voltage

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
		ret = _calc_interpolating_values(x_values, y_values, x_values_rev, y_values_rev,
						 freq_index,
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
				      temp_corr_coeffs[1], temp_corr_coeffs[2]) + _cubic_function(temp_compensation,
						      temp_corr_coeffs[0], temp_corr_coeffs[1], temp_corr_coeffs[2]);
	VIN1 = VIN1 - _cubic_function(VIN_TEMP_float, temp_corr_coeffs[0],
				      temp_corr_coeffs[1], temp_corr_coeffs[2]) + _cubic_function(temp_compensation,
						      temp_corr_coeffs[0], temp_corr_coeffs[1], temp_corr_coeffs[2]);

	double VIN0_next = VIN0 - _cubic_function(VIN_TEMP_float, temp_corr_coeffs[3],
			   temp_corr_coeffs[4], temp_corr_coeffs[5]) + _cubic_function(temp_compensation,
					   temp_corr_coeffs[3], temp_corr_coeffs[4], temp_corr_coeffs[5]);
	double VIN1_next = VIN1 - _cubic_function(VIN_TEMP_float, temp_corr_coeffs[3],
			   temp_corr_coeffs[4], temp_corr_coeffs[5]) + _cubic_function(temp_compensation,
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

	// Convert results back to volts using consistent scale factor
	// precision_values are stored with 10^3 scale factor (PRECISION_SCALE_FACTOR)
	adc_data_input.adc_p_fwd = _piecewise_cubic_Hermite_interpolation(x_values,
				   y_values,
				   PRECISION_POINTS_FREQ, VIN0_float);
	adc_data_input.adc_p_rev = _piecewise_cubic_Hermite_interpolation(x_values_rev,
				   y_values_rev,
				   PRECISION_POINTS_FREQ, VIN1_float);

// Extra requirement from 12.11.2025
	if (local_frequency_MHz > 5000 && local_frequency_MHz < 6000) {
		adc_data_input.adc_p_fwd = _correct_newtone_3rd_order(adc_data_input.adc_p_fwd,
					   local_frequency_MHz, false);
		adc_data_input.adc_p_rev = _correct_newtone_3rd_order(adc_data_input.adc_p_rev,
					   local_frequency_MHz, true);
	}

	return 0;
}
