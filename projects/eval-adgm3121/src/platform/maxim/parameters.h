/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-adgm3121
 *           project.
 *   @author Ciobanu Radu (Radu-rares.Ciobanu@analog.com)
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
#define UART_EXTRA      &adgm3121_uart_extra_ip
#define UART_OPS        &max_uart_ops

#define GPIO_OPS        &max_gpio_ops
#define GPIO_EXTRA      &adgm3121_gpio_extra_ip

#define GPIO_PIN_SPI_PIN_NUM   19
#define GPIO_PIN_SPI_PORT_NUM  0

#define GPIO_SW1_PORT_NUM    1
#define GPIO_SW1_PIN_NUM     6
#define GPIO_SW2_PORT_NUM    1
#define GPIO_SW2_PIN_NUM     7
#define GPIO_SW3_PORT_NUM    1
#define GPIO_SW3_PIN_NUM     8
#define GPIO_SW4_PORT_NUM    1
#define GPIO_SW4_PIN_NUM     9

extern struct max_uart_init_param adgm3121_uart_extra_ip;
extern struct max_gpio_init_param adgm3121_gpio_extra_ip;

#endif /* __PARAMETERS_H__ */
