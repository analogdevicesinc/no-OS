/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <stdint.h>
#include "stm32_hal.h"
#include "capi_time.h"

/**
 * @brief Busy-wait for at least the given number of microseconds.
 *
 * On cores with a DWT cycle counter (Cortex-M3/M4/M7) this uses the hardware
 * cycle counter for precise sub-microsecond resolution.  On cores without DWT
 * it falls back to the coarsest available HAL delay (1 ms).
 *
 * @param us Minimum number of microseconds to wait.
 */
#if defined(DWT)
#pragma GCC push_options
#pragma GCC optimize ("O3")
void capi_wait_us_impl(uint32_t us)
{
	static bool dwt_initialised = false;
	volatile uint32_t cycles = (SystemCoreClock / 1000000U) * us;
	volatile uint32_t start;

	if (!dwt_initialised) {
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
#ifdef STM32F7
		DWT->LAR = 0xC5ACCE55;
#endif
		DWT->CTRL |= 1;
		dwt_initialised = true;
	}

	start = DWT->CYCCNT;
	while (DWT->CYCCNT - start < cycles)
		;
}
#pragma GCC pop_options
#else
void capi_wait_us_impl(uint32_t us)
{
	if (us)
		HAL_Delay((us - 1) / 1000 + 1);
}
#endif

/**
 * @brief Delay for at least the given number of milliseconds.
 *
 * Uses the SysTick-based HAL_Delay which is accurate to +/- 1 ms.
 *
 * @param ms Minimum number of milliseconds to wait.
 */
void capi_wait_ms_impl(uint32_t ms)
{
	HAL_Delay(ms);
}

/**
 * @brief Return monotonic uptime in microseconds since boot.
 *
 * The millisecond portion comes from HAL_GetTick() (SysTick).  The sub-
 * millisecond fraction is derived from the SysTick current-value register,
 * which is the very counter that drives the millisecond tick, so the two are
 * phase-locked: (LOAD - VAL) is exactly the number of core cycles elapsed
 * within the current millisecond.  DWT->CYCCNT is deliberately NOT used - it
 * free-runs independently of SysTick, so mixing it with HAL_GetTick() yields a
 * random 0..999 us offset (up to +/-1 ms of error) instead of the true
 * fraction.  SysTick exists on every Cortex-M and is the default HAL time
 * base, so this stays generic across STM32 families.
 *
 * @param[out] us Receives uptime in microseconds.
 * @return 0 on success.
 */
int capi_uptime_impl(uint64_t *us)
{
	uint32_t load = SysTick->LOAD + 1U;		/* core cycles per ms */
	uint32_t cyc_per_us = SystemCoreClock / 1000000U;
	uint32_t ms;
	uint32_t val;
	uint32_t frac_us = 0U;

	/*
	 * Sample the millisecond count and the SysTick value as a consistent
	 * pair. SysTick counts DOWN and reloads on the same event that advances
	 * HAL_GetTick(); if a tick lands between the two reads, HAL_GetTick()
	 * changes and we retry, so val always belongs to the sampled ms.
	 */
	do {
		ms = HAL_GetTick();
		val = SysTick->VAL;
	} while (ms != HAL_GetTick());

	if (cyc_per_us) {
		frac_us = (load - val) / cyc_per_us;
		if (frac_us > 999U)
			frac_us = 999U;
	}

	*us = (uint64_t)ms * 1000ULL + frac_us;

	return 0;
}
