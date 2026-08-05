/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by adp1055 examples.
 *   @author Ivan Gil Mercano (ivangil.mercano@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_pwm.h"
#include "adp1055.h"

#define ADP1055_PMBUS_DEFAULT_ADDRESS   0x4B

extern struct no_os_uart_init_param adp1055_uart_ip;
extern struct no_os_i2c_init_param adp1055_i2c_ip;
extern struct no_os_gpio_init_param adp1055_pg_alt_ip;
extern struct adp1055_init_param adp1055_ip;

#endif /* __COMMON_DATA_H__ */
