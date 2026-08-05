/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ltm4700 examples.
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "no_os_uart.h"
#include "no_os_i2c.h"
#include "ltm4700.h"
#include "parameters.h"

extern struct no_os_uart_init_param uart_ip;
extern struct no_os_i2c_init_param i2c_ip;
extern struct ltm4700_init_param ltm4700_ip;

#endif /* __COMMON_DATA_H__ */
