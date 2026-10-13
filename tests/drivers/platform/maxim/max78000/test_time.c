/***************************************************************************//**
 *   @file   test_time.c
 *   @brief  Host test for the MAX78000 RISC-V time base.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include "no_os_delay.h"
#include "maxim_time.h"
#include "tmr.h"
#include "max78000.h"
#include "no_os_init.h"

mxc_tmr_regs_t test_timer;
test_gcr_t test_gcr;
uint32_t SystemCoreClock;
static uint32_t flags, irq_state = 8, init_error, delay_calls;
static uint32_t clock_rate = 60000000;
static uint64_t delayed_us;
static bool wrap_during_read, advance_on_read;

uint32_t maxim_irq_save(void)
{
	uint32_t state = irq_state;
	irq_state = 0;
	return state;
}

void maxim_irq_restore(uint32_t state)
{
	assert(!irq_state);
	irq_state = state;
}

void NVIC_ClearPendingIRQ(uint32_t id)
{
	assert(id == 11);
}
void NVIC_EnableIRQ(uint32_t id)
{
	assert(id == 11);
}
uint32_t MXC_SYS_RiscVClockRate(void)
{
	return clock_rate;
}

int MXC_Delay(uint32_t usecs)
{
	assert((uint64_t)usecs * (clock_rate / 1000000U) <= UINT32_MAX);
	delayed_us += usecs;
	delay_calls++;
	return 0;
}

int MXC_TMR_Init(mxc_tmr_regs_t *tmr, mxc_tmr_cfg_t *config, bool pins)
{
	assert(tmr == &test_timer && !pins);
	assert(config->cmp_cnt == UINT32_MAX);
	assert(config->pres == TMR_PRES_64 && config->clock == MXC_TMR_ISO_CLK);
	return init_error;
}

void MXC_TMR_SetCount(mxc_tmr_regs_t *tmr, uint32_t count)
{
	tmr->count = count;
}
uint32_t MXC_TMR_GetCount(mxc_tmr_regs_t *tmr)
{
	uint32_t value = tmr->count;
	if (advance_on_read)
		tmr->count++;
	if (wrap_during_read) {
		tmr->count = 2;
		flags = 1;
		wrap_during_read = false;
	}
	return value;
}
uint32_t MXC_TMR_GetFlags(mxc_tmr_regs_t *tmr)
{
	(void)tmr;
	return flags;
}
void MXC_TMR_ClearFlags(mxc_tmr_regs_t *tmr)
{
	(void)tmr;
	flags = 0;
}
void MXC_TMR_EnableInt(mxc_tmr_regs_t *tmr)
{
	assert(tmr == &test_timer);
}
void MXC_TMR_Start(mxc_tmr_regs_t *tmr)
{
	assert(tmr == &test_timer);
}
void TMR3_IRQHandler(void);

static uint64_t now_us(void)
{
	struct no_os_time time = no_os_get_time();
	assert(time.us < 1000000U);
	return (uint64_t)time.s * 1000000U + time.us;
}

int main(void)
{
	uint64_t before, after;

	test_gcr.clkctrl = (4U << 9) | (2U << 6);
	SystemCoreClockUpdate();
	assert(SystemCoreClock == 25000000);
	test_gcr.clkctrl = 1U << 6;
	SystemCoreClockUpdate();
	assert(SystemCoreClock == 30000000);
	test_gcr.clkctrl = 7U << 9;
	SystemCoreClockUpdate();
	assert(SystemCoreClock == 12000000);

	init_error = 1;
	assert(maxim_time_init() == -EIO);
	init_error = 0;
	assert(!maxim_time_init());
	assert(now_us() == 0);
	test_timer.count = 1406251;
	assert(now_us() == 1500000);
	assert(!maxim_time_init());
	assert(now_us() == 1500000);
	assert(irq_state == 8);

	/* A masked interrupt must not make elapsed time move backwards. */
	irq_state = 0;
	test_timer.count = UINT32_MAX;
	before = now_us();
	test_timer.count = 2;
	flags = 1;
	after = now_us();
	assert(after >= before && after - before <= 3);
	assert(irq_state == 0 && !flags);
	TMR3_IRQHandler();
	assert(now_us() == after);

	/* A reload between reading count and flags must use the new count. */
	test_timer.count = UINT32_MAX;
	before = now_us();
	wrap_during_read = true;
	after = now_us();
	assert(after >= before && after - before <= 3);

	flags = 1;
	test_timer.count = 10;
	TMR3_IRQHandler();
	assert(now_us() > after);
	before = now_us();
	no_os_udelay(UINT32_MAX);
	assert(delayed_us == UINT32_MAX && delay_calls > 1);
	assert(now_us() == before);
	delayed_us = 0;
	no_os_mdelay(2000);
	assert(delayed_us == 2000000);
	clock_rate = 500000;
	advance_on_read = true;
	before = now_us();
	no_os_udelay(1000);
	assert(now_us() - before >= 1000);
	assert(delayed_us == 2000000);
	return 0;
}
