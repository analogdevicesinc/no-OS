/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Parameters Definitions.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#if defined(ADUCM_PLATFORM)
#include "aducm3029_irq.h"
#include "aducm3029_gpio_irq.h"

#define SPI_DEVICE_ID		0
#define SPI_CS			1
#define INTC_DEVICE_ID		ADUCM_GPIO_B_GROUP_SOFT_CTRL
#define UART_DEVICE_ID		0
#define UART_IRQ_ID		ADUCM_UART_INT_ID
#define UART_BAUDRATE		230400
#define I2C_DEVICE_ID		0
#define I2C_BAUDRATE		100000
#define RESET_PIN		13 // 0.13
#define GP0_PIN			15 // 0.15
#define INT_IRQn		GP0_PIN
#define UART_OPS			&aducm_uart_ops
#define IRQ_OPS			&aducm_irq_ops
#define GPIO_IRQ_OPS    	&aducm_gpio_irq_ops

#elif defined(STM32_PLATFORM)
#include "stm32_hal.h"

#define SPI_DEVICE_ID		1
#define SPI_CS			15
#define SPI_CS_PORT		0
#define INTC_DEVICE_ID		0
#define INT_IRQn		EXTI9_5_IRQn
#define UART_DEVICE_ID		5
#define UART_IRQ_ID		UART5_IRQn
#define UART_OPS			&stm32_uart_ops
#define IRQ_OPS			&stm32_irq_ops
#define GPIO_IRQ_OPS    	&stm32_gpio_irq_ops
#ifdef IIO_SUPPORT
extern UART_HandleTypeDef 	huart5;
#endif
#define IIO_APP_HUART		&huart5
#define UART_BAUDRATE		230400
#define I2C_DEVICE_ID		1
#define I2C_BAUDRATE		100000

#define RESET_PIN		12 // D.12
#define GP0_PIN			7 // G.7
#endif

#endif // __PARAMETERS_H__
