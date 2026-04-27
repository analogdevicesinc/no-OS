/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _STM32_CAPI_UART_PRIV_H_
#define _STM32_CAPI_UART_PRIV_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "stm32_capi_uart.h"

/**
 * @brief STM32-specific UART private handle data
 */
struct stm32_uart_priv_handle {
	/** Pointer to the STM32 HAL UART handle */
	UART_HandleTypeDef *huart;
	/** User-registered CAPI async callback */
	capi_uart_callback callback;
	/** User-provided argument passed to callback */
	void *callback_arg;
	/** Cached interrupt reason for get_interrupt_reason */
	enum capi_uart_interrupt_reason last_interrupt_reason;
	/** Cached line status flags for get_line_status */
	uint32_t last_line_status;
};

#define CAPI_UART_HANDLE_STM32_INIT() \
	(&(struct capi_uart_handle){ \
		.ops = NULL, \
		.init_allocated = false, \
		.priv = &(struct stm32_uart_priv_handle){0}})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _STM32_CAPI_UART_PRIV_H_ */
