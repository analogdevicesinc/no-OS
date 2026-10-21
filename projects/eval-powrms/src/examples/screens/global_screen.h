/***************************************************************************//**
 *   @file   global_screen.h
 *   @brief  Global screen header for system-wide display management in eval-powrms project
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __GLOBAL_SCREEN_H__
#define __GLOBAL_SCREEN_H__

#include "example.h"
#include "common_data.h"
#include "lvgl.h"


// SSD1306 setup
#define SSD1306_HOR_REZ			128
#define SSD1306_VER_REZ			64
#define SSD1306_BUFFER_SIZE ((SSD1306_HOR_REZ * SSD1306_VER_REZ) / 8 + 8)

// Buffer for UI usage - defined in example.c
extern uint8_t display_buffer[SSD1306_HOR_REZ * SSD1306_VER_REZ / 8 + 8];

// ----------------------- Global variables	-----------------------

/**
 * @brief Display descriptor
 * 0 -> Startup Frequency Screen
 * 1 -> Main Screen (ADC values)
 * 2 -> Show Screen
 * 3 -> Settings Screen
 */
extern enum display_entry_t display_entry;

enum display_entry_t {
	DISPLAY_ENTRY_MENU,
	DISPLAY_ENTRY_SHOW,
	DISPLAY_ENTRY_SETTINGS,
	DISPLAY_ENTRY_BLANK
};

// Global screen management functions
lv_obj_t * get_global_screen(void);
int delete_global_screen(void);

#endif /* __GLOBAL_SCREEN_H__ */