/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Mbed platform used by adf4377 project.
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

#include <PinNames.h>
#include "mbed_uart.h"
#include "mbed_spi.h"
#include "no_os_uart.h"
#include "mbed_gpio.h"
#include "no_os_gpio.h"

#define UART_TX_PIN	CONSOLE_TX
#define	UART_RX_PIN	CONSOLE_RX
#define UART_DEVICE_ID  5
#define UART_IRQ_ID     53
#define UART_BAUDRATE   115200
#define UART_EXTRA	&adf4377_uart_extra_ip
#define UART_OPS        &mbed_uart_ops

#define SPI_BAUDRATE    4000000
#define SPI_OPS         &mbed_spi_ops
#define SPI_EXTRA       &adf4377_spi_extra
#define SPI_DEVICE_ID   5
#define SPI_CS          SDP_SPI_CS_A

/* SDP GPIO Configuration */
#define GPIO_OPS 	&mbed_gpio_ops
#define GPIO_EXTRA 	&adf4377_gpio_ip
// #define GPIO_SDP 	SDP_GPIO_0
#define GPIO_LKDET      SDP_GPIO_0
#define GPIO_ENCLK1     SDP_GPIO_1
#define GPIO_ENCLK2     SDP_GPIO_2
#define GPIO_MUXOUT     SDP_GPIO_4
#define GPIO_CE		SDP_GPIO_5




extern struct mbed_uart_init_param adf4377_uart_extra_ip;
extern struct mbed_spi_init_param adf4377_spi_extra;
extern struct no_os_gpio_init_param adf4377_gpio_ip;

#endif /* __PARAMETERS_H__ */
