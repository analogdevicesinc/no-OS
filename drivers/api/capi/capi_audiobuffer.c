/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_audiobuffer.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the AudioBuffer driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the AudioBuffer functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_audiobuffer.h>

int capi_audiobuffer_init(struct capi_audiobuffer_handle **handle,
			  const struct capi_audiobuffer_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_audiobuffer_deinit(const struct capi_audiobuffer_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_audiobuffer_set_input_channel_config(const struct capi_audiobuffer_handle *handle,
					      uint8_t chan,
					      const struct capi_audiobuffer_in_chan_config *config)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->set_input_channel_config != NULL) {
		return handle->ops->set_input_channel_config(handle, chan, config);
	}

	return -EINVAL;
}

int capi_audiobuffer_get_input_channel_config(const struct capi_audiobuffer_handle *handle,
					      uint8_t chan,
					      struct capi_audiobuffer_in_chan_config *config)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->get_input_channel_config != NULL) {
		return handle->ops->get_input_channel_config(handle, chan, config);
	}

	return -EINVAL;
}

int capi_audiobuffer_enable_input_channel(const struct capi_audiobuffer_handle *handle,
					  uint8_t chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable_input_channel != NULL) {
		return handle->ops->enable_input_channel(handle, chan);
	}

	return -EINVAL;
}

int capi_audiobuffer_disable_input_channel(const struct capi_audiobuffer_handle *handle,
					   uint8_t chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->disable_input_channel != NULL) {
		return handle->ops->disable_input_channel(handle, chan);
	}

	return -EINVAL;
}

int capi_audiobuffer_get_input_channel_status(const struct capi_audiobuffer_handle *handle,
					      uint8_t chan, bool *status)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->get_input_channel_status != NULL) {
		return handle->ops->get_input_channel_status(handle, chan, status);
	}

	return -EINVAL;
}

int capi_audiobuffer_set_output_channel_config(
	const struct capi_audiobuffer_handle *handle, uint8_t chan,
	const struct capi_audiobuffer_out_chan_config *config)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->set_output_channel_config != NULL) {
		return handle->ops->set_output_channel_config(handle, chan, config);
	}

	return -EINVAL;
}

int capi_audiobuffer_get_output_channel_config(const struct capi_audiobuffer_handle *handle,
					       uint8_t chan,
					       struct capi_audiobuffer_out_chan_config *config)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->get_output_channel_config != NULL) {
		return handle->ops->get_output_channel_config(handle, chan, config);
	}

	return -EINVAL;
}

int capi_audiobuffer_enable_output_channel(const struct capi_audiobuffer_handle *handle,
					   uint8_t chan, bool enable)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable_output_channel != NULL) {
		return handle->ops->enable_output_channel(handle, chan, enable);
	}

	return -EINVAL;
}

int capi_audiobuffer_disable_output_channel(const struct capi_audiobuffer_handle *handle,
					    uint8_t chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->disable_output_channel != NULL) {
		return handle->ops->disable_output_channel(handle, chan);
	}

	return -EINVAL;
}

int capi_audiobuffer_get_output_channel_status(const struct capi_audiobuffer_handle *handle,
					       uint8_t chan, bool *status)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->get_output_channel_status != NULL) {
		return handle->ops->get_output_channel_status(handle, chan, status);
	}

	return -EINVAL;
}