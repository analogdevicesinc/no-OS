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

#include "subscreen_main_menu_screen.h"

void underline_in_main_screen(lv_obj_t *label1, lv_obj_t *label2,
			      bool show_menu)
{
	bool tmp_var = show_menu ? LV_TEXT_DECOR_UNDERLINE : LV_TEXT_DECOR_NONE;
	lv_obj_set_style_text_decor(label1, tmp_var, LV_PART_MAIN);
	lv_obj_set_style_text_decor(label2, !tmp_var, LV_PART_MAIN);
}

void subscreen_main_menu_screen()
{
	static lv_obj_t *label1;
	static lv_obj_t *label2;
	static lv_obj_t *scr_local;
	static uint8_t initialized = 0;
// --------------------- SETUP SCREEN ------------------

	if (!initialized) {
		initialized = 1;
		scr_local = get_global_screen();
		label1 = lv_label_create(scr_local);
		lv_label_set_text(label1, "MAIN SCREEN");
		lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 10);
		lv_obj_set_style_text_color(label1, lv_color_black(), LV_PART_MAIN);

		label2 = lv_label_create(scr_local);
		lv_label_set_text(label2, "SETTINGS");
		lv_obj_align_to(label2, label1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
		lv_obj_set_style_text_color(label2, lv_color_black(), LV_PART_MAIN);

		underline_in_main_screen(label1, label2, show_menu);

		lv_scr_load(scr_local);
		lv_timer_handler();
	}

// ---------------- INPUT LOOP & SCREEN UPDATE ------------------

	if (display_entry == DISPLAY_ENTRY_MENU) {
		uint8_t tmp = read_input();
		switch (tmp) {
		case 1: // NEXT
			show_menu = !show_menu;
			underline_in_main_screen(label1, label2, show_menu);
			lv_scr_load(scr_local);
			lv_timer_handler();
			break;
		case 2: // BACK
			// NOP
			break;

		case 3: // ENTER
			if (show_menu) {
				display_entry = DISPLAY_ENTRY_SHOW;
				initialized = 0;
				delete_global_screen();
				return;
			} else {
				display_entry = DISPLAY_ENTRY_SETTINGS;
				delete_global_screen();
				initialized = 0;
				return;
			}
			break;
		case 4:
			display_entry = DISPLAY_ENTRY_BLANK;
			delete_global_screen();
			initialized = 0;
			return;
		default:
			break;
		}
	}

}
