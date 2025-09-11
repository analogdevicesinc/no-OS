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

#include "show_screen.h"
#include "ad7091r5.h"

// Define the global ADC data structure
struct adc_data adc_data_input = {
    .adc_vrmsf_raw = 0,
    .adc_vrmsr_raw = 0,
    .adc_vin0_raw = 0,
    .adc_vin1_raw = 0,
    .adc_vrmsf = 0.0,
    .adc_vrmsr = 0.0,
    .adc_vin0_voltage = 0.0,
    .adc_vin1_voltage = 0.0,
};

void show_screen()
{
    // Create a new screen object
    char input_char;
    char out_text[10];
    lv_obj_t *scr = lv_obj_create(NULL);

// --------------------- SETUP SCREEN ------------------

    // Define grid: 2 columns (label, value) and 4 rows (for 4 items)
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(scr, col_dsc, row_dsc);

    // Measurement names and value labels
    const char *names[] = OUTPUT_VARIABLE_NAMES;
    lv_obj_t *label_values[4];  // Store references to value labels (VIN0, VIN1, VRMSF, VRMSR)

    for (int i = 0; i < 4; i++) {
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

// ---------------- INPUT LOOP & SCREEN UPDATE ------------------

    while (display_entry == DISPLAY_ENTRY_SHOW) {

#ifdef POWRMS_UART_DEBUG
        printf(".");
#endif

// Read values from ADC
        // Read VIN0 (Channel 0)
        if (ad7091r5_read_one(adc_desc, ADC_VIN0_CHANNEL_NO, &adc_data_input.adc_vin0_raw) == 0) {
            // Convert raw ADC value to voltage (12-bit ADC, 4.096V reference)
            adc_data_input.adc_vin0_voltage = ((float)adc_data_input.adc_vin0_raw * ADC_V_REF) / ADC_COUNTER_MAX;
        }

        // Read VIN1 (Channel 1)
        if (ad7091r5_read_one(adc_desc, ADC_VIN1_CHANNEL_NO, &adc_data_input.adc_vin1_raw) == 0) {
            // Convert raw ADC value to voltage (12-bit ADC, 4.096V reference)
            adc_data_input.adc_vin1_voltage = ((float)adc_data_input.adc_vin1_raw * ADC_V_REF) / ADC_COUNTER_MAX;
        }

        // Display VIN0 and VIN1 voltage readings (now first in order)
        powrms_float_to_str(adc_data_input.adc_vin0_voltage, out_text, INTEGER_PRECISION, FLOATING_POINT_PRECISION);
        lv_label_set_text(label_values[0], out_text);
        powrms_float_to_str(adc_data_input.adc_vin1_voltage, out_text, INTEGER_PRECISION, FLOATING_POINT_PRECISION);
        lv_label_set_text(label_values[1], out_text);

        if (!update_adc_data()) {
            powrms_float_to_str(adc_data_input.adc_vrmsf, out_text, INTEGER_PRECISION, FLOATING_POINT_PRECISION);
            lv_label_set_text(label_values[2], out_text);
            powrms_float_to_str(adc_data_input.adc_vrmsr, out_text, INTEGER_PRECISION, FLOATING_POINT_PRECISION);
            lv_label_set_text(label_values[3], out_text);
            lv_timer_handler();  // Process LVGL updates
        }

        uint8_t tmp = read_input();
        switch (tmp) {
        case 1: // NEXT
            break;
        case 2: // BACK
            display_entry = DISPLAY_ENTRY_MENU;
            lv_obj_del(scr);
            return;
        case 3: // ENTER
            break;
        case 4:
            display_entry = DISPLAY_ENTRY_BLANK;
            lv_obj_del(scr);
            return;
        default:
            break;
        }
    }

    // Cleanup screen if we exit the loop abnormally
    lv_obj_del(scr);
}
