/***************************************************************************//**
 *   @file   example.c
 *   @brief  Implementation of ssd1306 example for demo project.
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
#include "example.h"
#include "malloc.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

char input_variables[VARIABLE_NUMBER][NUMERIC_LENGTH] = {{'1', '2', '3', '4', '5', '6'},
	{'2', '3', '4', '5', '6', '1'},
	{'3', '4', '5', '6', '1', '2'}
};

float input_impedance = 0;
float output_impedance = 0;
float signal_frequency = 0;

uint8_t pointer_poz_x = 0;
uint8_t pointer_poz_y = 0;
struct no_os_uart_desc *demo_uart_desc;

bool show_menu = 1;
bool enter_pressed = 0;
uint8_t display_entry = 0;


uint8_t read_input()
{
	char input_char;

	if (no_os_uart_read(demo_uart_desc, &input_char, 1) > 0) {
		if (input_char == 'q') {
#ifdef POWRMS_UART_DEBUG
			printf("NEXT\n\r");
#endif
			return 1;
		} else if (input_char == 'w') {
#ifdef POWRMS_UART_DEBUG
			printf("BACK\n\r");
#endif
			return 2;
		} else if (input_char == 'e') {
#ifdef POWRMS_UART_DEBUG
			printf("ENTER\n\r");
#endif
			return 3;
		}
	}
	return 0;
}

void print_heap_status()
{
	void *current_break = sbrk(0);
	printf("\n\r\n\rCurrent program break: %p\n\r", current_break);

	struct mallinfo mi = mallinfo();
	printf("Total non-mmapped bytes (arena): 				%d\n\r", mi.arena);
	printf("Number of free chunks (ordblks):				%d\n\r", mi.ordblks);
	printf("Number of free fastbin blocks (smblks):			%d\n\r", mi.smblks);
	printf("Number of mmapped regions (hblks): 				%d\n\r", mi.hblks);
	printf("Space allocated in mmapped regions (hblkhd):	%d\n\r", mi.hblkhd);
	printf("Maximum total allocated space (usmblks):		%d\n\r", mi.usmblks);
	printf("Space in free fastbin blocks (fsmblks):			%d\n\r", mi.fsmblks);
	printf("Total allocated space (uordblks):				%d\n\r", mi.uordblks);
	printf("Total free space (fordblks):					%d\n\r", mi.fordblks);
	printf("Top-most, releasable space (keepcost):			%d\n\r", mi.keepcost);
}

void my_flush_cb(lv_display_t * disp, const lv_area_t * area,
		 lv_color_t * color_p)
{
	int ret;

	uint8_t * lvgl_buff = &display_buffer[8];
	uint8_t buff_to_ssd1306[8 * 128] = {0};

	no_os_row_major_to_column_major_8bits(lvgl_buff, buff_to_ssd1306,
					      SSD1306_HOR_REZ, SSD1306_VER_REZ);

	ret = ssd_1306_print_buffer(oled_display, buff_to_ssd1306);
	lv_display_flush_ready(disp);
}

/**
 * @brief Initialize the display task
 *
 * @return int
 */
int display_task()
{
	int ret;
	lv_init();
	memset(display_buffer[8], 0x00, sizeof(display_buffer) - 8);
	lv_display_t * disp = lv_display_create(SSD1306_HOR_REZ, SSD1306_VER_REZ);
	lv_display_set_flush_cb(disp, my_flush_cb);
	lv_display_set_buffers(disp, display_buffer, NULL, sizeof(display_buffer),
			       LV_DISPLAY_RENDER_MODE_FULL);
	return 0;
}

/**
 * @brief Configure the ADC
 *
 * @return int
 */
int configure_adc()
{
#ifdef POWRMS_UART_DEBUG
	printf("Configuring ADC\n\r");
#endif
	// if (!adc_i2c_ini_param || !adc_gpio_ini_param || adc_ini_param) {
	// 	return -EINVAL;
	// }
	return ad7091r5_init(&adc_desc, &adc_ini_param);

}

/**
 * @brief Configure the UART
 *
 * @return int
 */
int configure_uart()
{
	int ret;

	ret = no_os_uart_init(&demo_uart_desc, &demo_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(demo_uart_desc);
	return 0;
}

/**
 * @brief Configure the display
 *
 * @return int
 */
int configure_display()
{
	int ret;

	ret = no_os_i2c_init(&oled_display_i2c_desc, &oled_display_i2c_init_param);
	if (ret) {
#ifdef POWRMS_UART_DEBUG
		printf("Failed to initialize I2C.\n\r");
#endif
		return ret;
	}
	oled_display_extra.i2c_desc = oled_display_i2c_desc;
	ret = display_init(&oled_display, &oled_display_ini_param);
	if (ret) {
#ifdef POWRMS_UART_DEBUG
		printf("Failed to initialize display.\n\r");
#endif
		return ret;
	}
	return 0;
}

void powrms_float_to_str(float var, char* text_buf)
{
	uint8_t i = -1;
	if (!text_buf)
		return -1;

#if INTEGER_PRECISION >= 4
	text_buf[++i] = '0' + (uint16_t)(var / 1000) % 10;
#endif
#if INTEGER_PRECISION >= 3
	text_buf[++i] = '0' + (uint16_t)(var / 100) % 10;
#endif
#if INTEGER_PRECISION >= 2
	text_buf[++i] = '0' + (uint16_t)(var / 10) % 10;
#endif
#if INTEGER_PRECISION >= 1
	text_buf[++i] = '0' + (uint16_t)(var / 1) % 10;
	text_buf[++i] = '.';
#endif
#if FLOATING_POINT_PRECISION >= 1
	text_buf[++i] = '0' + (uint32_t)(var * 10) % 10;
#endif
#if FLOATING_POINT_PRECISION >= 2
	text_buf[++i] = '0' + (uint32_t)(var * 100) % 10;
#endif
#if FLOATING_POINT_PRECISION >= 3
	text_buf[++i] = '0' + (uint32_t)(var * 1000) % 10;
#endif
#if FLOATING_POINT_PRECISION >= 4
	text_buf[++i] = '0' + (uint32_t)(var * 10000) % 10;
#endif
	text_buf[++i] = '\0';
}

void powrms_int_to_str(int var, char* text_buf)
{
	uint8_t i = -1;
	if (!text_buf)
		return -1;

#if INTEGER_PRECISION == 4
	text_buf[++i] = '0' + (uint16_t)(var / 1000) % 10;
#endif
#if INTEGER_PRECISION == 3
	text_buf[++i] = '0' + (uint16_t)(var / 100) % 10;
#endif
#if INTEGER_PRECISION == 2
	text_buf[++i] = '0' + (uint16_t)(var / 10) % 10;
#endif
#if INTEGER_PRECISION == 1
	text_buf[++i] = '0' + (uint16_t)(var / 1) % 10;
	text_buf[++i] = '\0';
#endif
}

void update_values()
{
	input_impedance = 1000 * (input_variables[0][0] - '0') +
			  100 * (input_variables[0][1] - '0') +
			  10 * (input_variables[0][2] - '0') +
			  1 * (input_variables[0][3] - '0') +
			  0.1 * (input_variables[0][4] - '0') +
			  0.01 * (input_variables[0][5] - '0');
	output_impedance = 1000 * (input_variables[1][0] - '0') +
			   100 * (input_variables[1][1] - '0') +
			   10 * (input_variables[1][2] - '0') +
			   1 * (input_variables[1][3] - '0') +
			   0.1 * (input_variables[1][4] - '0') +
			   0.01 * (input_variables[1][5] - '0');
	signal_frequency = 1000 * (input_variables[2][0] - '0') +
			   100 * (input_variables[2][1] - '0') +
			   10 * (input_variables[2][2] - '0') +
			   1 * (input_variables[2][3] - '0') +
			   0.1 * (input_variables[2][4] - '0') +
			   0.01 * (input_variables[2][5] - '0');


#ifdef POWRMS_UART_DEBUG
	// print variables:
	printf(""
	       "input_impedance: %04.2f\n\r"
	       "output_impedance: %04.2f\n\r"
	       "signal_frequency: %04.2f\n\r",
	       input_impedance, output_impedance, signal_frequency);
#endif
}

void settings_screen()
{
	enter_pressed = false;
	pointer_poz_x = 0;
	pointer_poz_y = 0;

// ------------------ SETUP SCREEN ------------------

	const char *first_column[3] = VARIABLE_NAMES;
	char input_char;

	// Grid descriptors
	static lv_coord_t col_dsc[] = {
		LV_GRID_FR(26), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
		LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
		LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST
	};
	static lv_coord_t row_dsc[] = {LV_GRID_FR(10), LV_GRID_FR(10), LV_GRID_FR(10), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

	lv_obj_t *screen = lv_obj_create(NULL);
	lv_obj_set_grid_dsc_array(screen, col_dsc, row_dsc);

	// Store only value labels (we don't need to store name labels)
	lv_obj_t *value_labels[VARIABLE_NUMBER][NUMERIC_LENGTH];

	for (int row = 0; row < VARIABLE_NUMBER; row++) {
		// Name label
		lv_obj_t *label_name = lv_label_create(screen);
		lv_label_set_text(label_name, first_column[row]);
		lv_obj_set_grid_cell(label_name, LV_GRID_ALIGN_START, 0, 1,
				     LV_GRID_ALIGN_START, row, 1);

		uint8_t col = 1;
		for (uint8_t var = 0; var < NUMERIC_LENGTH; var++) {
			if (var == INTEGER_NUMERIC_LENGTH) {
				// Comma label
				lv_obj_t *comma_label = lv_label_create(screen);
				lv_label_set_text(comma_label, ".");
				lv_obj_set_grid_cell(comma_label, LV_GRID_ALIGN_START, col++, 1,
						     LV_GRID_ALIGN_START, row, 1);
			}

			value_labels[row][var] = lv_label_create(screen);
			char text[2] = {input_variables[row][var], '\0'};
			lv_label_set_text(value_labels[row][var], text);
			lv_obj_set_grid_cell(value_labels[row][var], LV_GRID_ALIGN_START, col++, 1,
					     LV_GRID_ALIGN_START, row, 1);
		}
	}

	// Highlight current label
	lv_obj_set_style_text_decor(value_labels[pointer_poz_y][pointer_poz_x],
				    LV_TEXT_DECOR_UNDERLINE, 0);
	lv_scr_load(screen);
	lv_timer_handler();

// ---------------- INPUT LOOP & SCREEN UPDATE ------------------

	while (display_entry == 2) {

		uint8_t tmp = read_input();
		switch (tmp) {
		case 1: // NEXT
			lv_obj_set_style_text_decor(value_labels[pointer_poz_y][pointer_poz_x],
						    LV_TEXT_DECOR_NONE, 0);
			if (pointer_poz_x < NUMERIC_LENGTH - 1) {
				// Advance pointer
				pointer_poz_x++;
			} else {
				pointer_poz_x = 0;
				pointer_poz_y = (pointer_poz_y + 1) % VARIABLE_NUMBER;
			}

			// Set highlight on new position
			lv_obj_set_style_text_decor(value_labels[pointer_poz_y][pointer_poz_x],
						    LV_TEXT_DECOR_UNDERLINE, 0);
			lv_timer_handler();

			break;
		case 2: // BACK OR INCREMENT
			if (enter_pressed) { // Increment selected digit
				input_variables[pointer_poz_y][pointer_poz_x] =
					(input_variables[pointer_poz_y][pointer_poz_x] == '9') ? '0'
					: input_variables[pointer_poz_y][pointer_poz_x] + 1;
				char text[2] = {input_variables[pointer_poz_y][pointer_poz_x], '\0'};
				lv_label_set_text(value_labels[pointer_poz_y][pointer_poz_x], text);
				lv_timer_handler();
			} else { // Back button
				display_entry = 0;
				lv_obj_del(screen);
				return;
			}
			break;
		case 3: // ENTER
			enter_pressed = !enter_pressed;
			update_values();
			break;
		default:
			break;
		}
	}
}


void underline_in_main_screen(lv_obj_t *label1, lv_obj_t *label2,
			      bool show_menu)
{
	bool tmp_var = show_menu ? LV_TEXT_DECOR_UNDERLINE : LV_TEXT_DECOR_NONE;
	lv_obj_set_style_text_decor(label1, tmp_var, LV_PART_MAIN);
	lv_obj_set_style_text_decor(label2, !tmp_var, LV_PART_MAIN);
}

void main_menu()
{
	char input_char;

// --------------------- SETUP SCREEN ------------------

	lv_obj_t *scr = lv_obj_create(NULL);
	lv_obj_t *label1 = lv_label_create(scr);
	lv_label_set_text(label1, "MAIN SCREEN");
	lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 10);
	lv_obj_set_style_text_color(label1, lv_color_black(), LV_PART_MAIN);

	lv_obj_t *label2 = lv_label_create(scr);
	lv_label_set_text(label2, "SETTINGS");
	lv_obj_align_to(label2, label1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
	lv_obj_set_style_text_color(label2, lv_color_black(), LV_PART_MAIN);

	underline_in_main_screen(label1, label2, show_menu);

	lv_scr_load(scr);
	lv_timer_handler();

// ---------------- INPUT LOOP & SCREEN UPDATE ------------------

	while (display_entry == 0) {
		uint8_t tmp = read_input();
		switch (tmp) {
		case 1: // NEXT
			show_menu = !show_menu;
			underline_in_main_screen(label1, label2, show_menu);
			lv_scr_load(scr);
			lv_timer_handler();
			break;
		case 2: // BACK
			// NOP
			break;

		case 3: // ENTER
			if (show_menu) {
				display_entry = 1;
				lv_obj_del(scr);
				return;
			} else {
				display_entry = 2;
				lv_obj_del(scr);
				return;
			}
			break;
		default:
			break;
		}
	}
}

int update_adc_data()
{
	int ret = 0;

	ret = ad7091r5_read_one(adc_desc, ADC_VRMSF_CHANNEL_NO,
				&(adc_data_input.adc_vrmsf));
	if (ret) {
#ifdef POWRMS_UART_DEBUG
		printf("Failed to read ADC channel %d\n\r", ADC_VRMSF_CHANNEL_NO);
#endif
		return ret;
	}

	adc_data_input.adc_vrmsf = (adc_data_input.adc_vrmsf * ADC_V_REF) /
				   ADC_COUNTER_MAX;

	ret = ad7091r5_read_one(adc_desc, ADC_VRMSR_CHANNEL_NO,
				&(adc_data_input.adc_vrmsr));
	if (ret) {
#ifdef POWRMS_UART_DEBUG
		printf("Failed to read ADC channel %d\n\r", ADC_VRMSR_CHANNEL_NO);
#endif
		return ret;
	}

	adc_data_input.adc_vrmsr = (adc_data_input.adc_vrmsr * ADC_V_REF) /
				   ADC_COUNTER_MAX;

#ifdef POWRMS_UART_DEBUG
	printf("#");
#endif
	return 0;
}

void show_screen()
{
	// Create a new screen object
	char input_char;
	char out_text[10];
	lv_obj_t *scr = lv_obj_create(NULL);

// --------------------- SETUP SCREEN ------------------

	// Define grid: 2 columns (label, value) and 2 rows (for 2 items)
	static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
	lv_obj_set_grid_dsc_array(scr, col_dsc, row_dsc);

	// Measurement names and value labels
	const char *names[] = OUTPUT_VARIABLE_NAMES;
	lv_obj_t *label_values[2];  // Store references to value labels

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

// ---------------- INPUT LOOP & SCREEN UPDATE ------------------

	while (display_entry == 1) {

#ifdef POWRMS_UART_DEBUG
		printf(".");
#endif

		if (!update_adc_data()) {
			powrms_float_to_str(adc_data_input.adc_vrmsf, out_text);
			lv_label_set_text(label_values[0], out_text);
			powrms_float_to_str(adc_data_input.adc_vrmsr, out_text);
			lv_label_set_text(label_values[1], out_text);
			lv_timer_handler();  // Process LVGL updates
		}

		uint8_t tmp = read_input();
		switch (tmp) {
		case 1: // NEXT
			break;
		case 2: // BACK
			display_entry = 0;
			lv_obj_del(scr);
			break;
		case 3: // ENTER
			break;
		default:
			break;
		}
	}
}

void screen_while()
{
	// display_entry = 0: Selection menu
	// display_entry = 1: Show manu
	// display_entry = 2: Settings
	char input_char;
	while (1) {
		switch (display_entry) {
			{
			case 0:
				main_menu();
				break;
			case 1:
				show_screen();
				break;
			case 2:
				settings_screen();
				break;
			default:
				return;
			}
		}
	}
}

/**
 * @brief Main function
 *
 * @return int
 */
int example_main(void)
{
	int ret;

	no_os_init();

	ret = configure_uart();
	if (ret) {
#ifdef POWRMS_UART_DEBUG
		printf("Failed to initialize UART.\n\r");
#endif
		return ret;
	}
#ifdef POWRMS_UART_DEBUG
	printf("UART initialized.\n\r");
#endif

	ret = configure_adc();
	if (ret) {
#ifdef POWRMS_UART_DEBUG
		printf("Failed to initialize ADC.\n\r");
#endif
		return ret;
	}
#ifdef POWRMS_UART_DEBUG
	printf("ADC initialized.\n\r");
#endif

	ret = configure_display();
	if (ret) {
#ifdef POWRMS_UART_DEBUG
		printf("Failed to initialize display.\n\r");
#endif
		return ret;
	}
#ifdef POWRMS_UART_DEBUG
	printf("Display initialized.\n\r");
#endif

	ret = display_task();
	if (ret) {
#ifdef POWRMS_UART_DEBUG
		printf("Failed to initialize display task.\n\r");
#endif
		return ret;
	}
#ifdef POWRMS_UART_DEBUG
	printf("Display task started.\n\r");
#endif

	screen_while();

	return 0;
}
