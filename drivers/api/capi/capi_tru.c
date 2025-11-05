/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_tru.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the TRU driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the TRU functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_tru.h>

int capi_tru_init(struct capi_tru_handle **handle, const struct capi_tru_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_tru_deinit(struct capi_tru_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_tru_route_connect(struct capi_tru_handle *handle, route_id_t receiver_route_id,
			   route_id_t generator_route_id, bool lock)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->route_connect != NULL) {
		return handle->ops->route_connect(handle, receiver_route_id, generator_route_id,
						  lock);
	}

	return -EINVAL;
}

int capi_tru_route_disconnect(struct capi_tru_handle *handle, route_id_t receiver_route_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->route_disconnect != NULL) {
		return handle->ops->route_disconnect(handle, receiver_route_id);
	}

	return -EINVAL;
}

int capi_tru_get_route(struct capi_tru_handle *handle, route_id_t receiver_route_id,
		       route_id_t *generator_route_id, bool *locked)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_route != NULL) {
		return handle->ops->get_route(handle, receiver_route_id, generator_route_id,
					      locked);
	}

	return -EINVAL;
}

int capi_tru_trigger(struct capi_tru_handle *handle, route_id_t generator_route_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->trigger != NULL) {
		return handle->ops->trigger(handle, generator_route_id);
	}

	return -EINVAL;
}

int capi_tru_software_trigger_set_enabled(struct capi_tru_handle *handle, bool enabled)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->software_trigger_set_enabled != NULL) {
		return handle->ops->software_trigger_set_enabled(handle, enabled);
	}

	return -EINVAL;
}

int capi_tru_software_trigger_get_enabled(struct capi_tru_handle *handle, bool *enabled)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->software_trigger_get_enabled != NULL) {
		return handle->ops->software_trigger_get_enabled(handle, enabled);
	}

	return -EINVAL;
}

int capi_tru_register_callback(struct capi_tru_handle *handle, capi_tru_callback_t const callback,
			       void *callback_arg)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->register_callback != NULL) {
		return handle->ops->register_callback(handle, callback, callback_arg);
	}

	return -EINVAL;
}

void capi_tru_isr(struct capi_tru_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->isr != NULL) {
		handle->ops->isr(handle);
	}
}