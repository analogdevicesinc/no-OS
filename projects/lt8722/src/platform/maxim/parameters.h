/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by lt8722 project.
 *   @author Ramon Cristopher Calam (ramoncristopher.calam@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID 0
#endif

#define UART_IRQ_ID UART1_IRQn
#define UART_DEVICE_ID 1
#define UART_BAUDRATE 57600
#define UART_EXTRA &lt8722_uart_extra
#define UART_OPS &max_uart_ops

#define SPI_DEVICE_ID 1
#define SPI_CS 0
#define SPI_BAUDRATE 100000
#define SPI_OPS &max_spi_ops
#define SPI_EXTRA &lt8722_spi_extra

#define GPIO_SWEN_PORT_NUM 0
#define GPIO_SWEN_PIN_NUM 21

#define GPIO_EN_PORT_NUM 0
#define GPIO_EN_PIN_NUM 20

#define GPIO_OPS &max_gpio_ops
#define GPIO_EXTRA &lt8722_gpio_extra

extern struct max_uart_init_param lt8722_uart_extra;
extern struct max_spi_init_param lt8722_spi_extra;
extern struct max_gpio_init_param lt8722_gpio_extra;

#endif /* __PARAMETERS_H__ */
