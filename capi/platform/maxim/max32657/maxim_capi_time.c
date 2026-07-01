/*******************************************************************************
 *   @file   maxim_capi_time.c
 *   @brief  Implementation of Time functions with CAPI
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "capi_time.h"
#include "mxc_delay.h"
#include "mxc_sys.h"

/** Static variables **********************************************************/

static volatile unsigned long long _system_ticks = 0;

/** Function implementations **************************************************/

/**
 * @brief Delay for at least the given number of microseconds
 * @param us Minimum number of microseconds to wait
 */
void capi_wait_us_impl(uint32_t us)
{
	MXC_Delay(MXC_DELAY_USEC(us));
}

/**
 * @brief Delay for at least the given number of milliseconds
 * @param ms Minimum number of milliseconds to wait
 */
void capi_wait_ms_impl(uint32_t ms)
{
	MXC_Delay(MXC_DELAY_MSEC(ms));
}

/**
 * @brief Get the uptime in microseconds since boot
 * @param[out] us Uptime in microseconds
 */
int capi_uptime_impl(uint64_t *us)
{
	uint64_t ticks, sub_ms;
	uint32_t systick_val;

	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
	systick_val = SysTick->VAL;
	ticks = _system_ticks;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	sub_ms = ((SysTick->LOAD - systick_val) * 1000) / SysTick->LOAD;

	*us = ticks * 1000 + sub_ms;

	return 0;
}

/** Platform-specific functions ***********************************************/

extern void SysTick_Handler(void);

void SysTick_Handler(void)
{
	MXC_DelayHandler();
	_system_ticks++;
}
