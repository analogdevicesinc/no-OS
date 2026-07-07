/***************************************************************************//**
 * @file parameters.h
 * @brief Definitions specific to STM32 platform used by capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_capi_uart.h"
#include "stm32_capi_gpio.h"
#include "capi_uart.h"

extern UART_HandleTypeDef huart3;

#define UART_IDENTIFIER		0U
#define UART_OPS		&stm32_capi_uart_ops
#define UART_BAUDRATE		115200U
#define UART_EXTRA_TYPE		struct stm32_uart_extra_config
#define UART_EXTRA_INIT		{ .huart = &huart3 }
#define PLATFORM_NAME		"STM32"

/*
 * GPIO loopback pair on NUCLEO-F767ZI:
 *   PE0 (output, GPIOE pin 0) wired to PF0 (input, GPIOF pin 0).
 * Each port is opened with num_pins=1 so bit 0 maps to physical pin 0.
 */
#define GPIO_OUTPUT_IDENTIFIER		((uint64_t)(uintptr_t)GPIOE)
#define GPIO_OUTPUT_NUM_PINS		1U
#define GPIO_OUTPUT_OPS			&stm32_capi_gpio_ops
#define GPIO_OUTPUT_NAME		"PE0"
#define GPIO_OUTPUT_EXTRA		struct stm32_capi_gpio_port_config
#define GPIO_OUTPUT_EXTRA_INIT		{ .mode = GPIO_MODE_OUTPUT_PP, \
					  .speed = GPIO_SPEED_FREQ_LOW, \
					  .alternate = 0U, \
					  .pull = GPIO_NOPULL }

#define GPIO_INPUT_IDENTIFIER		((uint64_t)(uintptr_t)GPIOF)
#define GPIO_INPUT_NUM_PINS		1U
#define GPIO_INPUT_OPS			&stm32_capi_gpio_ops
#define GPIO_INPUT_NAME			"PF0"
#define GPIO_INPUT_EXTRA		struct stm32_capi_gpio_port_config
#define GPIO_INPUT_EXTRA_INIT		{ .mode = GPIO_MODE_INPUT, \
					  .speed = GPIO_SPEED_FREQ_LOW, \
					  .alternate = 0U, \
					  .pull = GPIO_NOPULL }

#endif /* __PARAMETERS_H__ */
