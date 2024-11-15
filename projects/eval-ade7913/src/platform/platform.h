/***************************************************************************//**
 *   @file   platform.h
 *   @brief  Defines platform data to be used by ADE7913 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ade7913.h"
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

/* Configuration for AD-APARD32690-SL */
// Port and pin for user LED
#define GPIO_LED_PORT               2
#define GPIO_LED_PIN                1
#define GPIO_OPS                    &max_gpio_ops
#define GPIO_EXTRA                  &ade9153_gpio_extra_ip
// Data ready pin
#define GPIO_RDY_PORT               2
#define GPIO_RDY_PIN                10
// SPI config
#define SPI_DEVICE_ID               1
#define SPI_BAUDRATE                1000000
#define SPI_CS0                     0
#define SPI_CS1                     1
#define SPI_CS2                     2
#define SPI_SLAVE_NUM               3
// UART config
#define UART_DEV_ID                 0
#define UART_BAUD                   115200
// IRQ config
#define GPIO_IRQ_OPS                &max_gpio_irq_ops
#define GPIO_CTRL_IRQ_ID            0
#define GPIO_IRQ_EXTRA              &ade7913_gpio_extra_ip
#define NVIC_GPIO_IRQ               GPIO2_IRQn

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

// Init NVIC
int init_nvic(struct no_os_irq_ctrl_desc *ade7913_nvic_desc);

#endif /* __PLATFORM_H__ */
