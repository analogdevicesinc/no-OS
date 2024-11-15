/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by adp1050 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_pwm.h"
#include "adp1050.h"

extern struct no_os_uart_init_param adp1050_uart_ip;
extern struct no_os_i2c_init_param adp1050_i2c_ip;
extern struct no_os_gpio_init_param adp1050_pg_alt_ip;
extern struct adp1050_init_param adp1050_ip;

#endif /* __COMMON_DATA_H__ */
