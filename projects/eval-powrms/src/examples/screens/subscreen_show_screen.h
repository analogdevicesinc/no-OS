/***************************************************************************//**
 *   @file   subscreen_show_screen.h
 *   @brief  Measurement display screen header for showing ADC values in eval-powrms project
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

#ifndef __SHOW_SCREEN_H__
#define __SHOW_SCREEN_H__

#include "example.h"

#define INTEGER_PRECISION			2
#define FLOATING_POINT_PRECISION	3

#define OUTPUT_VARIABLE_NAMES  	{"VIN0:", "VIN1:", "VRMSF:", "VRMSR:"}

#define ADC_VRMSF_CHANNEL_NO	0
#define ADC_VRMSR_CHANNEL_NO	1
#define ADC_VIN0_CHANNEL_NO		0  // VIN0 maps to channel 0
#define ADC_VIN1_CHANNEL_NO		1  // VIN1 maps to channel 1

#define ADC_V_REF				4.096
#define ADC_V_REF_GND			0.0
#define ADC_COUNTER_MAX			4096

struct adc_data {
	uint16_t adc_vrmsf_raw;
	uint16_t adc_vrmsr_raw;
	uint16_t adc_vin0_raw;    // Direct VIN0 reading
	uint16_t adc_vin1_raw;    // Direct VIN1 reading
	float adc_vrmsf;
	float adc_vrmsr;
	float adc_vin0_voltage;   // VIN0 in volts
	float adc_vin1_voltage;   // VIN1 in volts
};

extern volatile struct adc_data adc_data_input;

/**
 * @brief Display and continuously update ADC measurement values
 *
 * This function creates a real-time display showing ADC measurement results
 * for VRMSF (RMS Forward Voltage) and VRMSR (RMS Reverse Voltage).
 * The screen continuously updates with live ADC readings and provides
 * a user interface to return to the main menu.
 *
 * Navigation:
 * - 'w': Return to main menu
 *
 * @note This function runs in a continuous loop, updating ADC values
 *       in real-time until the user chooses to exit
 */
void subscreen_show_screen();

#endif // __SHOW_SCREEN_H__
