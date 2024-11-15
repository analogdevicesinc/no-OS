/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by lt7182s project.
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
#define UART_EXTRA		&lt7182s_uart_extra

#define I2C_DEVICE_ID		1
#define I2C_OPS			&max_i2c_ops
#define I2C_EXTRA		&lt7182s_i2c_extra

extern struct max_uart_init_param lt7182s_uart_extra;
extern struct max_i2c_init_param lt7182s_i2c_extra;

#endif /* __PARAMETERS_H__ */
