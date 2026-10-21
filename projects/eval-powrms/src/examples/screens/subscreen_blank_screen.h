/***************************************************************************//**
 *   @file   subscreen_blank_screen.h
 *   @brief  Blank screen header for blanking display in eval-powrms project
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __BLANK_SCREEN_H__
#define __BLANK_SCREEN_H__

#include "example.h"

/**
 * @brief Display a blank screen (black/empty display)
 *
 * This function creates a completely blank screen, effectively turning off
 * the display content while keeping the display hardware active. Used for
 * screen blanking functionality triggered by GPIO 0.15 falling edge.
 *
 * Navigation:
 * - Any button press: Return to previous screen/exit blank mode
 *
 * @note This screen is automatically triggered when GPIO 0.15 falling edge is detected
 */
void subscreen_blank_screen(void);

#endif // __BLANK_SCREEN_H__
