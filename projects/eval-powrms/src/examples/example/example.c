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
#include "buttons.h"
#include "m24512.h"
#include "../screens/subscreen_blank_screen.h"

// Timing constants for hardware operations
#define LTC3556_INIT_DELAY_MS       50
#define ADC_POWER_STABILIZE_MS      100
#define ADC_5V_STABILIZE_MS         50

// Buffer for UI usage - must be defined in .c file to avoid multiple instantiation
// Use LVGL's memory alignment macro for proper alignment across platforms
LV_ATTRIBUTE_MEM_ALIGN uint8_t display_buffer[SSD1306_HOR_REZ *
				SSD1306_VER_REZ / 8 + 8];

struct powrms_variables input_variables[VARIABLE_NUMBER] = {
	{
		.name = INPUT_IMPEDANCE,
		.value = 50.0,
		.min_possible_value = 0.0,
		.max_possible_value = 1000.0,
		.nr_of_int_digits = INTEGER_NUMERIC_LENGTH,
		.nr_of_float_digits = FLOATING_POINT_LENGTH,
		.digits = {'0', '0', '5', '0', '0', '0'},
		.unit = " Ohm"
	},
	{
		.name = OUTPUT_IMPEDANCE,
		.value = 50.0,
		.min_possible_value = 0.0,
		.max_possible_value = 1000.0,
		.nr_of_int_digits = INTEGER_NUMERIC_LENGTH,
		.nr_of_float_digits = FLOATING_POINT_LENGTH,
		.digits = {'0', '0', '5', '0', '0', '0'},
		.unit = " Ohm"

	},
	{
		.name = SIGNAL_FREQUENCY, // MHz
		.value = 1000.0,
		.min_possible_value = 10.0,
		.max_possible_value = 7000.0,
		.nr_of_int_digits = INTEGER_NUMERIC_LENGTH,
		.nr_of_float_digits = FLOATING_POINT_LENGTH,
		.digits = {'1', '0', '0', '0', '0', '0'},
		.unit = " MHz"
	}
};

bool show_menu = 1;
bool enter_pressed = 0;
enum display_entry_t display_entry = DISPLAY_ENTRY_SHOW;

void my_flush_cb(lv_display_t * disp, const lv_area_t * area,
		 uint8_t * color_p)
{
	uint8_t * lvgl_buff = &display_buffer[8];
	uint8_t buff_to_ssd1306[8 * 128] = {0};

	no_os_row_major_to_column_major_8bits(lvgl_buff, buff_to_ssd1306,
					      SSD1306_HOR_REZ, SSD1306_VER_REZ);

	ssd_1306_print_buffer(oled_display, (char *)buff_to_ssd1306);
	lv_display_flush_ready(disp);
}

/**
 * @brief Initialize the display task
 *
 * @return int
 */
int display_task()
{
	lv_init();
	memset(&display_buffer[8], 0x00, sizeof(display_buffer) - 8);
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
	return ad7091r5_init(&adc_desc, &adc_ini_param);
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
		return ret;
	}
	oled_display_extra.i2c_desc = oled_display_i2c_desc;
	ret = display_init(&oled_display, &oled_display_ini_param);
	if (ret) {
		return ret;
	}


	return 0;
}

int screen_while(void *)
{
	// Update ADC data
	_update_adc_data();

	// Process screen based on current display entry
	switch (display_entry) {
	case DISPLAY_INPUT_FREQUENCY:
		subscreen_startup_freq_screen();
		break;
	case DISPLAY_ENTRY_MENU:
		subscreen_main_menu_screen();
		break;
	case DISPLAY_ENTRY_SHOW:
		subscreen_show_screen();
		break;
	case DISPLAY_ENTRY_SETTINGS:
		subscreen_settings_screen();
		break;
	case DISPLAY_ENTRY_BLANK:
		subscreen_blank_screen();
		break;
	default:
		return -1;
	}

	return 0;
}

int ltc3556_write_init_command(struct no_os_i2c_desc *param)
{
	uint8_t data_to_write[2] = {0xFF, 0x85};
	int ret;

	ret = no_os_i2c_write(param, data_to_write, sizeof(data_to_write), 1);
	if (ret) {
		return ret;
	}

	no_os_mdelay(LTC3556_INIT_DELAY_MS);
	return 0;
}

/**
 * @brief Configure the LTC3556
 *
 * @return int
 */

int configure_ltc3556()
{
	int ret;

	ret = no_os_i2c_init(&ltc3556_i2c_desc, &ltc3556_i2c_init_param);
	if (ret) {
		return ret;
	}

	uint8_t data_to_write[2] = {0xFF, 0x81};
	ret = no_os_i2c_write(ltc3556_i2c_desc, data_to_write, sizeof(data_to_write),
			      1);
	if (ret) {
		no_os_i2c_remove(ltc3556_i2c_desc);
		return ret;
	}

	no_os_mdelay(LTC3556_INIT_DELAY_MS);

	ret = ltc3556_write_init_command(ltc3556_i2c_desc);
	if (ret) {
		no_os_i2c_remove(ltc3556_i2c_desc);
		return ret;
	}

	return 0;
}

int get_en_latch_value(void)
{
	int ret;
	uint8_t gpio_val;

	ret = no_os_gpio_get_value(en_latch_gpio_desc, &gpio_val);
	if (ret) {
		return ret;
	}

	return gpio_val;
}
int configure_gpios()
{
	int ret;

	// Set MCU_LED to HIGH
	ret = no_os_gpio_get(&mcu_led_gpio_desc, &mcu_led_ip);
	if (ret) {
		return ret;
	}
	ret = no_os_gpio_direction_output(mcu_led_gpio_desc, NO_OS_GPIO_HIGH);
	if (ret) {
		return ret;
	}

	// Set EN_ADC_5V to HIGH
	ret = no_os_gpio_get(&en_adc_5v_gpio_desc, &en_adc_5v_ip);
	if (ret) {
		return ret;
	}
	ret = no_os_gpio_direction_output(en_adc_5v_gpio_desc, NO_OS_GPIO_HIGH);
	if (ret) {
		return ret;
	}

	// Wait for ADC 5V rail to stabilize
	no_os_mdelay(ADC_5V_STABILIZE_MS);

	// Set EN_ADC_VDRIVE to HIGH
	ret = no_os_gpio_get(&en_adc_vdrive_gpio_desc, &en_adc_vdrive_ip);
	if (ret) {
		return ret;
	}
	ret = no_os_gpio_direction_output(en_adc_vdrive_gpio_desc, NO_OS_GPIO_HIGH);
	if (ret) {
		return ret;
	}

	// Wait for ADC power to stabilize
	no_os_mdelay(ADC_POWER_STABILIZE_MS);
	return 0;
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

	ret = no_os_eeprom_init(&m24512_desc, &eeprom_m24512_init_param);
	if (ret) {
		return ret;
	}

	// ret = configure_ltc3556();
	// if (ret) {
	//     return ret;
	// }

	// ret = configure_gpios();
	// if (ret) {
	//     return ret;
	// }

	// ret = configure_adc();
	// if (ret) {
	//     return ret;
	// }

	// ret = configure_display();
	// if (ret) {
	//     return ret;
	// }

	// ret = display_task();
	// if (ret) {
	//     return ret;
	// }

	// ret = buttons_init();
	// if (ret) {
	//     return ret;
	// }

	// static int powrms_desc = 0;
	// struct iio_app_desc *app;
	// struct iio_app_init_param app_init_param = {0};
	// struct iio_data_buffer buff = {
	//     .buff = (void *)iio_data_buffer_loc,
	//     .size = MAX_SIZE_BASE_ADDR * sizeof(IIO_BUFF_TYPE),
	// };

	// memset(iio_data_buffer_loc, 0, MAX_SIZE_BASE_ADDR * sizeof(IIO_BUFF_TYPE));

	// struct iio_app_device devices[] = {
	//     IIO_APP_DEVICE("powrms", &powrms_desc, &powrms_iio_descriptor, &buff, NULL, NULL)
	// };

	// app_init_param.devices = devices;
	// app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	// app_init_param.uart_init_params = iio_demo_usb_ip;

	// // Entering display
	// display_entry = DISPLAY_ENTRY_SHOW;

	// app_init_param.post_step_callback = &(screen_while);
	// ret = iio_app_init(&app, app_init_param);

	// if (ret)
	//     return ret;

	// ret = iio_app_run(app);

	// iio_app_remove(app);

	return 0;
}
