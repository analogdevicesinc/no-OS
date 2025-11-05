/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_i2s.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the I2S driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the I2S functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_i2s.h>

int capi_i2s_init(struct capi_i2s_handle **handle, const struct capi_i2s_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_i2s_deinit(const struct capi_i2s_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_i2s_get_config(const struct capi_i2s_handle *handle, struct capi_i2s_config *config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_config != NULL) {
		return handle->ops->get_config(handle, config);
	}

	return -EINVAL;
}

int capi_i2s_set_port_config(const struct capi_i2s_handle *handle,
			     const struct capi_i2s_port_config *port_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_port_config != NULL) {
		return handle->ops->set_port_config(handle, port_config);
	}

	return -EINVAL;
}

int capi_i2s_get_port_config(const struct capi_i2s_handle *handle,
			     struct capi_i2s_port_config *port_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_port_config != NULL) {
		return handle->ops->get_port_config(handle, port_config);
	}

	return -EINVAL;
}

int capi_i2s_port_enable(const struct capi_i2s_handle *handle, uint32_t port_no,
			 enum capi_i2s_direction direction)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->port_enable != NULL) {
		return handle->ops->port_enable(handle, port_no, direction);
	}

	return -EINVAL;
}

int capi_i2s_port_disable(const struct capi_i2s_handle *handle, uint32_t port_no,
			  enum capi_i2s_direction direction)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->port_disable != NULL) {
		return handle->ops->port_disable(handle, port_no, direction);
	}

	return -EINVAL;
}