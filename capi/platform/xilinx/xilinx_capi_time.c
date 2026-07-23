/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx strong implementations of capi_time weak hooks.
 *
 * These override the __attribute__((weak)) stubs in capi/src/capi_time.c at
 * link time.
 *
 * capi_wait_us_impl / capi_wait_ms_impl:
 *   Cortex-A9/A53/R5 targets expose usleep() from xil_sleep.h (BSP sleep.h).
 *   MicroBlaze uses usleep_MB().  _XPARAMETERS_PS_H_ distinguishes the two.
 *
 * capi_uptime_impl:
 *   Reads the 64-bit Cortex-A9 global timer via XTime_GetTime() from xtime_l.h.
 *   COUNTS_PER_SECOND is CPU_FREQ/2 on Zynq-7000 (667 MHz → 333 MHz counter).
 *   Division is integer: precision is ~3 µs at 333 MHz, fine for test/benchmark.
 *   Only compiled on PS targets (_XPARAMETERS_PS_H_).
 */

#include "capi_time.h"
#include "sleep.h"

/*
 * SDT BSP (Vitis 2025.2+): xiltimer.h replaces xtime_l.h. The compiler is
 * invoked with -DSDT for SDT builds. Legacy BSP: xtime_l.h on PS targets.
 */
#include "xiltimer.h"
#include "xtimer_config.h"

void capi_wait_us_impl(uint32_t us)
{
#ifdef _XPARAMETERS_PS_H_
	usleep(us);
#else
	usleep_MB(us);
#endif
}

void capi_wait_ms_impl(uint32_t ms)
{
#ifdef _XPARAMETERS_PS_H_
	usleep((uint32_t)(ms * 1000U));
#else
	usleep_MB((uint32_t)(ms * 1000U));
#endif
}

int capi_uptime_impl(uint64_t *us)
{
	XTime ticks;

	XTime_GetTime(&ticks);
	*us = (uint64_t)ticks * 1000000ULL / (uint64_t)(COUNTS_PER_SECOND);
	return 0;
}
