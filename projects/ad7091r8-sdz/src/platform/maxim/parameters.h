/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-ad7091r8-sdz
 *           project.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include "maxim_timer.h"
#include "no_os_timer.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID		0
#endif

#define UART_DEVICE_ID		0
#define UART_IRQ_ID		UART0_IRQn
#define UART_BAUDRATE		115200
#define UART_EXTRA		&ad7091r8_uart_extra_ip
#define UART_OPS		&max_uart_ops
#define INTC_DEVICE_ID		0
#define SPI_DEVICE_ID		1
#define SPI_CS			1
#define SPI_BAUDRATE		200000
#define SPI_OPS			&max_spi_ops
#define SPI_EXTRA		&ad7091r8_spi_extra_ip

#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&ad7091r8_gpio_extra_ip
#define GPIO_RESET_PIN_NUM	19
#define GPIO_RESET_PORT_NUM	0
#define GPIO_RESET_INIT		&ad7091r8_gpio_reset_ip
#define GPIO_CONVST_PIN_NUM	6
#define GPIO_CONVST_PORT_NUM	1
#define GPIO_CONVST_INIT	&ad7091r8_gpio_convst_ip
#define GPIO_IRQ_ID		0

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
/* AD7091R-8 Timer settings */
#define AD7091R8_TIMER_DEVICE_ID    0
#define AD7091R8_TIMER_FREQ_HZ      1000000
#define AD7091R8_TIMER_TICKS_COUNT  2000
#define AD7091R8_TIMER_EXTRA        NULL
#define TIMER_OPS                   &max_timer_ops

/* AD7091R-8 Timer trigger settings */
#define AD7091R8_TIMER_IRQ_ID       TMR0_IRQn
#define TIMER_IRQ_OPS               &max_irq_ops
#define AD7091R8_TIMER_IRQ_EXTRA    NULL

/* AD7091R-8 timer trigger settings */
#define AD7091R8_TIMER_CB_HANDLE    MXC_TMR0
#define AD7091R8_TIMER_TRIG_IRQ_ID  TMR0_IRQn

#endif /* IIO_TIMER_TRIGGER_EXAMPLE */

extern struct max_uart_init_param ad7091r8_uart_extra_ip;
extern struct max_spi_init_param ad7091r8_spi_extra_ip;
extern struct max_gpio_init_param ad7091r8_gpio_extra_ip;
extern struct no_os_gpio_init_param ad7091r8_gpio_reset_ip;
extern struct no_os_gpio_init_param ad7091r8_gpio_convst_ip;

#endif /* __PARAMETERS_H__ */
