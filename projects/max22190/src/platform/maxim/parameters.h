/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by max22190 project.
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
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID	0
#endif

#if (TARGET_NUM == 32690)
#define UART_IRQ_ID	UART0_IRQn
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	57600
#elif (TARGET_NUM == 32665)
#define UART_IRQ_ID	UART1_IRQn
#define UART_DEVICE_ID	1
#define UART_BAUDRATE	57600
#endif

#define UART_EXTRA	&max22190_uart_extra
#define UART_OPS	&max_uart_ops

#if (TARGET_NUM == 32690)
#define SPI_DEVICE_ID	4
#define SPI_CS		0
#elif (TARGET_NUM == 32665)
#define SPI_DEVICE_ID	1
#define SPI_CS		0
#endif

#define SPI_BAUDRATE	100000
#define SPI_OPS		&max_spi_ops
#define SPI_EXTRA	&max22190_spi_extra

extern struct max_uart_init_param max22190_uart_extra;
extern struct max_spi_init_param max22190_spi_extra;

#endif /* __PARAMATERS_H__ */
