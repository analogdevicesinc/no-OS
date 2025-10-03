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

#include "subscreen_startup_freq_screen.h"


void subscreen_startup_freq_screen()
{
	static uint8_t initialized = 0;
	static bool enter_pressed = false;
	// extern display_entry_t display_entry;
	static uint8_t pointer_poz_x = 0;
	// Store only value labels (we don't need to store name labels)
	static lv_obj_t *value_labels[NR_OF_INPUT_VARIABLES][NUMERIC_LENGTH];

// ------------------ SETUP SCREEN ------------------

	if (!initialized) {
		initialized = 1;
		// Grid descriptors using defined constants
		static lv_coord_t col_dsc[] = {
			LV_GRID_FR(12),                    // Name column
			LV_GRID_FR(FREQ_GRID_DIGIT_WIDTH), // Digit 1
			LV_GRID_FR(FREQ_GRID_DIGIT_WIDTH), // Digit 2
			LV_GRID_FR(FREQ_GRID_DIGIT_WIDTH), // Digit 3
			LV_GRID_FR(FREQ_GRID_DIGIT_WIDTH), // Digit 4
			LV_GRID_FR(FREQ_GRID_DIGIT_WIDTH), // Decimal point
			LV_GRID_FR(FREQ_GRID_DIGIT_WIDTH), // Digit 5
			LV_GRID_FR(FREQ_GRID_DIGIT_WIDTH), // Digit 6
			LV_GRID_FR(FREQ_GRID_DIGIT_WIDTH), // Empty space
			LV_GRID_FR(25),  // Unit
			LV_GRID_TEMPLATE_LAST
		};
		static lv_coord_t row_dsc[] = {
			LV_GRID_FR(FREQ_GRID_ROW_HEIGHT), // Row 1 (frequency input row)
			LV_GRID_FR(FREQ_GRID_ROW_HEIGHT), // Row 2 (spacing)
			LV_GRID_FR(FREQ_GRID_ROW_HEIGHT), // Row 3 (instructions)
			LV_GRID_TEMPLATE_LAST
		};

		lv_obj_t *screen = get_global_screen();
		lv_obj_set_grid_dsc_array(screen, col_dsc, row_dsc);


		// Name label in same row as values
		lv_obj_t *label_name = lv_label_create(screen);
		lv_label_set_text(label_name, "Fq:");
		lv_obj_set_grid_cell(label_name, LV_GRID_ALIGN_START, 0, 1,
				     LV_GRID_ALIGN_START, 0, 1);

		uint8_t col = 1;
		for (uint8_t var = 0; var < NUMERIC_LENGTH; var++) {
			if (var == INTEGER_NUMERIC_LENGTH) {
				// Comma label
				lv_obj_t *comma_label = lv_label_create(screen);
				lv_label_set_text(comma_label, ",");
				lv_obj_set_grid_cell(comma_label, LV_GRID_ALIGN_START, col++, 1,
						     LV_GRID_ALIGN_START, 0, 1);
			}

			value_labels[0][var] = lv_label_create(screen);
			char text[2] = {input_variables[SIGNAL_FREQUENCY].digits[var], '\0'};
			lv_label_set_text(value_labels[0][var], text);
			lv_obj_set_grid_cell(value_labels[0][var], LV_GRID_ALIGN_START, col++, 1,
					     LV_GRID_ALIGN_START, 0, 1);
		}

		// Unit label in the last column
		lv_obj_t *unit_label = lv_label_create(screen);
		lv_label_set_text(unit_label, input_variables[SIGNAL_FREQUENCY].unit);
		lv_obj_set_grid_cell(unit_label, LV_GRID_ALIGN_START, col, 1,
				     LV_GRID_ALIGN_START, 0, 1);

		// Highlight current label
		lv_obj_set_style_text_decor(value_labels[0][pointer_poz_x],
					    LV_TEXT_DECOR_UNDERLINE, 0);
		lv_scr_load(screen);
		lv_timer_handler();

	}


// ---------------- INPUT LOOP & SCREEN UPDATE ------------------

	if (display_entry == DISPLAY_INPUT_FREQUENCY) {

		uint8_t tmp = read_input();
		switch (tmp) {
		case 1: // NEXT
			lv_obj_set_style_text_decor(value_labels[0][pointer_poz_x],
						    LV_TEXT_DECOR_NONE, 0);
			if (pointer_poz_x < NUMERIC_LENGTH - 1) {
				// Advance pointer
				pointer_poz_x++;
			} else {
				pointer_poz_x = 0;
			}

			// Set highlight on new position
			lv_obj_set_style_text_decor(value_labels[0][pointer_poz_x],
						    LV_TEXT_DECOR_UNDERLINE, 0);
			lv_timer_handler();

			break;
		case 2: // BACK OR INCREMENT
			if (enter_pressed) { // Increment selected digit
				update_values(&input_variables[SIGNAL_FREQUENCY], pointer_poz_x);
				char text[2] = {input_variables[SIGNAL_FREQUENCY].digits[pointer_poz_x], '\0'};
				lv_label_set_text(value_labels[0][pointer_poz_x], text);
				lv_timer_handler();
			} else { // Back button
				display_entry = DISPLAY_ENTRY_MENU;
				initialized = 0;
				delete_global_screen();
				return;
			}
			break;
		case 3: // ENTER
			enter_pressed = !enter_pressed;
			break;
		case 4:
			display_entry = DISPLAY_ENTRY_BLANK;
			initialized = 0;
			delete_global_screen();
			return;
		default:
			break;
		}
	}
}
