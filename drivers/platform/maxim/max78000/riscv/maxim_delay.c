/***************************************************************************//**
 *   @file   maxim_delay.c
 *   @brief  MAX78000 RISC-V (CPU1) delay and time functions.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <stdint.h>
#include "no_os_delay.h"
#include "maxim_core.h"
#include "maxim_time.h"
#include "mxc_delay.h"
#include "mxc_sys.h"
#include "tmr.h"

#define MAXIM_TIME_TIMER MXC_TMR_GET_TMR(MAXIM_RISCV_TIME_TIMER)
#define MAXIM_TIME_FREQ (ISO_FREQ / 64U)
#define MAXIM_TIME_HANDLER_(id) TMR ## id ## _IRQHandler
#define MAXIM_TIME_HANDLER(id) MAXIM_TIME_HANDLER_(id)

static uint64_t elapsed_ticks;
static bool initialized;

int maxim_time_init(void)
{
	mxc_tmr_cfg_t config = {
		.pres = TMR_PRES_64,
		.mode = TMR_MODE_CONTINUOUS,
		.bitMode = TMR_BIT_MODE_32,
		.clock = MXC_TMR_ISO_CLK,
		.cmp_cnt = UINT32_MAX,
	};
	int ret;

	if (initialized)
		return 0;

	ret = MXC_TMR_Init(MAXIM_TIME_TIMER, &config, false);
	if (ret)
		return -EIO;

	/* Continuous mode reloads at 1, giving UINT32_MAX ticks per period. */
	MXC_TMR_SetCount(MAXIM_TIME_TIMER, 1);
	elapsed_ticks = 0;
	MXC_TMR_ClearFlags(MAXIM_TIME_TIMER);
	MXC_TMR_EnableInt(MAXIM_TIME_TIMER);
	NVIC_ClearPendingIRQ(MXC_TMR_GET_IRQ(MAXIM_RISCV_TIME_TIMER));
	NVIC_EnableIRQ(MXC_TMR_GET_IRQ(MAXIM_RISCV_TIME_TIMER));
	MXC_TMR_Start(MAXIM_TIME_TIMER);
	initialized = true;

	return 0;
}

/** @brief Extend the timer across reloads, also when its IRQ is masked. */
static void maxim_time_update(void)
{
	if (MXC_TMR_GetFlags(MAXIM_TIME_TIMER)) {
		MXC_TMR_ClearFlags(MAXIM_TIME_TIMER);
		elapsed_ticks += UINT32_MAX;
	}
}

void MAXIM_IRQ_HANDLER MAXIM_TIME_HANDLER(MAXIM_RISCV_TIME_TIMER)(void)
{
	maxim_time_update();
}

void no_os_udelay(uint32_t usecs)
{
	uint32_t cycles_per_us = MXC_SYS_RiscVClockRate() / 1000000U;
	uint32_t max_usecs;
	uint32_t chunk;
	uint64_t start;
	uint64_t now;
	struct no_os_time time;

	/* The SDK truncates clocks below 1 MHz to zero cycles per microsecond. */
	if (!cycles_per_us) {
		time = no_os_get_time();
		start = (uint64_t)time.s * 1000000U + time.us;
		do {
			time = no_os_get_time();
			now = (uint64_t)time.s * 1000000U + time.us;
		} while (now - start < usecs);
		return;
	}
	max_usecs = UINT32_MAX / cycles_per_us;

	/* The SDK delay resets PCCR; elapsed time uses an independent timer. */
	while (usecs) {
		chunk = usecs > max_usecs ? max_usecs : usecs;
		MXC_Delay(chunk);
		usecs -= chunk;
	}
}

void no_os_mdelay(uint32_t msecs)
{
	while (msecs--)
		no_os_udelay(1000U);
}

struct no_os_time no_os_get_time(void)
{
	struct no_os_time time;
	uint32_t state = maxim_irq_save();
	uint32_t count;
	uint64_t ticks;

	count = MXC_TMR_GetCount(MAXIM_TIME_TIMER);
	if (MXC_TMR_GetFlags(MAXIM_TIME_TIMER)) {
		maxim_time_update();
		count = MXC_TMR_GetCount(MAXIM_TIME_TIMER);
	}
	ticks = elapsed_ticks + count - 1U;
	maxim_irq_restore(state);

	time.s = ticks / MAXIM_TIME_FREQ;
	time.us = (ticks % MAXIM_TIME_FREQ) * 1000000ULL / MAXIM_TIME_FREQ;
	return time;
}
