/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief The Real-Time Clock API
 *
 * Generic RTC interface modeled after the Zephyr RTC subsystem.
 * IP-specific details (prescaler divider format, alarm tick limits,
 * calibration reference clocks) are pushed into driver headers and
 * accessed via the @c extra pointer in the configuration structs.
 */

#ifndef _CAPI_RTC_H
#define _CAPI_RTC_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

/**
 * @brief RTC event types for interrupts and callbacks.
 *
 * Additional platform-specific events should be defined starting
 * from @ref CAPI_RTC_EVENT_LIMIT in the driver header.
 */
enum capi_rtc_event {
	CAPI_RTC_EVENT_TICK = 0,       /**< Periodic tick event */
	CAPI_RTC_EVENT_ALARM = 1,      /**< Alarm match event */
	CAPI_RTC_EVENT_CALIB_DONE = 2, /**< Calibration measurement complete */
	CAPI_RTC_EVENT_ERROR = 3,      /**< Fault/error condition */
	CAPI_RTC_EVENT_OVERFLOW = 4,   /**< Counter overflow (wraparound) */
	CAPI_RTC_EVENT_LIMIT           /**< Sentinel — IP-specific events start here */
};

/**
 * @brief RTC status bitmask flags.
 *
 * Defined as macros (not an enum) because they are combined as a bitmask.
 */
#define CAPI_RTC_STATUS_RUNNING       (0x01U) /**< Counter is actively incrementing */
#define CAPI_RTC_STATUS_ALARM_ACTIVE  (0x02U) /**< Alarm is enabled */
#define CAPI_RTC_STATUS_ALARM_PENDING (0x04U) /**< Alarm event occurred (not yet cleared) */
#define CAPI_RTC_STATUS_CALIB_BUSY    (0x08U) /**< Calibration measurement in progress */
#define CAPI_RTC_STATUS_CALIB_DONE    (0x10U) /**< Calibration measurement complete */
#define CAPI_RTC_STATUS_ERROR         (0x20U) /**< Error/fault condition detected */

/**
 * @brief Initialization flags for @ref capi_rtc_config.
 */
#define CAPI_RTC_INIT_FLAG_PRESERVE_COUNT                                                          \
	(0x01U) /**< Do not load initial_count; keep existing HW value */

/**
 * @brief Alarm time-field mask bits for @ref capi_rtc_alarm_config.
 *
 * Each bit selects a calendar field to participate in alarm matching.
 * A mask of 0 disables the alarm.  Platforms that use simple relative
 * tick limits instead of calendar matching should document this and
 * pass the limit via the alarm config's @c extra pointer.
 */
#define CAPI_RTC_ALARM_TIME_MASK_SECOND  (1U << 0)
#define CAPI_RTC_ALARM_TIME_MASK_MINUTE  (1U << 1)
#define CAPI_RTC_ALARM_TIME_MASK_HOUR    (1U << 2)
#define CAPI_RTC_ALARM_TIME_MASK_DAY     (1U << 3)
#define CAPI_RTC_ALARM_TIME_MASK_MONTH   (1U << 4)
#define CAPI_RTC_ALARM_TIME_MASK_WEEKDAY (1U << 5)

/**
 * @brief User callback for RTC events (interrupt mode).
 *
 * Signature matches @c capi_timer_event_callback.
 *
 * @param[in] event Which event fired. See @ref capi_rtc_event.
 * @param[in] arg User data pointer passed during registration.
 * @param[in] event_extra Platform-specific extra info (e.g., error code).
 */
typedef void (*capi_rtc_event_callback)(uint32_t event, void *arg,
					int event_extra);

/**
 * @brief Calendar time representation with sub-second precision.
 */
struct capi_rtc_calendar_time {
	uint16_t year;   /**< Full year (e.g., 2026) */
	uint8_t month;   /**< Month (1–12) */
	uint8_t day;     /**< Day of month (1–31) */
	uint8_t hour;    /**< Hour (0–23) */
	uint8_t minute;  /**< Minute (0–59) */
	uint8_t second;  /**< Second (0–59) */
	uint8_t weekday; /**< Day of week (0=Sunday … 6=Saturday) */
	uint32_t nsec;   /**< Nanoseconds (0–999999999). 0 if not supported. */
};

/**
 * @brief Generic tick-rate configuration.
 *
 * Use @c tick_freq_hz for a portable requested rate. Platform-specific
 * divider/modulator parameters are passed via @c extra.
 */
struct capi_rtc_tick_config {
	uint32_t tick_freq_hz; /**< Desired RTC tick frequency in Hz. 0 = platform default. */
	void *extra;           /**< Platform-specific divider/modulator config */
};

/**
 * @brief RTC initialization configuration.
 */
struct capi_rtc_config {
	const struct capi_rtc_ops *ops;
	uint64_t identifier;
	uint32_t clk_freq_hz;
	struct capi_rtc_tick_config tick;
	uint32_t initial_count;
	uint32_t flags; /**< See CAPI_RTC_INIT_FLAG_* */
	capi_rtc_event_callback callback;
	void *arg;
	void *extra; /**< Platform-specific driver config */
};

/**
 * @brief Alarm configuration.
 *
 * For calendar-match RTCs, populate @c time and @c mask.
 * For simple tick-limit RTCs, the driver ignores @c time/@c mask and
 * reads the limit from a driver-specific struct via @c extra.
 */
struct capi_rtc_alarm_config {
	struct capi_rtc_calendar_time time; /**< Alarm match time (absolute) */
	uint32_t mask;                      /**< Bitmask of fields to match. 0 = disable. */
	void *extra;                        /**< Platform-specific extension */
};

/**
 * @brief Calibration configuration.
 *
 * The calibration hardware simultaneously counts cycles of the LF clock
 * and a reference clock. Field semantics are platform-dependent.
 */
struct capi_rtc_calib_config {
	uint32_t ref_clk;         /**< Platform-defined reference clock identifier */
	uint32_t lf_target_count; /**< Number of LF cycles to count. Max is platform-dependent. */
	void *extra;
};

/**
 * @brief Calibration measurement result.
 */
struct capi_rtc_calib_result {
	uint32_t lf_count;  /**< Number of LF clock cycles counted */
	uint32_t ref_count; /**< Number of reference clock cycles counted */
};

/**
 * @brief RTC handle.
 */
struct capi_rtc_handle {
	const struct capi_rtc_ops *ops;
	bool init_allocated;
	void *priv;
};

/* ===================================================================
 * Core lifecycle
 * =================================================================== */

/**
 * @brief Initialize an instance of the RTC.
 *
 * Configures the tick rate and registers an optional event callback.
 * After init, the counter does not start until capi_rtc_start().
 *
 * If @ref CAPI_RTC_INIT_FLAG_PRESERVE_COUNT is set in config->flags,
 * the existing hardware counter value is kept and @c initial_count is
 * ignored.  This is required for battery-backed RTCs where a reset
 * would lose the stored time.
 *
 * @retval 0 Success
 * @retval -EINVAL NULL handle, NULL config, or NULL ops
 * @retval -ENOTSUP ops->init is NULL
 * @retval -EBUSY RTC already initialized
 */
int capi_rtc_init(struct capi_rtc_handle **handle,
		  const struct capi_rtc_config *config);

/**
 * @brief Deinitialize the RTC and free resources.
 *
 * Whether the counter stops, or alarm/IRQ state is cleared, is
 * platform-dependent.  On always-on RTCs the counter may continue
 * running after deinit.
 *
 * @retval 0 Success
 * @retval -EINVAL Invalid handle
 */
int capi_rtc_deinit(struct capi_rtc_handle *handle);

int capi_rtc_start(struct capi_rtc_handle *handle);
int capi_rtc_stop(struct capi_rtc_handle *handle);

/* ===================================================================
 * Counter access
 * =================================================================== */

/**
 * @brief Read the current RTC counter value.
 *
 * @param[out] count Current counter value.
 *
 * @retval 0 Success
 * @retval -EINVAL Invalid handle or NULL count pointer
 */
int capi_rtc_get_count(struct capi_rtc_handle *handle, uint32_t *count);

/**
 * @brief Load a new value into the RTC counter.
 *
 * Whether this function blocks until the value is synchronized to the
 * RTC clock domain or returns immediately is platform-dependent.
 *
 * @retval 0 Success
 * @retval -EINVAL Invalid handle
 */
int capi_rtc_set_count(struct capi_rtc_handle *handle, uint32_t count);

/* ===================================================================
 * Tick-rate configuration
 * =================================================================== */

int capi_rtc_set_tick_config(struct capi_rtc_handle *handle,
			     const struct capi_rtc_tick_config *config);

int capi_rtc_get_tick_config(struct capi_rtc_handle *handle,
			     struct capi_rtc_tick_config *config);

/**
 * @brief Query the effective tick frequency.
 *
 * @param[out] freq_hz Tick frequency in Hz.
 *
 * @retval 0 Success
 * @retval -ENOTSUP Not implemented by this driver
 */
int capi_rtc_get_tick_freq(struct capi_rtc_handle *handle, uint32_t *freq_hz);

/* ===================================================================
 * Alarm
 * =================================================================== */

/**
 * @brief Configure and enable an alarm.
 *
 * @param[in] id Alarm channel index (0-based). Single-alarm IPs
 *               support id=0 only and return -EINVAL for others.
 * @param[in] config Alarm configuration.
 *
 * @retval 0 Success
 * @retval -EINVAL Invalid handle, NULL config, or unsupported alarm id
 */
int capi_rtc_alarm_set(struct capi_rtc_handle *handle, uint16_t id,
		       const struct capi_rtc_alarm_config *config);

int capi_rtc_alarm_disable(struct capi_rtc_handle *handle, uint16_t id);

int capi_rtc_alarm_get(struct capi_rtc_handle *handle, uint16_t id,
		       struct capi_rtc_alarm_config *config);

/**
 * @brief Register a per-alarm callback. Pass NULL to unregister.
 */
int capi_rtc_alarm_set_callback(struct capi_rtc_handle *handle, uint16_t id,
				capi_rtc_event_callback callback, void *arg);

/* ===================================================================
 * Calibration — hardware measurement
 * =================================================================== */

/**
 * @brief Start a hardware calibration measurement (non-blocking).
 *
 * @retval 0 Measurement started
 * @retval -EBUSY Previous calibration still in progress
 * @retval -ENOTSUP Not implemented by this driver
 */
int capi_rtc_calibrate(struct capi_rtc_handle *handle,
		       const struct capi_rtc_calib_config *config);

int capi_rtc_get_calib_result(struct capi_rtc_handle *handle,
			      struct capi_rtc_calib_result *result);

/* ===================================================================
 * Calibration — generic PPB trim (Zephyr-style)
 * =================================================================== */

/**
 * @brief Apply clock trim in parts-per-billion.
 *
 * Positive values increase the clock frequency; negative decrease it.
 *
 * @retval 0 Success
 * @retval -ENOTSUP Not implemented by this driver
 */
int capi_rtc_set_calibration(struct capi_rtc_handle *handle, int32_t cal_ppb);

int capi_rtc_get_calibration(struct capi_rtc_handle *handle, int32_t *cal_ppb);

/* ===================================================================
 * IRQ / status
 * =================================================================== */

int capi_rtc_irq_enable(struct capi_rtc_handle *handle, uint32_t event);
int capi_rtc_irq_disable(struct capi_rtc_handle *handle, uint32_t event);

int capi_rtc_get_status(struct capi_rtc_handle *handle, uint32_t *status);
int capi_rtc_clear_status(struct capi_rtc_handle *handle, uint32_t status);

/**
 * @brief Test whether an interrupt is pending for a given event.
 *
 * @retval 0 Success (*pending written)
 * @retval -ENOTSUP Not implemented by this driver
 */
int capi_rtc_is_irq_pending(struct capi_rtc_handle *handle, uint32_t event,
			    bool *pending);

/* ===================================================================
 * Callback
 * =================================================================== */

int capi_rtc_register_callback(struct capi_rtc_handle *handle,
			       capi_rtc_event_callback callback,
			       void *arg);

/* ===================================================================
 * Wake-up
 * =================================================================== */

int capi_rtc_wakeup_enable(struct capi_rtc_handle *handle);
int capi_rtc_wakeup_disable(struct capi_rtc_handle *handle);

/* ===================================================================
 * Time (for hardware-calendar RTCs)
 * =================================================================== */

/**
 * @brief Set the RTC time from a calendar structure.
 *
 * For RTCs with hardware calendar registers. Counter-only RTCs leave
 * this ops slot NULL (-ENOTSUP).
 *
 * @retval 0 Success
 * @retval -ENOTSUP Not implemented (use set_count + calendar helpers)
 */
int capi_rtc_set_time(struct capi_rtc_handle *handle,
		      const struct capi_rtc_calendar_time *time);

int capi_rtc_get_time(struct capi_rtc_handle *handle,
		      struct capi_rtc_calendar_time *time);

/**
 * @brief Query whether the RTC holds a valid time.
 *
 * Returns false after a cold boot/battery loss until set_time or
 * set_count has been called.
 *
 * @retval 0 Success (*valid written)
 * @retval -ENOTSUP Not implemented by this driver
 */
int capi_rtc_is_time_valid(struct capi_rtc_handle *handle, bool *valid);

/* ===================================================================
 * Calendar utility functions (pure date math, no handle needed)
 * =================================================================== */

/**
 * @brief Convert a counter value (seconds since epoch) to calendar time.
 *
 * This is a common utility — not a driver op. Every platform can use it.
 * The @c nsec field of the output is set to 0.
 *
 * @param[in] count Seconds since the start of @c epoch_year.
 * @param[in] epoch_year Year that count=0 represents (e.g., 2000 or 1970).
 * @param[out] time Calendar time output.
 *
 * @retval 0 Success
 * @retval -EINVAL NULL time pointer
 */
int capi_rtc_count_to_calendar(uint32_t count, uint16_t epoch_year,
			       struct capi_rtc_calendar_time *time);

/**
 * @brief Convert a calendar time to a counter value (seconds since epoch).
 *
 * @retval 0 Success
 * @retval -EINVAL NULL time or count pointer
 */
int capi_rtc_calendar_to_count(const struct capi_rtc_calendar_time *time,
			       uint16_t epoch_year,
			       uint32_t *count);

/* ===================================================================
 * ISR
 * =================================================================== */

void capi_rtc_isr(struct capi_rtc_handle *handle);

/* ===================================================================
 * Driver ops table
 * =================================================================== */

struct capi_rtc_ops {
	int (*init)(struct capi_rtc_handle **handle,
		    const struct capi_rtc_config *config);
	int (*deinit)(struct capi_rtc_handle *handle);
	int (*start)(struct capi_rtc_handle *handle);
	int (*stop)(struct capi_rtc_handle *handle);
	int (*get_count)(struct capi_rtc_handle *handle, uint32_t *count);
	int (*set_count)(struct capi_rtc_handle *handle, uint32_t count);
	int (*set_tick_config)(struct capi_rtc_handle *handle,
			       const struct capi_rtc_tick_config *config);
	int (*get_tick_config)(struct capi_rtc_handle *handle,
			       struct capi_rtc_tick_config *config);
	int (*get_tick_freq)(struct capi_rtc_handle *handle, uint32_t *freq_hz);
	int (*alarm_set)(struct capi_rtc_handle *handle, uint16_t id,
			 const struct capi_rtc_alarm_config *config);
	int (*alarm_disable)(struct capi_rtc_handle *handle, uint16_t id);
	int (*alarm_get)(struct capi_rtc_handle *handle, uint16_t id,
			 struct capi_rtc_alarm_config *config);
	int (*alarm_set_callback)(struct capi_rtc_handle *handle, uint16_t id,
				  capi_rtc_event_callback callback, void *arg);
	int (*calibrate)(struct capi_rtc_handle *handle,
			 const struct capi_rtc_calib_config *config);
	int (*get_calib_result)(struct capi_rtc_handle *handle,
				struct capi_rtc_calib_result *result);
	int (*set_calibration)(struct capi_rtc_handle *handle, int32_t cal_ppb);
	int (*get_calibration)(struct capi_rtc_handle *handle, int32_t *cal_ppb);
	int (*irq_enable)(struct capi_rtc_handle *handle, uint32_t event);
	int (*irq_disable)(struct capi_rtc_handle *handle, uint32_t event);
	int (*get_status)(struct capi_rtc_handle *handle, uint32_t *status);
	int (*clear_status)(struct capi_rtc_handle *handle, uint32_t status);
	int (*is_irq_pending)(struct capi_rtc_handle *handle, uint32_t event,
			      bool *pending);
	int (*register_callback)(struct capi_rtc_handle *handle,
				 capi_rtc_event_callback callback,
				 void *arg);
	int (*wakeup_enable)(struct capi_rtc_handle *handle);
	int (*wakeup_disable)(struct capi_rtc_handle *handle);
	int (*set_time)(struct capi_rtc_handle *handle,
			const struct capi_rtc_calendar_time *time);
	int (*get_time)(struct capi_rtc_handle *handle,
			struct capi_rtc_calendar_time *time);
	int (*is_time_valid)(struct capi_rtc_handle *handle, bool *valid);
	void (*isr)(struct capi_rtc_handle *handle);
};

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_RTC_H */
