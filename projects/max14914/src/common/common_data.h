/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by max14914 example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "max14914.h"

extern struct no_os_uart_init_param max14914_uart_ip;
extern struct no_os_gpio_init_param max14914_fault_ip;
extern struct no_os_gpio_init_param max14914_doilvl_ip;
extern struct no_os_gpio_init_param max14914_di_en_ip;
extern struct no_os_gpio_init_param max14914_in_ip;
extern struct no_os_gpio_init_param max14914_pp_ip;
extern struct max14914_init_param max14914_ip;

#endif /* __COMMON_DATA_H__ */
