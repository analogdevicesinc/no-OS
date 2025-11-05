/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_spi.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the SPI driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the SPI functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_spi.h>

int capi_spi_init(struct capi_spi_controller_handle **handle, const struct capi_spi_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_spi_deinit(struct capi_spi_controller_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_spi_transceive(struct capi_spi_device *device, struct capi_spi_transfer *transfer)
{
	if (device != NULL && device->controller != NULL && device->controller->ops != NULL &&
	    device->controller->ops->transceive != NULL) {
		return device->controller->ops->transceive(device, transfer);
	}

	return -EINVAL;
}

int capi_spi_transceive_async(struct capi_spi_device *device, struct capi_spi_transfer *transfer)
{
	if (device != NULL && device->controller != NULL && device->controller->ops != NULL &&
	    device->controller->ops->transceive_async != NULL) {
		return device->controller->ops->transceive_async(device, transfer, 0);
	}

	return -EINVAL;
}

int capi_spi_read_command(struct capi_spi_device *device, struct capi_spi_transfer *transfer)
{
	if (device != NULL && device->controller != NULL && device->controller->ops != NULL &&
	    device->controller->ops->read_command != NULL) {
		return device->controller->ops->read_command(device, transfer);
	}

	return -EINVAL;
}

int capi_spi_read_command_async(struct capi_spi_device *device, struct capi_spi_transfer *transfer)
{
	if (device != NULL && device->controller != NULL && device->controller->ops != NULL &&
	    device->controller->ops->read_command_async != NULL) {
		return device->controller->ops->read_command_async(device, transfer);
	}

	return -EINVAL;
}

int capi_spi_abort_async(struct capi_spi_device *device)
{
	if (device != NULL && device->controller != NULL && device->controller->ops != NULL &&
	    device->controller->ops->abort_async != NULL) {
		return device->controller->ops->abort_async(device);
	}

	return -EINVAL;
}

int capi_spi_register_callback(struct capi_spi_controller_handle *handle,
			       capi_spi_callback_t const callback, void *callback_arg)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->register_callback != NULL) {
		return handle->ops->register_callback(handle, callback, callback_arg);
	}

	return -EINVAL;
}

int capi_spi_set_cs(struct capi_spi_device *device, enum capi_spi_cs_control cs_control)
{
	if (device != NULL && device->controller != NULL && device->controller->ops != NULL &&
	    device->controller->ops->set_cs != NULL) {
		return device->controller->ops->set_cs(device, cs_control);
	}

	return -EINVAL;
}

void capi_spi_isr(struct capi_spi_controller_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->isr != NULL) {
		handle->ops->isr(handle);
	}
}