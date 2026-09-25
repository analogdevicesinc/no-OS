/***************************************************************************//**
 *   @file   maxim_coprocessor_riscv.h
 *   @brief  RISC-V (CPU1) side runtime helpers for the MAX32690 coprocessor.
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
 *
 * Header-only helpers meant to run ON the RISC-V core. The coprocessor runs as
 * freestanding firmware that cannot link the no-OS runtime or the PeriphDrivers,
 * so these are `static inline` and poke registers directly (same approach as the
 * maxim_ipc_raw_* helpers in maxim_ipc.h).
 *
 ******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#ifndef __MAXIM_COPROCESSOR_RISCV_H__
#define __MAXIM_COPROCESSOR_RISCV_H__

#include <stdint.h>

/* RISC-V instruction cache (ICC1) control registers.
 * MAX32690 ICC1 base address: 0x4002A800
 * CTRL register offset: 0x0100 -> Full address: 0x4002A900
 * INVALIDATE register offset: 0x0700 -> Full address: 0x4002AF00
 */
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

#endif /* __MAXIM_COPROCESSOR_RISCV_H__ */
