/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to aducm3029 platform used by ad7124-8pmdz
 *           project.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stdio.h"
#include "platform_init.h"
#include "aducm3029_spi.h"
#include "aducm3029_uart.h"
#include "aducm3029_irq.h"
#include "aducm3029_uart_stdio.h"

#define UART_IRQ_ID	ADUCM_UART_INT_ID
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	115200
#define UART_EXTRA	NULL
#define UART_OPS	&aducm_uart_ops

#define SPI_DEVICE_ID	1
#define SPI_BAUDRATE	10000000
#define SPI_CS		0
#define SPI_OPS		&aducm_spi_ops
#define SPI_EXTRA	&ad7124_spi_extra_ip

extern struct aducm_spi_init_param ad7124_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
