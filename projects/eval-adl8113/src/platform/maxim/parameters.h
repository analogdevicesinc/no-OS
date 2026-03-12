/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Maxim platform used by eval-adl8113 project.
 *   @author Raul Geo <Raul.Georgian@analog.com>
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "maxim_irq.h"

#define UART_DEVICE_ID  0
#define UART_IRQ_ID     UART0_IRQn
#define UART_BAUDRATE   57600
#define UART_EXTRA      &adl8113_uart_extra_ip
#define UART_OPS        &max_uart_ops

#ifdef IIO_SUPPORT
#define INTC_DEVICE_ID  0
#endif

/* GPIO Port and Pin definitions for ADL8113 */
#define GPIO_VA_PORT		1
#define GPIO_VA_PIN		6
#define GPIO_VB_PORT		1
#define GPIO_VB_PIN		7

/* LED definitions (active low on MAX32655FTHR) */
#define LED_RED_PORT		0
#define LED_RED_PIN		18
#define LED_GREEN_PORT		0
#define LED_GREEN_PIN		19
#define LED_BLUE_PORT		0
#define LED_BLUE_PIN		26

/* Button definitions */
#define BUTTON1_PORT		0
#define BUTTON1_PIN		2
#define BUTTON2_PORT		0
#define BUTTON2_PIN		3

#define GPIO_OPS		&max_gpio_ops
#define GPIO_EXTRA		&adl8113_gpio_extra_ip

extern struct max_gpio_init_param adl8113_gpio_extra_ip;
extern struct max_uart_init_param adl8113_uart_extra_ip;

#endif /* __PARAMETERS_H__ */
