/*******************************************************************************
 *   @file   parameters.h
 *   @brief  Definitions used by the MAX20362 project.
 *   @author Wilbert Jethro R. Limjoco (Wilbertjethro.Limjoco@analog.com)
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

#define UART_DEVICE_ID                         3
#define UART_ASYNC_RX                          false
#define UART_BAUDRATE                          115200
#define UART_OPS                               &max_uart_ops

#define I2C_OPS                                &max_i2c_ops
#define GPIO_OPS                               &max_gpio_ops

/* MAX20362 parameters */
#define MAX20362_I2C_DEVICE_ID                 0
#define MAX20362_I2C_CLK_SPEED                 400000
#define MAX20362_BBOUT_DEFAULT_UV              3300000      /* 3.3V default */
#define MAX20362_VCAP_DEFAULT_UV               5000000  	/* 5V default */
#define MAX20362_LDO_DEFAULT_UV                1800000  	/* 1.8V default */
#define MAX20362_ILIM_DEFAULT_MA               20  			/* 20mA default */
#define MAX20362_BBOUT_ENABLE_DEFAULT          false  		/* Start disabled for testing */
#define MAX20362_LDO_ENABLE_DEFAULT            false  		/* Start disabled for testing */

extern struct max_uart_init_param uart_extra_ip;
extern const struct max_i2c_init_param max20362_i2c_extra;
extern const struct max_gpio_init_param max20362_gpio_extra;

#endif // __PARAMETERS_H__
