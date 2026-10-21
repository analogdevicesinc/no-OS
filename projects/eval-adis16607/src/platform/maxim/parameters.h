/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-adis16607
 *           project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  		0
#endif

#define UART_DEVICE_ID  		0
#define UART_BAUDRATE   		115200
#define UART_EXTRA      		&adis16607_uart_extra_ip
#define UART_OPS        		&max_uart_ops
#define UART_IRQ_ID     		UART0_IRQn


#define SPI_DEVICE_ID   		1
#define SPI_CS          		0

#define SPI_BAUDRATE    		10000000
#define SPI_OPS         		&max_spi_ops
#define SPI_EXTRA       		&adis16607_spi_extra_ip

#define GPIO_OPS            		&max_gpio_ops
#define GPIO_EXTRA          		&adis16607_gpio_extra_ip

extern struct max_uart_init_param adis16607_uart_extra_ip;
extern struct max_spi_init_param adis16607_spi_extra_ip;
extern struct max_gpio_init_param adis16607_gpio_extra_ip;

#define GPIO_RESET_PIN_NUM   		8
#define GPIO_RESET_PORT_NUM  		0

#ifdef IIO_TRIGGER_EXAMPLE
#define GPIO_DRDY_PIN_NUM   		13
#define GPIO_DRDY_PORT_NUM  		2

#define NVIC_GPIO_IRQ			GPIO2_IRQn

#define ADIS16607_GPIO_TRIG_IRQ_ID	GPIO_DRDY_PIN_NUM
#define ADIS16607_GPIO_CB_HANDLE	MXC_GPIO_GET_GPIO(GPIO_DRDY_PORT_NUM)

#define GPIO_IRQ_ID			GPIO_DRDY_PORT_NUM
#define GPIO_IRQ_OPS			&max_gpio_irq_ops
#define GPIO_IRQ_EXTRA			&adis_gpio_drdy_extra_ip


extern struct no_os_gpio_init_param adis_gpio_drdy_ip;
extern struct max_gpio_init_param adis_gpio_drdy_extra_ip;
#endif

#endif /* __PARAMETERS_H__ */
