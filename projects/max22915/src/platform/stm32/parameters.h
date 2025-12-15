/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definition of STM32 platform data used by MAX22915 project.
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
#include "stm32_spi.h"
#include "stm32_uart.h"

#define INTC_DEVICE_ID 0

/* UART device parameters */
#define UART_INSTANCE		&huart5
#define UART_DEVICE_ID		5U // UART5
#define UART_IRQ_ID		UART5_IRQn
#define UART_BAUDRATE		230400
#define uart_platform_ops	stm32_uart_ops
#define UART_EXTRA		&max22915_uart_extra_ip

/* SPI device parameters */
#define SPI_DEVICE_ID		1 // SPI1
#define GPIO_CS_PIN		15 // PA15
#define GPIO_CS_PORT		0 // GPIOA
#define SPI_MAX_SPEED_HZ	50000
#define spi_platform_ops	stm32_spi_ops
#define SPI_EXTRA		&max22915_spi_extra_ip

extern UART_HandleTypeDef huart5;
extern struct stm32_uart_init_param max22915_uart_extra_ip;
extern struct stm32_spi_init_param max22915_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
