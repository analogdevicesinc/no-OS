/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef STM32_CAPI_UART_H_
#define STM32_CAPI_UART_H_

#include "stm32_hal.h"
#include "capi_uart.h"

/**
 * @brief STM32-specific UART extra configuration
 */
struct stm32_uart_extra_config {
	/** Pointer to the STM32 HAL UART handle */
	UART_HandleTypeDef *huart;
};

/**
 * @brief STM32 platform specific UART operations for CAPI
 */
extern const struct capi_uart_ops stm32_capi_uart_ops;

/**
 * @brief Enable stdio redirection over the STM32 UART peripheral.
 * @param handle - Pointer to an initialized UART handle.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_uart_stdio_enable(struct capi_uart_handle *handle);

#endif /* STM32_CAPI_UART_H_ */
