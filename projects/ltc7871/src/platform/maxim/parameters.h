/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by ltc7871 project.
 *   @author Aldrin Abacan (aldrin.abacan@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_IRQ_ID UART1_IRQn
#define UART_DEVICE_ID 2
#define UART_BAUDRATE 115200
#define UART_EXTRA &ltc7871_uart_extra
#define UART_OPS &max_uart_ops

#define SPI_DEVICE_ID 0
#define SPI_CS 1
#define SPI_BAUDRATE 100000
#define SPI_OPS &max_spi_ops
#define SPI_EXTRA &ltc7871_spi_extra

#define GPIO_PWMEN_PORT_NUM 4
#define GPIO_PWMEN_PIN_NUM 0

#define GPIO_OPS &max_gpio_ops
#define GPIO_EXTRA &ltc7871_gpio_extra

extern struct max_uart_init_param ltc7871_uart_extra;
extern struct max_spi_init_param ltc7871_spi_extra;
extern struct max_gpio_init_param ltc7871_gpio_extra;

#endif /* __PARAMETERS_H__ */
