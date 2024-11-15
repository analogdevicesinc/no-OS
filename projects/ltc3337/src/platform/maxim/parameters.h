/********************************************************************************
 *   @brief  Definitions specific to Maxim platform used by ltc3337 project.
 *   @author Brent Kowal (brent.kowal@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_i2c.h"
#include "maxim_irq.h"
#include "maxim_gpio.h"


#define UART_IRQ_ID	UART0_IRQn
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	57600
#define UART_OPS	&max_uart_ops
#define UART_EXTRA	&max_uart_extra


#if (TARGET_NUM == 32655)
#define I2C_DEVICE_ID	2
#else
#define I2C_DEVICE_ID	0
#endif

#define I2C_MAX_SPEED	100000
#define I2C_OPS		&max_i2c_ops
#define I2C_EXTRA	&max_i2c_extra

extern struct max_uart_init_param max_uart_extra;
extern struct max_i2c_init_param max_i2c_extra;

#endif /* __PARAMETERS_H__ */
