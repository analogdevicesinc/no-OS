/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by adin1140
 *           project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef PARAMETERS_H_
#define PARAMETERS_H_

#include "no_os_irq.h"
#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#include "stm32_spi.h"

extern UART_HandleTypeDef huart3;
#define UART_INSTANCE	(&huart3)

#define UART_IRQ_ID    USART3_IRQn

#define UART_DEVICE_ID	3U
#define UART_BAUDRATE	115200
#define UART_OPS	&stm32_uart_ops
#define UART_EXTRA	&adin1140_uart_extra_ip

#define SPI_DEVICE_ID 	1U
#define SPI_BAUDRATE    13000000
#define SPI_OPS         &stm32_spi_ops
#define SPI_CS          14
#define SPI_EXTRA       &adin1140_spi_extra_ip

#define GPIO_CS_PORT	3

#define ADIN1140_INT_IRQ_ID	15
#define ADIN1140_INT_PIN	15

#define ADIN1140_INT_IRQ_OPS &stm32_gpio_irq_ops
#define ADIN1140_INT_IRQ_EXTRA &adin1140_gpio_irq_extra_ip

#define ADIN1140_NVIC_IRQ_ID 0
#define ADIN1140_NVIC_IRQ_OPS &stm32_irq_ops

#define ADIN1140_INT_GPIO_IRQn EXTI15_IRQn
#define ADIN1140_INT_TRIG NO_OS_IRQ_EDGE_FALLING

extern struct stm32_uart_init_param adin1140_uart_extra_ip;
extern struct stm32_spi_init_param adin1140_spi_extra_ip;
extern struct stm32_gpio_irq_init_param adin1140_gpio_irq_extra_ip;

#endif /* PARAMETERS_H_ */
