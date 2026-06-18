/*
 * Copyright (c) 2025 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_time.c
 * @brief Strong wrappers over the weak time/delay hooks.
 *
 * The wrappers do the API-level argument checking and forward to the weak
 * *_impl symbols, which the platform overrides at link time. The weak defaults
 * keep the API linkable and give sensible behaviour even on a partially-ported
 * platform: capi_wait_ms_impl() is built on top of capi_wait_us_impl(), so a
 * platform only needs to provide a microsecond busy-wait to get both delays.
 */

#include "capi_time.h"

#include <errno.h>
#include <stddef.h>

/* --- Weak default implementations (overridden by the platform) --- */

/* No time source by default: do nothing rather than guess at a delay. */
__attribute__((weak)) void capi_wait_us_impl(uint32_t us)
{
	(void)us;
}

/*
 * Default millisecond delay built from the microsecond busy-wait.
 */
__attribute__((weak)) void capi_wait_ms_impl(uint32_t ms)
{
	while (ms-- > 0) {
		capi_wait_us_impl(1000);
	}
}

/* No time source by default. */
__attribute__((weak)) int capi_uptime_impl(uint64_t *us)
{
	(void)us;
	return -ENOSYS;
}

/* --- Strong public wrappers --- */

void capi_wait_us(uint32_t us)
{
	capi_wait_us_impl(us);
}

void capi_wait_ms(uint32_t ms)
{
	capi_wait_ms_impl(ms);
}

int capi_uptime(uint64_t *us)
{
	if (us == NULL) {
		return -EINVAL;
	}

	return capi_uptime_impl(us);
}
