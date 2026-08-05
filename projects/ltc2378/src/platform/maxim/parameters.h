/********************************************************************************
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by ltc2378 project.
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"

#define UART_DEVICE_ID		1
#define UART_IRQ_ID		UART1_IRQn
#define UART_BAUDRATE		115200
#define	UART_OPS		&max_uart_ops
#define UART_EXTRA		&max_uart_extra

#define SPI_DEVICE_ID	1
#define SPI_CS		0

#define SPI_MAX_SPEED	1000000  /* 1 MHz SPI clock */
#define SPI_OPS		&max_spi_ops
#define SPI_EXTRA	&max_spi_extra

/* Hardware configuration constants */
#define LTC2378_DEFAULT_VREF_UV	2500000  /* 2.5V reference voltage in microvolts */
#define BASIC_EXAMPLE_DELAY_MS	500      /* Delay between measurements in basic example */

#define GPIO_CNV_PORT_NUM  0
#define GPIO_CNV_PIN_NUM   9
#define GPIO_BUSY_PORT_NUM 0
#define GPIO_BUSY_PIN_NUM  10
#define GPIO_OPS	&max_gpio_ops
#define GPIO_EXTRA 	&max_gpio_extra

extern struct max_uart_init_param max_uart_extra;
extern struct max_spi_init_param max_spi_extra;
extern struct max_gpio_init_param max_gpio_extra;

#endif /* __PARAMETERS_H__ */
