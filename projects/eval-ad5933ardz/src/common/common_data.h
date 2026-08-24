/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-ad5933ardz examples.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ad5933.h"

#define EXTERNAL_CLK_SPEED 16000000

extern struct no_os_uart_init_param uip;

extern const struct no_os_i2c_init_param ad5933_i2c_ip;

extern struct ad5933_init_param ad5933_user_init;

#endif /* __COMMON_DATA_H__ */
