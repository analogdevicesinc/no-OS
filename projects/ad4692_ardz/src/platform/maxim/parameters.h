/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by ad4692 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_gpio_irq.h"
#include "maxim_pwm.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#if (TARGET_NUM == 32690)
#define	UART_IRQ_ID		UART0_IRQn
#define UART_DEVICE_ID		0
#define UART_BAUDRATE		57600
#endif

#define UART_EXTRA		&ad4692_uart_extra
#define UART_OPS		&max_uart_ops

#if (TARGET_NUM == 32690)
#define SPI_DEVICE_ID		1
#define SPI_CS			0
#endif

#define SPI_BAUDRATE		1000000
#define SPI_OPS			&max_spi_ops
#define SPI_EXTRA		&ad4692_spi_extra

extern struct max_uart_init_param ad4692_uart_extra;
extern struct max_spi_init_param ad4692_spi_extra;

#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&ad4692_gpio_extra_ip
#define GPIO_IRQ_OPS		&max_gpio_irq_ops
#define GPIO_IRQ_EXTRA		&ad4692_gpio_extra_ip

#define PWM_OPS			&max_pwm_ops
#define PWM_EXTRA		&ad4692_pwm_extra_ip
#define PWM_ID			1

extern struct max_pwm_init_param ad4692_pwm_extra_ip;
extern struct max_gpio_init_param ad4692_gpio_extra_ip;
#if (TARGET_NUM == 32690)
#define GPIO_DREADY_PORT_NUM	1
#define GPIO_DREADY_PIN_NUM	30
#define GPIO_IRQ_ID		1
#define GPIO_HANDLE		MXC_GPIO_GET_GPIO(GPIO_DREADY_PORT_NUM)

#define GPIO_RESET_PORT_NUM	0
#define GPIO_RESET_PIN_NUM	8

#define NVIC_GPIO_IRQ		GPIO3_IRQn
#endif

#endif /* __PARAMETERS_H__ */
