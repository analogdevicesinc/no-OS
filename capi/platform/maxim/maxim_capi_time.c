/*
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_capi_time.c
 * @brief Maxim (MSDK) time backend: overrides the weak capi_time hooks.
 *
 * capi_wait_us/ms_impl() forward to MXC_Delay so delays get real wall-clock
 * duration (the weak defaults are no-ops).
 *
 * capi_uptime_impl() reads a 1 kHz SysTick time base owned here: SysTick_Handler
 * increments _system_ticks and drives MXC_DelayHandler so blocking MXC_Delay
 * still works with SysTick interrupts enabled. The FTHR BSP owns Board_Init and
 * does not start SysTick, so a constructor starts the tick before main().
 * CONFIG_CAPI_TIME drops the legacy maxim_delay.c (see drivers/platform/maxim/
 * CMakeLists.txt) so its SysTick_Handler does not collide with this one.
 */

#include <errno.h>
#include <stdint.h>
#include "mxc_delay.h"
#include "mxc_device.h"
#include "capi_time.h"

static volatile uint64_t _system_ticks;

void SysTick_Handler(void)
{
	MXC_DelayHandler();
	_system_ticks++;
}

__attribute__((constructor)) static void _max_capi_time_start_tick(void)
{
	SysTick_Config(SystemCoreClock / 1000U);
}

void capi_wait_us_impl(uint32_t us)
{
	if (us)
		MXC_Delay(MXC_DELAY_USEC(us));
}

void capi_wait_ms_impl(uint32_t ms)
{
	if (ms)
		MXC_Delay(MXC_DELAY_MSEC(ms));
}

int capi_uptime_impl(uint64_t *us)
{
	uint64_t reload = (uint64_t)SysTick->LOAD + 1ULL;
	uint64_t ticks_start, ticks_end;
	uint32_t systick_val;

	if (!us)
		return -EINVAL;

	/* Lock-free double-read: reject a sample where the tick rolled over
	 * between reading the down-counter and the tick count. */
	do {
		ticks_start = _system_ticks;
		systick_val = SysTick->VAL;
		ticks_end = _system_ticks;
	} while (ticks_start != ticks_end);

	if (systick_val >= reload)
		systick_val = (uint32_t)(reload - 1ULL);

	*us = (ticks_start * 1000ULL) +
	      ((((reload - 1ULL) - systick_val) * 1000ULL) / reload);

	return 0;
}
