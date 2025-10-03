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

#include "powrms_utils.h"
#include "subscreen_main_menu_screen.h"
#include "subscreen_startup_freq_screen.h"
#include "subscreen_show_screen.h"
#include "subscreen_settings_screen.h"
#include "powrms_variables.h"

// #define POWRMS_UART_DEBUG

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
	DISPLAY_INPUT_FREQUENCY,
	DISPLAY_ENTRY_MENU,
	DISPLAY_ENTRY_SHOW,
	DISPLAY_ENTRY_SETTINGS,
	DISPLAY_ENTRY_BLANK
};

int example_main();

int ltc3556_write_init_command(struct no_os_i2c_desc *param);
int get_en_latch_value(void);

#endif /* __SSD1306_EXAMPLE__ */
