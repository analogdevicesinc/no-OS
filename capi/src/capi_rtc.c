/*
 * Copyright (c) 2024-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_rtc.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the RTC driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the RTC functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_rtc.h>

int capi_rtc_init(struct capi_rtc_handle **handle,
		  const struct capi_rtc_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_rtc_deinit(struct capi_rtc_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_rtc_start(struct capi_rtc_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->start != NULL) {
		return handle->ops->start(handle);
	}

	return -EINVAL;
}

int capi_rtc_stop(struct capi_rtc_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->stop != NULL) {
		return handle->ops->stop(handle);
	}

	return -EINVAL;
}

int capi_rtc_get_time(struct capi_rtc_handle *handle,
		      struct capi_rtc_time *time)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_time != NULL) {
		return handle->ops->get_time(handle, time);
	}

	return -EINVAL;
}

int capi_rtc_set_time(struct capi_rtc_handle *handle,
		      const struct capi_rtc_time *time)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_time != NULL) {
		return handle->ops->set_time(handle, time);
	}

	return -EINVAL;
}

int capi_rtc_get_datetime(struct capi_rtc_handle *handle,
			  struct capi_rtc_datetime *datetime)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->get_datetime != NULL) {
		return handle->ops->get_datetime(handle, datetime);
	}

	return -EINVAL;
}

int capi_rtc_set_datetime(struct capi_rtc_handle *handle,
			  const struct capi_rtc_datetime *datetime)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->set_datetime != NULL) {
		return handle->ops->set_datetime(handle, datetime);
	}

	return -EINVAL;
}

int capi_rtc_set_alarm(struct capi_rtc_handle *handle,
		       enum capi_rtc_alarm_type type, const void *alarm_value)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_alarm != NULL) {
		return handle->ops->set_alarm(handle, type, alarm_value);
	}

	return -EINVAL;
}

int capi_rtc_disable_alarm(struct capi_rtc_handle *handle,
			   enum capi_rtc_alarm_type type)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->disable_alarm != NULL) {
		return handle->ops->disable_alarm(handle, type);
	}

	return -EINVAL;
}

int capi_rtc_sqwave_enable(struct capi_rtc_handle *handle,
			   enum capi_rtc_sqwave_freq freq)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->sqwave_enable != NULL) {
		return handle->ops->sqwave_enable(handle, freq);
	}

	return -EINVAL;
}

int capi_rtc_sqwave_disable(struct capi_rtc_handle *handle)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->sqwave_disable != NULL) {
		return handle->ops->sqwave_disable(handle);
	}

	return -EINVAL;
}

int capi_rtc_trim(struct capi_rtc_handle *handle, int8_t trim)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->trim != NULL) {
		return handle->ops->trim(handle, trim);
	}

	return -EINVAL;
}

int capi_rtc_register_callback(struct capi_rtc_handle *handle,
			       capi_rtc_event_callback_t callback, void *event_ctx)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->register_callback != NULL) {
		return handle->ops->register_callback(handle, callback, event_ctx);
	}

	return -EINVAL;
}

int capi_rtc_enable_events(struct capi_rtc_handle *handle, uint32_t events_mask)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->enable_events != NULL) {
		return handle->ops->enable_events(handle, events_mask);
	}

	return -EINVAL;
}

int capi_rtc_disable_events(struct capi_rtc_handle *handle,
			    uint32_t events_mask)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->disable_events != NULL) {
		return handle->ops->disable_events(handle, events_mask);
	}

	return -EINVAL;
}

void capi_rtc_isr(void *handle)
{
	if (!handle)
		return;

	struct capi_rtc_handle *rtc_handle = (struct capi_rtc_handle *)handle;
	if (!rtc_handle->ops || !rtc_handle->ops->isr)
		return;

	rtc_handle->ops->isr(handle);
}
