/*******************************************************************************
 *   @file   maxim_delay.c
 *   @brief  Implementation of maxim delay functions.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "no_os_delay.h"
#include "no_os_util.h"
#include "mxc_delay.h"
#include "mxc_sys.h"

static volatile unsigned long long _system_ticks = 0;

extern void SysTick_Handler(void);

/* ************************************************************************** */
void SysTick_Handler(void)
{
	MXC_DelayHandler();
	_system_ticks++;
}

/**
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 * @return None.
 */
void no_os_udelay(uint32_t usecs)
{
	MXC_Delay(MXC_DELAY_USEC(usecs));
}

/**
 * @brief Generate milliseconds delay.
 * @param msecs - Delay in milliseconds.
 * @return None.
 */
void no_os_mdelay(uint32_t msecs)
{
	MXC_Delay(MXC_DELAY_MSEC(msecs));
}

/**
 * @brief Get current time.
 * @return Current time structure from system start (seconds, microseconds).
 */
struct no_os_time no_os_get_time(void)
{
	struct no_os_time t;
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

	return t;
}
