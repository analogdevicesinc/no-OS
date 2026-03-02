/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of Maxim platform data used by ad4692 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID		0
#endif

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
