/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  ADIOL100 project platform-specific parameters (STM32).
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#include "stm32_gpio_irq.h"

extern UART_HandleTypeDef huart5;

#define UART_DEVICE_ID  5
#define UART_BAUDRATE   115200
#define UART_OPS        &stm32_uart_ops
#define UART_EXTRA      &adiol100_uart_extra

#define SPI_DEVICE_ID   1
#define SPI_CS          15
#define SPI_CS_PORT     0
#define SPI_BAUDRATE    1000000
#define SPI_OPS         &stm32_spi_ops
#define SPI_EXTRA       &adiol100_spi_extra

#define GPIO_OPS        &stm32_gpio_ops
#define GPIO_EXTRA      NULL
#define GPIO_IRQ_OPS    &stm32_gpio_irq_ops
#define IRQ_OPS         &stm32_irq_ops

#define IRQA_PORT       6
#define IRQA_PIN        7
#define NVIC_GPIO_IRQ   EXTI9_5_IRQn
#define GPIO_IRQ_CTRL_ID  IRQA_PIN
#define GPIO_IRQ_EXTRA  &adiol100_gpio_irq_extra

extern struct stm32_gpio_irq_init_param adiol100_gpio_irq_extra;

extern struct stm32_uart_init_param adiol100_uart_extra;
extern struct stm32_spi_init_param  adiol100_spi_extra;

#endif
