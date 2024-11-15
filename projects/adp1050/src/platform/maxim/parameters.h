/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by adp1050 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
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

#define UART_DEVICE_ID		0
#define UART_BAUDRATE		57600
#define	UART_OPS		&max_uart_ops
#define UART_EXTRA		&adp1050_uart_extra

#define GPIO_PG_ALT_PORT	0
#define GPIO_PG_ALT_PIN		24
#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&adp1050_pg_alt_extra

#define I2C_DEVICE_ID		0
#define I2C_OPS			&max_i2c_ops
#define I2C_EXTRA		&adp1050_i2c_extra

extern struct max_uart_init_param adp1050_uart_extra;
extern struct max_i2c_init_param adp1050_i2c_extra;
extern struct max_gpio_init_param adp1050_pg_alt_extra;

#endif /* __PARAMETERS_H__ */
