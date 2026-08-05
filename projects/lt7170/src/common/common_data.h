/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by lt7170 examples.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "no_os_i2c.h"
#include "parameters.h"
#include "lt7170.h"

#define LT7170_PMBUS_ADDRESS                   0x4F

extern struct no_os_uart_init_param lt7170_uart_ip;
extern struct no_os_i2c_init_param lt7170_i2c_ip;
extern struct lt7170_init_param lt7170_ip;

#endif /* __COMMON_DATA_H__ */
