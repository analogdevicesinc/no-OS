/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <errno.h>
#include "stm32_capi_irq.h"

#define STM32_CAPI_IRQ_INVALID	((IRQn_Type)(-1))

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

/** IRQ callback table */
static struct stm32_capi_irq_entry irq_table[STM32_CAPI_IRQ_MAX_IRQS];

/** EXTI line to IRQn mapping */
static IRQn_Type exti_irq_map[STM32_CAPI_EXTI_MAX_LINES];

/** Controller initialized flag */
static bool irq_initialized = false;

/** Current priority grouping */
static uint32_t current_priority_grouping;

/** Default preemption priority */
static uint32_t default_preempt_priority = 0;

/** Default sub-priority */
static uint32_t default_sub_priority = 0;

/**
 * @brief Map GPIO pin number to EXTI IRQn.
 * @param pin - GPIO pin number (0-15).
 * @return IRQn_Type for the EXTI line.
 */
static IRQn_Type get_exti_irqn(uint16_t pin)
{
	if (pin >= STM32_CAPI_EXTI_MAX_LINES)
		return STM32_CAPI_IRQ_INVALID;

#if defined(STM32F0) || defined(STM32L0) || defined(STM32G0)
	/* Some STM32 families have combined EXTI handlers */
	if (pin <= 1)
		return EXTI0_1_IRQn;
	else if (pin <= 3)
		return EXTI2_3_IRQn;
	else
		return EXTI4_15_IRQn;
#elif defined(STM32F1) || defined(STM32F2) || defined(STM32F3) || \
      defined(STM32F4) || defined(STM32F7) || defined(STM32L1) || \
      defined(STM32L4) || defined(STM32L5) || defined(STM32H7) || \
      defined(STM32G4) || defined(STM32WB) || defined(STM32WL) || \
      defined(STM32U5) || defined(STM32H5)
	/* Most STM32 families have individual handlers for EXTI0-4 */
	switch (pin) {
	case 0:
		return EXTI0_IRQn;
	case 1:
		return EXTI1_IRQn;
	case 2:
		return EXTI2_IRQn;
	case 3:
		return EXTI3_IRQn;
	case 4:
		return EXTI4_IRQn;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		return EXTI9_5_IRQn;
	default:
		return EXTI15_10_IRQn;
	}
#else
	/* Fallback for unknown families */
	return EXTI0_IRQn;
#endif
}

/**
 * @brief Initialize EXTI IRQ mapping table.
 */
static void init_exti_irq_map(void)
{
	uint16_t i;

	for (i = 0; i < STM32_CAPI_EXTI_MAX_LINES; i++)
		exti_irq_map[i] = get_exti_irqn(i);
}

/**
 * @brief Check if IRQ number is valid.
 * @param irq - IRQ number.
 * @return true if valid, false otherwise.
 */
static inline bool is_valid_irq(uint32_t irq)
{
	return irq < STM32_CAPI_IRQ_MAX_IRQS;
}

/**
 * @brief Check if IRQ is an EXTI GPIO interrupt.
 * @param irq - IRQ number.
 * @param pin - Pointer to store GPIO pin (0-15) if EXTI.
 * @return true if EXTI GPIO interrupt, false otherwise.
 */
static bool is_exti_gpio_irq(uint32_t irq, uint16_t *pin)
{
	uint16_t i;

	for (i = 0; i < STM32_CAPI_EXTI_MAX_LINES; i++) {
		if ((uint32_t)exti_irq_map[i] == irq) {
			if (pin)
				*pin = i;
			return true;
		}
	}

	return false;
}

/**
 * @brief Initialize the STM32 IRQ controller.
 * @param config - Pointer to the IRQ configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_irq_init(struct capi_irq_config *config)
{
	struct stm32_capi_irq_extra_config *extra;

	if (irq_initialized)
		return 0;

	/* Clear callback table */
	memset(irq_table, 0, sizeof(irq_table));

	/* Initialize EXTI mapping */
	init_exti_irq_map();

	/* Apply extra configuration if provided */
	if (config && config->extra) {
		extra = (struct stm32_capi_irq_extra_config *)config->extra;
		current_priority_grouping = extra->priority_grouping;
		default_preempt_priority = extra->default_preempt_priority;
		default_sub_priority = extra->default_sub_priority;

		/* Configure NVIC priority grouping */
		HAL_NVIC_SetPriorityGrouping(current_priority_grouping);
	} else {
		/* Use default priority grouping (4 bits preemption, 0 bits sub) */
		current_priority_grouping = NVIC_PRIORITYGROUP_4;
		HAL_NVIC_SetPriorityGrouping(current_priority_grouping);
	}

	irq_initialized = true;

	return 0;
}

/**
 * @brief Deinitialize the STM32 IRQ controller.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_irq_deinit(void)
{
	uint32_t i;

	if (!irq_initialized)
		return 0;

	/* Disable all registered IRQs */
	for (i = 0; i < STM32_CAPI_IRQ_MAX_IRQS; i++) {
		if (irq_table[i].enabled) {
			NVIC_DisableIRQ((IRQn_Type)i);
			irq_table[i].enabled = false;
		}
		irq_table[i].callback = NULL;
		irq_table[i].arg = NULL;
	}

	irq_initialized = false;

	return 0;
}

/**
 * @brief Enable global interrupts.
 * @return 0 on success.
 */
static int stm32_capi_irq_global_enable(void)
{
	__enable_irq();
	return 0;
}

/**
 * @brief Disable global interrupts.
 * @return 0 on success.
 */
static int stm32_capi_irq_global_disable(void)
{
	__disable_irq();
	return 0;
}

/**
 * @brief Enable a specific IRQ.
 * @param irq - IRQ number.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_irq_enable(uint32_t irq)
{
	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq))
		return -EINVAL;

	/* Set default priority if not already configured */
	if (!irq_table[irq].enabled && !NVIC_GetEnableIRQ((IRQn_Type)irq))
		HAL_NVIC_SetPriority((IRQn_Type)irq, default_preempt_priority,
				     default_sub_priority);

	NVIC_EnableIRQ((IRQn_Type)irq);
	irq_table[irq].enabled = true;

	return 0;
}

/**
 * @brief Disable a specific IRQ.
 * @param irq - IRQ number.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_irq_disable(uint32_t irq)
{
	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq))
		return -EINVAL;

	NVIC_DisableIRQ((IRQn_Type)irq);
	irq_table[irq].enabled = false;

	return 0;
}

/**
 * @brief Connect an ISR to an IRQ.
 * @param irq - IRQ number.
 * @param isr - ISR callback function.
 * @param arg - Argument passed to ISR.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_irq_connect(uint32_t irq, capi_isr_callback_t isr,
				  void *arg)
{
	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq))
		return -EINVAL;

	if (!isr)
		return -EINVAL;

	/* Store callback info */
	irq_table[irq].callback = isr;
	irq_table[irq].arg = arg;

	return 0;
}

/**
 * @brief Disconnect an ISR from an IRQ.
 * @param irq - IRQ number.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_irq_disconnect(uint32_t irq)
{
	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq))
		return -EINVAL;

	/* Disable IRQ before disconnecting */
	if (irq_table[irq].enabled) {
		NVIC_DisableIRQ((IRQn_Type)irq);
		irq_table[irq].enabled = false;
	}

	/* Clear callback info */
	irq_table[irq].callback = NULL;
	irq_table[irq].arg = NULL;

	return 0;
}

/**
 * @brief Clear pending interrupt flag.
 * @param irq - IRQ number.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_irq_clear_pending(uint32_t irq)
{
	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq))
		return -EINVAL;

	NVIC_ClearPendingIRQ((IRQn_Type)irq);

	return 0;
}

/**
 * @brief Set pending interrupt flag.
 * @param irq - IRQ number.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_irq_set_pending(uint32_t irq)
{
	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq))
		return -EINVAL;

	NVIC_SetPendingIRQ((IRQn_Type)irq);

	return 0;
}

/**
 * @brief Get IRQ status (active/pending).
 * @param irq - IRQ number.
 * @param pactive - Pointer to store status (1=active/pending, 0=inactive).
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq) || !pactive)
		return -EINVAL;

	/* Check if IRQ is active or pending */
	*pactive = NVIC_GetPendingIRQ((IRQn_Type)irq) ||
		   NVIC_GetActive((IRQn_Type)irq);

	return 0;
}

/**
 * @brief Set IRQ priority.
 * @param irq - IRQ number.
 * @param priority - Priority value (used as preemption priority).
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	uint32_t preempt_priority;
	uint32_t sub_priority;

	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq))
		return -EINVAL;

	/*
	 * Priority encoding: upper bits = preemption priority,
	 * lower bits = sub-priority. The split depends on priority grouping.
	 * For simplicity, we use priority as preemption priority with 0 sub.
	 */
	preempt_priority = priority;
	sub_priority = 0;

	HAL_NVIC_SetPriority((IRQn_Type)irq, preempt_priority, sub_priority);

	return 0;
}

/**
 * @brief Get IRQ priority.
 * @param irq - IRQ number.
 * @param priority - Pointer to store priority value.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	uint32_t preempt_priority;
	uint32_t sub_priority;

	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq) || !priority)
		return -EINVAL;

	HAL_NVIC_GetPriority((IRQn_Type)irq, current_priority_grouping,
			     &preempt_priority, &sub_priority);

	/* Return preemption priority as the main priority value */
	*priority = preempt_priority;

	return 0;
}

/**
 * @brief Set IRQ trigger level/edge for EXTI lines.
 * @param irq - IRQ number (must be an EXTI IRQ).
 * @param trigger - Trigger type.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_irq_set_level_edge_trigger(uint32_t irq,
		enum capi_irq_trig_level trigger)
{
	uint16_t pin;
	uint32_t line_mask;

	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq))
		return -EINVAL;

	/* Check if this is an EXTI GPIO interrupt */
	if (!is_exti_gpio_irq(irq, &pin))
		return -ENOTSUP;

	/* Map CAPI trigger level to STM32 GPIO EXTI mode */
	switch (trigger) {
	case CAPI_IRQ_EDGE_RISING:
	case CAPI_IRQ_EDGE_FALLING:
	case CAPI_IRQ_EDGE_BOTH:
		break;
	case CAPI_IRQ_LEVEL_LOW:
	case CAPI_IRQ_LEVEL_HIGH:
		/* Level-triggered interrupts not supported on STM32 EXTI */
		return -ENOTSUP;
	default:
		return -EINVAL;
	}

	line_mask = (1UL << pin);

	/* Configure EXTI trigger through EXTI registers directly */
#if defined(STM32H7) || defined(STM32L5) || defined(STM32U5) || defined(STM32H5)
	/* New EXTI architecture with separate registers */
	EXTI->RTSR1 &= ~line_mask;
	EXTI->FTSR1 &= ~line_mask;

	if (trigger == CAPI_IRQ_EDGE_RISING || trigger == CAPI_IRQ_EDGE_BOTH)
		EXTI->RTSR1 |= line_mask;

	if (trigger == CAPI_IRQ_EDGE_FALLING || trigger == CAPI_IRQ_EDGE_BOTH)
		EXTI->FTSR1 |= line_mask;
#else
	/* Legacy EXTI architecture */
	EXTI->RTSR &= ~line_mask;
	EXTI->FTSR &= ~line_mask;

	if (trigger == CAPI_IRQ_EDGE_RISING || trigger == CAPI_IRQ_EDGE_BOTH)
		EXTI->RTSR |= line_mask;

	if (trigger == CAPI_IRQ_EDGE_FALLING || trigger == CAPI_IRQ_EDGE_BOTH)
		EXTI->FTSR |= line_mask;
#endif

	return 0;
}

/**
 * @brief Check if IRQ is enabled.
 * @param irq - IRQ number.
 * @param enabled - Pointer to store enabled status.
 * @return 0 on success, negative error code otherwise.
 */
int stm32_capi_irq_is_enabled(uint32_t irq, bool *enabled)
{
	if (!irq_initialized)
		return -ENODEV;

	if (!is_valid_irq(irq) || !enabled)
		return -EINVAL;

	*enabled = NVIC_GetEnableIRQ((IRQn_Type)irq) != 0;

	return 0;
}

/**
 * @brief Generic IRQ handler that dispatches to registered callbacks.
 * @param irq - IRQ number.
 */
void stm32_capi_irq_handler(uint32_t irq)
{
	const struct stm32_capi_irq_entry *entry;

	if (!is_valid_irq(irq))
		return;

	entry = &irq_table[irq];

	if (entry->callback)
		entry->callback(entry->arg);
}

/**
 * @brief EXTI callback handler for GPIO interrupts.
 * @param gpio_pin - GPIO pin number (0-15).
 */
void stm32_capi_exti_handler(uint16_t gpio_pin)
{
	IRQn_Type irq;

	if (gpio_pin >= STM32_CAPI_EXTI_MAX_LINES)
		return;

	irq = exti_irq_map[gpio_pin];
	stm32_capi_irq_handler((uint32_t)irq);
}

/**
 * @brief HAL GPIO EXTI Callback (weak override).
 * @param GPIO_Pin - GPIO pin bitmask that triggered the interrupt.
 *
 * This overrides the weak HAL_GPIO_EXTI_Callback to dispatch to
 * registered CAPI callbacks.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint16_t pin = 0;

	/* Find which pin triggered (GPIO_Pin is a bitmask) */
	while (GPIO_Pin > 1) {
		GPIO_Pin >>= 1;
		pin++;
	}

	stm32_capi_exti_handler(pin);
}

/*
 * CAPI IRQ interface implementation.
 * These functions are called directly by application code via the
 * capi_irq.h API. The capi_irq.c wrapper is only used for stub builds.
 */

/**
 * @brief Initialize the IRQ controller (CAPI interface).
 * @param config - Pointer to the IRQ configuration.
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_init(struct capi_irq_config *config)
{
	return stm32_capi_irq_init(config);
}

/**
 * @brief Deinitialize the IRQ controller (CAPI interface).
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_deinit(void)
{
	return stm32_capi_irq_deinit();
}

/**
 * @brief Enable global interrupts (CAPI interface).
 * @return 0 on success.
 */
int capi_irq_global_enable(void)
{
	return stm32_capi_irq_global_enable();
}

/**
 * @brief Disable global interrupts (CAPI interface).
 * @return 0 on success.
 */
int capi_irq_global_disable(void)
{
	return stm32_capi_irq_global_disable();
}

/**
 * @brief Enable a specific IRQ (CAPI interface).
 * @param irq - IRQ number.
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_enable(uint32_t irq)
{
	return stm32_capi_irq_enable(irq);
}

/**
 * @brief Disable a specific IRQ (CAPI interface).
 * @param irq - IRQ number.
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_disable(uint32_t irq)
{
	return stm32_capi_irq_disable(irq);
}

/**
 * @brief Connect an ISR to an IRQ (CAPI interface).
 * @param irq - IRQ number.
 * @param isr - ISR callback function.
 * @param arg - Argument passed to ISR.
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg)
{
	return stm32_capi_irq_connect(irq, isr, arg);
}

/**
 * @brief Clear pending interrupt flag (CAPI interface).
 * @param irq - IRQ number.
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_clear_pending(uint32_t irq)
{
	return stm32_capi_irq_clear_pending(irq);
}

/**
 * @brief Get IRQ status (CAPI interface).
 * @param irq - IRQ number.
 * @param pactive - Pointer to store status.
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	return stm32_capi_irq_get_status(irq, pactive);
}

/**
 * @brief Set IRQ priority (CAPI interface).
 * @param irq - IRQ number.
 * @param priority - Priority value.
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	return stm32_capi_irq_set_priority(irq, priority);
}

/**
 * @brief Get IRQ priority (CAPI interface).
 * @param irq - IRQ number.
 * @param priority - Pointer to store priority value.
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	return stm32_capi_irq_get_priority(irq, priority);
}

/**
 * @brief Set IRQ trigger level/edge (CAPI interface).
 * @param irq - IRQ number.
 * @param trigger - Trigger type.
 * @return 0 on success, negative error code otherwise.
 */
int capi_irq_set_level_edge_trigger(uint32_t irq,
				    enum capi_irq_trig_level trigger)
{
	return stm32_capi_irq_set_level_edge_trigger(irq, trigger);
}
