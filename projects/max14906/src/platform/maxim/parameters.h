/***************************************************************************//**
 *   @file   max14906/src/platform/maxim/parameters.h
 *   @brief  Definition of Maxim platform data used by max14906 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_spi.h"
#include "maxim_gpio.h"
#include "maxim_gpio_irq.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID	0
#endif

#if (TARGET_NUM == 32690)
#define	UART_IRQ_ID	UART0_IRQn
#define UART_DEVICE_ID	0
#define UART_BAUDRATE	57600
#elif (TARGET_NUM == 32665)
#define UART_IRQ_ID	UART1_IRQn
#define UART_DEVICE_ID	1
#define UART_BAUDRATE	57600
#endif
#define UART_EXTRA	&max14906_uart_extra
#define UART_OPS	&max_uart_ops

#if (TARGET_NUM == 32690)
#define SPI_DEVICE_ID	4
#define SPI_CS		0
#elif (TARGET_NUM == 32665)
#define SPI_DEVICE_ID	1
#define SPI_CS		0
#endif

#define SPI_BAUDRATE	100000
#define SPI_OPS		&max_spi_ops
#define SPI_EXTRA	&max14906_spi_extra

extern struct max_uart_init_param max14906_uart_extra;
extern struct max_spi_init_param max14906_spi_extra;

#ifdef BASIC_EXAMPLE
#define GPIO_OPS	&max_gpio_ops
#define GPIO_EXTRA	&max14906_gpio_extra_ip
#define GPIO_IRQ_OPS	&max_gpio_irq_ops
#define GPIO_IRQ_EXTRA	NULL

extern struct max_gpio_init_param max14906_gpio_extra_ip;
#if (TARGET_NUM == 32665)
#define GPIO_FAULT_PORT_NUM	0
#define GPIO_FAULT_PIN_NUM	5
#define GPIO_IRQ_ID		0
#define NVIC_GPIO_IRQ		GPIO0_IRQn
#elif (TARGET_NUM == 32690)
#define GPIO_FAULT_PORT_NUM	2
#define GPIO_FAULT_PIN_NUM	21
#define GPIO_IRQ_ID		2
#define NVIC_GPIO_IRQ		GPIO2_IRQn
#endif
#endif

#endif /* __PARAMETERS_H__ */
