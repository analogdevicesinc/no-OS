/***************************************************************************//**
 *   @file   subscreen_settings_screen.h
 *   @brief  Settings screen header for configuring input variables in eval-powrms project
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __SETTINGS_SCREEN_H__
#define __SETTINGS_SCREEN_H__

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