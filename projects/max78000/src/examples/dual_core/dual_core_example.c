/***************************************************************************//**
 *   @file   dual_core_example.c
 *   @brief  ARM Cortex-M4F (CPU0) side of dual-core example.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * This example demonstrates the coprocessor lifecycle API and inter-core
 * communication (IPC) via hardware semaphores and doorbells.
 *
 * Flow:
 * 1. ARM (CPU0) boots and initializes UART for console output.
 * 2. ARM initializes the coprocessor (CPU1 = RISC-V RV32).
 * 3. ARM boots the RISC-V core from the embedded RV firmware image.
 * 4. ARM hands a callback address to RISC-V via the coprocessor mailbox.
 * 5. ARM waits for RISC-V to ring the doorbell.
 * 6. ARM wakes and invokes the RISC-V-triggered callback.
 * 7. Loop: print stats and go back to sleep.
 *
 * RISC-V side (in separate firmware):
 * - Boots at the address in FCR->urvbootaddr.
 * - Reads the callback address from its inbound mailbox.
 * - Periodically echoes the address back via mailbox + doorbell (wakes ARM).
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

#include <stdio.h>
#include <stdint.h>
#include "no_os_uart.h"
#include "no_os_coprocessor.h"
#include "no_os_ipc.h"
#include "no_os_barrier.h"
#include "maxim_coprocessor.h"
#include "maxim_ipc.h"
#include "maxim_ipc_shared.h"
#include "common_data.h"
#include "mxc_sys.h"

/*
 * Shared IPC table at a fixed SRAM2 address both cores agree on, reached through
 * a fixed-address pointer (no linker section maps here). It sits in the gap in
 * the RISC-V's SRAM2 map: above the RV .data/.bss (which end at ~0x20010004)
 * and well below the RV stack (limit 0x2001B000, top 0x2001C000). SRAM3 is NOT
 * usable for this: when the RISC-V executes in place from flash, SRAM3 (the "RV
 * Code" region) is not accessible to the RV core as data.
 */
#define IPC_TABLE_ADDR 0x20011000UL
static volatile maxim_ipc_table_t *const g_ipc_table =
	(volatile maxim_ipc_table_t *)IPC_TABLE_ADDR;

/* Coprocessor descriptor */
static struct no_os_coprocessor_desc *cpu1_desc;

/* Inter-core IPC descriptor (doorbell + mailbox) */
static struct no_os_ipc_desc *ipc_desc;

/* UART descriptor for console output */
static struct no_os_uart_desc *uart_desc;

/* Count of doorbell events received from the RISC-V core */
static volatile uint32_t wakeup_count = 0;

/**
 * @brief Callback function that RISC-V will invoke via the mailbox echo.
 * This function executes on the ARM side when the ARM sees the RISC-V
 * wakeup signal. It's an example of how to route RISC-V signals back to
 * ARM code.
 */
static void risc_v_event_callback(void *ctx)
{
	(void)ctx;
	printf("  [ARM] RISC-V event callback invoked\r\n");
}

/**
 * @brief Publish the ARM callback address to RISC-V via the shared IPC table.
 * Writes the payload, then the magic/ready flags last, with a release barrier
 * in between so the RISC-V core never sees a fresh ready flag over stale data.
 * The RISC-V reads the address and echoes it back via mailbox + doorbell.
 */
static void publish_callback_to_risc_v(void)
{
	/* Invalidate the magic while the table is being updated */
	g_ipc_table->magic = 0;
	no_os_barrier_full();

	/* Write the callback address and context */
	g_ipc_table->fn[0] = (uint32_t)risc_v_event_callback;
	g_ipc_table->ctx[0] = (uint32_t)NULL;

	/* Release barrier: publish the payload before the ready/magic flags */
	no_os_barrier_full();

	/* Validate and release the RISC-V core to read */
	g_ipc_table->magic = MAXIM_IPC_MAGIC;
	g_ipc_table->ready = 1;
	no_os_barrier_full();

	printf("[ARM] Callback address published to RISC-V (0x%lx)\r\n",
	       (unsigned long)risc_v_event_callback);
}

/*
 * RISC-V boot address. The RISC-V image is embedded into this ARM ELF (in the
 * .riscv_flash section of the dual-core linker script) and runs in place from
 * flash -- no copy to SRAM. _riscv_boot is the linker-exported flash address of
 * the image's entry point, which the coprocessor driver programs into
 * FCR->urvbootaddr.
 */
extern uint8_t _riscv_boot;

/**
 * @brief Initialize the coprocessor (RISC-V CPU1).
 */
static int init_coprocessor(void)
{
	int ret;
	struct max_coprocessor_init_param extra = {
		.boot_addr = (uint32_t)&_riscv_boot, /* flash: RISC-V runs in place */
		.enable_debug = false                /* set true to attach a RISC-V JTAG debugger */
	};

	struct no_os_coprocessor_init_param param = {
		.id = 0,
		.platform_ops = &max_coprocessor_ops,
		.extra = &extra
	};

	printf("[ARM] Initializing coprocessor (CPU1)...\r\n");
	ret = no_os_coprocessor_init(&cpu1_desc, &param);
	if (ret) {
		printf("[ARM] ERROR: Coprocessor init failed (%d)\r\n", ret);
		return ret;
	}

	return 0;
}

/**
 * @brief Boot the RISC-V coprocessor.
 */
static int boot_coprocessor(void)
{
	int ret;

	printf("[ARM] Booting coprocessor (CPU1)...\r\n");
	ret = no_os_coprocessor_boot(cpu1_desc);
	if (ret) {
		printf("[ARM] ERROR: Coprocessor boot failed (%d)\r\n", ret);
		return ret;
	}

	printf("[ARM] Coprocessor booted successfully\r\n");
	return 0;
}

/**
 * @brief Initialize the inter-core IPC (doorbells + mailboxes).
 */
static int init_ipc(void)
{
	int ret;
	struct no_os_ipc_init_param param = {
		.id = 0,
		.platform_ops = &max_ipc_ops,
		.extra = NULL
	};

	printf("[ARM] Initializing IPC (doorbells + mailboxes)...\r\n");
	ret = no_os_ipc_init(&ipc_desc, &param);
	if (ret) {
		printf("[ARM] ERROR: IPC init failed (%d)\r\n", ret);
		return ret;
	}

	printf("[ARM] IPC initialized\r\n");
	return 0;
}

/**
 * @brief Main function: ARM dual-core example.
 */
int dual_core_example_main(void)
{
	int ret;

	/*
	 * Select the 100 MHz IPO as the system clock before UART init: the RISC-V
	 * fetches instructions from flash and needs this high-speed clock, and doing
	 * it before UART init means the baud divider is computed against the final
	 * clock (switching afterwards would corrupt the console).
	 */
	MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
	SystemCoreClockUpdate();

	/* Initialize UART before any printf output */
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret) {
		return ret;
	}

	/* Redirect stdout to UART */
	no_os_uart_stdio(uart_desc);

	printf("\r\n");
	printf("========================================\r\n");
	printf("MAX78000 Dual-Core Example (ARM Side)\r\n");
	printf("========================================\r\n");

	printf("[ARM] RISC-V firmware embedded in flash at 0x%08lx\r\n",
	       (unsigned long)&_riscv_boot);

	/* Initialize inter-core IPC (doorbells + mailboxes) */
	ret = init_ipc();
	if (ret)
		return ret;

	/* Initialize coprocessor (RISC-V CPU1) */
	ret = init_coprocessor();
	if (ret)
		return ret;

	/* Boot coprocessor (RISC-V starts executing and polls its mailbox) */
	ret = boot_coprocessor();
	if (ret)
		return ret;

	/* Publish the callback address; the RISC-V core polls the table for it */
	publish_callback_to_risc_v();

	printf("\r\n[ARM] Waiting for RISC-V events...\r\n\r\n");

	/*
	 * Main loop: wait for the RISC-V core to ring the doorbell.
	 *
	 * The RISC-V core signals an event by writing the callback address to its
	 * mailbox and notifying the host channel. We wait on that doorbell, read
	 * the mailbox, acknowledge, dispatch the echoed callback, and count the
	 * event. The example finishes after target_wakeups genuine doorbell events.
	 *
	 * In production this poll would be replaced by a SEMA interrupt handler
	 * registered via no_os_irq_register_callback().
	 */
	const uint32_t target_wakeups = 10;

	while (wakeup_count < target_wakeups) {
		uint32_t cb;

		/* Wait for the RISC-V core to ring our (host) doorbell */
		no_os_ipc_wait(ipc_desc, NO_OS_IPC_CHAN_HOST);

		/* Read the echoed address, then acknowledge the doorbell */
		no_os_ipc_mbox_recv(ipc_desc, NO_OS_IPC_CHAN_HOST, &cb);
		no_os_ipc_ack(ipc_desc, NO_OS_IPC_CHAN_HOST);
		wakeup_count++;

		/* Dispatch the echoed callback on the ARM side */
		if (cb == (uint32_t)risc_v_event_callback)
			risc_v_event_callback(NULL);

		printf("[ARM] Wakeup %lu/%lu (callback 0x%lx)\r\n",
		       (unsigned long)wakeup_count,
		       (unsigned long)target_wakeups,
		       (unsigned long)cb);
	}

	printf("\r\n[ARM] Example complete (%lu wakeups received)\r\n",
	       (unsigned long)wakeup_count);

	printf("[ARM] Halting coprocessor...\r\n");
	no_os_coprocessor_halt(cpu1_desc);

	printf("[ARM] Removing coprocessor descriptor...\r\n");
	no_os_coprocessor_remove(cpu1_desc);

	no_os_ipc_remove(ipc_desc);

	printf("[ARM] Dual-core example finished\r\n");
	return 0;
}
