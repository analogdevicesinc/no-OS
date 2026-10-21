/***************************************************************************//**
 *   @file   subscreen_blank_screen.c
 *   @brief  Blank screen implementation for display blanking functionality
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "subscreen_blank_screen.h"
#include "../example/powrms_gpios.h"
#include "../example/example.h"

void subscreen_blank_screen(void)
{
	// Set BOOST_EN pin to low
	static uint8_t initialized = 0;
	no_os_gpio_direction_output(boost_en_gpio_desc, NO_OS_GPIO_LOW);

	// --------------------- SETUP SCREEN ------------------

	if (!initialized) {
		initialized = 1;
		lv_obj_t * scr = get_global_screen();

		// Set the screen background to black
		lv_obj_set_style_bg_color(scr, lv_color_white(), LV_PART_MAIN);
		lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

		// Load the blank screen
		lv_scr_load(scr);
		lv_timer_handler();

		configure_gpios_upon_shutdown();
	}
	// ---------------- INPUT LOOP & SCREEN UPDATE ------------------
	// Not relevant, upon re-restart, MCU rests to initial screen

}
