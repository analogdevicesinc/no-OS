/***************************************************************************//**
 *   @file   stm32/stm32_delay.c
 *   @brief  Implementation of stm32 delay functions.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdbool.h>
#include "stm32_hal.h"
#include "no_os_delay.h"
/**
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 * @return None.
 */
#if defined(DWT)
#pragma GCC push_options
#pragma GCC optimize ("O3")
void no_os_udelay(uint32_t usecs)
{
	static bool firstrun = true;
	volatile uint32_t cycles = (SystemCoreClock / 1000000L) * usecs;
	if (firstrun) {
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
#ifdef STM32F7
		DWT->LAR = 0xC5ACCE55;
#endif
		DWT->CTRL |= 1;
		firstrun = false;
	}
	volatile uint32_t start = DWT->CYCCNT;
	while(DWT->CYCCNT - start < cycles);
}
#pragma GCC pop_options
#else
void no_os_udelay(uint32_t usecs)
{
	/* Fallback to lowest possible HAL delay of 1ms. */
	HAL_Delay(1);
}
#endif

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void no_os_mdelay(uint32_t msecs)
{
	HAL_Delay(msecs);
}

/**
 * @brief Get current time.
 * @return Current time structure from system start (seconds, microseconds).
 */
struct no_os_time no_os_get_time(void)
{
	unsigned long long Xtime_Global;
	float fractional_part = 0;
	struct no_os_time t;

	Xtime_Global = HAL_GetTick();
	t.s = Xtime_Global / 1000; // milliseconds

	fractional_part = (float)Xtime_Global / 1000 - Xtime_Global / 1000;
	t.us = fractional_part * 1000;
	t.us *= 1000;

	return t;
}
