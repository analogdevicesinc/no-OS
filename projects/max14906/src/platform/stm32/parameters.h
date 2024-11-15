/***************************************************************************//**
 *   @file   max14906/src/platform/maxim/parameters.h
 *   @brief  Definition of STM32 platform data used by max14906 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"

extern UART_HandleTypeDef huart2;

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID		0
#define IIO_APP_HUART		(&huart2)
#endif
#define UART_IRQ_ID     	USART2_IRQn
#define UART_DEVICE_ID		2
#define UART_BAUDRATE		115200
#define UART_OPS		&stm32_uart_ops
#define UART_EXTRA		&max14906_uart_extra_ip

#ifdef BASIC_EXAMPLE
extern struct stm32_gpio_irq_init_param max14906_gpio_irq_extra_ip;

#define GPIO_IRQ_ID		0 /* Pin 0 */
#define GPIO_IRQ_OPS		&stm32_gpio_irq_ops
#define GPIO_IRQ_EXTRA		&max14906_gpio_irq_extra_ip

#define GPIO_OPS		&stm32_gpio_ops
#define GPIO_EXTRA		NULL

#define GPIO_FAULT_PORT_NUM	0
#define GPIO_FAULT_PIN_NUM	0
#endif

#define SPI_DEVICE_ID		1
#define SPI_CS			4
#define SPI_CS_PORT		0
#define SPI_OPS			&stm32_spi_ops
#define SPI_EXTRA		&max14906_spi_extra_ip
#define SPI_BAUDRATE		100000

extern struct stm32_uart_init_param max14906_uart_extra_ip;
extern struct stm32_spi_init_param max14906_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
