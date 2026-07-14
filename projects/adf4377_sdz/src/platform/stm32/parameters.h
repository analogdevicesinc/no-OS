/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by adf4377 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
 *   @author Jude Osemene (jude.osemene@analog.com)
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

#include "stm32_gpio.h"
#include "stm32_irq.h"
#include "stm32_spi.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#include "no_os_gpio.h"

extern UART_HandleTypeDef huart5;

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#define IIO_APP_HUART   (&huart5)
#endif
#define UART_IRQ_ID     UART5_IRQn

#define UART_DEVICE_ID  5
#define UART_BAUDRATE   115200
#define UART_EXTRA      &adf4377_uart_extra_ip
#define UART_OPS        &stm32_uart_ops

#define SPI_DEVICE_ID   5
#define SPI_BAUDRATE    4000000
#define SPI_CS          9
#define SPI_CS_PORT     1
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &adf4377_spi_extra_ip

#define GPIO_OPS        &stm32_gpio_ops
#define GPIO_EXTRA      &adf4377_gpio_extra_ip

/*
 * The SDP-CK1Z (STM32F469) routes the SDP connector GPIO lines to GPIOJ
 * (port 9), but the SDP_GPIOn -> PJx mapping is NOT sequential; it must be
 * read from the SDP-CK1Z schematic per line.
 * The EVAL-ADF4377 assigns the control signals to the SDP GPIO lines as:
 *   LKDET = SDP_GPIO0, ENCLK1 = SDP_GPIO1, ENCLK2 = SDP_GPIO2,
 *   MUXOUT = SDP_GPIO4, CE = SDP_GPIO5.
 * Confirmed against the schematic:
 *   SDP_GPIO0 -> PJ0, SDP_GPIO2 (ENCLK2) -> PJ3, SDP_GPIO5 (CE) -> PJ12.
 * The PJ pins for ENCLK1/MUXOUT/LKDET below are still unverified.
 * Port numbering follows stm32_gpio: 0=GPIOA, 1=GPIOB, ..., 9=GPIOJ.
 */
#define GPIO_CE          12
#define GPIO_CE_PORT     9
#define GPIO_ENCLK1      1
#define GPIO_ENCLK1_PORT 9
#define GPIO_ENCLK2      3
#define GPIO_ENCLK2_PORT 9
#define GPIO_MUXOUT      5
#define GPIO_MUXOUT_PORT 9
#define GPIO_LKDET       0
#define GPIO_LKDET_PORT  9
#define GPIO_SDP         0
#define GPIO_SDP_PORT    9

extern struct stm32_gpio_init_param adf4377_gpio_extra_ip;
extern struct stm32_uart_init_param adf4377_uart_extra_ip;
extern struct stm32_spi_init_param  adf4377_spi_extra_ip;
extern struct no_os_gpio_init_param adf4377_gpio_ip;

#endif /* __PARAMETERS_H__ */
