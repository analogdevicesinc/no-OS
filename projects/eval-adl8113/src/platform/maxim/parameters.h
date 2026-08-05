/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-adl8113 project.
 *   @author Raul Geo <Raul.Georgian@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_irq.h"

#define UART_DEVICE_ID  0
#define UART_IRQ_ID     UART0_IRQn
#define UART_BAUDRATE   57600
#define UART_EXTRA      &adl8113_uart_extra_ip
#define UART_OPS        &max_uart_ops

/* GPIO Port and Pin definitions for ADL8113 */
#define GPIO_VA_PORT		1
#define GPIO_VA_PIN		6
#define GPIO_VB_PORT		1
#define GPIO_VB_PIN		7

/* LED definitions (active low on MAX32655FTHR) */
#define LED_RED_PORT		0
#define LED_RED_PIN		18
#define LED_GREEN_PORT		0
#define LED_GREEN_PIN		19
#define LED_BLUE_PORT		0
#define LED_BLUE_PIN		26

/* Button definitions */
#define BUTTON1_PORT		0
#define BUTTON1_PIN		2
#define BUTTON2_PORT		0
#define BUTTON2_PIN		3

#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&adl8113_gpio_extra_ip

extern struct max_gpio_init_param adl8113_gpio_extra_ip;
extern struct max_uart_init_param adl8113_uart_extra_ip;

#endif /* __PARAMETERS_H__ */
