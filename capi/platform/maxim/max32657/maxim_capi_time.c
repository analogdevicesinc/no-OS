/*******************************************************************************
 *   @file   maxim_capi_time.c
 *   @brief  Implementation of Time functions with CAPI
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
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
	uint64_t ticks_start, ticks_end, sub_us;
	uint32_t systick_val;
	uint64_t reload;

	if (!us)
		return -EINVAL;

	reload = (uint64_t)SysTick->LOAD + 1ULL;

	do {
		ticks_start = _system_ticks;
		systick_val = SysTick->VAL;
		ticks_end = _system_ticks;
	} while (ticks_start != ticks_end);

	if (systick_val >= reload)
		systick_val = (uint32_t)(reload - 1ULL);

	sub_us = (((reload - 1ULL) - systick_val) * 1000ULL) / reload;

	*us = (ticks_start * 1000ULL) + sub_us;

	return 0;
}

/** Platform-specific functions ***********************************************/

extern void SysTick_Handler(void);

void SysTick_Handler(void)
{
	MXC_DelayHandler();
	_system_ticks++;
}
