/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltp8800 examples.
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
#include "ltp8800.h"

#define LTP8800_ADDRESS                         0x41

extern struct no_os_uart_init_param ltp8800_uart_ip;
extern struct no_os_i2c_init_param ltp8800_i2c_ip;
extern struct ltp8800_init_param ltp8800_ip;

#endif /* __COMMON_DATA_H__ */
