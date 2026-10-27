/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_rtc.c
 * @brief RTC CAPI thin-layer wrappers and calendar utility functions.
 *
 * The thin layer dispatches each call through the driver ops table.
 * NULL handle/ops returns -EINVAL; a NULL function pointer returns
 * -ENOTSUP (following the capi_sem convention).
 *
 * Calendar conversion (count_to_calendar / calendar_to_count) is pure
 * date math implemented here as common code — not a driver op.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <errno.h>
#include <capi_rtc.h>

/* ===================================================================
 * Thin-layer wrappers
 * =================================================================== */

int capi_rtc_init(struct capi_rtc_handle **handle,
		  const struct capi_rtc_config *config)
{
	if (!handle || !config || !config->ops)
		return -EINVAL;
	if (!config->ops->init)
		return -ENOTSUP;
	return config->ops->init(handle, config);
}

int capi_rtc_deinit(struct capi_rtc_handle *handle)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->deinit)
		return -ENOTSUP;
	return handle->ops->deinit(handle);
}

int capi_rtc_start(struct capi_rtc_handle *handle)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->start)
		return -ENOTSUP;
	return handle->ops->start(handle);
}

int capi_rtc_stop(struct capi_rtc_handle *handle)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->stop)
		return -ENOTSUP;
	return handle->ops->stop(handle);
}

int capi_rtc_get_count(struct capi_rtc_handle *handle, uint32_t *count)
{
	if (!handle || !handle->ops || !count)
		return -EINVAL;
	if (!handle->ops->get_count)
		return -ENOTSUP;
	return handle->ops->get_count(handle, count);
}

int capi_rtc_set_count(struct capi_rtc_handle *handle, uint32_t count)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->set_count)
		return -ENOTSUP;
	return handle->ops->set_count(handle, count);
}

int capi_rtc_set_tick_config(struct capi_rtc_handle *handle,
			     const struct capi_rtc_tick_config *config)
{
	if (!handle || !handle->ops || !config)
		return -EINVAL;
	if (!handle->ops->set_tick_config)
		return -ENOTSUP;
	return handle->ops->set_tick_config(handle, config);
}

int capi_rtc_get_tick_config(struct capi_rtc_handle *handle,
			     struct capi_rtc_tick_config *config)
{
	if (!handle || !handle->ops || !config)
		return -EINVAL;
	if (!handle->ops->get_tick_config)
		return -ENOTSUP;
	return handle->ops->get_tick_config(handle, config);
}

int capi_rtc_get_tick_freq(struct capi_rtc_handle *handle, uint32_t *freq_hz)
{
	if (!handle || !handle->ops || !freq_hz)
		return -EINVAL;
	if (!handle->ops->get_tick_freq)
		return -ENOTSUP;
	return handle->ops->get_tick_freq(handle, freq_hz);
}

int capi_rtc_alarm_set(struct capi_rtc_handle *handle, uint16_t id,
		       const struct capi_rtc_alarm_config *config)
{
	if (!handle || !handle->ops || !config)
		return -EINVAL;
	if (!handle->ops->alarm_set)
		return -ENOTSUP;
	return handle->ops->alarm_set(handle, id, config);
}

int capi_rtc_alarm_disable(struct capi_rtc_handle *handle, uint16_t id)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->alarm_disable)
		return -ENOTSUP;
	return handle->ops->alarm_disable(handle, id);
}

int capi_rtc_alarm_get(struct capi_rtc_handle *handle, uint16_t id,
		       struct capi_rtc_alarm_config *config)
{
	if (!handle || !handle->ops || !config)
		return -EINVAL;
	if (!handle->ops->alarm_get)
		return -ENOTSUP;
	return handle->ops->alarm_get(handle, id, config);
}

int capi_rtc_alarm_set_callback(struct capi_rtc_handle *handle, uint16_t id,
				capi_rtc_event_callback callback, void *arg)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->alarm_set_callback)
		return -ENOTSUP;
	return handle->ops->alarm_set_callback(handle, id, callback, arg);
}

int capi_rtc_calibrate(struct capi_rtc_handle *handle,
		       const struct capi_rtc_calib_config *config)
{
	if (!handle || !handle->ops || !config)
		return -EINVAL;
	if (!handle->ops->calibrate)
		return -ENOTSUP;
	return handle->ops->calibrate(handle, config);
}

int capi_rtc_get_calib_result(struct capi_rtc_handle *handle,
			      struct capi_rtc_calib_result *result)
{
	if (!handle || !handle->ops || !result)
		return -EINVAL;
	if (!handle->ops->get_calib_result)
		return -ENOTSUP;
	return handle->ops->get_calib_result(handle, result);
}

int capi_rtc_set_calibration(struct capi_rtc_handle *handle, int32_t cal_ppb)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->set_calibration)
		return -ENOTSUP;
	return handle->ops->set_calibration(handle, cal_ppb);
}

int capi_rtc_get_calibration(struct capi_rtc_handle *handle, int32_t *cal_ppb)
{
	if (!handle || !handle->ops || !cal_ppb)
		return -EINVAL;
	if (!handle->ops->get_calibration)
		return -ENOTSUP;
	return handle->ops->get_calibration(handle, cal_ppb);
}

int capi_rtc_irq_enable(struct capi_rtc_handle *handle, uint32_t event)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->irq_enable)
		return -ENOTSUP;
	return handle->ops->irq_enable(handle, event);
}

int capi_rtc_irq_disable(struct capi_rtc_handle *handle, uint32_t event)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->irq_disable)
		return -ENOTSUP;
	return handle->ops->irq_disable(handle, event);
}

int capi_rtc_get_status(struct capi_rtc_handle *handle, uint32_t *status)
{
	if (!handle || !handle->ops || !status)
		return -EINVAL;
	if (!handle->ops->get_status)
		return -ENOTSUP;
	return handle->ops->get_status(handle, status);
}

int capi_rtc_clear_status(struct capi_rtc_handle *handle, uint32_t status)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->clear_status)
		return -ENOTSUP;
	return handle->ops->clear_status(handle, status);
}

int capi_rtc_is_irq_pending(struct capi_rtc_handle *handle, uint32_t event,
			    bool *pending)
{
	if (!handle || !handle->ops || !pending)
		return -EINVAL;
	if (!handle->ops->is_irq_pending)
		return -ENOTSUP;
	return handle->ops->is_irq_pending(handle, event, pending);
}

int capi_rtc_register_callback(struct capi_rtc_handle *handle,
			       capi_rtc_event_callback callback,
			       void *arg)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->register_callback)
		return -ENOTSUP;
	return handle->ops->register_callback(handle, callback, arg);
}

int capi_rtc_wakeup_enable(struct capi_rtc_handle *handle)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->wakeup_enable)
		return -ENOTSUP;
	return handle->ops->wakeup_enable(handle);
}

int capi_rtc_wakeup_disable(struct capi_rtc_handle *handle)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->wakeup_disable)
		return -ENOTSUP;
	return handle->ops->wakeup_disable(handle);
}

int capi_rtc_set_time(struct capi_rtc_handle *handle,
		      const struct capi_rtc_calendar_time *time)
{
	if (!handle || !handle->ops || !time)
		return -EINVAL;
	if (!handle->ops->set_time)
		return -ENOTSUP;
	return handle->ops->set_time(handle, time);
}

int capi_rtc_get_time(struct capi_rtc_handle *handle,
		      struct capi_rtc_calendar_time *time)
{
	if (!handle || !handle->ops || !time)
		return -EINVAL;
	if (!handle->ops->get_time)
		return -ENOTSUP;
	return handle->ops->get_time(handle, time);
}

int capi_rtc_is_time_valid(struct capi_rtc_handle *handle, bool *valid)
{
	if (!handle || !handle->ops || !valid)
		return -EINVAL;
	if (!handle->ops->is_time_valid)
		return -ENOTSUP;
	return handle->ops->is_time_valid(handle, valid);
}

void capi_rtc_isr(struct capi_rtc_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->isr) {
		return;
	}
	handle->ops->isr(handle);
}

/* ===================================================================
 * Calendar utility functions (pure date math, no driver ops)
 * =================================================================== */

static bool rtc_is_leap_year(uint16_t year)
{
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static const uint8_t rtc_days_in_month_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static uint8_t rtc_days_in_month(uint8_t month, uint16_t year)
{
	if (month == 2 && rtc_is_leap_year(year)) {
		return 29;
	}
	return rtc_days_in_month_table[month - 1];
}

static uint32_t rtc_days_in_year(uint16_t year)
{
	return rtc_is_leap_year(year) ? 366U : 365U;
}

int capi_rtc_count_to_calendar(uint32_t count, uint16_t epoch_year,
			       struct capi_rtc_calendar_time *time)
{
	if (time == NULL) {
		return -EINVAL;
	}

	uint32_t remaining = count;
	uint16_t year = epoch_year;

	while (remaining >= rtc_days_in_year(year) * 86400U) {
		remaining -= rtc_days_in_year(year) * 86400U;
		year++;
	}

	uint8_t month = 1;
	while (month <= 12
	       && remaining >= (uint32_t)rtc_days_in_month(month, year) * 86400U) {
		remaining -= (uint32_t)rtc_days_in_month(month, year) * 86400U;
		month++;
	}

	uint8_t day = (uint8_t)(remaining / 86400U) + 1U;
	remaining %= 86400U;

	time->year = year;
	time->month = month;
	time->day = day;
	time->hour = (uint8_t)(remaining / 3600U);
	remaining %= 3600U;
	time->minute = (uint8_t)(remaining / 60U);
	time->second = (uint8_t)(remaining % 60U);
	time->nsec = 0;

	/* Weekday via Tomohiko Sakamoto's method */
	uint16_t y = year;
	uint16_t m = month;
	static const uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	if (m < 3) {
		y--;
	}
	time->weekday = (uint8_t)((y + y / 4 - y / 100 + y / 400 + t[m - 1] + day) % 7);

	return 0;
}

int capi_rtc_calendar_to_count(const struct capi_rtc_calendar_time *time,
			       uint16_t epoch_year,
			       uint32_t *count)
{
	if (time == NULL || count == NULL) {
		return -EINVAL;
	}

	uint32_t total = 0;

	for (uint16_t y = epoch_year; y < time->year; y++) {
		total += rtc_days_in_year(y) * 86400U;
	}

	for (uint8_t m = 1; m < time->month; m++) {
		total += (uint32_t)rtc_days_in_month(m, time->year) * 86400U;
	}

	total += (uint32_t)(time->day - 1U) * 86400U;
	total += (uint32_t)time->hour * 3600U;
	total += (uint32_t)time->minute * 60U;
	total += time->second;

	*count = total;
	return 0;
}
