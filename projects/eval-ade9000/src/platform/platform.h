/***************************************************************************//**
 *   @file   platform.h
 *   @brief  Defines common data to be used by ADE9000 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ade9000.h"
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "maxim_irq.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

// UART init params
extern struct no_os_uart_init_param uart_ip;
// GPIO LED init params
extern struct no_os_gpio_init_param gpio_led1_ip;
// SPI init params
extern struct no_os_spi_init_param ade9000_spi_ip ;

/* Configuration for AD-APARD32690-SL */
// Port and Pin for user LED
#define GPIO_LED_PORT               2
#define GPIO_LED_PIN                1
#define GPIO_OPS                    &max_gpio_ops
#define GPIO_EXTRA                  &ade9000_gpio_extra_ip
// SPI config
#define SPI_DEVICE_ID               1
#define SPI_BAUDRATE                1000000
#define SPI_CS                      0
#define SPI_SLAVE_NUM               1
// UART config
#define UART_DEV_ID                 0
#define UART_BAUD                   115200

#define RESET_TIME                  500
// Read data interval in ms
#define READ_INTERVAL               3000

#endif // __PLATFORM_H__
