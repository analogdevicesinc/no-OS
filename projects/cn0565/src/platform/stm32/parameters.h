/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by cn0565 project.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_uart.h"
#include "stm32_spi.h"
#include "stm32_i2c.h"
#include "stm32_gpio.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_uart_stdio.h"

#define SPI_DEVICE_ID		1
#define SPI_CS			15
#define SPI_CS_PORT		0
#define INT_IRQn		EXTI9_5_IRQn
#define UART_DEVICE_ID		5
#define UART_IRQ_ID		UART5_IRQn
#define UART_OPS		&stm32_uart_ops
#define IRQ_OPS			&stm32_irq_ops
#define GPIO_IRQ_OPS		&stm32_gpio_irq_ops
#define GPIO_OPS		&stm32_gpio_ops
#define SPI_OPS			&stm32_spi_ops
#define I2C_OPS			&stm32_i2c_ops
#define UART_BAUDRATE		230400
#define I2C_DEVICE_ID		1
#define I2C_BAUDRATE		100000

#define RESET_PIN		12 // D.12
#define RESET_PORT		3
#define GP0_PIN			7 // G.7
#define GP0_PORT		6

#define GPIO_IRQ_CTRL_ID	GP0_PIN

#define UART_EXTRA		&cn0565_uart_extra_ip
#define SPI_EXTRA		&cn0565_spi_extra_ip
#define I2C_EXTRA		NULL
#define GPIO_RESET_EXTRA	&cn0565_reset_gpio_extra_ip
#define GPIO_GP0_EXTRA		&cn0565_gp0_gpio_extra_ip
#define GPIO_IRQ_EXTRA		&cn0565_gpio_irq_extra_ip

extern UART_HandleTypeDef huart5;

extern struct stm32_uart_init_param cn0565_uart_extra_ip;
extern struct stm32_spi_init_param cn0565_spi_extra_ip;
extern struct stm32_gpio_init_param cn0565_reset_gpio_extra_ip;
extern struct stm32_gpio_init_param cn0565_gp0_gpio_extra_ip;
extern struct stm32_gpio_irq_init_param cn0565_gpio_irq_extra_ip;

#endif // __PARAMETERS_H__
