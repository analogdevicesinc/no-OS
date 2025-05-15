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

struct no_os_uart_desc *demo_uart_desc;

bool show_menu = 1;
bool enter_pressed = 0;
enum display_entry_t display_entry = DISPLAY_INPUT_FREQUENCY;

void my_flush_cb(lv_display_t * disp, const lv_area_t * area,
                 uint8_t * color_p)
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

    printf("Display initialized.\n\r");

    return 0;
}

void screen_while()
{
    char input_char;
    while (1) {
        switch (display_entry) {
        case DISPLAY_INPUT_FREQUENCY:
            start_freq_menu();
            break;
        case DISPLAY_ENTRY_MENU:
            main_menu_screen();
            break;
        case DISPLAY_ENTRY_SHOW:
            show_screen();
            break;
        case DISPLAY_ENTRY_SETTINGS:
            value_settings_screen();
            break;
        default:
            return;
        }
    }
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
        printf("Failed to initialize I2C for LTC3556.\n\r");
        return ret;
    }


    uint8_t data_to_write[2] = {0xFF, 0x9F};

    // ret = no_os_i2c_write(ltc3556_i2c_desc, data_to_write, sizeof(data_to_write), 1);
    // if (ret) {
    //     printf("Failed to write data to LTC3556.\n\r");
    //     return ret;
    // }

    no_os_mdelay(100);


    printf("LTC3556 initialized successfully.\n\r");

    return 0;
}

int configure_gpios()
{
    int ret;

    // Set MCU_LED to HIGH
    ret = no_os_gpio_get(&mcu_led_gpio_desc, &mcu_led_ip);
    if (ret) {
        printf("Failed to initialize MCU LED GPIO.\n\r");
        return ret;
    }
    ret = no_os_gpio_direction_output(mcu_led_gpio_desc, NO_OS_GPIO_HIGH);
    if (ret) {
        printf("Failed to set MCU LED GPIO direction.\n\r");
        return ret;
    }

    // Set EN_ADC_5V to HIGH
    ret = no_os_gpio_get(&en_adc_5v_gpio_desc, &en_adc_5v_ip);
    if (ret) {
        printf("Failed to initialize EN_ADC_5V GPIO.\n\r");
        return ret;
    }
    ret = no_os_gpio_direction_output(en_adc_5v_gpio_desc, NO_OS_GPIO_HIGH);
    if (ret) {
        printf("Failed to set EN_ADC_5V GPIO direction.\n\r");
        return ret;
    }

    // Wait for 50 msec
    no_os_mdelay(50);

    // Set EN_ADC_VDRIVE to HIGH
    ret = no_os_gpio_get(&en_adc_vdrive_gpio_desc, &en_adc_vdrive_ip);
    if (ret) {
        printf("Failed to initialize EN_ADC_VDRIVE GPIO.\n\r");
        return ret;
    }
    ret = no_os_gpio_direction_output(en_adc_vdrive_gpio_desc, NO_OS_GPIO_HIGH);
    if (ret) {
        printf("Failed to set EN_ADC_VDRIVE GPIO direction.\n\r");
        return ret;
    }

    // Wait for 100 msec
    no_os_mdelay(100);

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

    ret = configure_uart();
    if (ret) {
        printf("Failed to initialize UART.\n\r");
        return ret;
    }

// 	ret = configure_adc();
// 	if (ret) {
// 		printf("Failed to initialize ADC.\n\r");
// 		return ret;
// 	}

// LTC3556 initialize
    ret = configure_ltc3556();
    if (ret) {
        printf("Failed to initialize LTC3556.\n\r");
        return ret;
    }

    ret  = configure_gpios();
    if (ret) {
        printf("Failed to initialize GPIOs.\n\r");
        return ret;
    }

    ret = configure_display();
    if (ret) {
        printf("Failed to initialize display.\n\r");
        return ret;
    }

    ret = display_task();
    if (ret) {
        printf("Failed to initialize display task.\n\r");
        return ret;
    }

    screen_while();

    return 0;
}
