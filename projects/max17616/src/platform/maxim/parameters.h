/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions of Maxim platform data used by the max17616 project.
 *   @author Carlos Jones (carlosjr.jones@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_gpio.h"
#include "maxim_i2c.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID			0
#endif

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
