/***************************************************************************//**
 *   @file   platform.h
 *   @brief  Defines platform data to be used by ADE7758 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "ade7758.h"
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
/* IRQN interrupt port and pin */
#define GPIO_OPS                    &max_gpio_ops
#define GPIO_EXTRA                  &ade9153_gpio_extra_ip
#define GPIO_IRQ_PORT               2
#define GPIO_IRQ_PIN                9
/* SPI config */
#define SPI_DEVICE_ID               1
#define SPI_BAUDRATE                1000000
#define SPI_CS                      0
#define SPI_SLAVE_NUM               1
/* UART config */
#define UART_DEV_ID                 0
#define UART_BAUD                   115200
/* IRQ config */
#define GPIO_IRQ_OPS                &max_gpio_irq_ops
#define GPIO_CTRL_IRQ_ID            0
#define GPIO_IRQ_EXTRA              &ade7758_gpio_extra_ip
#define NVIC_GPIO_IRQ               GPIO2_IRQn

/* Toggle user LED */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc);

#endif /* __PLATFORM_H__ */
