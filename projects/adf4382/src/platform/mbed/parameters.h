/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Mbed platform used by ADF4382 project.
 *   @author CHegbeli (ciprian.hegbeli@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <PinNames.h>
#include "mbed_uart.h"
#include "mbed_spi.h"
#include "no_os_uart.h"

#define UART_TX_PIN	CONSOLE_TX
#define	UART_RX_PIN	CONSOLE_RX
#define UART_DEVICE_ID  5
#define UART_IRQ_ID     53
#define UART_BAUDRATE   115200
#define UART_EXTRA	&adf4382_uart_extra_ip
#define UART_OPS        &mbed_uart_ops

#define SPI_BAUDRATE    4000000
#define SPI_OPS         &mbed_spi_ops
#define SPI_EXTRA       &adf4382_spi_extra
#define SPI_DEVICE_ID   5
#define SPI_CS          SDP_SPI_CS_A

extern struct mbed_uart_init_param adf4382_uart_extra_ip;
extern struct mbed_spi_init_param adf4382_spi_extra;

#endif /* __PARAMETERS_H__ */
