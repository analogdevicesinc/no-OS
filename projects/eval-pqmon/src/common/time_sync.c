/*******************************************************************************
 *   @file   time_sync.c
 *   @brief  UTC time base for PQLib from the host and the MAX31343 RTC
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/*
 * PQLib keeps its own time (ADI_PQLIB_TIME, ms) and advances it with the
 * processed line cycles. This module sets it to UTC:
 * - at start-up from the MAX31343 RTC (1 s resolution), if it holds a time;
 * - from the host, which writes UTC ms over IIO (system_time) at connect
 *   and then periodically to correct drift. The RTC is updated as well.
 * adi_pqlib_Reset(clearTime) on a measurement restart sets PQLib time to 0,
 * so the time is saved before and restored after it.
 */

#include <stdbool.h>
#include "time_sync.h"
#include "pqlib_example.h"
#include "pq_events.h"
#include "common_data.h"
#include "max31343.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "max32650.h"

static struct max31343_dev *rtcDev;
static enum time_sync_rtc_status rtcStatus = RTC_STATUS_ABSENT;
static enum time_sync_source timeSource = TIME_SOURCE_NONE;

/* Time base: UTC ms at system tick baseTick (ms) */
static ADI_PQLIB_TIME baseMs;
static uint64_t baseTick;
/* PQLib time was set from the base since the last reset */
static bool applied;
static ADI_PQLIB_TIME lastCorrection;

static uint64_t tick_ms(void)
{
	struct no_os_time t = no_os_get_time();

	return (uint64_t)t.s * 1000 + t.us / 1000;
}

/* Days since 1970-01-01 for a proleptic Gregorian date (H. Hinnant) */
static int64_t days_from_civil(int64_t y, unsigned m, unsigned d)
{
	int64_t era;
	unsigned yoe, doy, doe;

	y -= m <= 2;
	era = (y >= 0 ? y : y - 399) / 400;
	yoe = (unsigned)(y - era * 400);
	doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
	doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;

	return era * 146097 + (int64_t)doe - 719468;
}

/* Inverse of days_from_civil */
static void civil_from_days(int64_t z, int64_t *y, unsigned *m, unsigned *d)
{
	int64_t era;
	unsigned doe, yoe, doy, mp;

	z += 719468;
	era = (z >= 0 ? z : z - 146096) / 146097;
	doe = (unsigned)(z - era * 146097);
	yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
	doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
	mp = (5 * doy + 2) / 153;
	*d = doy - (153 * mp + 2) / 5 + 1;
	*m = mp < 10 ? mp + 3 : mp - 9;
	*y = (int64_t)yoe + era * 400 + (*m <= 2);
}

static int64_t rtc_to_unix(const struct max31343_time_stamp *ts)
{
	int64_t days = days_from_civil(ts->year, ts->mon + 1, ts->day);

	return days * 86400 + ts->hr * 3600 + ts->min * 60 + ts->sec;
}

static struct max31343_time_stamp unix_to_rtc(int64_t sec)
{
	struct max31343_time_stamp ts;
	int64_t days = sec / 86400;
	int64_t rem = sec % 86400;
	int64_t y;
	unsigned m, d;

	if (rem < 0) {
		rem += 86400;
		days--;
	}
	civil_from_days(days, &y, &m, &d);
	ts.year = (uint16_t)y;
	ts.mon = (uint8_t)(m - 1);
	ts.day = (uint8_t)d;
	ts.hr = (uint8_t)(rem / 3600);
	ts.min = (uint8_t)(rem / 60 % 60);
	ts.sec = (uint8_t)(rem % 60);

	return ts;
}

int time_sync_init(void)
{
	struct max31343_init_param ip = {
		.i2c_init = &rtc_i2c_ip,
	};
	struct max31343_time_stamp ts;
	uint8_t st = 0;

	/*
	 * The tick bridges a measurement restart (see time_sync_save()).
	 * At the default lowest priority it loses ~9 % of its ticks while
	 * the AFE GPIO interrupt (priority 1) reads waveform pages. Its
	 * handler is a few instructions, so it may preempt the AFE reads.
	 */
	NVIC_SetPriority(SysTick_IRQn, 0);

	if (max31343_init(&rtcDev, ip)) {
		rtcDev = NULL;
		rtcStatus = RTC_STATUS_ABSENT;
		return 0;
	}

	/* OSF: the oscillator stopped (e.g. no backup supply), time lost */
	if (max31343_reg_read(rtcDev, MAX31343_R_STATUS, &st) ||
	    (st & MAX31343_F_STATUS_OSF) ||
	    max31343_reg_read_time_stamp(rtcDev, &ts) || ts.year < 2024 ||
	    ts.mon > 11 || ts.day < 1 || ts.day > 31) {
		rtcStatus = RTC_STATUS_INVALID;
		return 0;
	}

	rtcStatus = RTC_STATUS_OK;
	baseMs = rtc_to_unix(&ts) * 1000;
	baseTick = tick_ms();
	timeSource = TIME_SOURCE_RTC;

	return 0;
}

int time_sync_set_host(ADI_PQLIB_TIME utcMs)
{
	ADI_PQLIB_HANDLE h = pqlibExample.hDevice;
	ADI_PQLIB_TIME old = 0;
	struct max31343_time_stamp ts;

	if (utcMs <= 0)
		return -EINVAL;

	baseMs = utcMs;
	baseTick = tick_ms();
	timeSource = TIME_SOURCE_HOST;

	if (pqlibExample.state == PQLIB_STATE_RUNNING) {
		adi_pqlib_GetTime(h, &old);
		/* uncertainty 0: an explicit host set always applies */
		adi_pqlib_SetTime(h, utcMs, 0, true);
		lastCorrection = utcMs - old;
		/* keep the durations of events that are in progress */
		pq_events_time_shift(lastCorrection);
		applied = true;
	}

	if (rtcDev) {
		ts = unix_to_rtc(utcMs / 1000);
		if (max31343_set_time_stamp(rtcDev, ts) == 0) {
			uint8_t st;

			/* the status flags (OSF) clear on read */
			max31343_reg_read(rtcDev, MAX31343_R_STATUS, &st);
			rtcStatus = RTC_STATUS_OK;
		}
	}

	return 0;
}

void time_sync_save(void)
{
	ADI_PQLIB_TIME now;

	/* Only a time that was set since the last reset is worth keeping;
	 * after it is saved once, PQLib time is stale until it is applied. */
	if (!applied)
		return;

	adi_pqlib_GetTime(pqlibExample.hDevice, &now);
	baseMs = now;
	baseTick = tick_ms();
	applied = false;
}

void time_sync_apply_after_reset(void)
{
	ADI_PQLIB_TIME now;

	if (timeSource == TIME_SOURCE_NONE)
		return;

	now = baseMs + (ADI_PQLIB_TIME)(tick_ms() - baseTick);
	adi_pqlib_SetTime(pqlibExample.hDevice, now, 0, true);
	applied = true;
}

enum time_sync_source time_sync_source(void)
{
	return timeSource;
}

enum time_sync_rtc_status time_sync_rtc_status(void)
{
	return rtcStatus;
}

ADI_PQLIB_TIME time_sync_last_correction(void)
{
	return lastCorrection;
}
