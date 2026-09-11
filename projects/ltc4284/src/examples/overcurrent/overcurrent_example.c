/***************************************************************************//**
 *   @file   overcurrent_example.c
 *   @brief  Overcurrent-protection demo for LTC4284 on DC2470A.
 *   @author Carlos Jones Jr <carlosjr.jones@analog.com>
 *   @author Christopher de Guzman <christopher.deguzman@analog.com>
********************************************************************************
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

/*
 * LTC4284 Overcurrent-Protection Demo
 * ====================================
 *
 * What this demonstrates:
 *   Configuring the LTC4284's steady-state current-limit and retry policy,
 *   observing the chip protect the load when current exceeds V_ILIM / RSENSE,
 *   and logging the retry / latch-off cycle.
 *
 * Hardware setup (DC2470A + MAX32690EVKIT):
 *   PSU  (+) --> DC2470A RTN (J3)
 *   PSU  (-) --> DC2470A -48VIN (J4)
 *   Load (+) --> DC2470A RTN
 *   Load (-) --> DC2470A -48VOUT  (NOT -48VIN -- that bypasses the FET)
 *
 *   REQUIRED: 10 AWG minimum leads with ring lugs at the trip current
 *   configured below. Alligator clips will burn at 48 A.
 *
 * With the defaults below (OC_ILIM_MV = 15, RSENSE = 312 uOhm on DC2470A):
 *   Steady-state trip:  15 mV / 312 uOhm  =  48 A
 *   Fast (short) trip:  30 mV / 312 uOhm  =  96 A
 *   Cooling delay:      512 ms (chip default)
 *   Retry:              1 retry, then latch off
 *
 * Expected sequence:
 *   1. Boot: FET on, PG asserted, VIN ~= 48000 mV, IIN ~= 0
 *   2. Ramp e-load to a few A: telemetry scales linearly
 *   3. Ramp past 48 A: chip trips, FET off, VOUT jumps toward VIN,
 *      FAULT = OC bit set, retry begins after cooling delay
 *   4. If load still above trip: second trip latches off. Manual clear
 *      via power cycle or ltc4284_clear_faults() to recover.
 *
 * KNOWN NON-ISSUE on weak bench PSUs / thin leads:
 *   If the PSU cannot cleanly supply the load current, VIN sags below
 *   the 43 V UV threshold when the load engages. The chip then trips
 *   on UV + POWER_FAILED (sometimes with a spurious OC bit set from
 *   the transient), which can look like a false OC trip at low current
 *   (observed ~500 mA on a small bench supply with alligator clips).
 *
 *   Signature: FAULT = 0x22 or 0x26 (UV + POWER_FAILED, sometimes with
 *   OC), and VIN is seen sagging in the log before the trip.
 *
 *   This is NOT an overcurrent event and NOT a code problem. A proper
 *   >60 A PSU with 10 AWG ring-lug leads will not exhibit it; VIN
 *   stays stable and the real OC trip fires at ~48 A as programmed.
 */

#include <stdio.h>
#include <stdlib.h>
#include "common_data.h"
#include "ltc4284.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/* --- OC configuration (change this to reprogram trip behavior) --- */
#define OC_ILIM_MV      15                      /* V_ILIM in mV, 15..30 */
#define OC_FOLDBACK     LTC4284_FB_50           /* Startup foldback */
#define OC_RETRY_POLICY LTC4284_RETRY_1         /* 1 retry, then latch */

/* --- Monitor loop ------------------------------------------------ */
#define LOOP_PERIOD_MS  1000                    /* Telemetry poll rate */

static void print_startup_summary(struct ltc4284_dev *dev)
{
	uint8_t cfg1 = 0, ctrl2 = 0;
	uint32_t trip_ma;

	ltc4284_read_byte(dev, LTC4284_REG_CONFIG_1, &cfg1);
	ltc4284_read_byte(dev, LTC4284_REG_CONTROL_2, &ctrl2);

	/* Steady-state trip current in mA: V_ILIM_mV * 1e6 / R_sense_uohm */
	trip_ma = (uint32_t)OC_ILIM_MV * 1000000UL / dev->rsense_uohm;

	pr_info("==== LTC4284 OC Demo ====\n");
	pr_info("  I2C address     : 0x%02X\n", dev->i2c_addr);
	pr_info("  RSENSE          : %lu uohm\n",
		(unsigned long)dev->rsense_uohm);
	pr_info("  VPWR divider    : %u:1\n", dev->vpwr_divider);
	pr_info("  CONFIG_1        : 0x%02X (after programming)\n", cfg1);
	pr_info("  CONTROL_2       : 0x%02X (after programming)\n", ctrl2);
	pr_info("  V_ILIM          : %u mV\n", OC_ILIM_MV);
	pr_info("  V_ILIM(FAST)    : %u mV (auto 2x)\n", OC_ILIM_MV * 2);
	pr_info("  Trip (steady)   : ~%lu mA\n", (unsigned long)trip_ma);
	pr_info("  Trip (fast)     : ~%lu mA\n", (unsigned long)(trip_ma * 2));
	pr_info("  OC retry policy : %s\n",
		OC_RETRY_POLICY == LTC4284_RETRY_LATCH_OFF ? "latch off" :
		OC_RETRY_POLICY == LTC4284_RETRY_1         ? "1 retry then latch" :
		OC_RETRY_POLICY == LTC4284_RETRY_7         ? "7 retries then latch" :
		"unlimited retries");
	pr_info("=========================\n\n");
}

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

	printf("  ** %s **\n", buf);
}

int example_main(void)
{
	struct ltc4284_dev *dev;
	struct no_os_uart_desc *uart;
	uint32_t vin_mv, iin_ma, vout_mv, power_mw;
	uint8_t sys, faults, prev_faults = 0;
	bool fet_was_on = true;
	int ret;

	ret = no_os_uart_init(&uart, &ltc4284_uart_ip);
	if (ret) {
		pr_err("UART init failed: %d\n", ret);
		return ret;
	}
	no_os_uart_stdio(uart);

	ret = ltc4284_init(&dev, &ltc4284_ip);
	if (ret) {
		pr_err("LTC4284 init failed: %d\n", ret);
		return ret;
	}

	/* Program the OC profile */
	ret = ltc4284_set_ilim_mv(dev, OC_ILIM_MV);
	if (ret) {
		pr_err("set_ilim failed: %d\n", ret);
		goto out;
	}

	ret = ltc4284_set_foldback(dev, OC_FOLDBACK);
	if (ret) {
		pr_err("set_foldback failed: %d\n", ret);
		goto out;
	}

	ret = ltc4284_set_oc_retry(dev, OC_RETRY_POLICY);
	if (ret) {
		pr_err("set_oc_retry failed: %d\n", ret);
		goto out;
	}

	print_startup_summary(dev);

	/* Ensure FET on (usually already on by EEPROM default) */
	ret = ltc4284_enable_fet(dev, true);
	if (ret) {
		pr_err("enable_fet failed: %d\n", ret);
		goto out;
	}
	no_os_mdelay(300); /* Wait for PG to assert */

	ltc4284_read_status(dev, &sys);
	pr_info("Post-enable STATUS: 0x%02X (FET_ON=%u PG=%u)\n\n", sys,
		!!(sys & LTC4284_SYSTEM_STATUS_FET_ON_STATUS),
		!!(sys & LTC4284_SYSTEM_STATUS_PG_STATUS));

	pr_info("Monitoring (bring the e-load above the trip point to observe OC):\n");
	pr_info("---------------------------------------------------------------\n");

	while (1) {
		ltc4284_read_vin(dev, &vin_mv);
		ltc4284_read_iin(dev, &iin_ma);
		ltc4284_read_vout(dev, &vout_mv);
		ltc4284_read_power(dev, &power_mw);
		ltc4284_read_status(dev, &sys);
		ltc4284_get_fault(dev, &faults);

		pr_info("VIN=%6lu mV | IIN=%6lu mA | VOUT=%6lu mV | P=%8lu mW | FET=%u PG=%u\n",
			(unsigned long)vin_mv,
			(unsigned long)iin_ma,
			(unsigned long)vout_mv,
			(unsigned long)power_mw,
			!!(sys & LTC4284_SYSTEM_STATUS_FET_ON_STATUS),
			!!(sys & LTC4284_SYSTEM_STATUS_PG_STATUS));

		/* Narrate state transitions */
		if (faults && faults != prev_faults) {
			decode_faults(faults);
			pr_info("  FET turned off. Waiting for auto-retry...\n");
			ltc4284_clear_faults(dev);
		}

		bool fet_on = !!(sys & LTC4284_SYSTEM_STATUS_FET_ON_STATUS);
		if (fet_on && !fet_was_on)
			pr_info("  --> Retry succeeded. FET back on.\n");
		if (!fet_on && fet_was_on) {
			pr_info("  --> FET is OFF.\n");
		}

		prev_faults = faults;
		fet_was_on = fet_on;
		no_os_mdelay(LOOP_PERIOD_MS);
	}

out:
	ltc4284_remove(dev);
	return ret;
}
