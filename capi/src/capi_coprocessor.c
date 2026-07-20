/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <capi_coprocessor.h>

int capi_coprocessor_init(struct capi_coprocessor_handle **handle,
			  const struct capi_coprocessor_config *config)
{
	if (!handle || !config || !config->ops || !config->ops->init)
		return -EINVAL;

	return config->ops->init(handle, config);
}

int capi_coprocessor_deinit(struct capi_coprocessor_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->deinit)
		return -EINVAL;

	return handle->ops->deinit(handle);
}

int capi_coprocessor_boot(struct capi_coprocessor_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->boot)
		return -EINVAL;

	return handle->ops->boot(handle);
}

int capi_coprocessor_halt(struct capi_coprocessor_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->halt)
		return -EINVAL;

	return handle->ops->halt(handle);
}
