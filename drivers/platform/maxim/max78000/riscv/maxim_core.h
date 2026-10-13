/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef MAXIM_CORE_H_
#define MAXIM_CORE_H_

#include <stdbool.h>
#include "max78000.h"

#define MAXIM_TIME_TIMER_ID MAXIM_RISCV_TIME_TIMER
#define MAXIM_IRQ_HANDLER __attribute__((interrupt("machine")))

static inline uint32_t maxim_irq_save(void)
{
	uint32_t state;

	__asm__ volatile("csrrci %0, mstatus, 8" : "=r"(state) :: "memory");
	return state;
}

static inline void maxim_irq_restore(uint32_t state)
{
	if (state & 8U)
		__asm__ volatile("csrsi mstatus, 8" ::: "memory");
}

static inline bool maxim_timer_available(uint32_t id)
{
	return id != MAXIM_RISCV_TIME_TIMER;
}

static inline void maxim_ipc_barrier(void)
{
	__asm__ volatile("fence iorw, iorw" ::: "memory");
}

#endif /* MAXIM_CORE_H_ */
