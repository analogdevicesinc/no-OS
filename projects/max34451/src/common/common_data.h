/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by MAX34451 examples.
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_i2c.h"
#include "max34451.h"

#define MAX34451_ADDRESS                         0x4E

extern struct no_os_uart_init_param max34451_uart_ip;
extern struct no_os_i2c_init_param max34451_i2c_ip;
extern struct max34451_init_param max34451_ip;

#endif /* __COMMON_DATA_H__ */
