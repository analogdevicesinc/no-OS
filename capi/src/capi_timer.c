/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_timer.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the TIMER driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the TIMER functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_timer.h>

int capi_timer_init(struct capi_timer_handle **handle, const struct capi_timer_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_timer_deinit(struct capi_timer_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_timer_start(struct capi_timer_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->start != NULL) {
		return handle->ops->start(handle);
	}

	return -EINVAL;
}

int capi_timer_stop(struct capi_timer_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->stop != NULL) {
		return handle->ops->stop(handle);
	}

	return -EINVAL;
}

int capi_timer_counter_config(struct capi_timer_handle *handle,
			      const struct capi_timer_counter_config *config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->counter_config != NULL) {
		return handle->ops->counter_config(handle, config);
	}

	return -EINVAL;
}

int capi_timer_counter_get(struct capi_timer_handle *handle, uint32_t *counter)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->counter_get != NULL) {
		return handle->ops->counter_get(handle, counter);
	}

	return -EINVAL;
}

int capi_timer_channel_init(struct capi_timer_handle *handle, uint32_t chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_init != NULL) {
		return handle->ops->channel_init(handle, chan);
	}

	return -EINVAL;
}

int capi_timer_channel_config(struct capi_timer_handle *handle, uint32_t chan,
			      const struct capi_timer_channel_config *ch_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_config != NULL) {
		return handle->ops->channel_config(handle, chan, ch_config);
	}

	return -EINVAL;
}

int capi_timer_channel_deinit(struct capi_timer_handle *handle, uint32_t chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_deinit != NULL) {
		return handle->ops->channel_deinit(handle, chan);
	}

	return -EINVAL;
}

int capi_timer_channel_enable(struct capi_timer_handle *handle, uint32_t chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_enable != NULL) {
		return handle->ops->channel_enable(handle, chan);
	}

	return -EINVAL;
}

int capi_timer_channel_disable(struct capi_timer_handle *handle, uint32_t chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_disable != NULL) {
		return handle->ops->channel_disable(handle, chan);
	}

	return -EINVAL;
}

int capi_timer_channel_irq_enable(struct capi_timer_handle *handle, uint32_t chan, uint32_t event)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_irq_enable != NULL) {
		return handle->ops->channel_irq_enable(handle, chan, event);
	}

	return -EINVAL;
}

int capi_timer_channel_irq_disable(struct capi_timer_handle *handle, uint32_t chan, uint32_t event)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_irq_disable != NULL) {
		return handle->ops->channel_irq_disable(handle, chan, event);
	}

	return -EINVAL;
}

int capi_timer_channel_register_callback(struct capi_timer_handle *handle, uint32_t chan,
					 capi_timer_channel_callback callback, void *callback_arg)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->channel_register_callback != NULL) {
		return handle->ops->channel_register_callback(handle, chan, callback, callback_arg);
	}

	return -EINVAL;
}

int capi_timer_channel_compare_set(struct capi_timer_handle *handle, uint32_t chan, uint32_t compare)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_compare_set != NULL) {
		return handle->ops->channel_compare_set(handle, chan, compare);
	}

	return -EINVAL;
}

int capi_timer_channel_compare_get(struct capi_timer_handle *handle, uint32_t chan, uint32_t *compare)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_compare_get != NULL) {
		return handle->ops->channel_compare_get(handle, chan, compare);
	}

	return -EINVAL;
}

int capi_timer_channel_capture_get(struct capi_timer_handle *handle, uint32_t chan, uint32_t *capture)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_capture_get != NULL) {
		return handle->ops->channel_capture_get(handle, chan, capture);
	}

	return -EINVAL;
}

int capi_timer_event_irq_enable(struct capi_timer_handle *handle, uint32_t event)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->event_irq_enable != NULL) {
		return handle->ops->event_irq_enable(handle, event);
	}

	return -EINVAL;
}

int capi_timer_event_irq_disable(struct capi_timer_handle *handle, uint32_t event)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->event_irq_disable != NULL) {
		return handle->ops->event_irq_disable(handle, event);
	}

	return -EINVAL;
}

int capi_timer_register_event_callback(struct capi_timer_handle *handle,
				       capi_timer_event_callback callback, void *callback_arg)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->register_event_callback != NULL) {
		return handle->ops->register_event_callback(handle, callback, callback_arg);
	}

	return -EINVAL;
}

int capi_timer_is_irq_pending(struct capi_timer_handle *handle, bool *pending)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->is_irq_pending != NULL) {
		return handle->ops->is_irq_pending(handle, pending);
	}

	return -EINVAL;
}

void capi_timer_isr(struct capi_timer_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->isr != NULL) {
		handle->ops->isr(handle);
	}
}

int capi_timer_nsec_to_ticks(const struct capi_timer_handle *handle, uint64_t duration_ns,
			     uint32_t *ticks)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->nsec_to_ticks != NULL) {
		return handle->ops->nsec_to_ticks(handle, duration_ns, ticks);
	}

	return -EINVAL;
}

int capi_timer_ticks_to_nsec(const struct capi_timer_handle *handle, uint64_t ticks,
			     uint32_t *duration_ns)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->ticks_to_nsec != NULL) {
		return handle->ops->ticks_to_nsec(handle, ticks, duration_ns);
	}

	return -EINVAL;
}