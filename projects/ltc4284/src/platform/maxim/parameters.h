/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform parameters for LTC4284 project
 *   @author Carlos Jones Jr <carlosjr.jones@analog.com>
 *   @author Christopher de Guzman <christopher.deguzman@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_uart.h"
#include "maxim_i2c.h"

/* CN2 FTDI USB-to-UART Bridge Configuration */
#define UART_IRQ_ID         UART2_IRQn
#define UART_DEVICE_ID      2
#define UART_BAUDRATE       115200
#define UART_SIZE           NO_OS_UART_CS_8
#define UART_PARITY         NO_OS_UART_PAR_NO
#define UART_STOP           NO_OS_UART_STOP_1_BIT

#define I2C_DEVICE_ID       0
#define INTC_DEVICE_ID      0
#define I2C_MAX_SPEED       100000

extern struct max_uart_init_param ltc4284_uart_extra_ip;
extern struct max_i2c_init_param ltc4284_i2c_extra_ip;

#endif /* __PARAMETERS_H__ */
