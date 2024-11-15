/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by the ADIN1110
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#define UART_DEVICE_ID  0
#define UART_BAUDRATE   57600
#define UART_OPS        &max_uart_ops
#define UART_EXTRA      &adin1110_uart_extra_ip

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    15000000
#define SPI_CS          0
#define SPI_OPS         &max_spi_ops
#define SPI_EXTRA       &adin1110_spi_extra_ip

#define GPIO_OPS	&max_gpio_ops
#define RST_GPIO_PORT   0
#define RST_GPIO_NUM    19
#define RST_GPIO_EXTRA	&adin1110_reset_gpio_extra

extern struct max_uart_init_param adin1110_uart_extra_ip;
extern struct max_spi_init_param adin1110_spi_extra_ip;
extern struct max_gpio_init_param adin1110_reset_gpio_extra;

#endif /* __PARAMETERS_H__ */
