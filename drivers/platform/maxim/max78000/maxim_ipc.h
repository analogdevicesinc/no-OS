/***************************************************************************//**
 *   @file   maxim_ipc.h
 *   @brief  Maxim MAX78000 inter-processor communication (IPC) driver header.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Implements the generic no_os_ipc API (include/no_os_ipc.h) on top of the
 * Maxim SEMA peripheral: core-to-core doorbell interrupts and single-word
 * mailbox registers.
 *
 * Channel mapping (MAX78000 SEMA):
 *   NO_OS_IPC_CHAN_HOST   (notify Arm CM4)   -> irq0 / CM4_IRQ,  mailbox mail1
 *   NO_OS_IPC_CHAN_COPROC (notify RISC-V RV32)-> irq1 / RV32_IRQ, mailbox mail0
 *
 * Two ways to use it:
 *  - Host (Arm) side: the full descriptor + ops API via no_os_ipc_*(). Requires
 *    the no-OS runtime (heap, etc.).
 *  - Coprocessor (RISC-V) side: this core runs as freestanding firmware and
 *    cannot link the no-OS API layer, so it uses the header-only inline helpers
 *    below (maxim_ipc_raw_*), which poke the SEMA registers directly. The host
 *    ops are implemented in terms of these same helpers, so both sides share one
 *    definition of the protocol.
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

#ifndef _MAXIM_IPC_H_
#define _MAXIM_IPC_H_

#include <stdint.h>
#include <stdbool.h>
#include "mxc_device.h"
#include "sema_regs.h"
#include "no_os_ipc.h"

/*
 * ----------------------------------------------------------------------------
 * Freestanding inline peer helpers (usable from any core, no runtime required)
 * ----------------------------------------------------------------------------
 * These operate directly on the SEMA registers so the RISC-V coprocessor -- a
 * freestanding image with no no-OS runtime -- can participate in the protocol.
 * The host-side ops (maxim_ipc.c) are built from the same helpers.
 */

/**
 * @brief Ring the Arm (CM4) doorbell. Called from the coprocessor to notify the
 * host, or from the host to notify itself (rarely useful).
 */
static inline void maxim_ipc_raw_ring_host(void)
{
	MXC_SEMA->irq0 = MXC_F_SEMA_IRQ0_EN | MXC_F_SEMA_IRQ0_CM4_IRQ;
}

/**
 * @brief Ring the coprocessor (RV32) doorbell. Called from the host to notify
 * the coprocessor.
 */
static inline void maxim_ipc_raw_ring_coproc(void)
{
	MXC_SEMA->irq1 = MXC_F_SEMA_IRQ1_EN | MXC_F_SEMA_IRQ1_RV32_IRQ;
}

/** @brief True if the Arm (CM4) doorbell is pending. */
static inline bool maxim_ipc_raw_pending_host(void)
{
	return (MXC_SEMA->irq0 & MXC_F_SEMA_IRQ0_CM4_IRQ) != 0;
}

/** @brief True if the coprocessor (RV32) doorbell is pending. */
static inline bool maxim_ipc_raw_pending_coproc(void)
{
	return (MXC_SEMA->irq1 & MXC_F_SEMA_IRQ1_RV32_IRQ) != 0;
}

/** @brief Acknowledge (clear) the Arm (CM4) doorbell, keeping it enabled. */
static inline void maxim_ipc_raw_ack_host(void)
{
	MXC_SEMA->irq0 = MXC_F_SEMA_IRQ0_EN;
}

/** @brief Acknowledge (clear) the coprocessor (RV32) doorbell, keeping enabled. */
static inline void maxim_ipc_raw_ack_coproc(void)
{
	MXC_SEMA->irq1 = MXC_F_SEMA_IRQ1_EN;
}

/** @brief Write the mailbox delivered to the host (read via mail1). */
static inline void maxim_ipc_raw_mbox_to_host(uint32_t value)
{
	MXC_SEMA->mail1 = value;
}

/** @brief Read the mailbox delivered to the host (written to mail1). */
static inline uint32_t maxim_ipc_raw_mbox_from_coproc(void)
{
	return MXC_SEMA->mail1;
}

/** @brief Write the mailbox delivered to the coprocessor (read via mail0). */
static inline void maxim_ipc_raw_mbox_to_coproc(uint32_t value)
{
	MXC_SEMA->mail0 = value;
}

/** @brief Read the mailbox delivered to the coprocessor (written to mail0). */
static inline uint32_t maxim_ipc_raw_mbox_from_host(void)
{
	return MXC_SEMA->mail0;
}

/*
 * ----------------------------------------------------------------------------
 * Host-side no_os_ipc ops (require the no-OS runtime)
 * ----------------------------------------------------------------------------
 */

/** Platform ops table implementing no_os_ipc over the Maxim SEMA peripheral. */
extern const struct no_os_ipc_platform_ops max_ipc_ops;

#endif /* _MAXIM_IPC_H_ */
