/***************************************************************************//**
 *   @file   dual_core_example.c
 *   @brief  ARM Cortex-M4F (CPU0) side of LTC4284 dual-core OC demo.
 *   @author Analog Devices Inc.
 *
 * This example demonstrates dual-core LTC4284 hot swap + overcurrent
 * protection on the MAX32690. The RISC-V core owns the I2C bus and the
 * ALERT GPIO; the ARM core is a pure observer that drives the console
 * over IPC.
 *
 * ARM (CPU0) responsibilities:
 * - Boot the RISC-V coprocessor from embedded flash
 * - Own the shared IPC table + UART console
 * - Poll RISC-V for telemetry and OC config, print the OC banner
 * - Decode FAULT bits and narrate FET on/off transitions
 * - Auto-clear cleared faults so the loop keeps flowing
 *
 * RISC-V (CPU1) responsibilities:
 * - Program the OC profile (V_ILIM, foldback, retry policy) via I2C
 * - Enable the FET and clear faults
 * - Monitor the LTC4284 ALERT pin via GPIO interrupt (real-time)
 * - Serve IPC commands: READ_TELEMETRY, READ_CONFIG_REGS, CLEAR_FAULTS,
 *   ENABLE_FET
 * - Ring the ARM doorbell on ALERT
 *
 * Hardware setup (DC2470A + MAX32690EVKIT):
 * - DC2470A SDA   -> P2.7  (I2C0_SDA)
 * - DC2470A SCL   -> P2.8  (I2C0_SCL)
 * - DC2470A ALERT -> P0.19 (GPIO, monitored by RISC-V core)
 * - Console: UART2 (115200 8N1) via on-board USB serial
 *
 * KNOWN NON-ISSUE — bench PSU sag on a real OC trip:
 *   If the supply cannot cleanly source the trip current, VIN sags below
 *   the ~43 V UV threshold when the load engages. The chip trips on
 *   UV + POWER_FAILED (sometimes with a spurious OC bit from the transient),
 *   which looks like a false OC trip at low current (observed ~500 mA on a
 *   small bench supply with alligator clips).
 *
 *   Signature: FAULT = 0x22 or 0x26 (UV + POWER_FAILED, sometimes with OC),
 *   and VIN visibly sagging in the log before the trip.
 *
 *   This is NOT an overcurrent event and NOT a code problem. A proper
 *   >60 A PSU with 10 AWG ring-lug leads will not exhibit it; VIN stays
 *   stable and the real OC trip fires at ~48 A as programmed.
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
#include <stdbool.h>
#include <errno.h>
#include "no_os_uart.h"
#include "no_os_delay.h"
#include "no_os_ipc.h"
#include "no_os_barrier.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_ipc.h"
#include "common_data.h"
#include "parameters.h"
#include "mxc_sys.h"
#include "fcr_regs.h"
#include "ltc4284.h"
#include "dual_core_ltc4284_ipc.h"

/* External symbol from linker script */
extern uint8_t _riscv_boot;

/* Pointer to shared IPC memory */
static volatile ltc4284_ipc_table_t *const g_ipc_table =
	(volatile ltc4284_ipc_table_t *)LTC4284_IPC_TABLE_ADDR;

/* IPC descriptor */
static struct no_os_ipc_desc *ipc_desc;

/* Alert statistics */
static volatile uint32_t arm_alert_count = 0;

/* OC narration state */
static uint8_t prev_faults = 0;
static bool fet_was_on = true;

/**
 * @brief Decode fault register bits into a human-readable string
 */
static void decode_faults(uint8_t faults)
{
	char buf[128];
	int n = 0;

	if (!faults)
		return;

	n += snprintf(buf + n, sizeof(buf) - n, "FAULT 0x%02X:", faults);
	if (faults & LTC4284_FAULT_OC)
		n += snprintf(buf + n, sizeof(buf) - n, " OC");
	if (faults & LTC4284_FAULT_UV)
		n += snprintf(buf + n, sizeof(buf) - n, " UV");
	if (faults & LTC4284_FAULT_OV)
		n += snprintf(buf + n, sizeof(buf) - n, " OV");
	if (faults & LTC4284_FAULT_FET_BAD)
		n += snprintf(buf + n, sizeof(buf) - n, " FET_BAD");
	if (faults & LTC4284_FAULT_FET_SHORT)
		n += snprintf(buf + n, sizeof(buf) - n, " FET_SHORT");
	if (faults & LTC4284_FAULT_POWER_BAD)
		n += snprintf(buf + n, sizeof(buf) - n, " POWER_FAILED");
	if (faults & LTC4284_FAULT_PGI)
		n += snprintf(buf + n, sizeof(buf) - n, " PGI");
	if (faults & LTC4284_FAULT_EXT)
		n += snprintf(buf + n, sizeof(buf) - n, " EXT");

	printf("  ** %s **\r\n", buf);
}

/**
 * @brief Format the retry policy code as a string
 */
static const char *retry_str(uint8_t code)
{
	switch (code) {
	case LTC4284_RETRY_LATCH_OFF:
		return "latch off";
	case LTC4284_RETRY_1:
		return "1 retry then latch";
	case LTC4284_RETRY_7:
		return "7 retries then latch";
	case LTC4284_RETRY_UNLIMITED:
		return "unlimited retries";
	default:
		return "?";
	}
}


/**
 * @brief Boot RISC-V coprocessor using SDK function
 */
static void boot_riscv_coprocessor(void)
{
	printf("[ARM] Booting RISC-V coprocessor at 0x%08lx...\r\n",
	       (unsigned long)&_riscv_boot);

	/* Matches MXC_SYS_RISCVRun() from sys_me18.c exactly, using our
	 * _riscv_boot symbol (embedded by maxim_coprocessor.cmake via .incbin)
	 * instead of the SDK's _binary_riscv_bin_start convention. */

	/* Disable RISC-V clock before configuring boot address */
	MXC_GCR->pclkdis1 |= MXC_F_GCR_PCLKDIS1_CPU1;

	/* urvbootaddr is at FCR offset 0x10 — must use the struct, not offset 0x00 */
	MXC_FCR->urvbootaddr = (uint32_t)&_riscv_boot;

	/* Re-enable clock and pulse reset to start the core */
	MXC_GCR->pclkdis1 &= ~MXC_F_GCR_PCLKDIS1_CPU1;
	MXC_GCR->rst1 |= MXC_F_GCR_RST1_CPU1;

	printf("[ARM] RISC-V coprocessor booted\r\n");
}

/**
 * @brief Check for and handle RISC-V alerts via IPC
 */
static void check_riscv_alerts(void)
{
	/* Read alert count from shared memory */
	no_os_barrier_full();
	uint32_t new_count = g_ipc_table->alert_count;

	/* Check if alert count increased */
	if (new_count > arm_alert_count) {
		arm_alert_count = new_count;
		/* This message is triggered by the RISC-V: it detected the ALERT pin,
		 * wrote the counter to shared memory, and rang the ARM doorbell.
		 * The ARM woke up, read the counter, and prints this line. */
		printf("\r\n[RV32->ARM] OVERCURRENT ALERT (RISC-V detected ALERT pin, notified ARM via IPC doorbell, count=%lu)\r\n",
		       (unsigned long)arm_alert_count);
	}
}

/**
 * @brief Initialize shared IPC table
 */
static void init_ipc_table(void)
{
	volatile ltc4284_ipc_table_t *tbl = g_ipc_table;

	/* Zero the entire table first, then set individual fields */
	volatile uint32_t *p = (volatile uint32_t *)tbl;
	for (unsigned i = 0; i < sizeof(*tbl) / sizeof(uint32_t); i++)
		p[i] = 0;
	no_os_barrier_full();

	/* Validate the table */
	tbl->magic = LTC4284_IPC_MAGIC;
	no_os_barrier_full();

	printf("[ARM] IPC table initialized at 0x%08lx\r\n", (unsigned long)tbl);
}

/**
 * @brief Initialize IPC (doorbell + mailbox)
 */
static int init_ipc(void)
{
	int ret;
	struct no_os_ipc_init_param ipc_param = {
		.platform_ops = &max_ipc_ops,
		.extra = NULL
	};

	ret = no_os_ipc_init(&ipc_desc, &ipc_param);
	if (ret) {
		printf("[ARM] ERROR: IPC init failed (%d)\r\n", ret);
		return ret;
	}

	printf("[ARM] IPC initialized (doorbell + mailbox)\r\n");
	return 0;
}

/**
 * @brief Send command to RISC-V and wait for response
 */
static int send_command(uint32_t opcode, uint32_t param1, uint32_t param2)
{
	volatile ltc4284_ipc_table_t *tbl = g_ipc_table;
	uint32_t start_ms;
	uint32_t expected_seq;

	/* Wait for RISC-V ready */
	start_ms = 0; /* Simplified timeout for no_os_mdelay */
	while (tbl->status & LTC4284_STATUS_BUSY) {
		if (start_ms++ > 100) {
			printf("[ARM] ERROR: RISC-V busy timeout\r\n");
			return -ETIMEDOUT;
		}
		no_os_udelay(1000);
	}

	/* Send command */
	expected_seq = tbl->cmd_sequence + 1;
	tbl->cmd_opcode = opcode;
	tbl->cmd_param1 = param1;
	tbl->cmd_param2 = param2;
	tbl->cmd_sequence = expected_seq;
	no_os_barrier_full();

	/* Notify RISC-V */
	no_os_ipc_notify(ipc_desc, NO_OS_IPC_CHAN_COPROC);

	/* Wait for response */
	start_ms = 0;
	while (tbl->rsp_sequence != expected_seq) {
		if (start_ms++ > 100) {
			printf("[ARM] ERROR: Response timeout\r\n");
			return -ETIMEDOUT;
		}
		no_os_udelay(1000);
	}

	no_os_barrier_full();
	return (tbl->rsp_error_code == 0) ? 0 : -EIO;
}

/**
 * @brief Request and display telemetry from RISC-V
 */
static void display_telemetry(void)
{
	volatile ltc4284_ipc_table_t *tbl = g_ipc_table;
	int ret;

	/* [ARM->RV32] ARM writes command to shared RAM and rings RISC-V doorbell */
	ret = send_command(LTC4284_CMD_READ_TELEMETRY, 0, 0);
	if (ret) {
		printf("[ARM] ERROR: Telemetry request failed\r\n");
		return;
	}

	/* [RV32] RISC-V woke up, read LTC4284 over I2C, wrote results to shared RAM, rang ARM doorbell
	 * [ARM] ARM reads those results from shared RAM and prints them below */
	no_os_barrier_full();

	uint8_t sys = tbl->telemetry.status_reg;
	uint8_t faults = tbl->telemetry.fault_reg;
	bool fet_on = !!(sys & LTC4284_SYSTEM_STATUS_FET_ON_STATUS);
	bool pg_on  = !!(sys & LTC4284_SYSTEM_STATUS_PG_STATUS);

	printf("[ARM] VIN=%lu.%03luV  IIN=%lu.%03luA  PIN=%lu.%03luW  VOUT=%lu.%03luV  VDS=%lu.%03luV  "
	       "FET=%u PG=%u  Alerts=%lu\r\n",
	       (unsigned long)(tbl->telemetry.vin_mv / 1000),
	       (unsigned long)(tbl->telemetry.vin_mv % 1000),
	       (unsigned long)(tbl->telemetry.iin_ma / 1000),
	       (unsigned long)(tbl->telemetry.iin_ma % 1000),
	       (unsigned long)(tbl->telemetry.pin_mw / 1000),
	       (unsigned long)(tbl->telemetry.pin_mw % 1000),
	       (unsigned long)(tbl->telemetry.vout_mv / 1000),
	       (unsigned long)(tbl->telemetry.vout_mv % 1000),
	       (unsigned long)(tbl->telemetry.vds_mv / 1000),
	       (unsigned long)(tbl->telemetry.vds_mv % 1000),
	       fet_on, pg_on,
	       (unsigned long)tbl->alert_count);

	/* Narrate state transitions */
	if (faults && faults != prev_faults) {
		decode_faults(faults);
		printf("  [RV32] FET turned off. Waiting for auto-retry...\r\n");
		(void)send_command(LTC4284_CMD_CLEAR_FAULTS, 0, 0);
	}
	if (fet_on && !fet_was_on)
		printf("  --> Retry succeeded. FET back on.\r\n");
	if (!fet_on && fet_was_on)
		printf("  --> FET is OFF.\r\n");

	prev_faults = faults;
	fet_was_on = fet_on;
}

/**
 * @brief Request RISC-V's config snapshot and print the OC banner
 */
static void print_startup_summary(void)
{
	volatile ltc4284_ipc_table_t *tbl = g_ipc_table;
	int ret;

	ret = send_command(LTC4284_CMD_READ_CONFIG_REGS, 0, 0);
	if (ret) {
		printf("[ARM] ERROR: config-snapshot request failed\r\n");
		return;
	}

	no_os_barrier_full();
	printf("==== LTC4284 OC Config ====\r\n");
	printf("  RSENSE          : %lu uohm\r\n",
	       (unsigned long)tbl->config.rsense_uohm);
	printf("  CONFIG_1        : 0x%02X (after programming)\r\n",
	       tbl->config.cfg1);
	printf("  CONTROL_2       : 0x%02X (after programming)\r\n",
	       tbl->config.ctrl2);
	printf("  V_ILIM          : %u mV\r\n", tbl->config.ilim_mv);
	printf("  V_ILIM(FAST)    : %u mV (auto 2x)\r\n",
	       tbl->config.ilim_mv * 2);
	printf("  Trip (steady)   : ~%lu mA\r\n",
	       (unsigned long)tbl->config.trip_ma);
	printf("  Trip (fast)     : ~%lu mA\r\n",
	       (unsigned long)(tbl->config.trip_ma * 2));
	printf("  OC retry policy : %s\r\n",
	       retry_str(tbl->config.retry_code));
	printf("=========================\r\n\r\n");
}

/**
 * @brief Main dual-core example
 */
int example_main(void)
{
	int ret;
	uint32_t start_ms;

	printf("\r\n====================================================\r\n");
	printf("  LTC4284 Dual-Core Monitoring Example (MAX32690)  \r\n");
	printf("====================================================\r\n");
	printf("ARM Core:    User interface and telemetry display\r\n");
	printf("RISC-V Core: I2C control and OC monitoring\r\n");
	printf("----------------------------------------------------\r\n\r\n");

	/* Initialize shared IPC table */
	init_ipc_table();

	/* Boot RISC-V coprocessor */
	boot_riscv_coprocessor();

	/* Give RISC-V time to initialize */
	no_os_mdelay(100);

	/* Initialize IPC */
	ret = init_ipc();
	if (ret)
		return ret;

	/* Wait for RISC-V ready signal */
	printf("[ARM] Waiting for RISC-V initialization...\r\n");
	start_ms = 0;
	while (!(g_ipc_table->status & LTC4284_STATUS_READY)) {
		if (g_ipc_table->status & LTC4284_STATUS_ERROR) {
			printf("[ARM] ERROR: RISC-V init failed (error code: %lu)\r\n",
			       (unsigned long)g_ipc_table->rsp_error_code);
			return -EIO;
		}
		if (start_ms++ > 5000) {
			printf("[ARM] ERROR: RISC-V timeout (status=0x%02lx)\r\n",
			       (unsigned long)g_ipc_table->status);
			return -ETIMEDOUT;
		}
		no_os_mdelay(1);
	}
	printf("[ARM] RISC-V core ready\r\n\r\n");

	/* Print OC configuration banner (fetches CFG1/CTRL2 from RISC-V) */
	print_startup_summary();

	/* Main monitoring loop */
	printf("[ARM] Starting telemetry monitoring (bring load above trip to observe OC)...\r\n\r\n");
	while (1) {
		/* Check for alerts from RISC-V */
		check_riscv_alerts();

		/* Display telemetry */
		display_telemetry();

		no_os_mdelay(1000);
	}

	return 0;
}
