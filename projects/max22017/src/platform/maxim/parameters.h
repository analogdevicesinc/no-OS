/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by max22017 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#define INTC_DEVICE_ID		0
#endif

#if (TARGET_NUM == 32690)
#define UART_IRQ_ID		UART0_IRQn
#define UART_DEVICE_ID		0

#define SPI_DEVICE_ID		4

#define GPIO_RSTB_PORT_NUM	1
#define GPIO_RSTB_PIN_NUM	6
#elif (TARGET_NUM == 32665)
#define UART_IRQ_ID		UART1_IRQn
#define UART_DEVICE_ID		1

#define SPI_DEVICE_ID		1

#define GPIO_RSTB_PORT_NUM	0
#define GPIO_RSTB_PIN_NUM	5
#endif

#define UART_BAUDRATE		57600
#define UART_EXTRA		&max22017_uart_extra
#define UART_OPS		&max_uart_ops

#define SPI_CS			0
#define SPI_BAUDRATE		100000
#define SPI_OPS			&max_spi_ops
#define SPI_EXTRA		&max22017_spi_extra

#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&max22017_gpio_extra_ip

extern struct max_gpio_init_param max22017_gpio_extra_ip;
extern struct max_uart_init_param max22017_uart_extra;
extern struct max_spi_init_param max22017_spi_extra;

#endif /* __PARAMATERS_H__ */
