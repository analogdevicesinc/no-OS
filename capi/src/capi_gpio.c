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
	if (!handle || !config || !config->ops || !config->ops->port_init) {
		return -EINVAL;
	}
	return config->ops->port_init(handle, config);
}

int capi_gpio_port_deinit(struct capi_gpio_port_handle **handle)
{
	if (!handle || !*handle || !(*handle)->ops || !(*handle)->ops->port_deinit) {
		return -EINVAL;
	}
	return (*handle)->ops->port_deinit(handle);
}

int capi_gpio_port_set_direction(struct capi_gpio_port_handle *handle,
				 uint64_t direction_bitmask)
{
	if (!handle || !handle->ops || !handle->ops->port_set_direction) {
		return -EINVAL;
	}
	return handle->ops->port_set_direction(handle, direction_bitmask);
}

int capi_gpio_port_get_direction(struct capi_gpio_port_handle *handle,
				 uint64_t *direction_bitmask)
{
	if (!handle || !handle->ops || !handle->ops->port_get_direction) {
		return -EINVAL;
	}
	return handle->ops->port_get_direction(handle, direction_bitmask);
}

int capi_gpio_port_set_value(struct capi_gpio_port_handle *handle,
			     uint64_t value_bitmask)
{
	if (!handle || !handle->ops || !handle->ops->port_set_value) {
		return -EINVAL;
	}
	return handle->ops->port_set_value(handle, value_bitmask);
}

int capi_gpio_port_get_value(struct capi_gpio_port_handle *handle,
			     uint64_t *value_bitmask)
{
	if (!handle || !handle->ops || !handle->ops->port_get_value) {
		return -EINVAL;
	}
	return handle->ops->port_get_value(handle, value_bitmask);
}

int capi_gpio_port_set_raw_value(struct capi_gpio_port_handle *handle,
				 uint64_t value_bitmask)
{
	if (!handle || !handle->ops || !handle->ops->port_set_raw_value) {
		return -EINVAL;
	}
	return handle->ops->port_set_raw_value(handle, value_bitmask);
}

int capi_gpio_port_get_raw_value(struct capi_gpio_port_handle *handle,
				 uint64_t *value_bitmask)
{
	if (!handle || !handle->ops || !handle->ops->port_get_raw_value) {
		return -EINVAL;
	}
	return handle->ops->port_get_raw_value(handle, value_bitmask);
}

int capi_gpio_port_toggle(struct capi_gpio_port_handle *handle,
			  uint64_t pins_bitmask)
{
	if (!handle || !handle->ops || !handle->ops->port_toggle) {
		return -EINVAL;
	}
	return handle->ops->port_toggle(handle, pins_bitmask);
}

int capi_gpio_pin_set_direction(struct capi_gpio_pin *pin, uint8_t direction)
{
	if (!pin || !pin->port_handle || !pin->port_handle->ops ||
	    !pin->port_handle->ops->pin_set_direction) {
		return -EINVAL;
	}
	return pin->port_handle->ops->pin_set_direction(pin, direction);
}

int capi_gpio_pin_get_direction(struct capi_gpio_pin *pin, uint8_t *direction)
{
	if (!pin || !pin->port_handle || !pin->port_handle->ops ||
	    !pin->port_handle->ops->pin_get_direction) {
		return -EINVAL;
	}
	return pin->port_handle->ops->pin_get_direction(pin, direction);
}

int capi_gpio_pin_set_value(struct capi_gpio_pin *pin, uint8_t value)
{
	if (!pin || !pin->port_handle || !pin->port_handle->ops ||
	    !pin->port_handle->ops->pin_set_value) {
		return -EINVAL;
	}
	return pin->port_handle->ops->pin_set_value(pin, value);
}

int capi_gpio_pin_get_value(struct capi_gpio_pin *pin, uint8_t *value)
{
	if (!pin || !pin->port_handle || !pin->port_handle->ops ||
	    !pin->port_handle->ops->pin_get_value) {
		return -EINVAL;
	}
	return pin->port_handle->ops->pin_get_value(pin, value);
}

int capi_gpio_pin_set_raw_value(struct capi_gpio_pin *pin, uint8_t value)
{
	if (!pin || !pin->port_handle || !pin->port_handle->ops ||
	    !pin->port_handle->ops->pin_set_raw_value) {
		return -EINVAL;
	}
	return pin->port_handle->ops->pin_set_raw_value(pin, value);
}

int capi_gpio_pin_get_raw_value(struct capi_gpio_pin *pin, uint8_t *value)
{
	if (!pin || !pin->port_handle || !pin->port_handle->ops ||
	    !pin->port_handle->ops->pin_get_raw_value) {
		return -EINVAL;
	}
	return pin->port_handle->ops->pin_get_raw_value(pin, value);
}

int capi_gpio_pin_toggle(struct capi_gpio_pin *pin)
{
	if (!pin || !pin->port_handle || !pin->port_handle->ops ||
	    !pin->port_handle->ops->pin_toggle) {
		return -EINVAL;
	}
	return pin->port_handle->ops->pin_toggle(pin);
}
