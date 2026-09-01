/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Platform-specific parameter declarations for eval-cn0391-ardz.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
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

extern UART_HandleTypeDef huart1;
#define UART_INSTANCE	(&huart1)
#define UART_IRQ_ID	USART1_IRQn

#define UART_DEVICE_ID	1
#define UART_BAUDRATE	115200
#define UART_EXTRA	&cn0391_uart_extra_ip
#define UART_OPS	&stm32_uart_ops

#define SPI_DEVICE_ID	3
#define SPI_BAUDRATE	1000000
#define SPI_CS		12
#define SPI_CS_PORT	6
#define SPI_OPS		&stm32_spi_ops
#define SPI_EXTRA	&cn0391_spi_extra_ip

#define GPIO_OPS	&stm32_gpio_ops

#define ADIN1110_GPIO_RESET_PORT	2
#define ADIN1110_GPIO_RESET_PIN		7
#define ADIN1110_SPI_DEVICE_ID		2
#define ADIN1110_SPI_CS			12
#define ADIN1110_SPI_CS_PORT		1
#define ADIN1110_SPI_CLK_RATE		25000000

extern struct stm32_uart_init_param cn0391_uart_extra_ip;
extern struct stm32_spi_init_param cn0391_spi_extra_ip;
extern struct stm32_gpio_init_param adin1110_reset_gpio_extra_ip;
extern struct stm32_spi_init_param adin1110_spi_extra_ip;

#endif /* __PARAMETERS_H__ */
