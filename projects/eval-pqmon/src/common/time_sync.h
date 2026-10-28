/*******************************************************************************
 *   @file   time_sync.h
 *   @brief  UTC time base for PQLib from the host and the MAX31343 RTC
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __TIME_SYNC_H__
#define __TIME_SYNC_H__

#include <stdint.h>
#include "adi_pqlib.h"

/* Where the current PQLib time comes from */
enum time_sync_source {
	TIME_SOURCE_NONE,	/* relative time since measurement start */
	TIME_SOURCE_RTC,	/* MAX31343 at start-up, 1 s resolution */
	TIME_SOURCE_HOST,	/* written by the host (IIO system_time) */
};

enum time_sync_rtc_status {
	RTC_STATUS_ABSENT,	/* RTC not found on I2C */
	RTC_STATUS_INVALID,	/* RTC found, but its time is not set */
	RTC_STATUS_OK,
};

/**
 * @brief Initialize the MAX31343 RTC and take its time as the start-up
 * time base. The firmware keeps running if the RTC is absent.
 * @return 0 (the RTC state is reported by time_sync_rtc_status())
 */
int time_sync_init(void);

/**
 * @brief Set the time from the host: applied to PQLib at once (if the
 * measurement is running) and written to the RTC.
 * @param utcMs - UTC time in ms since 1970-01-01
 * @return 0 on success, negative error code otherwise
 */
int time_sync_set_host(ADI_PQLIB_TIME utcMs);

/**
 * @brief Keep the current PQLib time before a measurement restart or a
 * calibration, so it can be restored after adi_pqlib_Reset().
 */
void time_sync_save(void);

/**
 * @brief Set PQLib time after adi_pqlib_Reset(clearTime): the time base
 * plus the time elapsed since it was taken. No-op without a time source.
 */
void time_sync_apply_after_reset(void);

enum time_sync_source time_sync_source(void);
enum time_sync_rtc_status time_sync_rtc_status(void);

/**
 * @brief Correction applied by the last host write (new - old PQLib time).
 * @return correction in ms
 */
ADI_PQLIB_TIME time_sync_last_correction(void);

#endif /* __TIME_SYNC_H__ */
