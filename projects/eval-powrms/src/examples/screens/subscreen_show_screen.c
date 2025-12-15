/***************************************************************************//**
 *   @file   subscreen_show_screen.c
 *   @brief  EVAL-POWRMS display screen implementation for powrms project
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

#include "subscreen_show_screen.h"
#include "ad7091r5.h"
#include "common_data.h"
#include "powrms_gpios.h"

void _update_adc_data()
{
	uint16_t temp_vin0, temp_vin1, temp_vin2;

	for (uint8_t nr_of_cycle = 0; nr_of_cycle < adc_data_input.adc_averaging_nr;
	     nr_of_cycle++) {
		// Read VIN0 (Channel 0)
		if (ad7091r5_read_one(adc_desc, ADC_VIN0_CHANNEL_NO, &temp_vin0) == 0) {
			if (nr_of_cycle == 0)
				adc_data_input.adc_vin0_raw = 0;

			adc_data_input.adc_vin0_raw += temp_vin0;
		}

		no_os_mdelay(10);

		// Read VIN1 (Channel 1)
		if (ad7091r5_read_one(adc_desc, ADC_VIN1_CHANNEL_NO, &temp_vin1) == 0) {
			if (nr_of_cycle == 0)
				adc_data_input.adc_vin1_raw = 0;

			adc_data_input.adc_vin1_raw += temp_vin1;
		}

		no_os_mdelay(10);

		// Read VIN2 (Channel 2)
		if (ad7091r5_read_one(adc_desc, ADC_VIN2_CHANNEL_NO, &temp_vin2) == 0) {
			if (nr_of_cycle == 0)
				adc_data_input.adc_vin2_raw = 0;
			adc_data_input.adc_vin2_raw += temp_vin2;
		}

		no_os_mdelay(10);
	}
	// Average the readings
	adc_data_input.adc_vin0_raw /= adc_data_input.adc_averaging_nr;
	adc_data_input.adc_vin1_raw /= adc_data_input.adc_averaging_nr;
	adc_data_input.adc_vin2_raw /= adc_data_input.adc_averaging_nr;

	adc_data_input.adc_vin0_voltage = ((float)adc_data_input.adc_vin0_raw *
					   ADC_V_REF) / ADC_COUNTER_MAX;

	adc_data_input.adc_vin1_voltage = ((float)adc_data_input.adc_vin1_raw *
					   ADC_V_REF) / ADC_COUNTER_MAX;

	adc_data_input.adc_vin2_voltage = ((float)adc_data_input.adc_vin2_raw *
					   ADC_V_REF) / ADC_COUNTER_MAX;

	int ret = calculate_power();
	if (ret) {
		// If power calculation fails, set power values to 0 for safe display
		adc_data_input.adc_p_fwd = 0.0;
		adc_data_input.adc_p_rev = 0.0;
	}
}

void subscreen_show_screen()
{
	// Create a new screen object
	char out_text[10];
	static uint8_t initialized = 0;
	lv_obj_t *scr = get_global_screen();
	// Measurement names and value labels
	static lv_obj_t
	*label_values[4];  // Store references to value labels (VIN0, VIN1, P_FWD, P_REV)

// --------------------- SETUP SCREEN ------------------
	if (!initialized) {
		initialized = 1;
		const char *names[] = OUTPUT_VARIABLE_NAMES;

		// Define grid: 2 columns (label, value) and 4 rows (for 4 items)
		static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
		static lv_coord_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
		lv_obj_set_grid_dsc_array(scr, col_dsc, row_dsc);

		for (int i = 0; i < 2; i++) {
			// Name label (left column)
			lv_obj_t *label_name = lv_label_create(scr);
			lv_label_set_text(label_name, names[i]);
			lv_obj_set_grid_cell(label_name, LV_GRID_ALIGN_START, 0, 1,
					     LV_GRID_ALIGN_CENTER, i, 1);

			// Value label (right column)
			label_values[i] = lv_label_create(scr);
			lv_obj_set_grid_cell(label_values[i], LV_GRID_ALIGN_END, 1, 1,
					     LV_GRID_ALIGN_CENTER, i, 1);
			lv_obj_set_style_text_color(label_values[i], lv_color_black(), LV_PART_MAIN);
		}

		// Load the screen once
		lv_scr_load(scr);
		lv_timer_handler();
	}


// ---------------- INPUT LOOP & SCREEN UPDATE ------------------

	if (display_entry == DISPLAY_ENTRY_SHOW) {
		// Display all 4 voltage readings in the order: VIN0, VIN1, P_FWD, P_REV
		// powrms_float_to_str(adc_data_input.adc_vin0_voltage_corrected, out_text,
		// 		    INTEGER_PRECISION, FLOATING_POINT_PRECISION, false);
		// lv_label_set_text(label_values[0], out_text);
		// powrms_float_to_str(adc_data_input.adc_vin1_voltage_corrected, out_text,
		// 		    INTEGER_PRECISION, FLOATING_POINT_PRECISION, false);
		// lv_label_set_text(label_values[1], out_text);
		powrms_float_to_str(adc_data_input.adc_p_fwd, out_text, INTEGER_PRECISION,
				    FLOATING_POINT_PRECISION, false);
		lv_label_set_text(label_values[0], out_text);
		powrms_float_to_str(adc_data_input.adc_p_rev, out_text, INTEGER_PRECISION,
				    FLOATING_POINT_PRECISION, false);
		lv_label_set_text(label_values[1], out_text);

		lv_timer_handler();

		uint8_t tmp = read_input();
		switch (tmp) {
		case 1: // NEXT
			break;
		case 2: // BACK
			display_entry = DISPLAY_ENTRY_MENU;
			initialized = 0;
			delete_global_screen();
			return;
		case 3: // ENTER
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
