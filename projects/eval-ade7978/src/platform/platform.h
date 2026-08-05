/***************************************************************************//**
 *   @file   platform.h
 *   @brief  Defines common data to be used by ADE7978 example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "ade7978.h"
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

/* UART init params */
extern struct no_os_uart_init_param uart_ip;
/* GPIO LED init params */
extern struct no_os_gpio_init_param gpio_led1_ip;
/* GPIO Reset init params */
extern struct no_os_gpio_init_param gpio_reset_ip;
/* SPI init params */
extern struct no_os_spi_init_param ade7978_spi_ip ;
/* GPIO RDY init params */
extern struct no_os_gpio_init_param ade7978_gpio_rdy_ip;
/* Interrupt init params */
extern struct no_os_irq_init_param ade7978_gpio_irq_ip;

/* Configuration for AD-APARD32690-SL */
/* Port and Pin for user LED */
#define GPIO_LED_PORT               2
#define GPIO_LED_PIN                1
/* Port and Pin for Reset */
#define GPIO_RESET_PORT             2
#define GPIO_RESET_PIN              9
/* Data ready pin */
#define GPIO_RDY_PORT               2
#define GPIO_RDY_PIN                7


#define GPIO_OPS                    &max_gpio_ops
#define GPIO_EXTRA                  &gpio_extra_ip
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
#define GPIO_IRQ_EXTRA              &gpio_extra_ip
#define NVIC_GPIO_IRQ               GPIO2_IRQn

#define RESET_TIME                  500
/* Read data interval in ms */
#define READ_INTERVAL               3000

/* Led toggle */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc);

#endif /* __PLATFORM_H__ */
