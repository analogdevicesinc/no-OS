/***************************************************************************//**
 *   @file   no_os_barrier.h
 *   @brief  Portable memory-barrier primitives.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Header-only compiler/hardware memory barriers usable from any translation
 * unit, including freestanding firmware built with -nostdlib (e.g. the RISC-V
 * side of a dual-core system). Implemented with the GCC/Clang __atomic builtins
 * so no runtime library is required.
 *
 * These order memory accesses as seen by other observers (a second CPU core,
 * DMA, or memory-mapped peripherals). They are the portable spelling of the
 * Arm __DSB()/__DMB() and RISC-V fence instructions.
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

#ifndef _NO_OS_BARRIER_H_
#define _NO_OS_BARRIER_H_

/*
 * These emit *system-scope* barriers so that a store on one core is ordered as
 * observed by a DIFFERENT core (or DMA), not merely within the local cache
 * hierarchy. This is deliberately stronger than __atomic_thread_fence(): on
 * Armv7-M the compiler lowers atomic fences to `dmb ish` (Inner-Shareable),
 * whose scope does not necessarily include an asymmetric coprocessor like the
 * RISC-V core on the MAX78000 -- cross-core handoffs then silently read stale
 * data. We therefore issue the same full-system barrier the vendor SDKs use:
 *   - Arm (Cortex-M): `dsb sy` (matches CMSIS __DSB()), which both orders and
 *     waits for completion, in the full system domain.
 *   - RISC-V: `fence iorw, iorw`, the full memory+I/O fence.
 * On other architectures we fall back to the strongest compiler fence.
 */

#if defined(__arm__) || defined(__thumb__)

static inline void no_os_barrier_full(void)
{
	__asm__ volatile("dsb sy" ::: "memory");
}

#elif defined(__riscv)

static inline void no_os_barrier_full(void)
{
	__asm__ volatile("fence iorw, iorw" ::: "memory");
}

#else

static inline void no_os_barrier_full(void)
{
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
}

#endif

#endif /* _NO_OS_BARRIER_H_ */
