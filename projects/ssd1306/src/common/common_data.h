/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ssd1306 example.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_util.h"
#include "display.h"
#include "ssd_1306.h"
#include "no_os_delay.h"
#include "no_os_i2c.h"
#include "maxim_i2c.h"
#include "example.h"

extern struct display_dev *oled_display;
extern struct no_os_i2c_desc *oled_display_i2c_desc;
extern struct no_os_uart_init_param demo_uart_ip;
extern struct no_os_i2c_init_param oled_display_i2c_init_param;
extern ssd_1306_extra oled_display_extra;
extern struct display_init_param oled_display_ini_param;

#endif /* __COMMON_DATA_H__ */
