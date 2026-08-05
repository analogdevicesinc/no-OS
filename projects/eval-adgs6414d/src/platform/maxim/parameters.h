/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by adgs6414d
 *           project.
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_spi.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_EXTRA      &adgs6414d_uart_extra_ip
#define UART_OPS        &max_uart_ops

#define SPI_DEVICE_ID   0
#define SPI_BAUDRATE    100000
#define SPI_CS          0
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adgs6414d_spi_extra_ip

extern struct max_uart_init_param adgs6414d_uart_extra_ip;
extern struct max_spi_init_param adgs6414d_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
