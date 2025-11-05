/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_gpio.c
 * @brief MAX32655 platform-specific GPIO CAPI implementation
 * @author Claude Code (noreply@anthropic.com)
 */

#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "capi_gpio.h"
#include "maxim_gpio.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

#include "gpio.h"
#include "gpio_regs.h"
#include "max32655.h"
#include "mxc_errors.h"

/**
 * @brief Get GPIO registers pointer from port identifier
 */
static mxc_gpio_regs_t *get_gpio_regs(uint64_t port_id)
{
	switch (port_id) {
	case 0:
		return MXC_GPIO0;
	case 1:
		return MXC_GPIO1;
	case 2:
		return MXC_GPIO2;
	case 3:
		return MXC_GPIO3;
	default:
		return NULL;
	}
}

/**
 * @brief Initialize GPIO port
 */
static int maxim_capi_gpio_port_init(struct capi_gpio_port_handle **handle,
                                     const struct capi_gpio_port_config *config)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	int ret;
	int i;

	if (!handle || !config)
		return -EINVAL;

	if (!*handle) {
		maxim_handle = no_os_calloc(1, sizeof(*maxim_handle));
		if (!maxim_handle)
			return -ENOMEM;

		maxim_handle->capi_handle.init_allocated = true;
	} else {
		maxim_handle = (struct maxim_capi_gpio_handle *)*handle;
		maxim_handle->capi_handle.init_allocated = false;
	}

	maxim_handle->port_id = config->identifier;
	maxim_handle->gpio_regs = get_gpio_regs(config->identifier);
	if (!maxim_handle->gpio_regs) {
		ret = -EINVAL;
		goto error;
	}

	maxim_handle->num_pins = config->num_pins;
	if (maxim_handle->num_pins > MXC_CFG_GPIO_PINS_PORT) {
		ret = -EINVAL;
		goto error;
	}

	/* Allocate pin flags array if provided */
	if (config->flags && config->num_pins > 0) {
		maxim_handle->pin_flags = no_os_calloc(config->num_pins, sizeof(uint32_t));
		if (!maxim_handle->pin_flags) {
			ret = -ENOMEM;
			goto error;
		}

		/* Copy flags */
		for (i = 0; i < config->num_pins; i++) {
			maxim_handle->pin_flags[i] = config->flags[i];
		}
	}

	/* Initialize GPIO port */
	MXC_GPIO_Init(maxim_handle->port_id);

	/* Set up CAPI handle */
	maxim_handle->capi_handle.ops = config->ops;
	maxim_handle->capi_handle.identifier = config->identifier;
	maxim_handle->capi_handle.priv = config->priv;

	*handle = &maxim_handle->capi_handle;

	return 0;

error:
	if (maxim_handle->capi_handle.init_allocated)
		no_os_free(maxim_handle);
	return ret;
}

/**
 * @brief Deinitialize GPIO port
 */
static int maxim_capi_gpio_port_deinit(struct capi_gpio_port_handle **handle)
{
	struct maxim_capi_gpio_handle *maxim_handle;

	if (!handle || !*handle)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)*handle;

	if (maxim_handle->pin_flags)
		no_os_free(maxim_handle->pin_flags);

	if (maxim_handle->capi_handle.init_allocated)
		no_os_free(maxim_handle);

	*handle = NULL;

	return 0;
}

/**
 * @brief Set direction of all pins in port using bitmask
 */
static int maxim_capi_gpio_port_set_direction(struct capi_gpio_port_handle *handle,
                                              uint64_t direction_bitmask)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	mxc_gpio_cfg_t gpio_cfg;
	uint32_t mask;
	int i;

	if (!handle)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)handle;

	/* Configure each pin based on direction bitmask */
	for (i = 0; i < maxim_handle->num_pins; i++) {
		mask = NO_OS_BIT(i);

		gpio_cfg.port = maxim_handle->gpio_regs;
		gpio_cfg.mask = mask;
		gpio_cfg.pad = MXC_GPIO_PAD_NONE;
		gpio_cfg.vssel = MXC_GPIO_VSSEL_VDDIO;

		if (direction_bitmask & mask) {
			/* Input */
			gpio_cfg.func = MXC_GPIO_FUNC_IN;
		} else {
			/* Output */
			gpio_cfg.func = MXC_GPIO_FUNC_OUT;
		}

		if (MXC_GPIO_Config(&gpio_cfg) != E_SUCCESS)
			return -EIO;
	}

	return 0;
}

/**
 * @brief Get direction of all pins in port as bitmask
 */
static int maxim_capi_gpio_port_get_direction(struct capi_gpio_port_handle *handle,
                                              uint64_t *direction_bitmask)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	uint32_t out_en_reg;

	if (!handle || !direction_bitmask)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)handle;

	/* Read output enable register - bit set means output, clear means input */
	out_en_reg = maxim_handle->gpio_regs->outen;

	/* Invert bits since CAPI uses 1 for input, 0 for output */
	*direction_bitmask = (~out_en_reg) & ((1ULL << maxim_handle->num_pins) - 1);

	return 0;
}

/**
 * @brief Apply active low logic to value based on pin flags
 */
static uint64_t apply_active_logic(uint64_t value, uint32_t *flags, uint8_t num_pins)
{
	uint64_t result = 0;
	int i;

	if (!flags)
		return value;

	for (i = 0; i < num_pins; i++) {
		uint64_t pin_mask = 1ULL << i;
		uint64_t pin_value = (value & pin_mask) ? 1 : 0;

		/* If pin is active low, invert the value */
		if (flags[i] & CAPI_GPIO_ACTIVE_LOW)
			pin_value = !pin_value;

		if (pin_value)
			result |= pin_mask;
	}

	return result;
}

/**
 * @brief Set value of all pins in port using bitmask
 */
static int maxim_capi_gpio_port_set_value(struct capi_gpio_port_handle *handle,
                                          uint64_t value_bitmask)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	uint64_t raw_value;
	uint32_t set_mask, clear_mask;

	if (!handle)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)handle;

	/* Apply active high/low logic */
	raw_value = apply_active_logic(value_bitmask, maxim_handle->pin_flags, maxim_handle->num_pins);

	/* Convert to 32-bit masks */
	set_mask = (uint32_t)(raw_value & 0xFFFFFFFF);
	clear_mask = (uint32_t)((~raw_value) & ((1ULL << maxim_handle->num_pins) - 1));

	/* Set pins that should be high */
	if (set_mask)
		MXC_GPIO_OutSet(maxim_handle->gpio_regs, set_mask);

	/* Clear pins that should be low */
	if (clear_mask)
		MXC_GPIO_OutClr(maxim_handle->gpio_regs, clear_mask);

	return 0;
}

/**
 * @brief Get value of all pins in port as bitmask
 */
static int maxim_capi_gpio_port_get_value(struct capi_gpio_port_handle *handle,
                                          uint64_t *value_bitmask)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	uint32_t raw_value;
	uint64_t logical_value;

	if (!handle || !value_bitmask)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)handle;

	/* Read raw hardware value */
	raw_value = MXC_GPIO_InGet(maxim_handle->gpio_regs,
	                          (1ULL << maxim_handle->num_pins) - 1);

	/* Apply active high/low logic (invert the inversion) */
	logical_value = apply_active_logic(raw_value, maxim_handle->pin_flags, maxim_handle->num_pins);

	*value_bitmask = logical_value;

	return 0;
}

/**
 * @brief Set raw value of all pins in port using bitmask (ignore active flags)
 */
static int maxim_capi_gpio_port_set_raw_value(struct capi_gpio_port_handle *handle,
                                              uint64_t value_bitmask)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	uint32_t set_mask, clear_mask;

	if (!handle)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)handle;

	/* Convert to 32-bit masks */
	set_mask = (uint32_t)(value_bitmask & 0xFFFFFFFF);
	clear_mask = (uint32_t)((~value_bitmask) & ((1ULL << maxim_handle->num_pins) - 1));

	/* Set pins that should be high */
	if (set_mask)
		MXC_GPIO_OutSet(maxim_handle->gpio_regs, set_mask);

	/* Clear pins that should be low */
	if (clear_mask)
		MXC_GPIO_OutClr(maxim_handle->gpio_regs, clear_mask);

	return 0;
}

/**
 * @brief Get raw value of all pins in port as bitmask (ignore active flags)
 */
static int maxim_capi_gpio_port_get_raw_value(struct capi_gpio_port_handle *handle,
                                              uint64_t *value_bitmask)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	uint32_t raw_value;

	if (!handle || !value_bitmask)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)handle;

	/* Read raw hardware value */
	raw_value = MXC_GPIO_InGet(maxim_handle->gpio_regs,
	                          (1ULL << maxim_handle->num_pins) - 1);

	*value_bitmask = raw_value;

	return 0;
}

/**
 * @brief Set direction of a specific GPIO pin
 */
static int maxim_capi_gpio_pin_set_direction(struct capi_gpio_pin *pin, uint8_t direction)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	mxc_gpio_cfg_t gpio_cfg;

	if (!pin || !pin->port_handle || pin->number < 0)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)pin->port_handle;

	if (pin->number >= maxim_handle->num_pins)
		return -EINVAL;

	gpio_cfg.port = maxim_handle->gpio_regs;
	gpio_cfg.mask = NO_OS_BIT(pin->number);
	gpio_cfg.pad = MXC_GPIO_PAD_NONE;
	gpio_cfg.vssel = MXC_GPIO_VSSEL_VDDIO;

	if (direction == CAPI_GPIO_INPUT) {
		gpio_cfg.func = MXC_GPIO_FUNC_IN;
	} else {
		gpio_cfg.func = MXC_GPIO_FUNC_OUT;
	}

	if (MXC_GPIO_Config(&gpio_cfg) != E_SUCCESS)
		return -EIO;

	return 0;
}

/**
 * @brief Get direction of a specific GPIO pin
 */
static int maxim_capi_gpio_pin_get_direction(struct capi_gpio_pin *pin, uint8_t *direction)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	uint32_t out_en_reg;

	if (!pin || !pin->port_handle || !direction || pin->number < 0)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)pin->port_handle;

	if (pin->number >= maxim_handle->num_pins)
		return -EINVAL;

	/* Check output enable register */
	out_en_reg = maxim_handle->gpio_regs->outen;

	if (out_en_reg & NO_OS_BIT(pin->number))
		*direction = CAPI_GPIO_OUTPUT;
	else
		*direction = CAPI_GPIO_INPUT;

	return 0;
}

/**
 * @brief Set value of a specific GPIO pin
 */
static int maxim_capi_gpio_pin_set_value(struct capi_gpio_pin *pin, uint8_t value)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	uint8_t raw_value;

	if (!pin || !pin->port_handle || pin->number < 0)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)pin->port_handle;

	if (pin->number >= maxim_handle->num_pins)
		return -EINVAL;

	/* Apply active high/low logic */
	raw_value = value;
	if (maxim_handle->pin_flags && (pin->flags & CAPI_GPIO_ACTIVE_LOW))
		raw_value = !raw_value;

	if (raw_value == CAPI_GPIO_HIGH)
		MXC_GPIO_OutSet(maxim_handle->gpio_regs, NO_OS_BIT(pin->number));
	else
		MXC_GPIO_OutClr(maxim_handle->gpio_regs, NO_OS_BIT(pin->number));

	return 0;
}

/**
 * @brief Get value of a specific GPIO pin
 */
static int maxim_capi_gpio_pin_get_value(struct capi_gpio_pin *pin, uint8_t *value)
{
	struct maxim_capi_gpio_handle *maxim_handle;
	uint8_t raw_value;

	if (!pin || !pin->port_handle || !value || pin->number < 0)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)pin->port_handle;

	if (pin->number >= maxim_handle->num_pins)
		return -EINVAL;

	/* Read raw hardware value */
	raw_value = (MXC_GPIO_InGet(maxim_handle->gpio_regs, NO_OS_BIT(pin->number))
	             >> pin->number) & 1;

	/* Apply active high/low logic */
	if (maxim_handle->pin_flags && (pin->flags & CAPI_GPIO_ACTIVE_LOW))
		raw_value = !raw_value;

	*value = raw_value;

	return 0;
}

/**
 * @brief Set raw value of a specific GPIO pin (ignore active flags)
 */
static int maxim_capi_gpio_pin_set_raw_value(struct capi_gpio_pin *pin, uint8_t value)
{
	struct maxim_capi_gpio_handle *maxim_handle;

	if (!pin || !pin->port_handle || pin->number < 0)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)pin->port_handle;

	if (pin->number >= maxim_handle->num_pins)
		return -EINVAL;

	if (value == CAPI_GPIO_HIGH)
		MXC_GPIO_OutSet(maxim_handle->gpio_regs, NO_OS_BIT(pin->number));
	else
		MXC_GPIO_OutClr(maxim_handle->gpio_regs, NO_OS_BIT(pin->number));

	return 0;
}

/**
 * @brief Get raw value of a specific GPIO pin (ignore active flags)
 */
static int maxim_capi_gpio_pin_get_raw_value(struct capi_gpio_pin *pin, uint8_t *value)
{
	struct maxim_capi_gpio_handle *maxim_handle;

	if (!pin || !pin->port_handle || !value || pin->number < 0)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_gpio_handle *)pin->port_handle;

	if (pin->number >= maxim_handle->num_pins)
		return -EINVAL;

	*value = (MXC_GPIO_InGet(maxim_handle->gpio_regs, NO_OS_BIT(pin->number))
	          >> pin->number) & 1;

	return 0;
}

/**
 * @brief MAX32655 GPIO CAPI operations structure
 */
const struct capi_gpio_ops maxim_capi_gpio_ops = {
	.port_init = maxim_capi_gpio_port_init,
	.port_deinit = maxim_capi_gpio_port_deinit,
	.port_set_direction = maxim_capi_gpio_port_set_direction,
	.port_get_direction = maxim_capi_gpio_port_get_direction,
	.port_set_value = maxim_capi_gpio_port_set_value,
	.port_get_value = maxim_capi_gpio_port_get_value,
	.port_set_raw_value = maxim_capi_gpio_port_set_raw_value,
	.port_get_raw_value = maxim_capi_gpio_port_get_raw_value,
	.pin_set_direction = maxim_capi_gpio_pin_set_direction,
	.pin_get_direction = maxim_capi_gpio_pin_get_direction,
	.pin_set_value = maxim_capi_gpio_pin_set_value,
	.pin_get_value = maxim_capi_gpio_pin_get_value,
	.pin_set_raw_value = maxim_capi_gpio_pin_set_raw_value,
	.pin_get_raw_value = maxim_capi_gpio_pin_get_raw_value,
};