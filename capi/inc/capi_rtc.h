/*
 * Copyright (c) 2024-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The Real-time Clock API
 */

#ifndef CAPI_RTC_H_
#define CAPI_RTC_H_

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

struct capi_rtc_time {
	/** Seconds count */
	uint32_t sec;
	/** Subseconds count */
	uint32_t subsec;
	/** Subsecond resolution */
	uint32_t subsec_resolution;
};

struct capi_rtc_datetime {
	/** Year (e.g. 2026 )*/
	uint16_t year;
	/** Month (1-12) */
	uint8_t month;
	/** Day of month (1-31) */
	uint8_t day;
	/** Hour (0-23) */
	uint8_t hour;
	/** Minute (0-59) */
	uint8_t minute;
	/** Second (0-59) */
	uint8_t second;
	/** Subseconds count */
	uint32_t subsec;
	/** Subsecond resolution */
	uint32_t subsec_resolution;
};

enum capi_rtc_alarm_type {
	/** Alarm based on seconds count (uses capi_rtc_time) */
	CAPI_RTC_ALARM_TIME,
	/** Alarm based on subseconds only */
	CAPI_RTC_ALARM_SUBSEC,
	/** Alarm based on calendar date/time (uses capi_rtc_datetime) */
	CAPI_RTC_ALARM_DATETIME,
	/** Can be used for platform-specific alarm types */
	CAPI_RTC_ALARM_LIMIT
};

enum capi_rtc_sqwave_freq {
	CAPI_RTC_SQWAVE_1HZ,
	CAPI_RTC_SQWAVE_512HZ,
	CAPI_RTC_SQWAVE_4KHZ,
	CAPI_RTC_SQWAVE_32KHZ,
	/** Can be used for platform-specific square wave frequencies */
	CAPI_RTC_SQWAVE_LIMIT
};

enum capi_rtc_event {
	/** Time-of-day alarm triggered */
	CAPI_RTC_EVENT_ALARM,
	/** Subsecond alarm triggered */
	CAPI_RTC_EVENT_SUBSEC_ALARM,
	/** RTC ready after initialization */
	CAPI_RTC_EVENT_READY,
	/** Can be used for platform-specific events */
	CAPI_RTC_EVENT_LIMIT
};

typedef void (*capi_rtc_event_callback_t)(struct capi_rtc_handle *handle,
		enum capi_rtc_event event,
		void *ctx);

struct capi_rtc_handle {
	/** If true, driver is owner of handle memory */
	bool init_allocated;
	/** Pointer to function implementation */
	const struct capi_rtc_ops *ops;
	/** Optional lock for thread safety */
	void *lock;
	/** Optional user-defined data */
	void *priv;
	/** Optional platform-specific data */
	void *extra;
};

struct capi_rtc_config {
	/** Unique identifier */
	uint64_t identifier;
	/** Initial time value in seconds */
	uint32_t initial_sec;
	/** Initial subseconds value */
	uint32_t initial_subsec;
	/** RTC clock frequency in Hz (e.g. 32768 or 32kHz crystal) */
	int32_t freq;
	/** Pointer to platform-specific function implementations */
	const struct capi_rtc_ops *ops;
	/** IRQ priority */
	uint32_t irq_priority;
	/** Optional platform-specific configuration */
	void *extra;
};

/**
 * @brief Initialize the RTC peripheral
 * @param handle Pointer to handle pointer (can be NULL for driver allocation)
 * @param config RTC configuration
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_init(struct capi_rtc_handle **handle,
		  const struct capi_rtc_config *config);

/**
 * @brief Deinitialize the RTC peripheral
 * @param handle The RTC handle
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_deinit(struct capi_rtc_handle *handle);

/**
 * @brief Start the RTC
 * @param handle The RTC handle
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_start(struct capi_rtc_handle *handle);

/**
 * @brief Stop the RTC
 * @param handle The RTC handle
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_stop(struct capi_rtc_handle *handle);

/**
 * @brief Get the current time
 * @param handle The RTC handle
 * @param time Pointer to store current time
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_get_time(struct capi_rtc_handle *handle,
		      struct capi_rtc_time *time);

/**
 * @brief Set the current time
 * @param handle The RTC handle
 * @param time Pointer to new time value
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_set_time(struct capi_rtc_handle *handle,
		      const struct capi_rtc_time *time);

/**
 * @brief Get the current datetime
 * @param handle The RTC handle
 * @param datetime Pointer to store current datetime
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_get_datetime(struct capi_rtc_handle *handle,
			  struct capi_rtc_datetime *datetime);

/**
 * @brief Set the current datetime
 * @param handle The RTC handle
 * @param datetime Pointer to new datetime value
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_set_datetime(struct capi_rtc_handle *handle,
			  const struct capi_rtc_datetime *datetime);

/**
 * @brief Set alarm
 * @param handle The RTC handle
 * @param type Alarm type
 * @param alarm_value Pointer to alarm value (uint32_t, capi_rtc_time, or
 * 		      capi_rtc_datetime depending on type)
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_set_alarm(struct capi_rtc_handle *handle,
		       enum capi_rtc_alarm_type type, const void *alarm_value);

/**
 * @brief Disable alarm
 * @param handle The RTC handle
 * @param type Alarm type
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_disable_alarm(struct capi_rtc_handle *handle,
			   enum capi_rtc_alarm_type);

/**
 * @brief Enable square wave output
 * @param handle The RTC handle
 * @param freq The square wave frequency
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_sqwave_enable(struct capi_rtc_handle *handle,
			   enum capi_rtc_sqwave_freq freq);

/**
 * @brief Disable square wave output
 * @param handle The RTC handle
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_sqwave_disable(struct capi_rtc_handle *handle);

/**
 * @brief Trim/calibrate the RTC frequency
 * @param handle The RTC handle
 * @param trim Trim value
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_trim(struct capi_rtc_handle *handle, int8_t trim);

/**
 * @brief Register event callback
 * @param handle The RTC handle
 * @param callback Callback function
 * @param event_ctx Context pointer for callback
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_register_callback(struct capi_rtc_handle *handle,
			       capi_rtc_event_callback_t callback,
			       void *event_ctx);

/**
 * @brief Enable RTC events
 * @param handle The RTC handle
 * @param events_mask Bitmask of events to enable (enum capi_rtc_event values)
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_enable_events(struct capi_rtc_handle *handle,
			   uint32_t events_mask);

/**
 * @brief Disable RTC events
 * @param handle The RTC handle
 * @param events_mask Bitmask of events to disable (enum capi_rtc_event values)
 * @return 0 on success, negative error code otherwise
 */
int capi_rtc_disable_events(struct capi_rtc_handle *handle,
			    uint32_t events_mask);

/**
 * @brief RTC interrupt handler
 * @param handle The RTC handle
 */
void capi_rtc_isr(void *handle);

struct capi_rtc_ops {
	/** See capi_rtc_init() */
	int (*init)(struct capi_rtc_handle **handle,
		    const struct capi_rtc_config *config);
	/** See capi_rtc_deinit() */
	int (*deinit)(struct capi_rtc_handle *handle);
	/** See capi_rtc_start() */
	int (*start)(struct capi_rtc_handle *handle);
	/** See capi_rtc_stop() */
	int (*stop)(struct capi_rtc_handle *handle);
	/** See capi_rtc_get_time() */
	int (*get_time)(struct capi_rtc_handle *handle,
			struct capi_rtc_time *time);
	/** See capi_rtc_set_time() */
	int (*set_time)(struct capi_rtc_handle *handle,
			const struct capi_rtc_time *time);
	/** See capi_rtc_get_datetime() */
	int (*get_datetime)(struct capi_rtc_handle *handle,
			    struct capi_rtc_datetime *datetime);
	/** See capi_rtc_set_datetime() */
	int (*set_datetime)(struct capi_rtc_handle *handle,
			    const struct capi_rtc_datetime *datetime);
	/** See capi_rtc_set_alarm() */
	int (*set_alarm)(struct capi_rtc_handle *handle,
			 enum capi_rtc_alarm_type type,
			 const void *alarm_value);
	/** See capi_rtc_disable_alarm() */
	int (*disable_alarm)(struct capi_rtc_handle *handle,
			     enum capi_rtc_alarm_type type);
	/** See capi_rtc_sqwave_enable() */
	int (*sqwave_enable)(struct capi_rtc_handle *handle,
			     enum capi_rtc_sqwave_freq freq);
	/** See capi_rtc_sqwave_disable() */
	int (*sqwave_disable)(struct capi_rtc_handle *handle);
	/** See capi_rtc_trim() */
	int (*trim)(struct capi_rtc_handle *handle, int8_t trim);
	/** See capi_rtc_register_callback() */
	int (*register_callback)(struct capi_rtc_handle *handle,
				 capi_rtc_event_callback_t callback,
				 void *event_ctx);
	/** See capi_rtc_enable_events() */
	int (*enable_events)(struct capi_rtc_handle *handle,
			     uint32_t events_mask);
	/** See capi_rtc_disable_events() */
	int (*disable_events)(struct capi_rtc_handle *handle,
			      uint32_t events_mask);
	/** See capi_rtc_isr() */
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif /* _cplusplus */

#endif /* CAPI_RTC_H_ */
