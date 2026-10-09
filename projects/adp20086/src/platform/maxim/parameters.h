/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Header file of Maxim platform data used by adp20086 project.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_irq.h"
#include "maxim_i2c.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_gpio_irq.h"
#include "maxim_uart_stdio.h"


#define UART_DEVICE_ID          0
#define UART_BAUDRATE           115200
#define UART_OPS                &max_uart_ops
#define UART_EXTRA              &adp20086_uart_extra

#define I2C_DEVICE_ID           2
#define I2C_MAX_SPEED           400000
#define I2C_OPS                 &max_i2c_ops
#define I2C_EXTRA               &adp20086_i2c_extra

#define GPIO_OPS                &max_gpio_ops
#define GPIO_EXTRA              &adp20086_gpio_extra
#define EN_PORT                 1
#define EN_PIN                  7

/* INTB is open drain and active low; it needs an external pull-up (>=2k). */
#define INTB_PORT               1
#define INTB_PIN                8

#define GPIO_IRQ_OPS            &max_gpio_irq_ops
#define GPIO_IRQ_EXTRA          &adp20086_gpio_extra
/* The maxim GPIO IRQ controller is identified by its port. */
#define GPIO_IRQ_ID             INTB_PORT
#define NVIC_GPIO_IRQ           MXC_GPIO_GET_IRQ(INTB_PORT)

extern struct max_uart_init_param adp20086_uart_extra;
extern struct max_i2c_init_param adp20086_i2c_extra;
extern struct max_gpio_init_param adp20086_gpio_extra;

#endif /* __PARAMETERS_H__ */