/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL Interrupt Controller.
 */

#ifndef _CAPI_IRQ_H_
#define _CAPI_IRQ_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Enumerated type that defines the trigger type.
 */
enum capi_irq_trig_level {
	CAPI_IRQ_LEVEL_LOW,    /**< Trigger when signal is low. */
	CAPI_IRQ_LEVEL_HIGH,   /**< Trigger when signal is high. */
	CAPI_IRQ_EDGE_FALLING, /**< Trigger on falling edge. */
	CAPI_IRQ_EDGE_RISING,  /**< Trigger on rising edge. */
	CAPI_IRQ_EDGE_BOTH     /**< Trigger on rising and falling edge. */
};

/**
 * @brief Interrupt Service Routine.
 *
 * @param [in] arg Points to application defined argument.
 */
typedef void (*capi_isr_callback_t)(void *arg);

/**
 * @struct capi_irq_config
 * @brief Structure that holds the IRQ Controller configuration.
 */
struct capi_irq_config {
	/** Interrupt request controller ID. */
	uint32_t irq_ctrl_id;
	/**
	 * This is intended to store irq controller specific configurations,
	 * it should not be a reference to any peripheral descriptor.
	 */
	void *extra;
};

/**
 * @brief Initialize an instance of the IRQ controller.
 *
 * @param [in] config Points to the configuration for the IRQ controller.
 * @return int 0 for success or error code.
 */
int capi_irq_init(struct capi_irq_config *config);

/**
 * @brief Deinitialize the IRQ controller, disable, and bring to default settings.
 *
 * @return int 0 for success or error code.
 */
int capi_irq_deinit(void);

/**
 * @brief Enable the interrupt controller.
 *
 * @return int 0 for success or error code.
 */
int capi_irq_global_enalbe(void);

/**
 * @brief Disable the interrupt controller.
 *
 * @return int 0 for success or error code.
 */
int capi_irq_global_disable(void);

/**
 * @brief Enable specific irq.
 *
 * @param [in] irq Defines the IRQ number.
 * @return int 0 for success or error code.
 */
int capi_irq_enable(uint32_t irq);

/**
 * @brief Disable specific irq.
 *
 * @param [in] irq Defines the IRQ number.
 * @return int 0 for success or error code.
 */
int capi_irq_disable(uint32_t irq);

/**
 * @brief Set isr for the irq
 *
 * @param [in] irq Defines the IRQ number.
 * @param [in] isr function pointer to isr that needs to be registered.
 * @param [in] arg A pointer to application defined argument returned when ISR is called.
 * @return int 0 for success or error code.
 */
int capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg);

/**
 * @brief Clear a specific pending irq.
 *
 * @param [in] irq Defines the IRQ number.
 * @return int 0 for success or error code.
 */
int capi_irq_clear_pending(uint32_t irq);

/**
 * @brief Get status of a specific irq.
 *
 * @param [in] irq Defines the IRQ number.
 * @param [out] pactive Pointer to irq status update.
 * @return int 0 for success or error code.
 */
int capi_irq_get_status(uint32_t irq, uint32_t *pactive);

/**
 * @brief Set irq priority.
 *
 * @param [in] irq Defines the IRQ number.
 * @param [in] priority  Priority to set.
 * @return int 0 for success or error code.
 */
int capi_irq_set_priority(uint32_t irq, uint32_t priority);

/**
 * @brief Get irq priority.
 *
 * @param [in] irq Defines the IRQ number.
 * @param [out] priority irq priority retrieved.
 * @return int 0 for success or error code.
 */
int capi_irq_get_priority(uint32_t irq, uint32_t *priority);

/**
 * @brief Set the IRQ to fire when defined level or edge is detected based on the given
 * configuration.
 *
 * @param [in] irq Defines the IRQ number.
 * @param [in] trigger The defined state for IRQ trigger which can be level or edge.
 * @return int 0 for success or error code.
 */
int capi_irq_set_level_edge_trigger(uint32_t irq, enum capi_irq_trig_level trigger);

#if defined(__cplusplus)
}
#endif

#endif // CAPI_IRQ_H_
