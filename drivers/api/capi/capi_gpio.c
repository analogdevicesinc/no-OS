/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_gpio.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the GPIO driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the GPIO functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_gpio.h>

int capi_gpio_port_init(struct capi_gpio_port_handle **handle,
			const struct capi_gpio_port_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->port_init != NULL) {
		return config->ops->port_init(handle, config);
	}

	return -EINVAL;
}

int capi_gpio_port_deinit(struct capi_gpio_port_handle **handle)
{
	if (handle != NULL && *handle != NULL && (*handle)->ops != NULL &&
	    (*handle)->ops->port_deinit != NULL) {
		return (*handle)->ops->port_deinit(handle);
	}

	return -EINVAL;
}

int capi_gpio_port_set_direction(struct capi_gpio_port_handle *handle, uint64_t direction_bitmask)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->port_set_direction != NULL) {
		return handle->ops->port_set_direction(handle, direction_bitmask);
	}

	return -EINVAL;
}

int capi_gpio_port_get_direction(struct capi_gpio_port_handle *handle, uint64_t *direction_bitmask)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->port_get_direction != NULL) {
		return handle->ops->port_get_direction(handle, direction_bitmask);
	}

	return -EINVAL;
}

int capi_gpio_port_set_value(struct capi_gpio_port_handle *handle, uint64_t value_bitmask)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->port_set_value != NULL) {
		return handle->ops->port_set_value(handle, value_bitmask);
	}

	return -EINVAL;
}

int capi_gpio_port_get_value(struct capi_gpio_port_handle *handle, uint64_t *value_bitmask)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->port_get_value != NULL) {
		return handle->ops->port_get_value(handle, value_bitmask);
	}

	return -EINVAL;
}

int capi_gpio_port_set_raw_value(struct capi_gpio_port_handle *handle, uint64_t value_bitmask)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->port_set_raw_value != NULL) {
		return handle->ops->port_set_raw_value(handle, value_bitmask);
	}

	return -EINVAL;
}

int capi_gpio_port_get_raw_value(struct capi_gpio_port_handle *handle, uint64_t *value_bitmask)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->port_get_raw_value != NULL) {
		return handle->ops->port_get_raw_value(handle, value_bitmask);
	}

	return -EINVAL;
}

int capi_gpio_pin_set_direction(struct capi_gpio_pin *pin, uint8_t direction)
{
	if (pin != NULL && pin->port_handle != NULL && pin->port_handle->ops != NULL &&
	    pin->port_handle->ops->pin_set_direction != NULL) {
		return pin->port_handle->ops->pin_set_direction(pin, direction);
	}

	return -EINVAL;
}

int capi_gpio_pin_get_direction(struct capi_gpio_pin *pin, uint8_t *direction)
{
	if (pin != NULL && pin->port_handle != NULL && pin->port_handle->ops != NULL &&
	    pin->port_handle->ops->pin_get_direction != NULL) {
		return pin->port_handle->ops->pin_get_direction(pin, direction);
	}

	return -EINVAL;
}

int capi_gpio_pin_set_value(struct capi_gpio_pin *pin, uint8_t value)
{
	if (pin != NULL && pin->port_handle != NULL && pin->port_handle->ops != NULL &&
	    pin->port_handle->ops->pin_set_value != NULL) {
		return pin->port_handle->ops->pin_set_value(pin, value);
	}

	return -EINVAL;
}

int capi_gpio_pin_get_value(struct capi_gpio_pin *pin, uint8_t *value)
{
	if (pin != NULL && pin->port_handle != NULL && pin->port_handle->ops != NULL &&
	    pin->port_handle->ops->pin_get_value != NULL) {
		return pin->port_handle->ops->pin_get_value(pin, value);
	}

	return -EINVAL;
}

int capi_gpio_pin_set_raw_value(struct capi_gpio_pin *pin, uint8_t value)
{
	if (pin != NULL && pin->port_handle != NULL && pin->port_handle->ops != NULL &&
	    pin->port_handle->ops->pin_set_raw_value != NULL) {
		return pin->port_handle->ops->pin_set_raw_value(pin, value);
	}

	return -EINVAL;
}

int capi_gpio_pin_get_raw_value(struct capi_gpio_pin *pin, uint8_t *value)
{
	if (pin != NULL && pin->port_handle != NULL && pin->port_handle->ops != NULL &&
	    pin->port_handle->ops->pin_get_raw_value != NULL) {
		return pin->port_handle->ops->pin_get_raw_value(pin, value);
	}

	return -EINVAL;
}