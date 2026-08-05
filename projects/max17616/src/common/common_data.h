/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by max17616 examples.
 *   @author Carlos Jones (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "no_os_uart.h"
#include "no_os_i2c.h"
#include "max17616.h"
#include "parameters.h"

extern struct no_os_uart_init_param uart_ip;
extern struct no_os_i2c_init_param i2c_ip;
extern struct max17616_init_param max17616_ip;

#endif /* __COMMON_DATA_H__ */
