/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Common data header for LTC4284 project
 *   @author Carlos Jones Jr <carlosjr.jones@analog.com>
 *   @author Christopher de Guzman <christopher.deguzman@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include <stdbool.h>
#include <stddef.h>
#include "no_os_uart.h"
#include "no_os_i2c.h"
#include "ltc4284.h"

extern struct no_os_uart_init_param ltc4284_uart_ip;
extern struct no_os_i2c_init_param ltc4284_i2c_ip;
extern struct ltc4284_init_param ltc4284_ip;

#endif /* __COMMON_DATA_H__ */
