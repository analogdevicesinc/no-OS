/***************************************************************************//**
 *   @file   platform.h
 *   @brief  Defines platform data to be used by ADHV4710 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "adhv4710.h"
#include "no_os_uart.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_spi.h"
#include "maxim_irq.h"

/* Configuration for AD-APARD32690-SL */
/* Port and pin for user LED */
#define GPIO_LED_PORT               2
#define GPIO_LED_PIN                1
/* Hardware reset port and pin */
#define GPIO_RESET_PORT             2
#define GPIO_RESET_PIN              7
/* SPI config */
#define SPI_DEVICE_ID               1
#define SPI_BAUDRATE                1000000
#define SPI_CS                      0
#define SPI_SLAVE_NUM               1
/* UART config */
#define UART_DEV_ID                 0
#define UART_BAUD                   115200

#endif /* __PLATFORM_H__ */
