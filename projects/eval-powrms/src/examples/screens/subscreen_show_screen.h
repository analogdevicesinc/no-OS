/***************************************************************************//**
 *   @file   subscreen_show_screen.h
 *   @brief  Measurement display screen header for showing ADC values in eval-powrms project
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __SHOW_SCREEN_H__
#define __SHOW_SCREEN_H__

#include "example.h"

#define INTEGER_PRECISION			2
#define FLOATING_POINT_PRECISION	3

#define OUTPUT_VARIABLE_NAMES  	{"P_fwd:", "P_rev:"}

#define ADC_P_FWD_CHANNEL_NO	0
#define ADC_P_REV_CHANNEL_NO	1
#define ADC_VIN0_CHANNEL_NO		0  // VIN0 maps to channel 0
#define ADC_VIN1_CHANNEL_NO		1  // VIN1 maps to channel 1
#define ADC_VIN2_CHANNEL_NO		2  // VIN2 maps to channel 2
#define ADC_VIN3_CHANNEL_NO		3  // VIN3 maps to channel 3

#define ADC_V_REF				4.096
#define ADC_V_REF_GND			0.0
#define ADC_COUNTER_MAX			4096


/**
 * @brief Display and continuously update ADC measurement values
 *
 * This function creates a real-time display showing ADC measurement results
 * for VIN0, VIN1, VIN2 (analog input channels), P_FWD (Power Forward Voltage)
 * and P_REV (Power Reverse Voltage).
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
