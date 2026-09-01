/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions of Maxim platform data used by the max17616 project.
 *   @author Carlos Jones (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_gpio.h"
#include "maxim_i2c.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

/* UART */
#define UART_DEVICE_ID		0
#define UART_IRQ_ID		UART1_IRQn
#define UART_ASYNC_RX		false
#define UART_BAUDRATE		115200
#define UART_SIZE		NO_OS_UART_CS_8
#define UART_PARITY		NO_OS_UART_PAR_NO
#define UART_STOP		NO_OS_UART_STOP_1_BIT
#define UART_OPS		&max_uart_ops
#define UART_EXTRA		&uart_extra

/* I2C */
#define I2C_OPS			&max_i2c_ops
#define I2C_DEVICE_ID		2
#define I2C_CLK_SPEED		100000
#define I2C_ADDR		0x16
#define I2C_EXTRA		&i2c_extra

extern struct max_uart_init_param uart_extra;
extern struct max_i2c_init_param i2c_extra;

#endif /* __PARAMETERS_H__ */
