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

int capi_timer_counter_set(struct capi_timer_handle *handle, uint32_t counter)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->counter_set != NULL) {
		return handle->ops->counter_set(handle, counter);
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

int capi_timer_channel_config(struct capi_timer_handle *handle, uint32_t chan, void *ch_config)
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

int capi_timer_channel_irq_enable(struct capi_timer_handle *handle, uint32_t chan,
				  enum capi_timer_event event)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->irq_enable != NULL) {
		return handle->ops->irq_enable(handle, chan, event);
	}

	return -EINVAL;
}

int capi_timer_channel_irq_disable(struct capi_timer_handle *handle, uint32_t chan,
				   enum capi_timer_event event)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->irq_disable != NULL) {
		return handle->ops->irq_disable(handle, chan, event);
	}

	return -EINVAL;
}

int capi_timer_channel_register_callback(struct capi_timer_handle *handle, uint32_t chan,
					 capi_timer_callback *const callback,
					 void *const callback_arg)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->channel_register_callback != NULL) {
		return handle->ops->channel_register_callback(handle, chan, callback, callback_arg);
	}

	return -EINVAL;
}

int capi_timer_compare_set(struct capi_timer_handle *handle, uint32_t chan, uint32_t compare)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->compare_set != NULL) {
		return handle->ops->compare_set(handle, chan, compare);
	}

	return -EINVAL;
}

int capi_timer_compare_get(struct capi_timer_handle *handle, uint32_t chan, uint32_t *compare)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->compare_get != NULL) {
		return handle->ops->compare_get(handle, chan, compare);
	}

	return -EINVAL;
}

int capi_timer_capture_get(struct capi_timer_handle *handle, uint32_t chan, uint32_t *capture)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->capture_get != NULL) {
		return handle->ops->capture_get(handle, chan, capture);
	}

	return -EINVAL;
}