/***************************************************************************//**
 *   @file   subscreen_settings_screen.h
 *   @brief  Settings screen header for configuring input variables in eval-powrms project
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

#ifndef __SETTINGS_SCREEN_H__
#define __SETTINGS_SCREEN_H__

// Constants

#define AD7091_VOLTAGE_DIV_VALUE    1.8461

#include "example.h"

/**
 * @brief Display and handle the settings screen for configuring input variables
 *
 * This function creates an interactive GUI for editing numerical values
 * for input impedance, output impedance, and signal frequency using LVGL.
 * Users can navigate through digits using keyboard input and modify values
 * in real-time with visual feedback.
 *
 * Navigation:
 * - 'q': Move to next digit/variable
 * - 'w': Increment digit (when in edit mode) or go back (when not editing)
 * - 'e': Enter/exit edit mode
 *
 * @note This function runs in a loop until the user exits to the main menu
 */
void subscreen_settings_screen();

#endif // __SETTINGS_SCREEN_H__