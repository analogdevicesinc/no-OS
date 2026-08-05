/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Linux platform used by eval-adis1647x project.
 *   @author Alisa-Dariana Roman <alisa.roman@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "linux_gpio.h"
#include "linux_spi.h"
#include "linux_uart.h"

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_IRQ_ID     0
#define UART_EXTRA      NULL
#define UART_OPS        &linux_uart_ops

#define SPI_DEVICE_ID   0
#define SPI_BAUDRATE    1000000
#define SPI_CS          0
#define SPI_OPS         &linux_spi_ops
#define SPI_EXTRA       NULL

#define GPIO_RESET_PIN_NUM   12
#define GPIO_RESET_PORT_NUM  0
#define GPIO_OPS             &linux_gpio_ops
#define GPIO_EXTRA           NULL

#endif /* __PARAMETERS_H__ */
