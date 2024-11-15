/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by max14919 example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "max14919.h"

extern struct no_os_uart_init_param max14919_uart_ip;
extern struct no_os_gpio_init_param max14919_fault_ip;
extern struct no_os_gpio_init_param max14919_in_ip[4];
extern struct no_os_gpio_init_param max14919_in_rush_ip;
extern struct no_os_gpio_init_param max14919_rev_ip;
extern struct max14919_init_param max14919_ip;

#endif /* __COMMON_DATA_H__ */
