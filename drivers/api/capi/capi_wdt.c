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

int capi_wdt_init(struct capi_wdt_handle **handle, const struct capi_wdt_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_wdt_deinit(struct capi_wdt_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_wdt_get_chan_count(struct capi_wdt_handle *handle, int *channels)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_chan_count != NULL) {
		return handle->ops->get_chan_count(handle, channels);
	}

	return -EINVAL;
}

int capi_wdt_setup_chan(struct capi_wdt_handle *handle, int chan_id,
			const struct capi_wdt_chan_config *chan_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->setup_chan != NULL) {
		return handle->ops->setup_chan(handle, chan_id, chan_config);
	}

	return -EINVAL;
}

int capi_wdt_disable_chan(struct capi_wdt_handle *handle, int chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->disable_chan != NULL) {
		return handle->ops->disable_chan(handle, chan_id);
	}

	return -EINVAL;
}

int capi_wdt_feed_chan(struct capi_wdt_handle *handle, int chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->feed_chan != NULL) {
		return handle->ops->feed_chan(handle, chan_id);
	}

	return -EINVAL;
}

void capi_wdt_isr(struct capi_wdt_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->isr != NULL) {
		handle->ops->isr(handle);
	}
}