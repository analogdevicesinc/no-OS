/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by issd1306 example.
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

#include "common_data.h"

struct display_dev *oled_display;
struct ad7091r5_dev *adc_desc;
struct no_os_eeprom_desc *m24512_desc;

struct no_os_i2c_desc *oled_display_i2c_desc;
struct no_os_i2c_desc *ltc3556_i2c_desc;

struct no_os_gpio_desc *mcu_led_gpio_desc;
struct no_os_gpio_desc *en_adc_5v_gpio_desc;
struct no_os_gpio_desc *en_adc_vdrive_gpio_desc;
struct no_os_gpio_desc *en_latch_gpio_desc;


struct no_os_i2c_init_param oled_display_i2c_init_param = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_SPEED,
	.slave_address = SSD1306_I2C_ADDR,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA_DISPLAY,
};

ssd_1306_extra oled_display_extra = {
	.comm_type = SSD1306_I2C,
	.i2c_ip = &oled_display_i2c_init_param,
};

struct display_init_param oled_display_ini_param = {
	.cols_nb = DISPLAY_HOR_REZ,
	.rows_nb = DISPLAY_VER_REZ,
	.controller_ops = &ssd1306_ops,
	.extra = &oled_display_extra,
};

struct no_os_i2c_init_param	adc_i2c_ini_param = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_SPEED,
	.slave_address = ADC_I2C_SLAVE_ADDRESS,
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA_ADC,
};

struct no_os_gpio_init_param adc_gpio_ini_param = {
	.number = GPIO_ADC_RESET,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct ad7091r5_init_param adc_ini_param = {
	.i2c_init = &adc_i2c_ini_param,
	.gpio_resetn = &adc_gpio_ini_param,
};

struct no_os_i2c_init_param ltc3556_i2c_init_param = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_SPEED,
	.slave_address = 0x09,
	.platform_ops = I2C_OPS,
	.extra = GPIO_EXTRA,
};

// LEDs
struct no_os_gpio_init_param mcu_led_ip = {
	.port = 0,
	.number = 9,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param en_adc_5v_ip = {
	.port = 0,
	.number = 19,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct no_os_gpio_init_param en_adc_vdrive_ip = {
	.port = 0,
	.number = 20,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

// IIO

struct no_os_uart_init_param iio_demo_usb_ip = {
	.device_id = UART_DEVICE_ID,
	.irq_id = UART_IRQ_ID,
	.asynchronous_rx = true,
	.baud_rate = UART_BAUDRATE,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = UART_EXTRA_A,
	.platform_ops = &max_uart_ops,
}; // USB initialization parameter for iio connection

// M24512

struct no_os_gpio_init_param wc_gpio_init = {
	.port = M24512_WC_PORT,
	.number = M24512_WC_NR,
	.platform_ops = GPIO_OPS,
	.extra = GPIO_EXTRA,
};

struct m24512_init_param m24512_init_extra_param = {
	.i2c_init = {
		.device_id = M24512_I2C_DEVICE_ID,
		.max_speed_hz = M24512_I2C_SPEED,
		.slave_address = M24512_I2C_ADDR,
		.platform_ops = I2C_OPS,
		.extra = I2C_EXTRA_ADC,
	},
	.wc_gpio_init = &wc_gpio_init,
	.i2c_addr = M24512_I2C_ADDR,
};

struct no_os_eeprom_init_param eeprom_m24512_init_param = {
	.device_id = 0,
	.platform_ops = &eeprom_m24512_ops,
	.extra = &m24512_init_extra_param,
};