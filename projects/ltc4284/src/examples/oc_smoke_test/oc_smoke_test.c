/***************************************************************************//**
 *   @file   oc_smoke_test.c
 *   @brief  Software-forced OC smoke test for LTC4284.
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
 * LTC4284 Overcurrent Smoke Test
 * ===============================
 *
 * Exercises the OC fault-decode, retry, and latch-off code paths WITHOUT
 * needing to physically push 48 A through the board. Uses the LTC4284's
 * writable FAULT register (0x04) to inject a simulated OC fault.
 *
 * Every 5 seconds this example writes 1<<2 (OC_FAULT bit) into register
 * 0x04. From the chip's perspective this is indistinguishable from a real
 * OC event — it turns off the FET, waits its cooling delay, and retries
 * per the programmed OC_RETRY policy.
 *
 * Used to validate:
 *   - Fault-register write path
 *   - decode_faults() output on every fault-bit type
 *   - ltc4284_clear_faults() actually clears the latched fault
 *   - CONTROL_2.OC_RETRY behavior (latch-off after N retries)
 *
 * Hardware setup: SAME as basic telemetry. No PSU required; you can even
 * run this with only the LTC4284 board's 5 V INTVCC-derived rail (if
 * externally powered) — the smoke test writes registers, no load current
 * needed. But: a real -48 V bus is still useful to see VIN sanity.
 *
 * Not intended for handoff — this is our own bench validation only.
 */

#include <stdio.h>
#include <stdlib.h>
#include "common_data.h"
#include "ltc4284.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

#define INJECT_PERIOD_MS  5000
#define OC_ILIM_MV        15
#define OC_FOLDBACK       LTC4284_FB_50
#define OC_RETRY_POLICY   LTC4284_RETRY_1

static void decode_faults(uint8_t faults)
{
	char buf[128];
	int n = 0;

	if (!faults)
		return;

	n += snprintf(buf + n, sizeof(buf) - n, "FAULT 0x%02X:", faults);
	if (faults & LTC4284_FAULT_OC)          n += snprintf(buf + n, sizeof(buf) - n,
				" OC");
	if (faults & LTC4284_FAULT_UV)          n += snprintf(buf + n, sizeof(buf) - n,
				" UV");
	if (faults & LTC4284_FAULT_OV)          n += snprintf(buf + n, sizeof(buf) - n,
				" OV");
	if (faults & LTC4284_FAULT_FET_BAD)     n += snprintf(buf + n, sizeof(buf) - n,
				" FET_BAD");
	if (faults & LTC4284_FAULT_FET_SHORT)   n += snprintf(buf + n, sizeof(buf) - n,
				" FET_SHORT");
	if (faults & LTC4284_FAULT_POWER_BAD)   n += snprintf(buf + n, sizeof(buf) - n,
				" POWER_FAILED");
	if (faults & LTC4284_FAULT_PGI)         n += snprintf(buf + n, sizeof(buf) - n,
				" PGI");
	if (faults & LTC4284_FAULT_EXT)         n += snprintf(buf + n, sizeof(buf) - n,
				" EXT");
	printf("  ** %s **\n", buf);
}

int example_main(void)
{
	struct ltc4284_dev *dev;
	struct no_os_uart_desc *uart;
	uint8_t sys, faults;
	int iter = 0;
	int ret;

	ret = no_os_uart_init(&uart, &ltc4284_uart_ip);
	if (ret) return ret;
	no_os_uart_stdio(uart);

	ret = ltc4284_init(&dev, &ltc4284_ip);
	if (ret) {
		pr_err("init failed: %d\n", ret);
		return ret;
	}

	ltc4284_set_ilim_mv(dev, OC_ILIM_MV);
	ltc4284_set_foldback(dev, OC_FOLDBACK);
	ltc4284_set_oc_retry(dev, OC_RETRY_POLICY);

	pr_info("==== LTC4284 OC Smoke Test ====\n");
	pr_info("  Injecting OC fault every %d ms via FAULT register write.\n",
		INJECT_PERIOD_MS);
	pr_info("  Retry policy: 1 retry then latch off.\n");
	pr_info("===============================\n\n");

	ltc4284_enable_fet(dev, true);
	no_os_mdelay(300);

	while (1) {
		iter++;

		/* Read current chip state */
		ltc4284_read_status(dev, &sys);
		ltc4284_get_fault(dev, &faults);

		printf("[iter %d]  STATUS=0x%02X (FET_ON=%u PG=%u)\n",
		       iter, sys,
		       !!(sys & LTC4284_SYSTEM_STATUS_FET_ON_STATUS),
		       !!(sys & LTC4284_SYSTEM_STATUS_PG_STATUS));

		if (faults) {
			decode_faults(faults);
			printf("  --> clearing faults + re-enabling FET.\n");
			ltc4284_clear_faults(dev);
			ltc4284_enable_fet(dev, true);
		}

		/* Inject a synthetic OC fault */
		printf("  --> INJECTING OC fault (writing 0x04 to FAULT)\n");
		ltc4284_write_byte(dev, LTC4284_REG_FAULT,
				   LTC4284_FAULT_OC_FAULT_BIT);

		no_os_mdelay(INJECT_PERIOD_MS);
	}

	ltc4284_remove(dev);
	return 0;
}