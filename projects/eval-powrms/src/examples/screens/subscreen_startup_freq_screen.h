/***************************************************************************//**
 *   @file   subscreen_startup_freq_screen.h
 *   @brief  Startup frequency input screen header for eval-powrms project
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
#ifndef __STARTUP_FREQ_SCREEN_H__
#define __STARTUP_FREQ_SCREEN_H__

#include "example.h"

// Frequency input configuration - Format AAAA.BB
#define FREQ_NUMERIC_LENGTH			6
#define FREQ_INTEGER_NUMERIC_LENGTH	4
#define FREQ_FLOATING_POINT_LENGTH	2

// Grid layout constants for improved readability
#define FREQ_GRID_LABEL_WIDTH       26  // Width fraction for frequency label column
#define FREQ_GRID_DIGIT_WIDTH       1   // Width fraction for each digit column
#define FREQ_GRID_ROW_HEIGHT        10  // Height fraction for each row

extern char freq_input_variable[FREQ_NUMERIC_LENGTH];
extern float startup_frequency;
extern uint8_t freq_pointer_poz_x;
extern bool freq_enter_pressed;

/**
 * @brief Display and handle the startup frequency input screen
 *
 * This function creates an interactive GUI for setting the initial frequency
 * value at startup. Users can edit a single frequency value in AAAA.BB format
 * (4 integer digits + 2 decimal places) with real-time validation and feedback.
 *
 * Navigation:
 * - 'q': Move to next digit (wraps around)
 * - 'w': Increment digit (when in edit mode) or go back to menu (when not editing)
 * - 'e': Enter/exit edit mode, save value and proceed to main menu
 *
 * @note This screen is typically shown on first startup or when frequency
 *       configuration is required before proceeding to main application
 */
void subscreen_startup_freq_screen();

#endif // __STARTUP_FREQ_SCREEN_H__
