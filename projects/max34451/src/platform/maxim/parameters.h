/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by MAX34451 project.
 *   @author Jared Tristan Arbolado (jaredtristan.arbolado@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_i2c.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_DEVICE_ID		2
#define UART_IRQ_ID		UART2_IRQn
#define UART_BAUDRATE		115200
#define	UART_OPS		&max_uart_ops
#define UART_EXTRA		&max34451_uart_extra

#define I2C_DEVICE_ID		0
#define I2C_OPS			&max_i2c_ops
#define I2C_EXTRA		&max34451_i2c_extra

extern struct max_uart_init_param max34451_uart_extra;
extern struct max_i2c_init_param max34451_i2c_extra;

#endif /* __PARAMETERS_H__ */
