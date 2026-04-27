/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_asrc.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the ASRC driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the ASRC functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_asrc.h>

int capi_asrc_init(struct capi_asrc_handle **handle, const struct capi_asrc_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_asrc_deinit(const struct capi_asrc_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit((struct capi_asrc_handle *)handle);
	}

	return -EINVAL;
}

int capi_asrc_set_input_config(const struct capi_asrc_handle *handle, enum capi_asrc_input index,
			       const struct capi_asrc_input_ctrl_config *config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_input_config != NULL) {
		return handle->ops->set_input_config(handle, index, config);
	}

	return -EINVAL;
}

int capi_asrc_set_output_config(const struct capi_asrc_handle *handle, enum capi_asrc_output index,
				const struct capi_asrc_output_ctrl_config *config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_output_config != NULL) {
		return handle->ops->set_output_config(handle, index, config);
	}

	return -EINVAL;
}

int capi_asrc_set_input_power_mode(const struct capi_asrc_handle *handle,
				   enum capi_asrc_input index, enum capi_asrc_pwr_mode mode)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_input_power_mode != NULL) {
		return handle->ops->set_input_power_mode(handle, index, mode);
	}

	return -EINVAL;
}

int capi_asrc_set_output_power_mode(const struct capi_asrc_handle *handle,
				    enum capi_asrc_output index, enum capi_asrc_pwr_mode mode)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_output_power_mode != NULL) {
		return handle->ops->set_output_power_mode(handle, index, mode);
	}

	return -EINVAL;
}

int capi_asrc_get_input_lock_status(const struct capi_asrc_handle *handle,
				    enum capi_asrc_input index, bool *locked)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_input_lock_status != NULL) {
		return handle->ops->get_input_lock_status(handle, index, locked);
	}

	return -EINVAL;
}

int capi_asrc_get_output_lock_status(const struct capi_asrc_handle *handle,
				     enum capi_asrc_output index, bool *locked)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_output_lock_status != NULL) {
		return handle->ops->get_output_lock_status(handle, index, locked);
	}

	return -EINVAL;
}

int capi_asrc_enable_input_channel(const struct capi_asrc_handle *handle,
				   enum capi_asrc_input index, enum capi_asrc_input_channel chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable_input_channel != NULL) {
		return handle->ops->enable_input_channel(handle, index, chan);
	}

	return -EINVAL;
}

int capi_asrc_enable_output_channel(const struct capi_asrc_handle *handle,
				    enum capi_asrc_output index, enum capi_asrc_output_channel chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable_output_channel != NULL) {
		return handle->ops->enable_output_channel(handle, index, chan);
	}

	return -EINVAL;
}

int capi_asrc_disable_input_channel(const struct capi_asrc_handle *handle,
				    enum capi_asrc_input index, enum capi_asrc_input_channel chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->disable_input_channel != NULL) {
		return handle->ops->disable_input_channel(handle, index, chan);
	}

	return -EINVAL;
}

int capi_asrc_disable_output_channel(const struct capi_asrc_handle *handle,
				     enum capi_asrc_output index,
				     enum capi_asrc_output_channel chan)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->disable_output_channel != NULL) {
		return handle->ops->disable_output_channel(handle, index, chan);
	}

	return -EINVAL;
}

int capi_asrc_get_input_channel_status(const struct capi_asrc_handle *handle,
				       enum capi_asrc_input index,
				       enum capi_asrc_input_channel chan, bool *status)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->get_input_channel_status != NULL) {
		return handle->ops->get_input_channel_status(handle, index, chan, status);
	}

	return -EINVAL;
}

int capi_asrc_get_output_channel_status(const struct capi_asrc_handle *handle,
					enum capi_asrc_output index,
					enum capi_asrc_output_channel chan, bool *status)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->get_output_channel_status != NULL) {
		return handle->ops->get_output_channel_status(handle, index, chan, status);
	}

	return -EINVAL;
}