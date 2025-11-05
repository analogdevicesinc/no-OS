/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_i2c.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the I2C driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the I2C functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_i2c.h>

int capi_i2c_init(struct capi_i2c_controller_handle **handle, const struct capi_i2c_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_i2c_deinit(struct capi_i2c_controller_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_i2c_transmit(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer)
{
	if (device != NULL && device->controller != NULL && device->controller->ops != NULL &&
	    device->controller->ops->transmit != NULL) {
		return device->controller->ops->transmit(device, transfer);
	}

	return -EINVAL;
}

int capi_i2c_receive(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer)
{
	if (device != NULL && device->controller != NULL && device->controller->ops != NULL &&
	    device->controller->ops->receive != NULL) {
		return device->controller->ops->receive(device, transfer);
	}

	return -EINVAL;
}

int capi_i2c_register_callback(struct capi_i2c_controller_handle *handle,
			       capi_i2c_callback *const callback, void *const callback_arg)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->register_callback != NULL) {
		return handle->ops->register_callback(handle, callback, callback_arg);
	}

	return -EINVAL;
}

int capi_i2c_transmit_async(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer)
{
	if (device != NULL && device->controller != NULL && device->controller->ops != NULL &&
	    device->controller->ops->transmit_async != NULL) {
		return device->controller->ops->transmit_async(device, transfer);
	}

	return -EINVAL;
}

int capi_i2c_receive_async(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer)
{
	if (device != NULL && device->controller != NULL && device->controller->ops != NULL &&
	    device->controller->ops->receive_async != NULL) {
		return device->controller->ops->receive_async(device, transfer);
	}

	return -EINVAL;
}

void capi_i2c_isr(struct capi_i2c_controller_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->isr != NULL) {
		handle->ops->isr(handle);
	}
}