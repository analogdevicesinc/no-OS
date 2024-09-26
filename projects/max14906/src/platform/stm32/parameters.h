/***************************************************************************//**
 *   @file   max14906/src/platform/maxim/parameters.h
 *   @brief  Definition of STM32 platform data used by max14906 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"

extern UART_HandleTypeDef huart2;

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID		0
#define IIO_APP_HUART		(&huart2)
#endif
#define UART_IRQ_ID     	USART2_IRQn
#define UART_DEVICE_ID		2
#define UART_BAUDRATE		115200
#define UART_OPS		&stm32_uart_ops
#define UART_EXTRA		&max14906_uart_extra_ip

#ifdef BASIC_EXAMPLE
extern struct stm32_gpio_irq_init_param max14906_gpio_irq_extra_ip;

#define GPIO_IRQ_ID		0 /* Pin 0 */
#define GPIO_IRQ_OPS		&stm32_gpio_irq_ops
#define GPIO_IRQ_EXTRA		&max14906_gpio_irq_extra_ip

#define GPIO_OPS		&stm32_gpio_ops
#define GPIO_EXTRA		NULL

#define GPIO_FAULT_PORT_NUM	0
#define GPIO_FAULT_PIN_NUM	0
#endif

#define SPI_DEVICE_ID		1
#define SPI_CS			4
#define SPI_CS_PORT		0
#define SPI_OPS			&stm32_spi_ops
#define SPI_EXTRA		&max14906_spi_extra_ip
#define SPI_BAUDRATE		100000

extern struct stm32_uart_init_param max14906_uart_extra_ip;
extern struct stm32_spi_init_param max14906_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
