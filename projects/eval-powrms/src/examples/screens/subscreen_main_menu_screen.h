/***************************************************************************//**
 *   @file   subscreen_main_menu_screen.h
 *   @brief  Main menu screen header for navigation in eval-powrms project
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __MAIN_MENU_SCREEN_H__
#define __MAIN_MENU_SCREEN_H__

#include "example.h"

extern bool show_menu;

/**
 * @brief Display and handle the main menu navigation screen
 *
 * This function creates the main menu interface allowing users to navigate
 * between different application screens (Show measurements, Settings).
 *
 * Navigation:
 * - 'q': Switch between menu options (Show/Settings)
 * - 'e': Select current menu option and navigate to selected screen
 *
 * @note This is the central navigation hub of the application
 */
void subscreen_main_menu_screen();

/**
 * @brief Apply visual highlighting to menu labels
 *
 * @param label1 First menu label (typically "MAIN SCREEN")
 * @param label2 Second menu label (typically "SETTINGS")
 * @param show_menu Boolean indicating which label should be highlighted
 */
void underline_in_main_screen(lv_obj_t *label1, lv_obj_t *label2,
			      bool show_menu);

#endif // __MAIN_MENU_SCREEN_H__