/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ssd1306 example.
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
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "no_os_util.h"
#include "display.h"
#include "ssd_1306.h"
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "maxim_i2c.h"
#include "ad7091r5.h"
#include "iio_powrms.h"
#include "global_screen.h"
#include "m24512.h"

#define DISPLAY_HOR_REZ             128
#define DISPLAY_VER_REZ             64

#define ADC_I2C_SLAVE_ADDRESS       0x2A

// Display related data
extern struct display_dev *oled_display;
extern ssd_1306_extra oled_display_extra;
extern struct no_os_i2c_desc *oled_display_i2c_desc;
extern struct display_init_param oled_display_ini_param;
extern struct no_os_i2c_init_param oled_display_i2c_init_param;

// ADC related data
extern struct no_os_i2c_init_param	adc_i2c_ini_param;
extern struct no_os_gpio_init_param	adc_gpio_ini_param;
extern struct ad7091r5_init_param adc_ini_param;
extern struct ad7091r5_dev *adc_desc;
extern volatile struct adc_data adc_data_input;

// LTC3556 data
extern struct no_os_i2c_desc *ltc3556_i2c_desc;
extern struct no_os_i2c_init_param ltc3556_i2c_init_param;

// GPIOs
extern struct no_os_gpio_desc *mcu_led_gpio_desc;
extern struct no_os_gpio_desc *en_adc_5v_gpio_desc;
extern struct no_os_gpio_desc *en_adc_vdrive_gpio_desc;
extern struct no_os_gpio_desc *en_latch_gpio_desc;

extern struct no_os_gpio_init_param mcu_led_ip;
extern struct no_os_gpio_init_param en_adc_5v_ip;
extern struct no_os_gpio_init_param en_adc_vdrive_ip;

// IIO
extern struct no_os_uart_init_param uart_iio_ip_stdio;
extern struct no_os_uart_init_param iio_demo_usb_ip;
extern struct no_os_uart_init_param iio_demo_swd_ip;

// M24512
extern struct no_os_eeprom_desc *m24512_desc;
extern struct no_os_eeprom_init_param eeprom_m24512_init_param;

// Functions

void _update_adc_data();

#endif /* __COMMON_DATA_H__ */
