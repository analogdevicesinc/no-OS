/* SPDX-License-Identifier: BSD-3-Clause */

/** @file maxim_ipc_raw.h
 *  @brief SEMA transport shared by CPU0 and CPU1, with no CAPI dependency.
 */
#ifndef MAXIM_IPC_RAW_H_
#define MAXIM_IPC_RAW_H_

#include <stdint.h>
#include <stdbool.h>
#include "mxc_device.h"
#include "sema_regs.h"
#include "maxim_core.h"

#define MAX_IPC_HOST_ID    0u
#define MAX_IPC_COPROC_ID  1u

/*
 * ----------------------------------------------------------------------------
 * Freestanding inline peer helpers (usable from any core, no runtime required)
 * ----------------------------------------------------------------------------
 * These operate directly on the SEMA registers so the RISC-V coprocessor -- a
 * image without the CAPI dispatcher -- can participate in the protocol.
 * The host-side ops (maxim_ipc.c) are built from the same helpers.
 */

/**
 * @brief Ring the Arm (CM4) doorbell. Called from the coprocessor to notify the
 * host, or from the host to notify itself (rarely useful).
 */
static inline void maxim_ipc_raw_ring_host(void)
{
	maxim_ipc_barrier();
	MXC_SEMA->irq0 = MXC_F_SEMA_IRQ0_EN | MXC_F_SEMA_IRQ0_CM4_IRQ;
}

/**
 * @brief Ring the coprocessor (RV32) doorbell. Called from the host to notify
 * the coprocessor.
 */
static inline void maxim_ipc_raw_ring_coproc(void)
{
	maxim_ipc_barrier();
	MXC_SEMA->irq1 = MXC_F_SEMA_IRQ1_EN | MXC_F_SEMA_IRQ1_RV32_IRQ;
}

/** @brief True if the Arm (CM4) doorbell is pending. */
static inline bool maxim_ipc_raw_pending_host(void)
{
	bool pending = (MXC_SEMA->irq0 & MXC_F_SEMA_IRQ0_CM4_IRQ) != 0;

	if (pending)
		maxim_ipc_barrier();
	return pending;
}

/** @brief True if the coprocessor (RV32) doorbell is pending. */
static inline bool maxim_ipc_raw_pending_coproc(void)
{
	bool pending = (MXC_SEMA->irq1 & MXC_F_SEMA_IRQ1_RV32_IRQ) != 0;

	if (pending)
		maxim_ipc_barrier();
	return pending;
}

/** @brief Acknowledge (clear) the Arm (CM4) doorbell, keeping it enabled. */
static inline void maxim_ipc_raw_ack_host(void)
{
	maxim_ipc_barrier();
	MXC_SEMA->irq0 = MXC_F_SEMA_IRQ0_EN;
}

/** @brief Acknowledge (clear) the coprocessor (RV32) doorbell, keeping enabled. */
static inline void maxim_ipc_raw_ack_coproc(void)
{
	maxim_ipc_barrier();
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
	uint32_t value = MXC_SEMA->mail1;

	maxim_ipc_barrier();
	return value;
}

/** @brief Write the mailbox delivered to the coprocessor (read via mail0). */
static inline void maxim_ipc_raw_mbox_to_coproc(uint32_t value)
{
	MXC_SEMA->mail0 = value;
}

/** @brief Read the mailbox delivered to the coprocessor (written to mail0). */
static inline uint32_t maxim_ipc_raw_mbox_from_host(void)
{
	uint32_t value = MXC_SEMA->mail0;

	maxim_ipc_barrier();
	return value;
}

#endif /* MAXIM_IPC_RAW_H_ */
