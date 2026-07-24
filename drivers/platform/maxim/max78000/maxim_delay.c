/*******************************************************************************
 *   @file   maxim_delay.c
 *   @brief  Implementation of maxim delay functions.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "no_os_delay.h"
#include "no_os_util.h"
#include "mxc_delay.h"
#include "mxc_sys.h"

static volatile unsigned long long _system_ticks = 0;

#ifndef __riscv
extern void SysTick_Handler(void);

/* ************************************************************************** */
void SysTick_Handler(void)
{
	MXC_DelayHandler();
	_system_ticks++;
}
#endif /* __riscv */

/**
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 */
void no_os_udelay(uint32_t usecs)
{
#ifdef __riscv
	while (usecs--) {
		for (uint32_t i = 0; i < 120U; i++)
			__asm__ volatile("nop");
	}
#else
	MXC_Delay(MXC_DELAY_USEC(usecs));
#endif
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 */
void no_os_mdelay(uint32_t msecs)
{
#ifdef __riscv
	while (msecs--)
		no_os_udelay(1000U);
#else
	MXC_Delay(MXC_DELAY_MSEC(msecs));
#endif
}

/**
 * @brief Get current time.
 * @return Current time structure from system start (seconds, microseconds).
 */
struct no_os_time no_os_get_time(void)
{
	struct no_os_time t;
#ifdef __riscv
	/*
	 * The RISC-V (CPU1) core has no SysTick peripheral (it is Cortex-M only),
	 * so wall-clock time is not available here. Return zero; the coprocessor
	 * firmware relies on no_os_udelay/no_os_mdelay (backed by MXC_Delay's
	 * RISC-V cycle-counter path), not on no_os_get_time.
	 */
	t.s = 0;
	t.us = 0;
#else
	uint64_t sub_ms;
	uint32_t systick_val;
	uint64_t ticks;

	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
	systick_val = SysTick->VAL;
	ticks = _system_ticks;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	sub_ms = ((SysTick->LOAD - systick_val) * 1000) / SysTick->LOAD;
	t.s = ticks / 1000;
	t.us = (ticks - t.s * 1000) * 1000 + sub_ms;
#endif /* __riscv */

	return t;
}
