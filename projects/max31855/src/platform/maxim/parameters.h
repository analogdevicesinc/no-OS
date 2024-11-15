/********************************************************************************
 *   @brief  Definitions specific to Maxim platform used by max31855 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_spi.h"
#include "maxim_irq.h"
#include "maxim_gpio.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID	0
#endif
#define UART_IRQ_ID	UART0_IRQn
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	57600
#define UART_OPS	&max_uart_ops
#define UART_EXTRA      &max_uart_extra

#if (TARGET_NUM == 32650)
#define SPI_DEVICE_ID	1
#define SPI_CS		0
#elif (TARGET_NUM == 32660) || (TARGET_NUM == 32655)
#define SPI_DEVICE_ID	0
#define SPI_CS		0
#elif (TARGET_NUM == 32665)
#define SPI_DEVICE_ID	2
#define SPI_CS		0
#elif (TARGET_NUM == 78000)
#define SPI_DEVICE_ID	1
#define SPI_CS		1
#endif

#define SPI_MAX_SPEED	1000000
#define SPI_OPS		&max_spi_ops
#define SPI_EXTRA	&max_spi_extra

extern struct max_uart_init_param max_uart_extra;
extern struct max_spi_init_param max_spi_extra;

#endif /* __PARAMETERS_H__ */
