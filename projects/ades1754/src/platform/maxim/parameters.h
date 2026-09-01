/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by ades1754 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_spi.h"
#include "maxim_irq.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_IRQ_ID	UART0_IRQn
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	57600

#define COMM_DEVICE_ID	1
#define COMM_BAUDRATE	500000

#define COMM_EXTRA	&ades1754_comm_extra
#define COMM_OPS	&max_uart_ops

#define UART_EXTRA	&ades1754_uart_extra
#define UART_OPS	&max_uart_ops

extern struct max_uart_init_param ades1754_uart_extra;
extern struct max_uart_init_param ades1754_comm_extra;

#endif /* __PARAMATERS_H__ */
