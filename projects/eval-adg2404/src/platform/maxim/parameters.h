/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by adg2404 project.
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
#define UART_EXTRA      &adg2404_uart_extra_ip
#define UART_OPS        &max_uart_ops

/* GPIO Port and Pin definitions for ADG2404 */
#define GPIO_A0_PORT    1
#define GPIO_A0_PIN     6
#define GPIO_A1_PORT    1
#define GPIO_A1_PIN     7
#define GPIO_EN_PORT    1
#define GPIO_EN_PIN     8

#define GPIO_OPS        &max_gpio_ops
#define GPIO_EXTRA      &adg2404_gpio_extra_ip

extern struct max_uart_init_param adg2404_uart_extra_ip;
extern struct max_gpio_init_param adg2404_gpio_extra_ip;

#endif /* __PARAMETERS_H__ */
