/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  ADIOL100 project platform-specific parameters (Maxim).
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_irq.h"
#include "mxc_device.h"

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   115200
#define UART_OPS        &max_uart_ops
#define UART_EXTRA      &adiol100_uart_extra

#define SPI_DEVICE_ID   1
#define SPI_CS          1
#define SPI_BAUDRATE    1000000
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adiol100_spi_extra

#define GPIO_OPS        &max_gpio_ops
#define GPIO_EXTRA      &adiol100_gpio_extra
#define GPIO_IRQ_OPS    &max_gpio_irq_ops
#define IRQ_OPS         &max_irq_ops

#define IRQA_PORT       0
#define IRQA_PIN        19

extern struct max_uart_init_param adiol100_uart_extra;
extern struct max_spi_init_param  adiol100_spi_extra;
extern struct max_gpio_init_param adiol100_gpio_extra;

#endif
