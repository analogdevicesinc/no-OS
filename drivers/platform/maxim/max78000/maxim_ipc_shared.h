/***************************************************************************//**
 *   @file   maxim_ipc_shared.h
 *   @brief  Maxim MAX78000 cross-core IPC shared memory interface.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Defines the shared-memory interface for inter-processor communication (IPC)
 * between the ARM Cortex-M4F (CPU0) and RISC-V RV32 (CPU1) cores on the MAX78000.
 *
 * Both cores see an identical, unified address map. The shared table is placed
 * by the application at a fixed address in a region unused by either core (for
 * the dual-core example, SRAM3 @ 0x2001C000, which is free because the RISC-V
 * runs in place from flash). This header only defines the layout, not the
 * address.
 *
 * This header is included by BOTH ARM and RISC-V translation units.
 * No compiler-specific types — uses only C99 stdint.h types for portability.
 *
 * Cross-Core Function Pointer Table:
 * ARM side publishes function addresses as uint32_t values in the table.
 * RISC-V side reads the addresses and echoes them back via mailbox/doorbell
 * so the ARM side can invoke its own callbacks from the ARM ISR.
 * This pattern avoids cross-architecture calling convention issues.
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

#ifndef _MAXIM_IPC_SHARED_H_
#define _MAXIM_IPC_SHARED_H_

#include <stdint.h>

/**
 * @def MAXIM_IPC_MAGIC
 * Magic number to validate the IPC table. Written last by the publisher
 * core (ARM) after populating all function addresses and context pointers.
 * The consumer core (RISC-V) spins on this value before reading the table.
 */
#define MAXIM_IPC_MAGIC    0xCAFEFEEDUL

/**
 * @def MAXIM_IPC_N_SLOTS
 * Number of function pointer slots in the IPC table.
 * Each slot holds a function address (uint32_t) and context pointer (uint32_t).
 */
#define MAXIM_IPC_N_SLOTS  8

/**
 * @struct maxim_ipc_table_t
 * @brief Cross-core function pointer table in shared memory.
 *
 * Placed in the .shared section at 0x2001BF80 (128-byte SHARED SRAM region).
 * Both cores declare this struct identically and access it at the same
 * physical address.
 *
 * Synchronization model:
 * - ARM side populates fn[0..N-1] and ctx[0..N-1], then sets magic, then ready.
 * - RISC-V side spins on (ready && magic == MAXIM_IPC_MAGIC) before reading.
 * - No cache coherency issues: both cores have no data cache.
 * - Read barriers (dsb, fence) are optional but recommended for ARM side.
 *
 * Usage:
 * ARM side (in initialization):
 * ```
 * g_ipc_table.magic = 0;              // invalidate
 * g_ipc_table.fn[0] = (uint32_t)my_cb; // write addresses
 * g_ipc_table.ctx[0] = (uint32_t)NULL;
 * __DSB();                             // ARM data sync barrier (optional)
 * g_ipc_table.ready = 1;              // release
 * maxim_sema_ring_rv32();             // doorbell notify
 * ```
 *
 * RISC-V side (in callback wait loop):
 * ```
 * while (!g_ipc_table.ready || g_ipc_table.magic != MAXIM_IPC_MAGIC)
 *     ;  // spin; no cache, so reads see ARM writes immediately
 * uint32_t addr = g_ipc_table.fn[0];
 * if (addr) {
 *     // Echo address back to ARM via mailbox or doorbell for ARM to dispatch
 *     MXC_SEMA->mail1 = addr;
 *     maxim_sema_ring_cm4();
 * }
 * ```
 */
typedef struct {
	/**
	 * Magic number. Writes 0 before publishing, then the real magic after
	 * all function addresses are written. RISC-V side spins until this
	 * equals MAXIM_IPC_MAGIC.
	 */
	volatile uint32_t magic;

	/**
	 * Function addresses (code pointers as uint32_t).
	 * ARM side writes here; RISC-V side reads.
	 * A zero value indicates the slot is unused.
	 * Address must be a valid code address (flash).
	 */
	volatile uint32_t fn[MAXIM_IPC_N_SLOTS];

	/**
	 * Function context pointers (serialized void * as uint32_t).
	 * Paired with fn[i]: the ARM callback will receive ctx[i] as its
	 * context argument when invoked.
	 */
	volatile uint32_t ctx[MAXIM_IPC_N_SLOTS];

	/**
	 * Release flag. Set to 1 (non-zero) by ARM after writing all data
	 * and the magic. RISC-V spins until this is nonzero AND magic matches.
	 */
	volatile uint32_t ready;
} maxim_ipc_table_t;

#endif /* _MAXIM_IPC_SHARED_H_ */
