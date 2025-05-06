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
#ifndef __SSD1306_EXAMPLE__
#define __SSD1306_EXAMPLE__

#include "common_data.h"
#include "no_os_util.h"
#include <stdint.h>
#include <stdio.h>
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "display.h"
#include "ssd_1306.h"
#include "lvgl.h"
#include "lv_types.h"
#include "lv_display.h"
#include "lv_display_private.h"
#include "no_os_init.h"
#include <string.h>
#include "no_os_display.h"
#include "no_os_error.h"

#define POWRMS_UART_DEBUG

// SSD1306 setup
#define SSD1306_HOR_REZ			128
#define SSD1306_VER_REZ			64
#define SSD1306_BUFFER_SIZE ((SSD1306_HOR_REZ * SSD1306_VER_REZ) / 8 + 8)

// Buffer for UI usage
static uint8_t display_buffer[SSD1306_HOR_REZ * SSD1306_VER_REZ / 8 + 8];

// Display options
#define NUMERIC_FONT_SIZE &lv_font_montserrat_12

// ----------------------- Global variables	-----------------------

/**
 * 0 -> Main Menu
 * 1 -> Main Screen (ADC values)
 * 2 -> Settings Screen
 */
extern uint8_t display_entry;


// ----------------------- MAIN SCREEN -----------------------
#define INTEGER_PRECISION			2
#define FLOATING_POINT_PRECISION	3

#define OUTPUT_VARIABLE_NAMES  	{"Vrmsf:", "Vrmsr:"}

#define ADC_VRMSF_CHANNEL_NO	0
#define ADC_VRMSR_CHANNEL_NO	1

#define ADC_V_REF				4.096
#define ADC_V_REF_GND			0.0
#define ADC_COUNTER_MAX			4096

struct adc_data {
	uint16_t adc_vrmsf_raw;
	uint16_t adc_vrmsr_raw;
	float adc_vrmsf;
	float adc_vrmsr;
};

struct adc_data adc_data_input = {
	.adc_vrmsf_raw = 0,
	.adc_vrmsr_raw = 0,
	.adc_vrmsf = 0.0,
	.adc_vrmsr = 0.0,
};

// ---------------------- SETTINGS ----------------------
// Format AAAA.BB
#define VARIABLE_NUMBER			3
#define NUMERIC_LENGTH			6
#define INTEGER_NUMERIC_LENGTH	4
#define FLOATING_POINT_LENGTH	2
// indexing starting from 0
#define DECIMAL_POSITION		4

#define VARIABLE_NAMES  		{"IN:", "OUT:", "FQ:"}
extern char input_variables[VARIABLE_NUMBER][NUMERIC_LENGTH];

extern float input_impedance;
extern float output_impedance;
extern float signal_frequency;

// between 0 and 6
extern uint8_t pointer_poz_x;
/**
* 0 -> input_impedance
* 1 -> output_impedance
* 2 -> signal_frequency
*/

extern uint8_t pointer_poz_y;
extern bool enter_pressed;

int example_main();

#endif /* __SSD1306_EXAMPLE__ */
