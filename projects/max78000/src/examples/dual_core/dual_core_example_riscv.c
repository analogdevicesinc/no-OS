/***************************************************************************//**
 *   @file   dual_core_example_riscv.c
 *   @brief  RISC-V RV32 (CPU1) side of dual-core example.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/*
 * This is the RISC-V firmware that runs on CPU1 (auxiliary RISC-V core).
 * Compiled separately with riscv-none-elf-gcc and embedded in the ARM ELF.
 *
 * Flow:
 * 1. Boot at address in FCR->urvbootaddr (set by ARM CPU0).
 * 2. Initialize minimal system (UART for debug if desired).
 * 3. Wait for ARM to publish callback address in the IPC table.
 * 4. Periodically read the callback address and echo it back via doorbell.
 * 5. This wakes the ARM, which reads the address and invokes the callback.
 *
 * Note: This example is intentionally simple. In a real dual-core system,
 * the RISC-V side would do actual work (CNN inference, DSP, etc.) and only
 * notify ARM when results are ready.
 */

#include <stdint.h>

/* MAX78000 register/device definitions. */
#include "mxc_device.h"
#include "sema.h"

/* Shared IPC table layout (magic/fn/ctx/ready). */
#include "maxim_ipc_shared.h"

/*
 * Inline SEMA doorbell/mailbox helpers (maxim_ipc_raw_*). This core is
 * uses the register transport without enabling the CAPI dispatcher. The callback payload travels through the shared IPC table
 * in SRAM2; the doorbell (irq0) signals the ARM.
 */
#include "maxim_ipc_raw.h"
#include "no_os_delay.h"

/* Inline RISC-V-side runtime helpers (instruction-cache enable). */
#include "maxim_coprocessor_riscv.h"

/* Shared table in the reserved low SRAM2 window, outside CPU1 private data. */
#define IPC_TABLE_ADDR 0x20011000
static volatile maxim_ipc_table_t *const g_ipc_table =
	(volatile maxim_ipc_table_t *)IPC_TABLE_ADDR;

/* Counter for loop iterations */
static volatile uint32_t iteration = 0;

/**
 * @brief RISC-V main function (entry point on CPU1).
 * Called after CPU0 releases CPU1 from reset and sets the boot address.
 */
__attribute__((used))
int main(void)
{
	uint32_t callback_addr = 0;
	const uint32_t max_iterations = 10;

	/* Enable this core's instruction cache before the hot loop. */
	maxim_riscv_icc_enable();

	/* Acquire the callback slots published by CPU0 before reading them. */
	while (!maxim_ipc_table_ready(g_ipc_table))
		no_os_mdelay(10);

	callback_addr = g_ipc_table->fn[0];

	while (iteration < max_iterations) {
		/*
		 * Emit one event and wait for the ARM to acknowledge it before
		 * emitting the next -- a 1:1 handshake. The host doorbell is a level,
		 * not a counter, so coalesced rings would be lost; waiting for the ARM
		 * to clear it guarantees every notification is seen exactly once.
		 */
		if (callback_addr != 0) {
			/* Echo the callback address to the host mailbox, then notify */
			maxim_ipc_raw_mbox_to_host(callback_addr);
			maxim_ipc_raw_ring_host();

			/* Wait for the ARM to acknowledge (clear the host doorbell) */
			while (maxim_ipc_raw_pending_host())
				;

			/* Count this acknowledged notification */
			iteration++;
		}

		/* Brief pace between events */
		no_os_mdelay(10);
	}

	/*
	 * Finished: we've sent 10 wakeups to ARM. Now spin forever.
	 * In a real system, you'd do useful work here (CNN inference, DSP, etc.)
	 * or sleep in a low-power mode.
	 */
	while (1) {
		__asm__ volatile("wfi");  /* Wait for interrupt */
	}

	return 0;
}
