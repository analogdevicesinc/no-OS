/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions of linux-userspace platform parameters for max14916.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#ifdef CONFIG_FTD2XX
#include "ftd2xx_spi.h"
#include "ftd2xx_uart.h"
#else
#include "linux_spi.h"
#include "linux_uart.h"
#endif

#define UART_IRQ_ID	0
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	0
#define UART_EXTRA	NULL

#ifdef CONFIG_FTD2XX
extern struct ftd2xx_spi_init max14916_spi_extra;
#define SPI_EXTRA	&max14916_spi_extra
#define SPI_OPS		&ftd2xx_spi_ops
#define UART_OPS	&ftd2xx_uart_ops
#else
#define SPI_EXTRA	NULL
#define SPI_OPS		&linux_spi_ops
#define UART_OPS	&linux_uart_ops
#endif

#define SPI_DEVICE_ID	0
#define SPI_CS		0
#define SPI_BAUDRATE	100000

#endif /* __PARAMETERS_H__ */
