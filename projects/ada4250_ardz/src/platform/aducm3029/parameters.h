/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to ADuCM3029 platform used by ada4250_ardz
 *           project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <stdint.h>
#include "platform_init.h"
#include "aducm3029_spi.h"
#include "aducm3029_irq.h"
#include "aducm3029_uart.h"

#define UART_DEVICE_ID	0
#define UART_IRQ_ID	ADUCM_UART_INT_ID
#define UART_BAUDRATE	115200
#define UART_EXTRA	NULL
#define UART_OPS	&aducm_uart_ops

#define SPI_DEVICE_ID	0
#define SPI_BAUDRATE	1000000
#define SPI_CS		1
#define SPI_OPS		&aducm_spi_ops
#define SPI_EXTRA	&ada4250_spi_extra_ip

extern struct aducm_spi_init_param ada4250_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
