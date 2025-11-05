/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_syncsource.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the SYNCSOURCE driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the SYNCSOURCE functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_syncsource.h>

int capi_syncsource_init(struct capi_syncsource_handle **handle,
			 const struct capi_syncsource_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_syncsource_deinit(const struct capi_syncsource_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_syncsource_set_ref_clk(const struct capi_syncsource_handle *handle,
				capi_syncsource_ref_clk ref_clk)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_ref_clk != NULL) {
		return handle->ops->set_ref_clk(handle, ref_clk);
	}

	return -EINVAL;
}

int capi_syncsource_get_ref_clk(const struct capi_syncsource_handle *handle,
				capi_syncsource_ref_clk *ref_clk)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_ref_clk != NULL) {
		return handle->ops->get_ref_clk(handle, ref_clk);
	}

	return -EINVAL;
}

int capi_syncsource_init_ref_clk(const struct capi_syncsource_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->init_ref_clk != NULL) {
		return handle->ops->init_ref_clk(handle);
	}

	return -EINVAL;
}

int capi_syncsource_enable_ref_clk_detection(const struct capi_syncsource_handle *handle)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->enable_ref_clk_detection != NULL) {
		return handle->ops->enable_ref_clk_detection(handle);
	}

	return -EINVAL;
}

int capi_syncsource_disable_ref_clk_detection(const struct capi_syncsource_handle *handle)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->disable_ref_clk_detection != NULL) {
		return handle->ops->disable_ref_clk_detection(handle);
	}

	return -EINVAL;
}

int capi_syncsource_get_ref_clk_lock_status(const struct capi_syncsource_handle *handle,
					    bool *status)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_ref_clk_lock_status != NULL) {
		return handle->ops->get_ref_clk_lock_status(handle, status);
	}

	return -EINVAL;
}

int capi_syncsource_enable_synchronization(const struct capi_syncsource_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable_synchronization != NULL) {
		return handle->ops->enable_synchronization(handle);
	}

	return -EINVAL;
}

int capi_syncsource_disable_synchronization(const struct capi_syncsource_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->disable_synchronization != NULL) {
		return handle->ops->disable_synchronization(handle);
	}

	return -EINVAL;
}

int capi_syncsource_enable_sync_signal(const struct capi_syncsource_handle *handle,
				       capi_syncsource_sync_signal sync_signal)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable_sync_signal != NULL) {
		return handle->ops->enable_sync_signal(handle, sync_signal);
	}

	return -EINVAL;
}

int capi_syncsource_disable_sync_signal(const struct capi_syncsource_handle *handle,
					capi_syncsource_sync_signal sync_signal)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->disable_sync_signal != NULL) {
		return handle->ops->disable_sync_signal(handle, sync_signal);
	}

	return -EINVAL;
}

int capi_syncsource_enable_bit_clk(const struct capi_syncsource_handle *handle,
				   capi_syncsource_bit_clk bit_clk)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable_bit_clk != NULL) {
		return handle->ops->enable_bit_clk(handle, bit_clk);
	}

	return -EINVAL;
}

int capi_syncsource_disable_bit_clk(const struct capi_syncsource_handle *handle,
				    capi_syncsource_bit_clk bit_clk)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->disable_bit_clk != NULL) {
		return handle->ops->disable_bit_clk(handle, bit_clk);
	}

	return -EINVAL;
}