/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common HAL memory-barrier primitives.
 */

#ifndef _CAPI_BARRIER_H_
#define _CAPI_BARRIER_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

static inline void capi_barrier_full(void)
{
#if defined(__arm__) || defined(__thumb__)
	__asm__ volatile("dsb sy" ::: "memory");
#elif defined(__riscv)
	__asm__ volatile("fence iorw, iorw" ::: "memory");
#else
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
#endif
}

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_BARRIER_H_ */
