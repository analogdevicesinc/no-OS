#ifndef STM32_CAPI_IRQ_H_
#define STM32_CAPI_IRQ_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"
#include "capi/capi_irq.h"

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
 * @struct stm32_capi_irq_entry
 * @brief IRQ callback entry
 */
struct stm32_capi_irq_entry {
	/** Callback function */
	capi_isr_callback_t callback;
	/** Callback argument */
	void *arg;
	/** IRQ enabled flag */
	bool enabled;
};

/**
 * @brief Initialize the STM32 IRQ controller
 * @param config - IRQ configuration
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_init(struct capi_irq_config *config);

/**
 * @brief Deinitialize the STM32 IRQ controller
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_deinit(void);

/**
 * @brief Enable global interrupts
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_global_enable(void);

/**
 * @brief Disable global interrupts
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_global_disable(void);

/**
 * @brief Enable a specific IRQ
 * @param irq - IRQ number
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_enable(uint32_t irq);

/**
 * @brief Disable a specific IRQ
 * @param irq - IRQ number
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_disable(uint32_t irq);

/**
 * @brief Connect an ISR to an IRQ
 * @param irq - IRQ number
 * @param isr - ISR callback function
 * @param arg - Argument passed to ISR
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg);

/**
 * @brief Disconnect an ISR from an IRQ
 * @param irq - IRQ number
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_disconnect(uint32_t irq);

/**
 * @brief Clear pending interrupt flag
 * @param irq - IRQ number
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_clear_pending(uint32_t irq);

/**
 * @brief Set pending interrupt flag
 * @param irq - IRQ number
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_set_pending(uint32_t irq);

/**
 * @brief Get IRQ status (active/pending)
 * @param irq - IRQ number
 * @param pactive - Pointer to store status (1=active/pending, 0=inactive)
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_get_status(uint32_t irq, uint32_t *pactive);

/**
 * @brief Set IRQ priority
 * @param irq - IRQ number
 * @param priority - Priority value (encoded preempt + sub priority)
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_set_priority(uint32_t irq, uint32_t priority);

/**
 * @brief Get IRQ priority
 * @param irq - IRQ number
 * @param priority - Pointer to store priority value
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_get_priority(uint32_t irq, uint32_t *priority);

/**
 * @brief Set IRQ trigger level/edge (for EXTI lines)
 * @param irq - IRQ number (must be an EXTI IRQ)
 * @param trigger - Trigger type
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_set_level_edge_trigger(uint32_t irq,
					  enum capi_irq_trig_level trigger);

/**
 * @brief Check if IRQ is enabled
 * @param irq - IRQ number
 * @param enabled - Pointer to store enabled status
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_irq_is_enabled(uint32_t irq, bool *enabled);

/**
 * @brief Get the callback entry for an IRQ (for use in ISR handlers)
 * @param irq - IRQ number
 * @return Pointer to callback entry, or NULL if invalid
 */
const struct stm32_capi_irq_entry *stm32_capi_irq_get_entry(uint32_t irq);

/**
 * @brief Generic IRQ handler that dispatches to registered callbacks
 * @param irq - IRQ number
 *
 * This can be called from the actual ISR handlers (e.g., EXTI0_IRQHandler)
 * to dispatch to the registered callback.
 */
void stm32_capi_irq_handler(uint32_t irq);

/**
 * @brief EXTI callback handler for GPIO interrupts
 * @param gpio_pin - GPIO pin number (0-15)
 *
 * This should be called from HAL_GPIO_EXTI_Callback or directly from
 * EXTI IRQ handlers.
 */
void stm32_capi_exti_handler(uint16_t gpio_pin);

#if defined(__cplusplus)
}
#endif

#endif /* STM32_CAPI_IRQ_H_ */
