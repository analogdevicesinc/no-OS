/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common API time and delay services
 *
 * Singleton system services for blocking delays and monotonic uptime. There is
 * no device instance to manage, so (like capi_alloc / capi_log) these are plain
 * functions that forward to a weak *_impl symbol. The platform provides a strong
 * implementation at link time; the weak defaults keep the API linkable on
 * platforms that have not yet wired up a time source.
 */

#ifndef _CAPI_TIME_H_
#define _CAPI_TIME_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Busy-wait for at least the given number of microseconds.
 *
 * Blocks the calling CPU in a tight loop. Intended for short, precise delays
 * (e.g. device timing requirements) where giving up the CPU is undesirable or
 * the requested delay is shorter than one scheduler/SysTick period. For longer,
 * coarse delays prefer capi_wait_ms(), which may yield.
 *
 * @param [in] us Minimum number of microseconds to wait.
 */
void capi_wait_us(uint32_t us);

/**
 * @brief Delay for at least the given number of milliseconds.
 *
 * Coarse delay. On an RTOS this may sleep the calling thread and yield the CPU
 * to other work; on bare metal it typically falls back to a tick-based busy
 * wait. Use capi_wait_us() when you need sub-millisecond precision or must not
 * yield.
 *
 * @param [in] ms Minimum number of milliseconds to wait.
 */
void capi_wait_ms(uint32_t ms);

/**
 * @brief Get the monotonic time elapsed since boot, in microseconds.
 *
 * The returned value increases monotonically and is intended for measuring
 * elapsed time by subtracting two readings. It is not wall-clock time.
 *
 * @param [out] us Receives the microseconds since boot. Must not be NULL.
 *
 * @return 0 on success, -EINVAL if @p us is NULL, -ENOSYS if the platform
 *         provides no time source.
 */
int capi_uptime(uint64_t *us);

/*
 * Weak hook symbols overridden by the platform/app (don't call directly).
 *
 * A platform that implements only capi_wait_us_impl() gets a working
 * capi_wait_ms() for free: the default capi_wait_ms_impl() delegates to it.
 */
void capi_wait_us_impl(uint32_t us);
void capi_wait_ms_impl(uint32_t ms);
int capi_uptime_impl(uint64_t *us);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CAPI_TIME_H_ */
