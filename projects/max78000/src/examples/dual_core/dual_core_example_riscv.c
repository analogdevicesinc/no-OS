/***************************************************************************//**
 *   @file   dual_core_example_riscv.c
 *   @brief  RISC-V RV32 (CPU1) side of dual-core example.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
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
 * OR PROFITS; OR BUSINESS BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include <stdint.h>

/* MAX78000 register/device definitions. */
#include "mxc_device.h"
#include "sema.h"

/* Shared IPC table layout (magic/fn/ctx/ready). */
#include "maxim_ipc_shared.h"

/*
 * Inline SEMA doorbell/mailbox helpers (maxim_ipc_raw_*). This core is
 * freestanding and can't link the no-OS runtime, so it drives the doorbell
 * registers directly. The callback payload travels through the shared IPC table
 * in SRAM2; the doorbell (irq0) signals the ARM.
 */
#include "maxim_ipc.h"

/* Inline RISC-V-side runtime helpers (instruction-cache enable). */
#include "maxim_coprocessor_riscv.h"

/*
 * Shared IPC table, at a fixed SRAM2 address both cores reach through an
 * identical fixed-address pointer. It sits above this core's .data/.bss and well
 * below its stack (limit 0x2001B000). It must be in SRAM2, not SRAM3: when this
 * core runs in place from flash, SRAM3 is not accessible to it as data.
 */
#define IPC_TABLE_ADDR 0x20011000
static volatile maxim_ipc_table_t *const g_ipc_table =
	(volatile maxim_ipc_table_t *)IPC_TABLE_ADDR;

/* Counter for loop iterations */
static volatile uint32_t iteration = 0;

/**
 * @brief Simple delay function (approx milliseconds).
 * Uses a busy-wait loop calibrated for the MAX78000 at ~100 MHz.
 * @param ms Milliseconds to delay.
 */
static void delay_ms(uint32_t ms)
{
	/* Approximate: 100 MHz / 4 instructions per loop = 25k loops per ms */
	uint32_t loops = ms * 25000;
	while (loops--) {
		__asm__ volatile("nop");
	}
}

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

	/*
	 * The C runtime is already up (stack, .data/.bss, trap vector set by the
	 * startup). Wait for the ARM to publish the callback address in the shared
	 * table, then read it. Shared memory is coherent (no data cache); the ARM's
	 * stack is confined to SRAM0/1 so it cannot corrupt the table in SRAM2.
	 *
	 * No memory barrier here: on this core a `fence` corrupts the following load,
	 * and with no data cache, `volatile` plus the doorbell handshake already
	 * order the accesses correctly.
	 */
	while (g_ipc_table->magic != MAXIM_IPC_MAGIC || !g_ipc_table->ready)
		delay_ms(10);

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
		delay_ms(10);
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
