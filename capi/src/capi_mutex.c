/* SPDX-License-Identifier: BSD-3-Clause */

#include <errno.h>
#include <capi_mutex.h>

int capi_mutex_init(struct capi_mutex **handle,
		    const struct capi_mutex_config *config)
{
	if (!handle || !config || !config->ops || !config->ops->init)
		return -EINVAL;

	return config->ops->init(handle, config);
}

int capi_mutex_deinit(struct capi_mutex *handle)
{
	if (!handle || !handle->ops || !handle->ops->deinit)
		return -EINVAL;

	return handle->ops->deinit(handle);
}

int capi_mutex_lock(struct capi_mutex *handle)
{
	if (!handle || !handle->ops || !handle->ops->lock)
		return -EINVAL;

	return handle->ops->lock(handle);
}

int capi_mutex_trylock(struct capi_mutex *handle)
{
	if (!handle || !handle->ops || !handle->ops->trylock)
		return -EINVAL;

	return handle->ops->trylock(handle);
}

int capi_mutex_unlock(struct capi_mutex *handle)
{
	if (!handle || !handle->ops || !handle->ops->unlock)
		return -EINVAL;

	return handle->ops->unlock(handle);
}
