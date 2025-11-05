/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_mixer.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the MIXER driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the MIXER functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_mixer.h>

int capi_mixer_init(struct capi_mixer_handle **handle, const struct capi_mixer_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_mixer_deinit(const struct capi_mixer_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_mixer_set_gain_mode(const struct capi_mixer_handle *handle,
			     enum capi_mixer_gain_mode gain_mode)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_gain_mode != NULL) {
		return handle->ops->set_gain_mode(handle, gain_mode);
	}

	return -EINVAL;
}

int capi_mixer_set_reference(const struct capi_mixer_handle *handle, enum capi_mixer_chan ref_chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_reference != NULL) {
		return handle->ops->set_reference(handle, ref_chan);
	}

	return -EINVAL;
}

int capi_mixer_channel_enable(const struct capi_mixer_handle *handle, enum capi_mixer_chan chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_enable != NULL) {
		return handle->ops->channel_enable(handle, chan);
	}

	return -EINVAL;
}

int capi_mixer_channel_disable(const struct capi_mixer_handle *handle, enum capi_mixer_chan chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->channel_disable != NULL) {
		return handle->ops->channel_disable(handle, chan);
	}

	return -EINVAL;
}

int capi_mixer_channel_set_channel_gain(const struct capi_mixer_handle *handle,
					enum capi_mixer_chan chan, float gain_db)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->channel_set_channel_gain != NULL) {
		return handle->ops->channel_set_channel_gain(handle, chan, gain_db);
	}

	return -EINVAL;
}