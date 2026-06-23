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
 * The millisecond portion comes from HAL_GetTick() (SysTick).  On cores with
 * the DWT cycle counter the sub-millisecond fraction is derived from CYCCNT
 * for microsecond-level resolution; otherwise the fractional part is zero.
 *
 * @param[out] us Receives uptime in microseconds.
 * @return 0 on success.
 */
int capi_uptime_impl(uint64_t *us)
{
	uint32_t ms = HAL_GetTick();
	uint64_t result = (uint64_t)ms * 1000U;

#if defined(DWT)
	{
		uint32_t cycles_per_ms = SystemCoreClock / 1000U;
		uint32_t cyccnt = DWT->CYCCNT;
		uint32_t frac_us = (cyccnt % cycles_per_ms) / (SystemCoreClock / 1000000U);

		result += frac_us;
	}
#endif

	*us = result;

	return 0;
}
