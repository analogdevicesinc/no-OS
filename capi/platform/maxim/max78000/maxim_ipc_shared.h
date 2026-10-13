/* SPDX-License-Identifier: BSD-3-Clause */

/**
 *   @file   maxim_ipc_shared.h
 *   @brief  Maxim MAX78000 cross-core IPC shared memory interface.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Defines the shared-memory interface for inter-processor communication (IPC)
 * between the ARM Cortex-M4F (CPU0) and RISC-V RV32 (CPU1) cores on the MAX78000.
 *
 * Both cores see an identical, unified address map. The shared table is placed
 * by the application at a fixed address in a region unused by either core (for
 * instance, an application may reserve part of SRAM3 when CPU1 runs from
 * flash). Both linker layouts must exclude that region from their heap, stack,
 * and data sections. This header defines the layout, not the address.
 *
 * This header is included by BOTH ARM and RISC-V translation units.
 * Fields have fixed-width types; publication uses the selected core barrier.
 *
 * Cross-Core Function Pointer Table:
 * ARM side publishes function addresses as uint32_t values in the table.
 * RISC-V side reads the addresses and echoes them back via mailbox/doorbell
 * so the ARM side can invoke its own callbacks from the ARM ISR.
 * This pattern avoids cross-architecture calling convention issues.
 */

#ifndef MAX78000_CAPI_IPC_SHARED_H_
#define MAX78000_CAPI_IPC_SHARED_H_

#include <stdint.h>
#include <stdbool.h>
#include "maxim_core.h"

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
 * The application reserves at least sizeof(maxim_ipc_table_t) bytes at the
 * same physical address in both images. Function addresses are opaque to CPU1;
 * only CPU0 may invoke CPU0 callbacks.
 *
 * Halt the peer before modifying an already published table. Fill fn and ctx,
 * then call maxim_ipc_table_publish(). The receiver waits until
 * maxim_ipc_table_ready() returns true before reading the slots. Volatile alone
 * does not order shared-memory writes against the SEMA doorbell.
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

/** @brief Publish initialized slots before notifying the peer. */
static inline void maxim_ipc_table_publish(volatile maxim_ipc_table_t *table)
{
	table->magic = MAXIM_IPC_MAGIC;
	maxim_ipc_barrier();
	table->ready = 1;
}

/** @brief Acquire published slots after observing the ready flag. */
static inline bool maxim_ipc_table_ready(const volatile maxim_ipc_table_t
		*table)
{
	if (!table->ready)
		return false;
	maxim_ipc_barrier();
	return table->magic == MAXIM_IPC_MAGIC;
}

#endif /* MAX78000_CAPI_IPC_SHARED_H_ */
