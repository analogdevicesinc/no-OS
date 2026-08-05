/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by adp5055 examples.
 *   @author Angelo Catapang (angelo.catapang@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_i2c.h"
#include "adp5055.h"

extern struct no_os_uart_init_param adp5055_uart_ip;
extern struct no_os_i2c_init_param adp5055_i2c_ip;
extern struct adp5055_init_param adp5055_ip;

// External Feedback Resistors
#define EXAMPLE_RTOP1 20000.0
#define EXAMPLE_RBOT1 30100.0
#define EXAMPLE_RTOP2 20000.0
#define EXAMPLE_RBOT2 16900.0
#define EXAMPLE_RTOP3 20000.0
#define EXAMPLE_RBOT3 10000.0

// Target Output Voltage
#define EXAMPLE_VOUT1 1.2
#define EXAMPLE_VOUT2 1.3
#define EXAMPLE_VOUT3 1.8

#endif /* __COMMON_DATA_H__ */
