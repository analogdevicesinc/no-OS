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

#ifndef POWRMS_VARIABLES_H
#define POWRMS_VARIABLES_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "example.h"

// Format AAAA.BB

// indexing starting from 0
enum variable_names {
	INPUT_IMPEDANCE,
	OUTPUT_IMPEDANCE,
	SIGNAL_FREQUENCY,
	NR_OF_INPUT_VARIABLES
};

#define VARIABLE_NAMES  		{"In:", "Out:", "Fq:"}

// between 0 and 6
extern uint8_t pointer_poz_x;


extern uint8_t pointer_poz_y;

#define VARIABLE_NUMBER			3
#define NUMERIC_LENGTH			6
#define INTEGER_NUMERIC_LENGTH	4
#define FLOATING_POINT_LENGTH	2
#define UNIT_LENGTH				4


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

extern struct powrms_variables input_variables[VARIABLE_NUMBER];

extern bool enter_pressed;

void update_values(struct powrms_variables *input_var, uint8_t increment_pos);


#endif // POWRMS_VARIABLES_H