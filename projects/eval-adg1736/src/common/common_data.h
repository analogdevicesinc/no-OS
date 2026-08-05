/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-adg1736 project.
 *   @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "adg1736.h"

extern struct no_os_uart_init_param adg1736_uart_ip;
extern struct adg1736_init_param adg1736_ip;

/* LEDs */
extern struct no_os_gpio_init_param led_red_ip;
extern struct no_os_gpio_init_param led_green_ip;

/* Test signals */
extern struct no_os_gpio_init_param gpio_d_ip;
extern struct no_os_gpio_init_param gpio_s_ip;

#endif /* __COMMON_DATA_H__ */
