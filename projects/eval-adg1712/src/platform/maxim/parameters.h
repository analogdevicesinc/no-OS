/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-adg1712
 *           project.
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
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

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_EXTRA      &adg1712_uart_extra_ip
#define UART_OPS        &max_uart_ops

#define GPIO_OPS    &max_gpio_ops
#define GPIO_EXTRA  &adg1712_gpio_extra_ip

extern struct max_uart_init_param adg1712_uart_extra_ip;
extern struct max_gpio_init_param adg1712_gpio_extra_ip;


#define GPIO_SW_PORT_NUM    0
#define GPIO_SW1_PIN_NUM    31
#define GPIO_SW2_PIN_NUM    25
#define GPIO_SW3_PIN_NUM    24
#define GPIO_SW4_PIN_NUM    30

#endif /* __PARAMETERS_H__ */
