/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Implementation of parameters.h
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define UART_OPS                &max_uart_ops
#define UART_EXTRA              &max11205_uart_extra_ip
extern struct max_uart_init_param max11205_uart_extra_ip;

#define INTC_DEVICE_ID          0
#define UART_DEVICE_ID          0
#define UART_BAUDRATE           57600
#define UART_IRQ_ID             UART0_IRQn

#define MAX11205_VREF_MV        3300

#define SPI_DEVICE_ID           0
#define SPI_BAUDRATE            5000000
#define SPI_CS                  0
#define SPI_OPS                 &max_spi_ops
#define SPI_EXTRA               &max11205_spi_extra_ip

#define GPIO_OPS                &max_gpio_ops
#define GPIO_EXTRA              &max11205_gpio_extra_ip
#if (TARGET_NUM != 32655)
#error PIN and PORT configuration have to be adapted for the used target.
#else
#define GPIO_SYNC_PIN_NUM       6
#define GPIO_SYNC_PORT_NUM      1
#endif

#define GPIO_IRQ_OPS            &max_gpio_irq_ops
#define GPIO_CTRL_IRQ_ID        1 /* For PORT1 */
#define GPIO_IRQ_EXTRA          &max11205_gpio_extra_ip
#define NVIC_GPIO_IRQ           GPIO1_IRQn /* For PORT1 */

extern struct max_gpio_init_param max11205_gpio_extra_ip;
extern struct max_spi_init_param max11205_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
