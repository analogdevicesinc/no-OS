/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by ad7124-8pmdz
 *           project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#if (TARGET_NUM == 32690)
#define UART_IRQ_ID	UART0_IRQn
#define UART_DEVICE_ID	0
#define SPI_DEVICE_ID	4
#elif (TARGET_NUM == 32665)
#define UART_IRQ_ID	UART1_IRQn
#define UART_DEVICE_ID	1
#define SPI_DEVICE_ID	1
#elif (TARGET_NUM == 32655)
#define UART_IRQ_ID	UART0_IRQn
#define UART_DEVICE_ID	0
#define SPI_DEVICE_ID	0
#endif

#define UART_BAUDRATE	115200
#define UART_EXTRA	&ad7124_uart_extra_ip
#define UART_OPS	&max_uart_ops

#define SPI_CS		0
#define SPI_BAUDRATE	1000000
#define SPI_OPS		&max_spi_ops
#define SPI_EXTRA	&ad7124_spi_extra_ip

extern struct max_uart_init_param ad7124_uart_extra_ip;
extern struct max_spi_init_param ad7124_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
