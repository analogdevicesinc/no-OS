/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef STM32_CAPI_IRQ_H_
#define STM32_CAPI_IRQ_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"
#include "capi_irq.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Maximum number of external interrupts supported.
 *
 * This value should cover all NVIC IRQs for the target MCU.
 * Most STM32 devices have fewer than 128 interrupts.
 */
#ifndef STM32_CAPI_IRQ_MAX_IRQS
#define STM32_CAPI_IRQ_MAX_IRQS		128
#endif

/**
 * @brief Maximum number of EXTI lines supported.
 *
 * STM32 devices typically have 16 GPIO EXTI lines (0-15).
 */
#define STM32_CAPI_EXTI_MAX_LINES	16

/**
 * @struct stm32_capi_irq_extra_config
 * @brief STM32-specific IRQ controller configuration for CAPI interface
 */
struct stm32_capi_irq_extra_config {
	/** Priority group configuration (0-7, see NVIC_SetPriorityGrouping) */
	uint32_t priority_grouping;
	/** Default preemption priority for new IRQs */
	uint32_t default_preempt_priority;
	/** Default sub-priority for new IRQs */
	uint32_t default_sub_priority;
};

/**
 * @brief Disconnect an ISR from an IRQ.
 * @param irq - IRQ number.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_irq_disconnect(uint32_t irq);

/**
 * @brief Set pending interrupt flag.
 * @param irq - IRQ number.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_irq_set_pending(uint32_t irq);

/**
 * @brief Check if IRQ is enabled.
 * @param irq - IRQ number.
 * @param enabled - Pointer to store enabled status.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_irq_is_enabled(uint32_t irq, bool *enabled);

/**
 * @brief Generic IRQ handler that dispatches to registered callbacks.
 * @param irq - IRQ number.
 *
 * This can be called from the actual ISR handlers (e.g., EXTI0_IRQHandler)
 * to dispatch to the registered callback.
 */
void stm32_capi_irq_handler(uint32_t irq);

/**
 * @brief EXTI callback handler for GPIO interrupts.
 * @param gpio_pin - GPIO pin number (0-15).
 *
 * This should be called from HAL_GPIO_EXTI_Callback or directly from
 * EXTI IRQ handlers.
 */
void stm32_capi_exti_handler(uint16_t gpio_pin);

#if defined(__cplusplus)
}
#endif

#endif /* STM32_CAPI_IRQ_H_ */
