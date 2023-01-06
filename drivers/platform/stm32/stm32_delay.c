/***************************************************************************//**
 *   @file   stm32/stm32_delay.c
 *   @brief  Implementation of stm32 delay functions.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
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
