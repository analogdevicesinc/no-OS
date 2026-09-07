/*
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_capi_time.c
 * @brief Maxim (MSDK) time backend: overrides the weak capi_time hooks.
 *
 * Without this file, capi_wait_us_impl() is the weak no-op defined in
 * capi_time.c, so every capi_wait_us() returns immediately. That silently
 * breaks any caller that measures elapsed time by summing its own step delays
 * rather than reading a clock -- notably the test framework's TEST_WAIT_UNTIL,
 * whose "2 second" budget then expires in a few milliseconds of loop overhead
 * and abandons an async transfer that had barely started. MXC_Delay() is MSDK's
 * blocking microsecond busy-wait, so the delay gets real wall-clock duration.
 *
 * capi_uptime_impl() is intentionally left as the weak -ENOSYS default: MSDK
 * exposes no free-running microsecond counter that is safe to assume is already
 * running in this build, and no CAPI wait path needs it (TEST_WAIT_UNTIL paces
 * on wait_us, not uptime).
 */

#include <stdint.h>
#include "mxc_delay.h"
#include "capi_time.h"

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
