/*******************************************************************************
 *   @file   maxim_capi_rtc_priv.h
 *   @brief  Header file for private RTC handle
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_RTC_PRIV_H_
#define MAXIM_CAPI_RTC_PRIV_H_

#include "maxim_capi_rtc.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/** Number of alarm channels (enum max_capi_rtc_alarm_id) */
#define MAX_CAPI_RTC_NUM_ALARMS		(MAX_CAPI_RTC_ALARM_SSEC + 1)

/**
 * @struct max_capi_rtc_priv
 * @brief Private RTC data structure
 */
struct max_capi_rtc_priv {
	/** Global event callback */
	capi_rtc_event_callback callback;
	/** Argument passed to the global event callback */
	void *arg;
	/** Per-alarm callbacks */
	capi_rtc_event_callback alarm_callback[MAX_CAPI_RTC_NUM_ALARMS];
	/** Arguments passed to the per-alarm callbacks */
	void *alarm_arg[MAX_CAPI_RTC_NUM_ALARMS];
	/** Alarms fired in the ISR that no callback consumed */
	volatile bool alarm_pending[MAX_CAPI_RTC_NUM_ALARMS];
	/** Bitmask of events enabled through capi_rtc_irq_enable() */
	uint32_t irq_events;
	/** RTC IRQ connected and enabled in capi_irq (NVIC gated separately) */
	bool irq_connected;
	/** RTC alarm wake-up enabled through capi_rtc_wakeup_enable() */
	bool wakeup_enabled;
	/** Square-wave output frequency, driven while the counter runs */
	enum max_capi_rtc_sqwave_freq sqwave;
};

#define CAPI_RTC_HANDLE_MAXIM_INIT()			\
	(&(struct capi_rtc_handle) {			\
		.ops = NULL,				\
		.init_allocated = false,		\
		.priv = &(struct max_capi_rtc_priv){0}	\
	})

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_RTC_PRIV_H_ */
