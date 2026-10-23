/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by EVAL-POWRMS example.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
#include "powrms_max77986.h"

#define FW_VERSION_MAJOR            1
#define FW_VERSION_MINOR            0
#define FW_VERSION_PATCH            0

#define DISPLAY_HOR_REZ             128
#define DISPLAY_VER_REZ             64

#define ADC_I2C_SLAVE_ADDRESS       0x2F

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

// GPIOs
extern struct no_os_gpio_desc *boost_en_gpio_desc;
extern struct no_os_gpio_desc *en_adc_5v_gpio_desc;
extern struct no_os_gpio_desc *en_adc_vdrive_gpio_desc;
extern struct no_os_gpio_desc *on_off_gpio_desc;
extern struct no_os_gpio_desc *en_latch_gpio_desc;

extern struct no_os_gpio_init_param boost_en_ip;
extern struct no_os_gpio_init_param en_adc_5v_ip;
extern struct no_os_gpio_init_param en_adc_vdrive_ip;
extern struct no_os_gpio_init_param en_latch_vdrive_ip;

// IIO
extern struct no_os_uart_init_param uart_iio_ip_stdio;
extern struct no_os_uart_init_param iio_demo_usb_ip;
extern struct no_os_uart_init_param iio_demo_swd_ip;

// Debug / stdio UART (shares the IIO UART)
extern struct no_os_uart_desc *uart_iio_desc;

// M24512
extern struct no_os_eeprom_desc *m24512_desc;
extern struct no_os_eeprom_init_param eeprom_m24512_init_param;

// MAX77986
extern struct no_os_i2c_desc *max77986_i2c_desc;
extern struct no_os_i2c_init_param max77986_i2c_init_param;

// Functions

void _update_adc_data();

#endif /* __COMMON_DATA_H__ */
