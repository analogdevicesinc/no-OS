/*
 * Copyright (c) 2024-2026 Analog Devices, Inc.
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
	if (!handle || !config || !config->ops || !config->ops->init) {
		return -EINVAL;
	}
	return config->ops->init(handle, config);
}

int capi_i2c_deinit(struct capi_i2c_controller_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->deinit) {
		return -EINVAL;
	}
	return handle->ops->deinit(handle);
}

int capi_i2c_transmit(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !device->controller->ops ||
	    !device->controller->ops->transmit) {
		return -EINVAL;
	}
	return device->controller->ops->transmit(device, transfer);
}

int capi_i2c_receive(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !device->controller->ops ||
	    !device->controller->ops->receive) {
		return -EINVAL;
	}
	return device->controller->ops->receive(device, transfer);
}

int capi_i2c_configure_bus_speed(struct capi_i2c_controller_handle *handle,
				 enum capi_i2c_speed speed, uint8_t duty_cycle)
{
	if (!handle || !handle->ops || !handle->ops->configure_bus_speed) {
		return -EINVAL;
	}
	return handle->ops->configure_bus_speed(handle, speed, duty_cycle);
}

int capi_i2c_register_callback(struct capi_i2c_controller_handle *handle,
			       capi_i2c_callback const callback, void *const callback_arg)
{
	if (!handle || !handle->ops || !handle->ops->register_callback) {
		return -EINVAL;
	}
	return handle->ops->register_callback(handle, callback, callback_arg);
}

int capi_i2c_transmit_async(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !device->controller->ops ||
	    !device->controller->ops->transmit_async) {
		return -EINVAL;
	}
	return device->controller->ops->transmit_async(device, transfer);
}

int capi_i2c_receive_async(struct capi_i2c_device *device, struct capi_i2c_transfer *transfer)
{
	if (!device || !device->controller || !device->controller->ops ||
	    !device->controller->ops->receive_async) {
		return -EINVAL;
	}
	return device->controller->ops->receive_async(device, transfer);
}

int capi_i2c_recover_bus(struct capi_i2c_controller_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->recover_bus) {
		return -EINVAL;
	}
	return handle->ops->recover_bus(handle);
}

int capi_i2c_register_target(struct capi_i2c_controller_handle *handle, uint16_t addr)
{
	if (!handle || !handle->ops || !handle->ops->register_target) {
		return -EINVAL;
	}
	return handle->ops->register_target(handle, addr);
}

int capi_i2c_unregister_target(struct capi_i2c_controller_handle *handle)
{
	if (!handle || !handle->ops || !handle->ops->unregister_target) {
		return -EINVAL;
	}
	return handle->ops->unregister_target(handle);
}

void capi_i2c_isr(void *handle)
{
	if (!handle) {
		return;
	}
	struct capi_i2c_controller_handle *ctrl_handle =
		(struct capi_i2c_controller_handle *)handle;
	if (!ctrl_handle->ops || !ctrl_handle->ops->isr) {
		return;
	}
	ctrl_handle->ops->isr(handle);
}
