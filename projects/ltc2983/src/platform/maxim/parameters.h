/********************************************************************************
 *   @brief  Definitions specific to Maxim platform used by ltc2983 project.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID	0
#endif
#define UART_IRQ_ID	UART1_IRQn
#define UART_DEVICE_ID	1
#define UART_BAUDRATE	115200
#define UART_OPS	&max_uart_ops
#define UART_EXTRA      &max_uart_extra

#if (TARGET_NUM == 32650)
#define SPI_DEVICE_ID	1
#define SPI_CS		0
#elif (TARGET_NUM == 32660) || (TARGET_NUM == 32655)
#define SPI_DEVICE_ID	0
#define SPI_CS		0
#elif (TARGET_NUM == 32665)
#define SPI_DEVICE_ID	1
#define SPI_CS		0
#elif (TARGET_NUM == 78000)
#define SPI_DEVICE_ID	1
#define SPI_CS		1
#endif

#define SPI_MAX_SPEED	1000000
#define SPI_OPS		&max_spi_ops
#define SPI_EXTRA	&max_spi_extra

#define GPIO_RSTN_PORT_NUM	0
#define GPIO_RSTN_PIN_NUM	27
#define GPIO_OPS	&max_gpio_ops
#define GPIO_EXTRA 	&max_gpio_extra

extern struct max_uart_init_param max_uart_extra;
extern struct max_spi_init_param max_spi_extra;
extern struct max_gpio_init_param max_gpio_extra;

#endif /* __PARAMETERS_H__ */
