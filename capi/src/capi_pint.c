/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief CAPI PINT thin layer implementation
 */

#include "capi_pint.h"
#include <errno.h>

int capi_pint_port_init(struct capi_pint_port_handle **handle,
			const struct capi_pint_port_config *config)
{
	if (!handle || !config || !config->ops || !config->ops->port_init) {
		return -EINVAL;
	}

	return config->ops->port_init(handle, config);
}

int capi_pint_port_deinit(struct capi_pint_port_handle **handle)
{
	if (!handle || !*handle || !(*handle)->ops || !(*handle)->ops->port_deinit) {
		return -EINVAL;
	}

	return (*handle)->ops->port_deinit(handle);
}

int capi_pint_configure_pin(struct capi_pint_port_handle *handle,
			    uint8_t pin_index,
			    const struct capi_pint_pin_config *config)
{
	if (!handle || !handle->ops || !handle->ops->configure_pin) {
		return -EINVAL;
	}

	return handle->ops->configure_pin(handle, pin_index, config);
}

int capi_pint_enable_pin(struct capi_pint_port_handle *handle,
			 uint8_t pin_index)
{
	if (!handle || !handle->ops || !handle->ops->enable_pin) {
		return -EINVAL;
	}

	return handle->ops->enable_pin(handle, pin_index);
}

int capi_pint_disable_pin(struct capi_pint_port_handle *handle,
			  uint8_t pin_index)
{
	if (!handle || !handle->ops || !handle->ops->disable_pin) {
		return -EINVAL;
	}

	return handle->ops->disable_pin(handle, pin_index);
}

int capi_pint_register_callback(struct capi_pint_port_handle *handle,
				uint8_t pin_index,
				capi_pint_callback_t callback, void *user_data)
{
	if (!handle || !handle->ops || !handle->ops->register_callback) {
		return -EINVAL;
	}

	return handle->ops->register_callback(handle, pin_index, callback, user_data);
}

int capi_pint_unregister_callback(struct capi_pint_port_handle *handle,
				  uint8_t pin_index)
{
	if (!handle || !handle->ops || !handle->ops->unregister_callback) {
		return -EINVAL;
	}

	return handle->ops->unregister_callback(handle, pin_index);
}

int capi_pint_clear_pending(struct capi_pint_port_handle *handle,
			    uint8_t pin_index)
{
	if (!handle || !handle->ops || !handle->ops->clear_pending) {
		return -EINVAL;
	}

	return handle->ops->clear_pending(handle, pin_index);
}

int capi_pint_get_pending(struct capi_pint_port_handle *handle,
			  uint32_t *pending)
{
	if (!handle || !handle->ops || !handle->ops->get_pending) {
		return -EINVAL;
	}

	return handle->ops->get_pending(handle, pending);
}

int capi_pint_handle_interrupt(struct capi_pint_port_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->handle_interrupt) {
		return -EINVAL;
	}

	return handle->ops->handle_interrupt(handle);
}
