/* SPDX-License-Identifier: BSD-3-Clause */

/**
 *   @file   maxim_coprocessor_riscv.h
 *   @brief  RISC-V (CPU1) side runtime helpers for the MAX78000 coprocessor.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Header-only helpers meant to run ON the RISC-V core. The coprocessor runs as
 * freestanding firmware that cannot link the no-OS runtime or the PeriphDrivers,
 * so these are `static inline` and poke registers directly (same approach as the
 * maxim_ipc_raw_* helpers in maxim_ipc.h).
 */

#ifndef MAX78000_CAPI_COPROCESSOR_RISCV_H_
#define MAX78000_CAPI_COPROCESSOR_RISCV_H_

#include <stdint.h>

/* RISC-V instruction cache (ICC1) control registers. */
#define MAXIM_RISCV_ICC_CTRL       (*(volatile uint32_t *)0x4002A900)
#define MAXIM_RISCV_ICC_INVALIDATE (*(volatile uint32_t *)0x4002AF00)
#define MAXIM_RISCV_ICC_CTRL_EN    (1U << 0)
#define MAXIM_RISCV_ICC_CTRL_RDY   (1U << 16)

/**
 * @brief Enable (and invalidate) the RISC-V instruction cache.
 *
 * The RISC-V core must do this itself before relying on cached instruction
 * fetch; the ARM core does not do it on the coprocessor's behalf. Call it once at
 * the start of the coprocessor's main(), before the hot loop. Sequence: disable,
 * invalidate, wait ready, enable, wait ready.
 */
static inline void maxim_riscv_icc_enable(void)
{
	MAXIM_RISCV_ICC_CTRL &= ~MAXIM_RISCV_ICC_CTRL_EN;
	MAXIM_RISCV_ICC_INVALIDATE = 1;
	while (!(MAXIM_RISCV_ICC_CTRL & MAXIM_RISCV_ICC_CTRL_RDY))
		;
	MAXIM_RISCV_ICC_CTRL |= MAXIM_RISCV_ICC_CTRL_EN;
	while (!(MAXIM_RISCV_ICC_CTRL & MAXIM_RISCV_ICC_CTRL_RDY))
		;
}

#endif /* MAX78000_CAPI_COPROCESSOR_RISCV_H_ */
