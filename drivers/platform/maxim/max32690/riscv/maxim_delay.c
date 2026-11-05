/***************************************************************************//**
 *   @file   maxim_delay.c
 *   @brief  MAX32690 RISC-V (CPU1) delay and time functions.
 *   @author Christopher de Guzman (christopher.deguzman@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_util.h"
#include "maxim_core.h"
#include "maxim_time.h"
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
	/* The RISC-V interrupt controller latches the request; clear it as well. */
	NVIC_ClearPendingIRQ(MXC_TMR_GET_IRQ(MAXIM_RISCV_TIME_TIMER));
}

/** @brief Read the free-running CPU1 time base, in ISO/64 ticks. */
static uint64_t maxim_time_ticks(void)
{
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

	return ticks;
}

/*
 * MXC_Delay() counts CPU1 cycles assuming CPU1 runs at PeripheralClock, which
 * made delays about 33% too long on a MAX32690EVKIT. Wait on the CPU1 time
 * base instead; its resolution is one ISO/64 tick (about 1 us).
 */
static void maxim_delay_ticks(uint64_t ticks)
{
	uint64_t start;

	if (!initialized && maxim_time_init())
		return;

	start = maxim_time_ticks();
	while (maxim_time_ticks() - start < ticks)
		;
}

void no_os_udelay(uint32_t usecs)
{
	maxim_delay_ticks(NO_OS_DIV_ROUND_UP((uint64_t)usecs * MAXIM_TIME_FREQ,
					     1000000U));
}

void no_os_mdelay(uint32_t msecs)
{
	maxim_delay_ticks(NO_OS_DIV_ROUND_UP((uint64_t)msecs * MAXIM_TIME_FREQ,
					     1000U));
}

struct no_os_time no_os_get_time(void)
{
	struct no_os_time time;
	uint64_t ticks = maxim_time_ticks();

	time.s = ticks / MAXIM_TIME_FREQ;
	time.us = (ticks % MAXIM_TIME_FREQ) * 1000000ULL / MAXIM_TIME_FREQ;

	return time;
}
