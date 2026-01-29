/***************************************************************************//**
 *   @file   platform.h
 *   @brief  Header file of platform configuration.
 *   @author Radu Etz (radu.etz@analog.com)
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
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "no_os_gpio.h"
#include "no_os_uart.h"
#include "no_os_irq.h"
#include "maxim_gpio.h"
#include "maxim_irq.h"
#include "maxim_uart.h"

#define GPIO_IRQ_OPS                &max_gpio_irq_ops
#define GPIO_CTRL_IRQ_ID            0
#define GPIO_IRQ_EXTRA              &max_gpio_extra_ip
#define NVIC_GPIO_IRQ               GPIO2_IRQn


/* GPIO IRQ init params */
extern struct no_os_gpio_init_param gnss_gpio_irq_ip;
/* GPIO interrupt init params */
extern struct no_os_irq_init_param gnss_gpio_int_ip;

/* Reset port and pin */
#define GNSS_RESET_PORT			2
#define GNSS_RESET_PIN			7

/* User LED port and pin */
#define GNSS_LED_PORT			2
#define GNSS_LED_PIN			1

/* IRQN interrupt port and pin */
#define GPIO_IRQ_PORT            2
#define GPIO_IRQ_PIN             9

/* GPIO toggle function for LED */
int platform_toggle_led(struct no_os_gpio_desc *gpio_desc);

#endif /* __PLATFORM_H__ */
