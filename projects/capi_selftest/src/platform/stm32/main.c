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
 * The suite drives the loopback output pin and observes the edge arrive on the
 * wired input pin through the CAPI IRQ contract. Everything the test touches
 * goes through capi_irq_*; these hooks supply only the part CAPI cannot
 * express portably: routing the input pin to an EXTI line, telling the test
 * which CAPI IRQ number to connect, and clearing the pin as the source.
 *
 * The pin, port, EXTI line, NVIC number, port clock and vector name all come
 * from the board mapping in parameters.h - nothing below is board-specific.
 * They were once redefined here, which silently overrode the board mapping and
 * armed the wrong pin on any board that was not the Nucleo. The test drives a
 * low->high transition, hence a rising-edge trigger.
 */

/**
 * @brief Route the loopback input pin to its EXTI line.
 * @param irq_line - Out: CAPI IRQ number to connect/enable.
 * @return 0 on success, -EINVAL on a NULL argument.
 *
 * Configures the input pin for a rising-edge external interrupt and hands back
 * the CAPI IRQ line. It does NOT enable the NVIC line -- capi_irq_enable() owns
 * that, so arming the pin and enabling the CAPI line stay independent gates
 * (the test relies on that separation).
 */
int platform_gpio_irq_arm(uint32_t *irq_line)
{
	GPIO_InitTypeDef init = { 0 };

	if (!irq_line)
		return -EINVAL;

	/*
	 * EXTI line routing lives in SYSCFG; the pin needs its own port clock.
	 * The port clock is not optional even when the CAPI GPIO suite has
	 * already opened that port: CubeMX only clocks ports its .ioc maps, and
	 * an unclocked port leaves the pin's input path dead while SYSCFG and
	 * EXTI still program cleanly - the line looks armed and no edge arrives.
	 */
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	GPIO_IRQ_CLK_ENABLE();

	/*
	 * GPIO_MODE_IT_RISING wires the SYSCFG EXTICR entry for this line to the
	 * pin's port and sets the EXTI rising trigger + unmask in one call. The
	 * CAPI GPIO input port was opened as a plain input beforehand; re-initing
	 * the pin here only adds the interrupt configuration.
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
 * @brief Clear the loopback input pin as the interrupt source.
 * @return true if the pin's EXTI line was pending and was cleared.
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
 * @brief Mask the loopback input pin's EXTI line again.
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
 * @brief EXTI vector for the loopback input pin. Overrides the weak default.
 *
 * Dispatches through the CAPI IRQ layer WITHOUT clearing the EXTI pending bit:
 * the connected CAPI callback calls platform_gpio_irq_ack(), which inspects and
 * then clears it. stm32_capi_exti_handler() maps the line to its NVIC number
 * and then to the registered CAPI callback, so the line passed here must be the
 * one the test connected on - both come from GPIO_IRQ_* in parameters.h.
 * (Using HAL_GPIO_EXTI_IRQHandler() here would clear the flag first and defeat
 * that check.)
 *
 * The vector name is board-dependent because lines 5..9 and 10..15 share one
 * vector each, so it comes from the board mapping rather than being spelled
 * out here.
 */
void GPIO_IRQ_EXTI_HANDLER(void)
{
	stm32_capi_exti_handler(GPIO_IRQ_LINE);
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
