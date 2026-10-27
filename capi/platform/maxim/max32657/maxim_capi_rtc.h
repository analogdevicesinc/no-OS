/*******************************************************************************
 *   @file   maxim_capi_rtc.h
 *   @brief  Header file for RTC functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_RTC_H_
#define MAXIM_CAPI_RTC_H_

#include <stdint.h>
#include <stdbool.h>
#include "capi_rtc.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/** RTC counter tick frequency (the SEC register increments once per second) */
#define MAX_CAPI_RTC_TICK_FREQ_HZ	1U

/** Sub-second counter frequency (12-bit SSEC register) */
#define MAX_CAPI_RTC_SSEC_FREQ_HZ	4096U

/** Number of significant bits matched by the time-of-day alarm */
#define MAX_CAPI_RTC_TOD_ALARM_BITS	20U

/**
 * @enum max_capi_rtc_alarm_id
 * @brief Alarm channels available on the MAX32657 RTC
 */
enum max_capi_rtc_alarm_id {
	/**
	 * Time-of-day alarm. Fires when SEC[19:0] matches the lower 20 bits
	 * of the programmed absolute seconds count, i.e. at most
	 * 2^20 seconds (~12.1 days) after it is set.
	 */
	MAX_CAPI_RTC_ALARM_TOD = 0,
	/**
	 * Sub-second alarm. Periodic alarm that fires every N sub-second
	 * ticks (1/4096 s), and reloads automatically.
	 */
	MAX_CAPI_RTC_ALARM_SSEC = 1,
};

/**
 * @enum max_capi_rtc_event
 * @brief MAX32657 specific RTC events
 */
enum max_capi_rtc_event {
	/** SEC/SSEC registers updated and safe to read */
	MAX_CAPI_RTC_EVENT_READY = CAPI_RTC_EVENT_LIMIT,
};

/**
 * @struct max_capi_rtc_alarm_extra
 * @brief MAX32657 alarm configuration, passed through
 * 	  capi_rtc_alarm_config.extra. The time and mask fields of
 * 	  capi_rtc_alarm_config are ignored by this driver.
 */
struct max_capi_rtc_alarm_extra {
	/**
	 * MAX_CAPI_RTC_ALARM_TOD: absolute seconds count to match. Only the
	 * lower MAX_CAPI_RTC_TOD_ALARM_BITS bits are compared by hardware.
	 * MAX_CAPI_RTC_ALARM_SSEC: alarm period in 1/4096 s ticks (non-zero).
	 */
	uint32_t value;
	/** Output of capi_rtc_alarm_get() only: whether the alarm is enabled */
	bool enabled;
};

/**
 * @enum max_capi_rtc_sqwave_freq
 * @brief Square-wave output frequencies
 */
enum max_capi_rtc_sqwave_freq {
	/** Square-wave output disabled */
	MAX_CAPI_RTC_SQWAVE_DISABLED = 0,
	MAX_CAPI_RTC_SQWAVE_1HZ,
	MAX_CAPI_RTC_SQWAVE_512HZ,
	MAX_CAPI_RTC_SQWAVE_4KHZ,
	MAX_CAPI_RTC_SQWAVE_32KHZ,
};

/**
 * @struct max_capi_rtc_extra
 * @brief MAX32657 RTC configuration, passed through capi_rtc_config.extra.
 * 	  A NULL extra pointer selects the defaults (all fields zero).
 */
struct max_capi_rtc_extra {
	/**
	 * Square-wave output frequency. The output is only driven while the
	 * counter runs: it is enabled by capi_rtc_start() and disabled by
	 * capi_rtc_stop(). It cannot be changed after init.
	 */
	enum max_capi_rtc_sqwave_freq sqwave;
};

extern const struct capi_rtc_ops max_capi_rtc_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_RTC_H_ */
