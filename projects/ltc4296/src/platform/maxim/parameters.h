/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by ltc4296 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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

#define UART_DEVICE_ID		0
#define UART_BAUDRATE		57600
#define	UART_OPS		&max_uart_ops
#define UART_EXTRA		&ltc4296_uart_extra

#define SPI_DEVICE_ID   	0
#define SPI_BAUDRATE    	50000
#define SPI_CS          	1
#define SPI_OPS        	 	&max_spi_ops
#define SPI_EXTRA       	&ltc4296_spi_extra

extern struct max_uart_init_param ltc4296_uart_extra;
extern struct max_spi_init_param ltc4296_spi_extra;
extern struct max_gpio_init_param ltc4296_pg_alt_extra;

#endif /* __PARAMETERS_H__ */
