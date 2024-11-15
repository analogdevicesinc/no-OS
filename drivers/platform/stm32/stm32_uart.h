/***************************************************************************//**
 *   @file   stm32/stm32_uart.h
 *   @brief  Header file of UART driver for STM32
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _STM32_UART_H_
#define _STM32_UART_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_uart.h"
#include "no_os_irq.h"
#include "stm32_hal.h"

/**
 * @struct stm32_uart_init_param
 * @brief Specific initialization parameters for stm32 UART.
 */
struct stm32_uart_init_param {
	/** UART instance */
	UART_HandleTypeDef *huart;
	/** UART transaction timeout (HAL_IncTick() units) */
	uint32_t timeout;
};

/**
 * @struct stm32_uart_desc
 * @brief stm32 platform specific UART descriptor
 */
struct stm32_uart_desc {
	/** UART instance */
	UART_HandleTypeDef *huart;
	/** UART transaction timeout (HAL_IncTick() units) */
	uint32_t timeout;
	/** Interrupt controller descriptor */
	struct no_os_irq_ctrl_desc *nvic;
	/** RX complete callback */
	struct no_os_callback_desc rx_callback;
};

/**
 * @brief STM32 specific UART platform ops structure
 */
extern const struct no_os_uart_platform_ops stm32_uart_ops;

#endif
