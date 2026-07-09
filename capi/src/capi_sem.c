/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <capi_sem.h>

int capi_sem_init(struct capi_sem_handle **handle,
		  const struct capi_sem_config *config)
{
	if (!handle || !config || !config->ops)
		return -EINVAL;
	if (!config->ops->init)
		return -ENOTSUP;

	return config->ops->init(handle, config);
}

int capi_sem_attach(struct capi_sem_handle **handle,
		    const struct capi_sem_config *config)
{
	if (!handle || !config || !config->ops)
		return -EINVAL;
	if (!config->ops->attach)
		return -ENOTSUP;

	return config->ops->attach(handle, config);
}

int capi_sem_detach(struct capi_sem_handle *handle)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->detach)
		return -ENOTSUP;

	return handle->ops->detach(handle);
}

int capi_sem_deinit(struct capi_sem_handle *handle)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->deinit)
		return -ENOTSUP;

	return handle->ops->deinit(handle);
}

int capi_sem_try_acquire(struct capi_sem_handle *handle)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->try_acquire)
		return -ENOTSUP;

	return handle->ops->try_acquire(handle);
}

int capi_sem_acquire(struct capi_sem_handle *handle)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->acquire)
		return -ENOTSUP;

	return handle->ops->acquire(handle);
}

int capi_sem_acquire_timeout(struct capi_sem_handle *handle,
			     uint32_t timeout_us)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->acquire_timeout)
		return -ENOTSUP;

	return handle->ops->acquire_timeout(handle, timeout_us);
}

int capi_sem_release(struct capi_sem_handle *handle)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->release)
		return -ENOTSUP;

	return handle->ops->release(handle);
}

int capi_sem_get_value(struct capi_sem_handle *handle, uint32_t *value)
{
	if (!handle || !handle->ops || !value)
		return -EINVAL;
	if (!handle->ops->get_value)
		return -ENOTSUP;

	return handle->ops->get_value(handle, value);
}

int capi_sem_set_value(struct capi_sem_handle *handle, uint32_t value)
{
	if (!handle || !handle->ops)
		return -EINVAL;
	if (!handle->ops->set_value)
		return -ENOTSUP;

	return handle->ops->set_value(handle, value);
}
