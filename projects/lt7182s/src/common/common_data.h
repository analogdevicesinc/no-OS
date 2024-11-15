/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by lt7182s examples.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "no_os_i2c.h"
#include "lt7182s.h"

#define LT7182S_PMBUS_ADDRESS                   0x4F

extern struct no_os_uart_init_param lt7182s_uart_ip;
extern struct no_os_i2c_init_param lt7182s_i2c_ip;
extern struct lt7182s_init_param lt7182s_ip;

#endif /* __COMMON_DATA_H__ */
