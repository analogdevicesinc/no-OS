/***************************************************************************//**
 * @file main.c
 * @brief Main file for STM32 platform of capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include <errno.h>
#include "parameters.h"
#include "common_data.h"
#include "stm32_hal.h"

extern int example_main(void);

#if defined(IRQ_CTRL_IDENTIFIER) && defined(GPIO_OUTPUT_OPS)
/*
 * GPIO-interrupt platform hooks for the capi_loopback IRQ test.
 *
 * The suite drives the loopback output pin (PE0) and observes the edge arrive
 * on the wired input pin (PC0) through the CAPI IRQ contract. Everything the
 * test touches goes through capi_irq_*; these hooks supply only the part CAPI
 * cannot express portably: routing PC0 to an EXTI line, telling the test which
 * CAPI IRQ number to connect, and clearing the pin as the source.
 *
 * PC0 is EXTI line 0, so the CAPI IRQ number is EXTI0_IRQn. The test drives a
 * low->high transition, hence a rising-edge trigger.
 */
#define GPIO_IRQ_PIN		GPIO_PIN_0	/* PC0 -> EXTI line 0 */
#define GPIO_IRQ_PORT		GPIOC
#define GPIO_IRQ_IRQN		EXTI0_IRQn

/**
 * @brief Route the loopback input pin (PC0) to its EXTI line.
 * @param irq_line - Out: CAPI IRQ number to connect/enable (EXTI0_IRQn).
 * @return 0 on success, -EINVAL on a NULL argument.
 *
 * Configures PC0 for a rising-edge external interrupt and hands back the CAPI
 * IRQ line. It does NOT enable the NVIC line -- capi_irq_enable() owns that, so
 * arming the pin and enabling the CAPI line stay independent gates (the test
 * relies on that separation).
 */
int platform_gpio_irq_arm(uint32_t *irq_line)
{
	GPIO_InitTypeDef init = { 0 };

	if (!irq_line)
		return -EINVAL;

	/* EXTI line routing lives in SYSCFG; the pin lives on GPIOC. */
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*
	 * GPIO_MODE_IT_RISING wires SYSCFG EXTICR line 0 to port C and sets the
	 * EXTI rising trigger + unmask in one call. The CAPI GPIO input port was
	 * opened as a plain input beforehand; re-initing the pin here only adds
	 * the interrupt configuration.
	 */
	init.Pin = GPIO_IRQ_PIN;
	init.Mode = GPIO_MODE_IT_RISING;
	init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIO_IRQ_PORT, &init);

	/* Drop any edge latched during setup so the first real edge is clean. */
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_IRQ_PIN);
	HAL_NVIC_ClearPendingIRQ(GPIO_IRQ_IRQN);

	*irq_line = (uint32_t)GPIO_IRQ_IRQN;

	return 0;
}

/**
 * @brief Clear the loopback input pin (PC0) as the interrupt source.
 * @return true if PC0's EXTI line was pending and was cleared.
 *
 * Read-and-clear of the EXTI pending bit. The ISR dispatches without touching
 * the pending flag, so this -- called from the CAPI callback -- is what both
 * proves the pin was the source and stops it re-asserting.
 */
bool platform_gpio_irq_ack(void)
{
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_IRQ_PIN) != 0U) {
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_IRQ_PIN);
		return true;
	}

	return false;
}

/**
 * @brief Mask the loopback input pin's (PC0) EXTI line again.
 *
 * Masks the line and drops any latched edge without disturbing the CAPI-owned
 * pin configuration.
 */
void platform_gpio_irq_disarm(void)
{
	EXTI->IMR &= ~GPIO_IRQ_PIN;
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_IRQ_PIN);
	HAL_NVIC_ClearPendingIRQ(GPIO_IRQ_IRQN);
}

/**
 * @brief EXTI line 0 vector (PC0). Overrides the weak startup default.
 *
 * Dispatches through the CAPI IRQ layer WITHOUT clearing the EXTI pending bit:
 * the connected CAPI callback calls platform_gpio_irq_ack(), which inspects and
 * then clears it. stm32_capi_exti_handler() maps line 0 -> EXTI0_IRQn -> the
 * registered CAPI callback. (Using HAL_GPIO_EXTI_IRQHandler() here would clear
 * the flag first and defeat that check.)
 */
void EXTI0_IRQHandler(void)
{
	stm32_capi_exti_handler(0U);
}
#endif /* IRQ_CTRL_IDENTIFIER && GPIO_OUTPUT_OPS */

/**
 * @brief Main function execution for STM32 platform.
 * @return Result of the enabled example execution.
 */
int main(void)
{
	stm32_init();

#if SPI_HAS_IRQ || TIMER_HAS_IRQ
	if (capi_irq_init(&irq_config) == 0)
		(void)capi_irq_global_enable();
#endif

	return example_main();
}
