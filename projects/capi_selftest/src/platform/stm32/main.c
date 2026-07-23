/***************************************************************************//**
 * @file main.c
 * @brief Main file for STM32 platform of capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#include "parameters.h"
#include "common_data.h"
#include "stm32_hal.h"

extern int example_main(void);

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
