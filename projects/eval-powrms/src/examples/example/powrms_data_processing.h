/***************************************************************************//**
 *   @file   powrms_data_processing.h
 *   @brief  POWRMS data processing header for POWRMS project
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
#ifndef __POWRMS_DATA_PROCESSING_H__
#define __POWRMS_DATA_PROCESSING_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "example.h"

#define VARIABLE_NUMBER			3
#define NUMERIC_LENGTH			6
#define INTEGER_NUMERIC_LENGTH	4
#define FLOATING_POINT_LENGTH	2

#define VARIABLE_NAMES  		{"Fq:", "Ri:", "Ro:"}

#define PRECISION_SCALE_FACTOR      1000  // 10^3 for 3-digit precision (integer)
#define PRECISION_RESOLUTION        (1.0 / PRECISION_SCALE_FACTOR)  // 10^-3 precision when converting to float


// Precision array definitions
#define FREQUENCY_RANGE_NR              8  // Number of frequency ranges for callibration
#define PRECISION_ARRAY_SIZE            224
#define PRECISION_POINTS_FREQ			14
#define PRECISION_POINTS_FREQ_ALL		(2 * PRECISION_POINTS_FREQ)
#define TEMPERATURE_CORRECTION_COEFFS   3


// Format AAAA.BB
// indexing starting from 0
enum variable_names {
	SIGNAL_FREQUENCY,
	INPUT_IMPEDANCE,
	OUTPUT_IMPEDANCE,
	NR_OF_INPUT_VARIABLES
};
// between 0 and 6
extern uint8_t pointer_poz_x;
// between 0 and 2
extern uint8_t pointer_poz_y;

struct powrms_variables {
	enum variable_names name;
	double value;
	double min_possible_value;
	double max_possible_value;
	uint8_t nr_of_int_digits;
	uint8_t nr_of_float_digits;
	char digits[NUMERIC_LENGTH];
	char unit[4];
};

// Global variable declaration
extern struct powrms_variables input_variables[VARIABLE_NUMBER];
// Global variable for enter key press status
extern bool enter_pressed;

// Storage for frequency ranges for callibration
extern uint32_t frequency_MHz_ranges[FREQUENCY_RANGE_NR];

// Define the global ADC data structure
struct adc_data {
	uint8_t adc_averaging_nr;
	uint16_t adc_p_fwd_raw;
	uint16_t adc_p_rev_raw;
	uint16_t adc_vin0_raw;    // Direct VIN0 reading
	uint16_t adc_vin1_raw;    // Direct VIN1 reading
	uint16_t adc_vin2_raw;    // Direct VIN2 reading
	float adc_p_fwd;        // Calculated Power Forward in watts
	float adc_p_rev;        // Calculated Power Reverse in watts
	float adc_vin0_voltage;   // VIN0 in volts
	float adc_vin0_voltage_corrected;   // VIN0 in volts
	float adc_vin1_voltage;   // VIN1 in volts
	float adc_vin1_voltage_corrected;   // VIN1 in volts
	float adc_vin2_voltage;   // VIN2 in volts
};

// External declaration of the global ADC data variable
extern volatile struct adc_data adc_data_input;

// Global variable to check if using eeprom saved calibration data or default ones
extern int32_t use_eeprom_calibration_data;

// Storage for the 48 precision values as 32-bit integers
extern int32_t precision_values[PRECISION_ARRAY_SIZE];
// Storage for the 48 reverse precision values as 32-bit integers
extern int32_t precision_values_reverse[PRECISION_ARRAY_SIZE];
// External declaration of the temperature precision values array
extern double temperature_precision_values[TEMPERATURE_CORRECTION_COEFFS *
					      FREQUENCY_RANGE_NR];
// External declaration of the temperature compensation value
extern int32_t temperature_compensation_value;

// Polynomial calibration coefficients for 5000MHz (stored as double for 20 decimal precision)
// These are used for 3rd order polynomial correction: intercept + c_x*x + c_f*f + c_x2*x^2 + c_xf*x*f + c_f2*f^2 + c_x3*x^3 + c_x2f*x^2*f + c_xf2*x*f^2 + c_f3*f^3
extern double poly_5000MHz_intercept;
extern double poly_5000MHz_c_x;
extern double poly_5000MHz_c_f;
extern double poly_5000MHz_c_x2;
extern double poly_5000MHz_c_xf;
extern double poly_5000MHz_c_f2;
extern double poly_5000MHz_c_x3;
extern double poly_5000MHz_c_x2f;
extern double poly_5000MHz_c_xf2;
extern double poly_5000MHz_c_f3;

// Polynomial calibration coefficients for 5000MHz reverse (stored as double for 20 decimal precision)
// These are used for 3rd order polynomial correction for reverse power
extern double poly_5000MHz_intercept_reverse;
extern double poly_5000MHz_c_x_reverse;
extern double poly_5000MHz_c_f_reverse;
extern double poly_5000MHz_c_x2_reverse;
extern double poly_5000MHz_c_xf_reverse;
extern double poly_5000MHz_c_f2_reverse;
extern double poly_5000MHz_c_x3_reverse;
extern double poly_5000MHz_c_x2f_reverse;
extern double poly_5000MHz_c_xf2_reverse;
extern double poly_5000MHz_c_f3_reverse;

/**
 * @brief Update the value of an input variable based on digit increment
 * @param input_var Pointer to the variable structure to update
 * @param increment_pos Position of the digit to increment (0-based index)
 */
void update_values_of_digit(struct powrms_variables *input_var,
			    uint8_t increment_pos);

/**
 * @brief Update the digits array based on a new value
 * @param input_var Pointer to the variable structure to update
 * @param new_value The new value to set (will be clamped to valid range)
 */
void update_values_from_new_value(struct powrms_variables *input_var,
				  double new_value);

/**
 * @brief Synchronize all input variables' digits arrays with their current values
 *
 * This function updates the digits array for all input variables based on
 * their current value field. Useful after loading values from EEPROM or
 * other external sources that modify the value but not the digits.
 */
void sync_all_digits_from_values(void);

int calculate_power();

#endif /* __POWRMS_DATA_PROCESSING_H__ */