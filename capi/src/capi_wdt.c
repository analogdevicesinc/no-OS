/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_wdt.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the WDT driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the WDT functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_wdt.h>

int capi_wdt_init(struct capi_wdt_handle **handle,
		  const struct capi_wdt_config *config)
{
	if (!handle || !config || !config->ops || !config->ops->init) {
		return -EINVAL;
	}
	return config->ops->init(handle, config);
}

int capi_wdt_deinit(struct capi_wdt_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->deinit) {
		return -EINVAL;
	}
	return handle->ops->deinit(handle);
}

int capi_wdt_get_chan_count(struct capi_wdt_handle *handle, int *channels)
{
	if (!handle || !handle->ops || !handle->ops->get_chan_count) {
		return -EINVAL;
	}
	return handle->ops->get_chan_count(handle, channels);
}

int capi_wdt_setup_chan(struct capi_wdt_handle *handle, int chan_id,
			const struct capi_wdt_chan_config *chan_config)
{
	if (!handle || !handle->ops || !handle->ops->setup_chan) {
		return -EINVAL;
	}
	return handle->ops->setup_chan(handle, chan_id, chan_config);
}

int capi_wdt_disable_chan(struct capi_wdt_handle *handle, int chan_id)
{
	if (!handle || !handle->ops || !handle->ops->disable_chan) {
		return -EINVAL;
	}
	return handle->ops->disable_chan(handle, chan_id);
}

int capi_wdt_feed_chan(struct capi_wdt_handle *handle, int chan_id)
{
	if (!handle || !handle->ops || !handle->ops->feed_chan) {
		return -EINVAL;
	}
	return handle->ops->feed_chan(handle, chan_id);
}

void capi_wdt_isr(struct capi_wdt_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->isr) {
		return;
	}
	handle->ops->isr(handle);
}
