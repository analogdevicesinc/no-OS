/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform-specific parameters for MAXM86161 on MAX32655.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
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

#include "maxim_i2c.h"
#include "maxim_irq.h"
#include "maxim_gpio_irq.h"
#include "maxim_gpio.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"

#define UART_DEVICE_ID 0
#define UART_BAUDRATE 230400
#define UART_IRQ_ID UART0_IRQn
#define UART_OPS &max_uart_ops
#define UART_EXTRA &maxm86161_uart_extra

#define I2C_DEVICE_ID 2
#define I2C_OPS &max_i2c_ops
#define I2C_EXTRA &maxm86161_i2c_extra

#define GPIO_INTB_PORT_NUM              2
#define GPIO_INTB_PIN_NUM               7
#define NVIC_GPIO_IRQ                   MXC_GPIO_GET_IRQ(GPIO_INTB_PORT_NUM)
#define GPIO_IRQ_ID                     GPIO_INTB_PORT_NUM
#define MAXM86161_GPIO_TRIG_IRQ_ID      GPIO_INTB_PIN_NUM
#define MAXM86161_GPIO_CB_HANDLE        MXC_GPIO_GET_GPIO(GPIO_INTB_PORT_NUM)
#define GPIO_IRQ_OPS                    &max_gpio_irq_ops
#define IRQ_OPS                         &max_irq_ops
#define GPIO_OPS                        &max_gpio_ops
#define GPIO_IRQ_EXTRA                  &maxm86161_gpio_extra

#define INTC_DEVICE_ID 2
#define INTB_PORT 2
#define INTB_PIN 7

extern struct max_uart_init_param maxm86161_uart_extra;
extern struct max_i2c_init_param maxm86161_i2c_extra;
extern struct max_gpio_init_param maxm86161_gpio_extra;

#endif /* __PARAMETERS_H__ */
