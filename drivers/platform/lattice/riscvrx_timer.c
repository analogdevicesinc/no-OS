/***************************************************************************//**
 *   @file   lattice_timer.c
 *   @brief  Timer control module source for Lattice RISC-V RX.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2025-2026(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "riscvrx_timer.h"
#include "hal.h"
#include "parameters.h"

/******************************************************************************/
/*************************** Macros & Constants *******************************/
/******************************************************************************/

#ifndef CLOCK_OS
#error "CLOCK_OS must be defined in parameters.h"
#endif

/******************************************************************************/
/************************ Static Functions Prototypes *************************/
/******************************************************************************/

static void lattice_timer_isr(void *context);
static uint64_t lattice_timer_read_mtime(void);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static void lattice_timer_isr(void *context)
{
	struct no_os_timer_desc *desc = context;
	struct latt_timer_desc *latt_desc;

	if (!desc || !desc->extra)
		return;

	latt_desc = desc->extra;

	if (!latt_desc->running)
		return;

	clint_reload_timer(desc->ticks_count);

	if (latt_desc->callback)
		latt_desc->callback(latt_desc->callback_ctx);
}

static uint64_t lattice_timer_read_mtime(void)
{
	uint32_t hi;
	uint32_t lo;
	uint32_t hi2;

	do {
		hi = CLINT_MTIME_H;
		lo = CLINT_MTIME_L;
		hi2 = CLINT_MTIME_H;
	} while (hi != hi2);

	return ((uint64_t)hi << 32) | lo;
}

int lattice_timer_set_callback(struct no_os_timer_desc *desc,
		irq_handler handler,
		void *ctx)
{
	struct latt_timer_desc *latt_desc;

	if (!desc || !desc->extra)
		return -EINVAL;

	latt_desc = desc->extra;
	latt_desc->callback = handler;
	latt_desc->callback_ctx = ctx;

	return 0;
}

int32_t lattice_timer_start(struct no_os_timer_desc *desc)
{
	struct latt_timer_desc *latt_desc;

	if (!desc || !desc->extra || !desc->ticks_count)
		return -EINVAL;

	latt_desc = desc->extra;
	clint_start_timer(desc->ticks_count, latt_desc->mode,
		lattice_timer_isr, desc);
	latt_desc->running = true;

	return 0;
}

int32_t lattice_timer_stop(struct no_os_timer_desc *desc)
{
	struct latt_timer_desc *latt_desc;

	if (!desc || !desc->extra)
		return -EINVAL;

	latt_desc = desc->extra;
	clint_stop_timer(latt_desc->mode);
	latt_desc->running = false;

	return 0;
}

int32_t lattice_timer_counter_get(struct no_os_timer_desc *desc,
		uint32_t *counter)
{
	if (!desc || !counter)
		return -EINVAL;

	*counter = (uint32_t)lattice_timer_read_mtime();

	return 0;
}

int32_t lattice_timer_counter_set(struct no_os_timer_desc *desc,
		uint32_t new_val)
{
	struct latt_timer_desc *latt_desc;

	if (!desc || !desc->extra || !new_val)
		return -EINVAL;

	latt_desc = desc->extra;
	desc->ticks_count = new_val;

	if (latt_desc->running)
		clint_reload_timer(new_val);

	return 0;
}

int32_t lattice_timer_count_clk_get(struct no_os_timer_desc *desc,
		uint32_t *freq_hz)
{
	if (!desc || !freq_hz)
		return -EINVAL;

	*freq_hz = desc->freq_hz;

	return 0;
}

int32_t lattice_timer_count_clk_set(struct no_os_timer_desc *desc,
		uint32_t freq_hz)
{
	if (!desc)
		return -EINVAL;

	if (freq_hz != CLOCK_OS)
		return -ENOSYS;

	desc->freq_hz = CLOCK_OS;

	return 0;
}

int32_t lattice_timer_get_elapsed_time_nsec(struct no_os_timer_desc *desc,
		uint64_t *elapsed_time)
{
	uint64_t cycles;

	if (!desc || !elapsed_time || !desc->freq_hz)
		return -EINVAL;

	cycles = lattice_timer_read_mtime();
	*elapsed_time = (cycles * 1000000000ULL) / desc->freq_hz;

	return 0;
}

int32_t lattice_timer_init(struct no_os_timer_desc **desc,
		const struct no_os_timer_init_param *param)
{
	struct latt_timer_init_param *latt_init;
	struct no_os_timer_desc *no_os_desc;
	struct latt_timer_desc *latt_desc;
	uint32_t freq_hz;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	latt_init = param->extra;

	if (latt_init->type != LATT_TIMER_CLINT)
		return -EINVAL;

	no_os_desc = no_os_calloc(1, sizeof(*no_os_desc));
	if (!no_os_desc)
		return -ENOMEM;

	latt_desc = no_os_calloc(1, sizeof(*latt_desc));
	if (!latt_desc) {
		no_os_free(no_os_desc);
		return -ENOMEM;
	}

	freq_hz = param->freq_hz ? param->freq_hz : CLOCK_OS;
	if (freq_hz != CLOCK_OS || !param->ticks_count) {
		no_os_free(latt_desc);
		no_os_free(no_os_desc);
		return -EINVAL;
	}

	no_os_desc->id = param->id;
	no_os_desc->freq_hz = freq_hz;
	no_os_desc->ticks_count = param->ticks_count;
	no_os_desc->extra = latt_desc;

	latt_desc->type = latt_init->type;
	latt_desc->mode = latt_init->mode ? latt_init->mode : ePRIV_M;
	latt_desc->running = false;

	*desc = no_os_desc;

	return 0;
}

int32_t lattice_timer_remove(struct no_os_timer_desc *desc)
{
	struct latt_timer_desc *latt_desc;

	if (!desc || !desc->extra)
		return -EINVAL;

	latt_desc = desc->extra;

	if (latt_desc->running)
		clint_stop_timer(latt_desc->mode);

	no_os_free(latt_desc);
	no_os_free(desc);

	return 0;
}

const struct no_os_timer_platform_ops latt_timer_ops = {
	.init = &lattice_timer_init,
	.start = &lattice_timer_start,
	.stop = &lattice_timer_stop,
	.counter_get = &lattice_timer_counter_get,
	.counter_set = &lattice_timer_counter_set,
	.count_clk_get = &lattice_timer_count_clk_get,
	.count_clk_set = &lattice_timer_count_clk_set,
	.get_elapsed_time_nsec = &lattice_timer_get_elapsed_time_nsec,
	.remove = &lattice_timer_remove,
};
