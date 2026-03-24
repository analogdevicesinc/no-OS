/********************************************************************************
 *   @file   ptp_clock_adin1110_hw.h
 *   @brief  ADIN1110 PTP clock driver API header file
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
 ******************************************************************************/

#ifndef PTP_CLOCK_ADIN1110_HW_H_
#define PTP_CLOCK_ADIN1110_HW_H_

#include <zephyr/device.h>
#include <zephyr/net/ptp_time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Update stored hardware time from RX/TX frame timestamp
 *
 * Called by the ethernet driver every time a hardware timestamp is
 * captured (RX frame timestamp or TX TTSCAH/TTSCAL capture).
 * This is the ONLY way to read the ADIN1110 syntonized counter
 * since TS_SEC_CNT/TS_NS_CNT are write-only registers.
 *
 * @param sec  Seconds from hardware timestamp
 * @param nsec Nanoseconds from hardware timestamp
 */
void adin1110_ptp_update_hw_time(uint32_t sec, uint32_t nsec);

/**
 * @brief Configure TS_TIMER output signal
 *
 * Generates a repeating waveform on the TS_TIMER pin (P2.9) driven by
 * the syntonized counter. HI/LO values are auto-rounded to multiples
 * of 16 and the remainder is compensated via TS_TIMER_QE_CORR.
 *
 * @param high_ns  High period in nanoseconds
 * @param low_ns   Low period in nanoseconds
 * @param start_ns Nanosecond compare value for one-shot start trigger
 * @return 0 on success, negative error code on failure
 */
int adin1110_ptp_configure_ts_timer(uint32_t high_ns, uint32_t low_ns,
                                    uint32_t start_ns);

/**
 * @brief Stop TS_TIMER output
 *
 * Stops the TS_TIMER signal. Output returns to the default level.
 *
 * @return 0 on success, negative error code on failure
 */
int adin1110_ptp_stop_ts_timer(void);

/**
 * @brief Initialize TS_CAPT GPIO pin (P2.30)
 *
 * Configures the TS_CAPT trigger pin as output, idle low.
 * Must be called before adin1110_ptp_capture_timestamp().
 *
 * @return 0 on success, negative error code on failure
 */
int adin1110_ptp_ts_capt_init(void);

/**
 * @brief Capture current syntonized counter value via TS_CAPT
 *
 * Pulses P2.30 (rising edge triggers ADIN1110 to latch the syntonized
 * counter), then reads TS_CAPT_PIN_1 (seconds) and TS_CAPT_PIN_0
 * (nanoseconds). This provides on-demand counter reads independent
 * of PTP frame traffic.
 *
 * @param tm Pointer to store the captured timestamp
 * @return 0 on success, negative error code on failure
 */
int adin1110_ptp_capture_timestamp(struct net_ptp_time *tm);

/**
 * @brief Initialize TS_TIMER interrupt-driven RTC sync
 *
 * Sets up P2.9 as GPIO input with rising edge interrupt (reconfigured from
 * MSSEL output used during ADIN1110 bootstrap). Creates a cooperative thread
 * at K_PRIO_COOP(0) that performs the RTC update with scheduler locked.
 *
 * On each armed 1PPS pulse:
 *   ISR:    Pulse P2.30 → ADIN1110 latches syntonized counter (TS_CAPT)
 *   Thread: SPI read TS_CAPT → I2C SWRST + ENOSC + write time (no preemption)
 *
 * Must be called after:
 * - PTP convergence (TS_TIMER outputs aligned between boards)
 * - adin1110_ptp_configure_ts_timer() (TS_TIMER generating 1PPS on P2.9)
 * - adin1110_ptp_ts_capt_init() (P2.30 configured for TS_CAPT)
 *
 * @param rtc_dev MAX31343 RTC device
 * @return 0 on success, negative error code on failure
 */
int adin1110_ptp_ts_timer_irq_init(const struct device *rtc_dev);

/**
 * @brief Arm RTC sync to trigger on next TS_TIMER 1PPS pulse
 *
 * After arming, the next TS_TIMER rising edge will:
 * 1. ISR: Pulse P2.30 to latch syntonized counter (TS_CAPT)
 * 2. Thread: Read TS_CAPT via SPI, update RTC via I2C (SWRST + ENOSC)
 * 3. Automatically disarm after sync completes
 */
void adin1110_ptp_arm_rtc_sync(void);

/**
 * @brief Check if the last armed RTC sync has completed
 *
 * @return true if sync completed (or never armed), false if pending
 */
bool adin1110_ptp_rtc_sync_done(void);

/**
 * @brief Enable continuous PQLib time sync from TS_TIMER 1PPS
 *
 * When enabled, every 1PPS rising edge:
 *   1. ISR pulses TS_CAPT to latch syntonized counter
 *   2. Thread reads TS_CAPT via SPI
 *   3. Sets ptp_time_ms and newSyncTimeAvailable for SyncLibTime()
 *
 * This provides PQLib with precise PTP time at every second boundary.
 * Multiple boards sync PQLib to the same wall-clock instant (~100ns).
 */
void adin1110_ptp_enable_pqlib_sync(void);

/**
 * @brief Check if the PTP servo has started adjusting the clock.
 *
 * Returns true after the first clock_set() or rate_adjust() call from
 * the PTP servo, proving sync messages are being processed. Use this
 * to gate actions that depend on real PTP sync (not default offset=0).
 */
bool adin1110_ptp_servo_active(void);

/**
 * @brief Disable continuous PQLib time sync
 */
void adin1110_ptp_disable_pqlib_sync(void);

/**
 * @brief Arm the 1PPS handler for AFE sync restart.
 *
 * Called by afe_sync.c after the UDP handshake arms a target second.
 * The 1PPS thread will call afe_sync_check_1pps() on each edge
 * until the target second is reached.
 */
void adin1110_ptp_arm_afe_sync(void);

/**
 * @brief Check if 1PPS-triggered print is ready
 *
 * Returns true once per 1PPS edge (when pqlib_sync is enabled).
 * Both boards return true at the same wall-clock instant (~100ns),
 * so output is time-aligned across boards.
 *
 * @return true if ready (clears the flag), false otherwise
 */
bool adin1110_ptp_pqlib_print_ready(void);

#ifdef __cplusplus
}
#endif

#endif /* PTP_CLOCK_ADIN1110_HW_H_ */