/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by eval-adg1736 project.
 *   @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_DEVICE_ID   0
#define UART_BAUDRATE    115200
#define UART_OPS         &max_uart_ops
#define UART_EXTRA       &adg1736_uart_extra

extern struct max_uart_init_param adg1736_uart_extra;

#define GPIO_OPS         &max_gpio_ops
#define GPIO_EXTRA       &adg1736_gpio_extra

extern struct max_gpio_init_param adg1736_gpio_extra;

/* Test configuration */
#define TEST_SIDE        0    /* 0 = A side, 1 = B side */
#define TEST_SWITCH      0    /* 0 = SW1, 1 = SW2 */

/* ADG1736 control pins - IN1 and IN2 can use separate pins if needed */
#define GPIO_IN1_PORT    1
#define GPIO_IN1_PIN     6
#define GPIO_IN2_PORT    2
#define GPIO_IN2_PIN     0
#define GPIO_EN_PORT     1
#define GPIO_EN_PIN      7

/* Test signal pins */
#define GPIO_D_PORT      1
#define GPIO_D_PIN       8
#define GPIO_S_PORT      1
#define GPIO_S_PIN       9

/* LEDs (active-low) */
#define LED_RED_PORT     0
#define LED_RED_PIN      18
#define LED_GREEN_PORT   0
#define LED_GREEN_PIN    19

#endif
