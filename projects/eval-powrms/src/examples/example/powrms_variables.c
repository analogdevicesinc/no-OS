/***************************************************************************//**
 *   @file   example.h
 *   @brief  Ssd1306 example header for ssd1306 project
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

#include "powrms_variables.h"

void update_values(struct powrms_variables *input_var, uint8_t increment_pos)
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
		divisor /= 10.0f;
	}

	if (new_value >= input_var->min_possible_value
	    && new_value <= input_var->max_possible_value) {
		input_var->value = new_value;
		memcpy(input_var->digits, temp_digits, NUMERIC_LENGTH);
	} else {
		input_var->value = input_var->min_possible_value;
		powrms_float_to_str(input_var->value, input_var->digits,
				    input_var->nr_of_int_digits, input_var->nr_of_float_digits);
	}

#ifdef POWRMS_UART_DEBUG
	// print variables:
	printf(""
	       "input_impedance: %04.2f\n\r"
	       "output_impedance: %04.2f\n\r"
	       "signal_frequency: %04.2f\n\r",
	       input_variables[INPUT_IMPEDANCE].value,
	       input_variables[OUTPUT_IMPEDANCE].value,
	       input_variables[SIGNAL_FREQUENCY].value);
#endif
}