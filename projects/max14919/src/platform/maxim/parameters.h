/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Maxim platform data used by max14919 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_gpio.h"
#include "maxim_gpio_irq.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_IRQ_ID		UART0_IRQn
#define UART_DEVICE_ID		0
#define UART_BAUDRATE		57600
#define UART_EXTRA		&max14919_uart_extra
#define UART_OPS		&max_uart_ops

// GPIOs
#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&max14919_gpio_extra_ip
#define GPIO_FAULT_PORT_NUM	1
#define GPIO_FAULT_PIN_NUM	6
#define GPIO_IN_PORT_NUM	1
#define GPIO_IN_PIN_NUM(x)	(x)
#define GPIO_IN_RUSH_PORT_NUM	2
#define GPIO_IN_RUSH_PIN_NUM	21
#define GPIO_REV_PORT_NUM	1
#define GPIO_REV_PIN_NUM	4

extern struct max_gpio_init_param max14919_gpio_extra_ip;
extern struct max_uart_init_param max14919_uart_extra;

#endif /* __PARAMETERS_H__ */
