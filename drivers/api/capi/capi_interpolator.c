/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_interpolator.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the Interpolator driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the Interpolator
 * functions. If this is needed, it is suggested that this file and all of the other driver wrappers
 * be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_interpolator.h>

int capi_interpolator_init(struct capi_interpolator_handle **handle,
			   const struct capi_interpolator_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_interpolator_deinit(const struct capi_interpolator_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_interpolator_set_channel_pair_config(
	const struct capi_interpolator_handle *handle,
	enum capi_interpolator_chan_pair chan_pair_id,
	const struct capi_interpolator_chan_pair_config *chan_pair_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_channel_pair_config != NULL) {
		return handle->ops->set_channel_pair_config(handle, chan_pair_id, chan_pair_config);
	}

	return -EINVAL;
}

int capi_interpolator_get_channel_pair_config(
	const struct capi_interpolator_handle *handle,
	enum capi_interpolator_chan_pair chan_pair_id,
	struct capi_interpolator_chan_pair_config *chan_pair_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_channel_pair_config != NULL) {
		return handle->ops->get_channel_pair_config(handle, chan_pair_id, chan_pair_config);
	}

	return -EINVAL;
}

int capi_interpolator_channel_enable(const struct capi_interpolator_handle *handle,
				     enum capi_interpolator_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_enable != NULL) {
		return handle->ops->channel_enable(handle, chan_id);
	}

	return -EINVAL;
}

int capi_interpolator_channel_disable(const struct capi_interpolator_handle *handle,
				      enum capi_interpolator_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_disable != NULL) {
		return handle->ops->channel_disable(handle, chan_id);
	}

	return -EINVAL;
}

int capi_interpolator_get_channel_status(const struct capi_interpolator_handle *handle,
					 enum capi_interpolator_chan chan_id, bool *status)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_channel_status != NULL) {
		return handle->ops->get_channel_status(handle, chan_id, status);
	}

	return -EINVAL;
}