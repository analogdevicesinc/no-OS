/***************************************************************************//**
 *   @file   max14916/src/platform/maxim/parameters.h
 *   @brief  Definition of Maxim platform data used by max14916 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_gpio_irq.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"


#if (TARGET_NUM == 32690)
#define UART_IRQ_ID		UART0_IRQn
#define UART_DEVICE_ID		0
#define UART_BAUDRATE		57600
#endif
#define UART_EXTRA		&ade7816_uart_extra
#define UART_OPS		&max_uart_ops

#if (TARGET_NUM == 32690)
#define SPI_DEVICE_ID		4
#define SPI_CS			0
#endif

#define SPI_BAUDRATE		100000
#define SPI_OPS			&max_spi_ops
#define SPI_EXTRA		&ade7816_spi_extra

#if (TARGET_NUM == 32690)
#define GPIO_RESET_PORT_NUM	1
#define GPIO_RESET_PIN_NUM	6

#define GPIO_IRQ0_PORT_NUM	2
#define GPIO_IRQ0_PIN_NUM	21

#define GPIO_IRQ_ID		2
#define GPIO_HANDLE		MXC_GPIO_GET_GPIO(2)

#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&ade7816_gpio_extra

#define GPIO_IRQ_OPS		&max_gpio_irq_ops
#define GPIO_IRQ_EXTRA		&ade7816_gpio_extra

#define NVIC_GPIO_IRQ		GPIO2_IRQn
#endif

extern struct max_uart_init_param ade7816_uart_extra;
extern struct max_gpio_init_param ade7816_gpio_extra;
extern struct max_spi_init_param ade7816_spi_extra;

#endif  /* __PARAMETERS_H__ */
