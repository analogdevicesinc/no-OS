/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions used by the LTC7841 project.
 *   @author Marvin Cabuenas (marvinneil.cabuenas@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_gpio.h"
#include "maxim_i2c.h"
#include "maxim_uart.h"

#define I2C_OPS                                &max_i2c_ops
#define UART_OPS                               &max_uart_ops

/* LTC7841 parameters */
#define LTC7841_I2C_DEVICE_ID                  0
#define LTC7841_I2C_CLK_SPEED                  400000
#define LTC7841_I2C_ADDR                       0x20
/* Unit in MilliOhms*/
#define RSENSE_VALUE_DC2798A                   2

extern struct max_uart_init_param uart_extra_ip;
extern const struct max_i2c_init_param ltc7841_i2c_extra;
extern const struct max_gpio_init_param ltc7841_gpio_extra;

#endif /* __PARAMETERS_H__ */
