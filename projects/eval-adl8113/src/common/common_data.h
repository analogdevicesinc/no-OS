/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-adl8113 project.
 *   @author Raul Geo <Raul.Georgian@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "adl8113.h"
#include "no_os_gpio.h"
#include "no_os_uart.h"
#include "parameters.h"

extern struct no_os_gpio_init_param led_red_ip;
extern struct no_os_gpio_init_param led_green_ip;
extern struct no_os_gpio_init_param led_blue_ip;
extern struct no_os_gpio_init_param button1_ip;
extern struct no_os_gpio_init_param button2_ip;
extern struct adl8113_init_param adl8113_param;
extern struct no_os_uart_init_param adl8113_uart_ip;

#endif /* __COMMON_DATA_H__ */
