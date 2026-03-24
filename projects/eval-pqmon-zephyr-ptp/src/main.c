/*******************************************************************************
 *   @file   main.c
 *   @brief  PQMON Zephyr Application
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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
 * Combines PTP time synchronization with ADE9430 power quality monitoring.
 *
 * Boot flow:
 *   1. Wait for network interface
 *   2. Start IIO server (TCP or USB)
 *   3. Verify PTP clock + RTC devices
 *   4. Init TS_CAPT, 1PPS, TS_TIMER IRQ
 *   5. Init ADE9430 AFE + PQLib, start measurement
 *   6. Enable PQLib 1PPS sync + arm RTC sync
 *   7. Main loop: poll AFE, process PQLib, log PTP status
 *
 * PQ measurements start immediately — PTP convergence happens in
 * the background.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/ptp_time.h>
#include <zephyr/drivers/ptp_clock.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <time.h>

#include "ptp_clock_adin1110_hw.h"
#include "rtc_max31343.h"
#include "pqlib_example.h"
#include "pqlib_afe.h"
#include "afe_calibration.h"
#include "flash_storage.h"
#include "iio_server.h"
#include "ptp_status.h"
#include "afe_sync.h"
#include "uart_export.h"

LOG_MODULE_REGISTER(pqmon_app, LOG_LEVEL_INF);

/* 1PPS configuration: 500ms high + 500ms low = 1Hz */
#define TS_TIMER_1PPS_HI_NS    500000000U
#define TS_TIMER_1PPS_LO_NS    500000000U
#define TS_TIMER_START_NS      16U

/* TS_TIMER re-alignment: wait until PTP offset is below 200ns.
 * The 1PPS phase is locked at re-alignment time, so this must be tight. */
#define TS_TIMER_REALIGN_THRESHOLD_NS  200
/* Require this many consecutive readings below threshold before
 * re-aligning. Prevents false triggers during PI servo oscillation. */
#define TS_TIMER_STABLE_COUNT          4

/* Minimum uptime before trusting GRAND_MASTER convergence.
 * BMCA resolves in ~5-10s (3 announce intervals at 1Hz).
 * 20s gives margin — if still GM after this, it's the real GM. */
#define PTP_BMCA_SETTLE_MS  20000

#define PTP_CLOCK_NODE DT_INST(0, adi_adin1110_ptp_clock)
static const struct device *ptp_clock_dev = DEVICE_DT_GET(PTP_CLOCK_NODE);

#define RTC_NODE DT_NODELABEL(max31343)
static const struct device *rtc_dev = DEVICE_DT_GET(RTC_NODE);

/* Declared in pqlib_example.c - PTP time in ms for SyncLibTime */
extern volatile int64_t ptp_time_ms;
extern volatile uint8_t newSyncTimeAvailable;

static void wait_for_network(void)
{
	struct net_if *iface;
	int timeout = 100;

	do {
		k_sleep(K_MSEC(100));
		iface = net_if_get_default();
		timeout--;
	} while ((!iface || !net_if_is_up(iface)) && timeout > 0);

	if (!iface || !net_if_is_up(iface)) {
		LOG_ERR("Network interface failed to initialize");
	}
}

/**
 * @brief Update PQLib time from PTP syntonized counter via TS_CAPT
 */
static void update_pqlib_time(void)
{
	struct net_ptp_time capt;

	if (adin1110_ptp_capture_timestamp(&capt) == 0) {
		ptp_time_ms = (int64_t)capt.second * 1000LL +
			      capt.nanosecond / 1000000;
		newSyncTimeAvailable = 1;
	}
}

int main(void)
{
	bool ts_timer_enabled = false;
	bool ts_capt_ready = false;
	bool rtc_ready = false;
	bool rtc_irq_sync_ready = false;
	bool afe_initialized = false;
	bool pqlib_initialized = false;
	bool measurement_started = false;
	bool events_enabled = false;
	bool ts_timer_realigned = false;
	bool afe_sync_inited = false;
	bool afe_sync_ready_sent = false;
	int64_t uart_export_next_ms = 0;
	int ts_timer_stable_count = 0;
	int status;
	int ret;

	printk("\n\n=== PQMON BOOT ===\n\n");
	printf("PQMON: starting (PTP priority1=%d)\n",
	       CONFIG_PTP_PRIORITY1);

	/* --- Phase 1: Network + IIO server --- */
	wait_for_network();

	ret = iio_server_init();
	if (ret == 0) {
		printf("IIO: server started OK\n");
	} else {
		printf("IIO: server init FAILED (%d)\n", ret);
	}

	/* UART2 CSV export */
	ret = uart_export_init();
	if (ret != 0) {
		printf("UART2: CSV export disabled (%d)\n", ret);
	}

	/* --- Phase 2: Verify devices --- */
	if (!device_is_ready(ptp_clock_dev)) {
		printf("PTP: clock device not ready\n");
		return -ENODEV;
	}
	printf("PTP: clock device ready\n");

	if (!device_is_ready(rtc_dev)) {
		printf("RTC: MAX31343 not ready - sync disabled\n");
	} else {
		rtc_ready = true;
		printf("RTC: MAX31343 ready\n");
	}

	/* --- Phase 3: Init 1PPS, TS_CAPT, TS_TIMER IRQ --- */
	ret = adin1110_ptp_ts_capt_init();
	if (ret == 0) {
		ts_capt_ready = true;
		printf("TS_CAPT: initialized (P2.30)\n");
	} else {
		printf("TS_CAPT: init FAILED (%d)\n", ret);
	}

	ret = adin1110_ptp_configure_ts_timer(
		TS_TIMER_1PPS_HI_NS, TS_TIMER_1PPS_LO_NS, TS_TIMER_START_NS);
	if (ret == 0) {
		ts_timer_enabled = true;
		printf("1PPS: enabled on P2.9\n");
	} else {
		printf("1PPS: enable FAILED (%d)\n", ret);
	}

	if (rtc_ready && ts_capt_ready && ts_timer_enabled) {
		ret = adin1110_ptp_ts_timer_irq_init(rtc_dev);
		if (ret == 0) {
			rtc_irq_sync_ready = true;
			printf("TS_TIMER: IRQ sync ready\n");
		} else {
			printf("TS_TIMER: IRQ init FAILED (%d)\n", ret);
		}
	}

	/* --- Phase 4: Init AFE + PQLib --- */
	printf("AFE: initializing...\n");
	status = afe_init();
	if (status == 0) {
		afe_initialized = true;
		printf("AFE: init OK\n");
	} else {
		printf("AFE: init FAILED (%d)\n", status);
	}

	if (afe_initialized) {
		printf("PQLib: opening...\n");
		status = pqm_measurement_init();
		if (status == 0) {
			pqlib_initialized = true;
			printf("PQLib: init OK\n");
		} else {
			printf("PQLib: init FAILED (%d)\n", status);
		}
	}

	/* --- Phase 5: Start measurement + enable sync --- */
	if (pqlib_initialized && ts_capt_ready) {
		update_pqlib_time();
		printf("PQLib: starting measurement...\n");
		status = pqm_start_measurement(false);
		if (status == 0) {
			measurement_started = true;
			printf("PQLib: measurement started\n");

			/* Start dedicated AFE processing thread.
			 * This runs pqm_one_cycle() in a tight cooperative
			 * loop so we never miss an RMSONERDY event. */
			pqm_afe_thread_start();

			/* Enable 1PPS-driven PQLib time sync.
			 * Even before PTP converges, the initial seed
			 * time provides a reasonable epoch. PTP will
			 * correct it as sync messages arrive. */
			adin1110_ptp_enable_pqlib_sync();
		} else {
			printf("PQLib: start FAILED (%d)\n", status);
		}
	}

	/* Arm RTC sync — will fire on first 1PPS after PTP has time */
	if (rtc_irq_sync_ready) {
		adin1110_ptp_arm_rtc_sync();
		printf("RTC: sync armed\n");
	}

	/* --- Phase 6: Main loop --- */
	printf("Entering main loop (%s)\n",
	       measurement_started ? "PQM active" : "PQM init failed");

	while (1) {
		if (measurement_started) {
			/* AFE polling and PQLib processing now run in the
			 * dedicated AFE thread. Main loop only handles:
			 * - PTP convergence checks
			 * - TS_TIMER re-alignment
			 * - Event accumulation gating
			 * - AFE sync orchestration */

			/* Main loop is no longer time-critical */
			k_msleep(10);

			/* UART2 CSV export — once per second */
			if (k_uptime_get() >= uart_export_next_ms) {
				uart_export_send();
				uart_export_next_ms = k_uptime_get() + 1000;
			}
		} else {
			/* PQM init failed — just idle */
			k_sleep(K_SECONDS(5));
		}

		/* PQLib time sync is now handled by TS_TIMER 1PPS ISR —
		 * ptp_time_ms is updated every second at exact PTP boundary.
		 * SyncLibTime() in process_and_prepare_output() picks it up. */

		/* Event accumulation: enable once PTP is truly operational.
		 *
		 * TIME_RECEIVER: require servo_active (rate_adjust called
		 * by PI servo) + converged.
		 *
		 * GRAND_MASTER: the GM's servo never runs, so servo_active
		 * is never true. Instead, wait for BMCA to settle: if the
		 * board is still converged after PTP_BMCA_SETTLE_MS, it's
		 * the real GM (the slave would have transitioned to TR by
		 * then). BMCA resolves in ~5-10s; 20s gives margin. */
		if (!events_enabled) {
			struct ptp_sync_status ptp_st;

			if (ptp_get_sync_status(&ptp_st) == 0 &&
			    ptp_st.converged) {
				if (adin1110_ptp_servo_active() ||
				    k_uptime_get() > PTP_BMCA_SETTLE_MS) {
					pqlib_enable_event_accumulation();
					events_enabled = true;
				}
			}
		}

		/* Re-align TS_TIMER only on TIME_RECEIVER after the
		 * PI servo is active and offset has been stable below
		 * 200ns for TS_TIMER_STABLE_COUNT consecutive readings.
		 * A single reading below threshold can be a transient
		 * zero-crossing during the servo's damped oscillation.
		 * The GRAND_MASTER's TS_TIMER from boot is the reference;
		 * the slave aligns to it. */
		if (!ts_timer_realigned && ts_timer_enabled &&
		    adin1110_ptp_servo_active()) {
			struct ptp_sync_status ptp_st;

			if (ptp_get_sync_status(&ptp_st) == 0 &&
			    ptp_st.port_state == PTP_PORT_STATE_TIME_RECEIVER) {
				int64_t abs_off = ptp_st.offset_from_tt;

				if (abs_off < 0) {
					abs_off = -abs_off;
				}

				if (abs_off < TS_TIMER_REALIGN_THRESHOLD_NS) {
					ts_timer_stable_count++;
				} else {
					ts_timer_stable_count = 0;
				}

				if (ts_timer_stable_count >=
				    TS_TIMER_STABLE_COUNT) {
					adin1110_ptp_stop_ts_timer();
					k_msleep(10);
					ret = adin1110_ptp_configure_ts_timer(
						TS_TIMER_1PPS_HI_NS,
						TS_TIMER_1PPS_LO_NS,
						TS_TIMER_START_NS);
					if (ret == 0) {
						LOG_INF("TS_TIMER re-aligned "
							"(offset=%lldns "
							"delay=%lldns, "
							"stable=%d)",
							(long long)ptp_st.offset_from_tt,
							(long long)ptp_st.mean_delay,
							ts_timer_stable_count);
					}
					ts_timer_realigned = true;
				}
			}
		}

		/* AFE sync: initialize once we know our PTP role.
		 * Master (GRAND_MASTER) listens for slave READY.
		 * Slave (TIME_RECEIVER) will send READY after TS_TIMER realign. */
		if (!afe_sync_inited && events_enabled) {
			struct ptp_sync_status ptp_st;

			if (ptp_get_sync_status(&ptp_st) == 0 &&
			    ptp_st.converged) {
				bool is_master = (ptp_st.port_state !=
						  PTP_PORT_STATE_TIME_RECEIVER);
				const char *peer = is_master ?
					"192.0.2.2" : "192.0.2.1";
				ret = afe_sync_init(is_master, peer);
				if (ret == 0) {
					afe_sync_inited = true;
				} else {
					LOG_WRN("AFE sync init failed: %d "
						"(continuing without sync)",
						ret);
					afe_sync_inited = true;  /* Don't retry */
				}
			}
		}

		/* AFE sync: slave sends READY after TS_TIMER is realigned.
		 * This ensures PTP offset < 200ns and 1PPS edges are aligned
		 * before we attempt the synchronized AFE restart. */
		if (afe_sync_inited && ts_timer_realigned &&
		    !afe_sync_ready_sent && !afe_sync_is_complete()) {
			struct ptp_sync_status ptp_st;

			if (ptp_get_sync_status(&ptp_st) == 0 &&
			    ptp_st.port_state == PTP_PORT_STATE_TIME_RECEIVER) {
				afe_sync_send_ready();
				afe_sync_ready_sent = true;
			}
		}

	}

	return 0;
}
