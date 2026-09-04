/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Defines common data to be used by MAX20362 examples.
 *   @author Wilbert Jethro R. Limjoco (Wilbertjethro.Limjoco@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "no_os_uart.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "max20362.h"
#include "parameters.h"

extern struct no_os_uart_init_param uart_ip;
extern struct max20362_init_param max20362_ip;

#endif // __COMMON_DATA_H__
