/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by eval-adxrs290-pmdz
 *           project.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"

#ifdef IIO_SUPPORT
extern UART_HandleTypeDef huart5;
#define INTC_DEVICE_ID  0
#define IIO_APP_HUART   (&huart5)
#define UART_IRQ_ID     UART5_IRQn
#define UART_DEVICE_ID  5
#define UART_BAUDRATE   115200
#define UART_EXTRA		&adxrs290_uart_extra_ip
#define UART_OPS		&stm32_uart_ops
#endif

#define SPI_DEVICE_ID   1
#define SPI_BAUDRATE    4000000
#define SPI_CS          15
#define SPI_CS_PORT     0
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &adxrs290_spi_extra_ip

#define GPIO_SYNC_PIN_NUM   2
#define GPIO_SYNC_PORT_NUM  0
#define GPIO_OPS            &stm32_gpio_ops
#define GPIO_EXTRA          &adxrs290_gpio_extra_ip

extern struct stm32_uart_init_param adxrs290_uart_extra_ip;
extern struct stm32_spi_init_param adxrs290_spi_extra_ip;
extern struct stm32_gpio_init_param adxrs290_gpio_extra_ip;

#endif /* __PARAMETERS_H__ */
