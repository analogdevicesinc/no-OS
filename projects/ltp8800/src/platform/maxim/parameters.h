/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by ltp8800 project.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_i2c.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID		0
#endif

#define UART_DEVICE_ID		1
#define UART_IRQ_ID		UART1_IRQn
#define UART_BAUDRATE		115200
#define	UART_OPS		&max_uart_ops
#define UART_EXTRA		&ltp8800_uart_extra

#define I2C_DEVICE_ID		0
#define I2C_OPS			&max_i2c_ops
#define I2C_EXTRA		&ltp8800_i2c_extra

#define CTRL_GPIO_PORT		0
#define CTRL_GPIO_NUMBER	5
#define CTRL_GPIO_OPS		&max_gpio_ops
#define CTRL_GPIO_EXTRA		&ltp8800_gpio_extra

extern struct max_uart_init_param ltp8800_uart_extra;
extern struct max_i2c_init_param ltp8800_i2c_extra;
extern struct max_gpio_init_param ltp8800_gpio_extra;

#endif /* __PARAMETERS_H__ */
