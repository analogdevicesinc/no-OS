/***************************************************************************//**
 *   @file   platform.h
 *   @brief  Header file of platform configuration.
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
