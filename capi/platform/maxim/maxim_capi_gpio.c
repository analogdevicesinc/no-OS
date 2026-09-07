/***************************************************************************//**
 *   @file   maxim_capi_gpio.c
 *   @brief  Implementation of GPIO functions with CAPI.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/*
 * Common Maxim CAPI GPIO backend.
 *
 * One file for every MSDK target. Every MAX32xxx/MAX78xxx GPIO block exposes the
 * same programming model through the MSDK MXC_GPIO_* API (config by pin mask,
 * OutSet/OutClr/InGet, per-mask vssel/drvstr), so there is no per-part shim here
 * -- the part only changes how many instances exist (MXC_CFG_GPIO_INSTANCES) and
 * how many pins each carries (MXC_CFG_GPIO_PINS_PORT), both of which come from
 * the CMSIS device header.
 *
 * Direction is derived from the CAPI call (a set bit in the direction bitmask is
 * an input, matching CAPI_GPIO_INPUT == 1), while the pad/voltage/drive-strength
 * come from the port's extra config; both are folded into the mxc_gpio_cfg_t
 * handed to MXC_GPIO_Config(). The ACTIVE_LOW flag is a logical attribute: the
 * value ops invert the wire level for flagged pins, the raw ops never do, and a
 * toggle (an inversion of an inversion) is unaffected by it.
 */

#include <errno.h>
#include <stdlib.h>
/*
 * mxc_device.h selects this part's max<n>.h (device-level GPIO macros:
 * MXC_CFG_GPIO_INSTANCES/PINS_PORT, MXC_GPIO_GET_GPIO/IDX) and mxc_errors.h
 * (E_NO_ERROR). gpio.h itself only pulls in the register layout, so include it
 * here rather than relying on another header having done so first.
 */
#include "mxc_device.h"
#include "capi_alloc.h"
#include "capi_gpio.h"
#include "maxim_capi_gpio_priv.h"

/** Pins per MSDK GPIO port. */
#define MAX_GPIO_PINS_PER_PORT		MXC_CFG_GPIO_PINS_PORT

static int max_capi_gpio_port_set_raw_value(struct capi_gpio_port_handle *handle,
		uint64_t value_bitmask);
static int max_capi_gpio_port_get_raw_value(struct capi_gpio_port_handle *handle,
		uint64_t *value_bitmask);
static int max_capi_gpio_pin_set_raw_value(struct capi_gpio_pin *pin,
		uint8_t value);
static int max_capi_gpio_pin_get_raw_value(struct capi_gpio_pin *pin,
		uint8_t *value);

/**
 * @brief Resolve a CAPI port identifier to an MSDK GPIO instance.
 * @param identifier - Port identifier: a small instance index (0, 1, ...) or a
 *                     GPIO peripheral base address.
 * @param out_idx - If non-NULL, receives the resolved instance index.
 * @return Pointer to the GPIO peripheral, or NULL if the identifier is invalid.
 *
 * A base address is always larger than the instance count, so the two encodings
 * never collide: values below MXC_CFG_GPIO_INSTANCES are treated as indices, and
 * anything else is validated as a base-address pointer via MXC_GPIO_GET_IDX().
 */
static mxc_gpio_regs_t *max_gpio_port_from_identifier(uint64_t identifier,
		int *out_idx)
{
	mxc_gpio_regs_t *port;
	int idx;

	if (identifier < (uint64_t)MXC_CFG_GPIO_INSTANCES) {
		idx = (int)identifier;
		port = MXC_GPIO_GET_GPIO(idx);
	} else {
		port = (mxc_gpio_regs_t *)(uintptr_t)identifier;
		idx = MXC_GPIO_GET_IDX(port);
	}

	if (!port || idx < 0)
		return NULL;

	if (out_idx)
		*out_idx = idx;

	return port;
}

/**
 * @brief Apply an MSDK GPIO configuration to a set of pins.
 * @param priv - Port private data (source of pad/vssel/drvstr).
 * @param mask - Pin mask to configure.
 * @param func - GPIO function to program (input, output or alternate).
 * @return 0 on success, negative error code otherwise.
 */
static int max_gpio_apply_cfg(struct max_capi_gpio_port_priv *priv,
			      uint32_t mask, mxc_gpio_func_t func)
{
	mxc_gpio_cfg_t cfg;

	if (!mask)
		return 0;

	cfg.port = priv->port;
	cfg.mask = mask;
	cfg.func = func;
	cfg.pad = (mxc_gpio_pad_t)priv->extra.pad;
	cfg.vssel = (mxc_gpio_vssel_t)priv->extra.vssel;
	cfg.drvstr = (mxc_gpio_drvstr_t)priv->extra.drvstr;

	return MXC_GPIO_Config(&cfg) == E_NO_ERROR ? 0 : -EIO;
}

/**
 * @brief Initialize a GPIO port.
 * @param handle - Pointer to port handle pointer. If *handle is NULL, memory
 *                 is allocated by the driver.
 * @param config - Pointer to the GPIO port configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_port_init(struct capi_gpio_port_handle **handle,
				   const struct capi_gpio_port_config *config)
{
	struct capi_gpio_port_handle *port_handle;
	struct max_capi_gpio_port_priv *priv;
	mxc_gpio_regs_t *port;
	uint8_t num_pins;
	int idx;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	port = max_gpio_port_from_identifier(config->identifier, &idx);
	if (!port)
		return -EINVAL;

	num_pins = config->num_pins ? config->num_pins : MAX_GPIO_PINS_PER_PORT;
	if (num_pins > MAX_GPIO_PINS_PER_PORT)
		return -EINVAL;

	/* Enable the port clock. Idempotent, safe to repeat across ports. */
	ret = MXC_GPIO_Init(1U << idx);
	if (ret != E_NO_ERROR)
		return -EIO;

	if (*handle == NULL) {
		port_handle = capi_calloc(1, sizeof(*port_handle));
		if (!port_handle)
			return -ENOMEM;

		priv = capi_calloc(1, sizeof(*priv));
		if (!priv) {
			capi_free(port_handle);
			return -ENOMEM;
		}

		port_handle->priv = priv;
		port_handle->init_allocated = true;
	} else {
		port_handle = *handle;
		port_handle->init_allocated = false;

		if (!port_handle->priv)
			return -EINVAL;

		priv = port_handle->priv;
	}

	port_handle->ops = config->ops;

	priv->id = (uint32_t)idx;
	priv->port = port;
	priv->num_pins = num_pins;
	priv->pin_mask = (num_pins >= MAX_GPIO_PINS_PER_PORT) ?
			 0xFFFFFFFFU : ((1U << num_pins) - 1U);
	/* Reset state is input on every Maxim GPIO block. */
	priv->direction_mask = priv->pin_mask;

	/*
	 * Fold the per-pin ACTIVE_LOW flags into a single mask. The CAPI flags
	 * array is one entry per pin and only lives as long as the caller's
	 * config, so snapshot it here; the port-level logical value ops use this
	 * mask to invert the wire level (raw = logical ^ active_low).
	 */
	priv->active_low_mask = 0U;
	if (config->flags) {
		for (uint8_t i = 0U; i < num_pins; i++) {
			if (config->flags[i] & CAPI_GPIO_ACTIVE_LOW)
				priv->active_low_mask |= (1U << i);
		}
	}

	if (config->extra) {
		priv->extra = *(struct max_capi_gpio_extra_config *)config->extra;
	} else {
		priv->extra.vssel = MAX_CAPI_GPIO_VSSEL_VDDIO;
		priv->extra.func = MAX_CAPI_GPIO_FUNC_IN;
		priv->extra.pad = MAX_CAPI_GPIO_PAD_NONE;
		priv->extra.drvstr = MAX_CAPI_GPIO_DRVSTR_0;
	}

	/*
	 * A sparse mask, when supplied, wins over the contiguous window derived
	 * from num_pins -- see max_capi_gpio_extra_config::pin_mask. num_pins is
	 * left alone: it still bounds the pin indices the caller may name.
	 */
	if (priv->extra.pin_mask) {
		priv->pin_mask = priv->extra.pin_mask;
		priv->direction_mask = priv->pin_mask;
	}

	*handle = port_handle;
	return 0;
}

/**
 * @brief Deinitialize a GPIO port.
 * @param handle - Pointer to port handle pointer. Set to NULL on return.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_port_deinit(struct capi_gpio_port_handle **handle)
{
	struct capi_gpio_port_handle *port_handle;

	if (!handle || !*handle)
		return -EINVAL;

	port_handle = *handle;

	if (port_handle->init_allocated) {
		capi_free(port_handle->priv);
		capi_free(port_handle);
	}

	*handle = NULL;
	return 0;
}

/**
 * @brief Set direction for all pins in the port.
 * @param handle - Pointer to the GPIO port handle.
 * @param direction_bitmask - Direction bitmask (1=input, 0=output per pin).
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_port_set_direction(struct capi_gpio_port_handle *handle,
		uint64_t direction_bitmask)
{
	struct max_capi_gpio_port_priv *priv;
	uint32_t input_pins;
	uint32_t output_pins;
	int ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	/* A set bit selects an input (CAPI_GPIO_INPUT == 1). */
	input_pins = (uint32_t)direction_bitmask & priv->pin_mask;
	output_pins = ~(uint32_t)direction_bitmask & priv->pin_mask;

	ret = max_gpio_apply_cfg(priv, input_pins, MXC_GPIO_FUNC_IN);
	if (ret)
		return ret;

	ret = max_gpio_apply_cfg(priv, output_pins, MXC_GPIO_FUNC_OUT);
	if (ret)
		return ret;

	priv->direction_mask = (uint32_t)direction_bitmask & priv->pin_mask;
	return 0;
}

/**
 * @brief Get direction of all pins in the port.
 * @param handle - Pointer to the GPIO port handle.
 * @param direction_bitmask - Pointer to store direction bitmask.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_port_get_direction(struct capi_gpio_port_handle *handle,
		uint64_t *direction_bitmask)
{
	struct max_capi_gpio_port_priv *priv;

	if (!handle || !handle->priv || !direction_bitmask)
		return -EINVAL;

	priv = handle->priv;
	*direction_bitmask = priv->direction_mask;
	return 0;
}

/**
 * @brief Set value for all pins in the port (considering ACTIVE_LOW flags).
 * @param handle - Pointer to the GPIO port handle.
 * @param value_bitmask - Value bitmask to set.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_port_set_value(struct capi_gpio_port_handle *handle,
					uint64_t value_bitmask)
{
	struct max_capi_gpio_port_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	/* Logical -> raw: invert the electrical level of active-low pins. */
	return max_capi_gpio_port_set_raw_value(handle,
						value_bitmask ^ (uint64_t)priv->active_low_mask);
}

/**
 * @brief Get value of all pins in the port (considering ACTIVE_LOW flags).
 * @param handle - Pointer to the GPIO port handle.
 * @param value_bitmask - Pointer to store value bitmask.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_port_get_value(struct capi_gpio_port_handle *handle,
					uint64_t *value_bitmask)
{
	struct max_capi_gpio_port_priv *priv;
	int ret;

	if (!handle || !handle->priv || !value_bitmask)
		return -EINVAL;

	priv = handle->priv;

	ret = max_capi_gpio_port_get_raw_value(handle, value_bitmask);
	if (ret)
		return ret;

	/* Raw -> logical: invert the reported level of active-low pins. */
	*value_bitmask ^= (uint64_t)priv->active_low_mask;
	return 0;
}

/**
 * @brief Set raw value for all pins (ignoring ACTIVE_LOW flag).
 * @param handle - Pointer to the GPIO port handle.
 * @param value_bitmask - Raw value bitmask to set.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_port_set_raw_value(struct capi_gpio_port_handle *handle,
		uint64_t value_bitmask)
{
	struct max_capi_gpio_port_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	/* OutPut writes only the bits selected by pin_mask. */
	MXC_GPIO_OutPut(priv->port, priv->pin_mask, (uint32_t)value_bitmask);
	return 0;
}

/**
 * @brief Get raw value of all pins (ignoring ACTIVE_LOW flag).
 * @param handle - Pointer to the GPIO port handle.
 * @param value_bitmask - Pointer to store raw value bitmask.
 * @return 0 on success, negative error code otherwise.
 *
 * Reads the pad level (GPIO_IN) for every pin, matching the input-register
 * semantics: a push-pull output reads back the level it is driving.
 */
static int max_capi_gpio_port_get_raw_value(struct capi_gpio_port_handle *handle,
		uint64_t *value_bitmask)
{
	struct max_capi_gpio_port_priv *priv;

	if (!handle || !handle->priv || !value_bitmask)
		return -EINVAL;

	priv = handle->priv;
	*value_bitmask = MXC_GPIO_InGet(priv->port, priv->pin_mask);
	return 0;
}

/**
 * @brief Toggle the selected output pins of the port.
 * @param handle - Pointer to the GPIO port handle.
 * @param pins_bitmask - Bitmask of pins to toggle (1 = toggle that pin).
 * @return 0 on success, negative error code otherwise.
 *
 * Toggling flips the raw electrical level of each selected output. ACTIVE_LOW is
 * a logical attribute and does not affect a toggle (inverting an inversion is a
 * no-op), so no mask fold-in is needed here.
 */
static int max_capi_gpio_port_toggle(struct capi_gpio_port_handle *handle,
				     uint64_t pins_bitmask)
{
	struct max_capi_gpio_port_priv *priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;
	MXC_GPIO_OutToggle(priv->port, (uint32_t)pins_bitmask & priv->pin_mask);
	return 0;
}

/**
 * @brief Set direction for a single pin.
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param direction - Direction (CAPI_GPIO_INPUT or CAPI_GPIO_OUTPUT).
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_pin_set_direction(struct capi_gpio_pin *pin,
		uint8_t direction)
{
	struct max_capi_gpio_port_priv *priv;
	uint32_t mask;
	int ret;

	if (!pin || !pin->port_handle || !pin->port_handle->priv)
		return -EINVAL;

	priv = pin->port_handle->priv;

	if (pin->number >= priv->num_pins)
		return -EINVAL;

	mask = 1U << pin->number;

	if (direction == CAPI_GPIO_INPUT) {
		ret = max_gpio_apply_cfg(priv, mask, MXC_GPIO_FUNC_IN);
		if (ret)
			return ret;
		priv->direction_mask |= mask;
	} else {
		ret = max_gpio_apply_cfg(priv, mask, MXC_GPIO_FUNC_OUT);
		if (ret)
			return ret;
		priv->direction_mask &= ~mask;
	}

	return 0;
}

/**
 * @brief Get direction of a single pin.
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param direction - Pointer to store the direction.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_pin_get_direction(struct capi_gpio_pin *pin,
		uint8_t *direction)
{
	struct max_capi_gpio_port_priv *priv;

	if (!pin || !pin->port_handle || !pin->port_handle->priv || !direction)
		return -EINVAL;

	priv = pin->port_handle->priv;

	if (pin->number >= priv->num_pins)
		return -EINVAL;

	if (priv->direction_mask & (1U << pin->number))
		*direction = CAPI_GPIO_INPUT;
	else
		*direction = CAPI_GPIO_OUTPUT;

	return 0;
}

/**
 * @brief Set value for a single pin (considering ACTIVE_LOW flag).
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param value - Logical value (CAPI_GPIO_HIGH or CAPI_GPIO_LOW).
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_pin_set_value(struct capi_gpio_pin *pin, uint8_t value)
{
	uint8_t raw_value;

	if (!pin)
		return -EINVAL;

	/* Logical -> raw: invert for active-low pins. */
	if (pin->flags & CAPI_GPIO_ACTIVE_LOW)
		raw_value = value ? CAPI_GPIO_LOW : CAPI_GPIO_HIGH;
	else
		raw_value = value;

	return max_capi_gpio_pin_set_raw_value(pin, raw_value);
}

/**
 * @brief Get value of a single pin (considering ACTIVE_LOW flag).
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param value - Pointer to store the logical value.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_pin_get_value(struct capi_gpio_pin *pin, uint8_t *value)
{
	uint8_t raw_value;
	int ret;

	if (!pin || !value)
		return -EINVAL;

	ret = max_capi_gpio_pin_get_raw_value(pin, &raw_value);
	if (ret)
		return ret;

	/* Raw -> logical: invert for active-low pins. */
	if (pin->flags & CAPI_GPIO_ACTIVE_LOW)
		*value = raw_value ? CAPI_GPIO_LOW : CAPI_GPIO_HIGH;
	else
		*value = raw_value;

	return 0;
}

/**
 * @brief Set raw value for a single pin (ignoring ACTIVE_LOW flag).
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param value - Raw value to set.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_pin_set_raw_value(struct capi_gpio_pin *pin,
		uint8_t value)
{
	struct max_capi_gpio_port_priv *priv;
	uint32_t mask;

	if (!pin || !pin->port_handle || !pin->port_handle->priv)
		return -EINVAL;

	priv = pin->port_handle->priv;

	if (pin->number >= priv->num_pins)
		return -EINVAL;

	mask = 1U << pin->number;

	if (value)
		MXC_GPIO_OutSet(priv->port, mask);
	else
		MXC_GPIO_OutClr(priv->port, mask);

	return 0;
}

/**
 * @brief Get raw value of a single pin (ignoring ACTIVE_LOW flag).
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param value - Pointer to store the raw value.
 * @return 0 on success, negative error code otherwise.
 */
static int max_capi_gpio_pin_get_raw_value(struct capi_gpio_pin *pin,
		uint8_t *value)
{
	struct max_capi_gpio_port_priv *priv;

	if (!pin || !pin->port_handle || !pin->port_handle->priv || !value)
		return -EINVAL;

	priv = pin->port_handle->priv;

	if (pin->number >= priv->num_pins)
		return -EINVAL;

	*value = MXC_GPIO_InGet(priv->port, 1U << pin->number) ? CAPI_GPIO_HIGH :
		 CAPI_GPIO_LOW;

	return 0;
}

/**
 * @brief Toggle a single output pin.
 * @param pin - Pointer to the GPIO pin descriptor.
 * @return 0 on success, negative error code otherwise.
 *
 * Flips the raw electrical level of the pin. The ACTIVE_LOW flag is a logical
 * attribute and does not affect a toggle.
 */
static int max_capi_gpio_pin_toggle(struct capi_gpio_pin *pin)
{
	struct max_capi_gpio_port_priv *priv;

	if (!pin || !pin->port_handle || !pin->port_handle->priv)
		return -EINVAL;

	priv = pin->port_handle->priv;

	if (pin->number >= priv->num_pins)
		return -EINVAL;

	MXC_GPIO_OutToggle(priv->port, 1U << pin->number);
	return 0;
}

/**
 * @brief Maxim platform specific GPIO operations for CAPI
 */
const struct capi_gpio_ops max_capi_gpio_ops = {
	.port_init = max_capi_gpio_port_init,
	.port_deinit = max_capi_gpio_port_deinit,
	.port_set_direction = max_capi_gpio_port_set_direction,
	.port_get_direction = max_capi_gpio_port_get_direction,
	.port_set_value = max_capi_gpio_port_set_value,
	.port_get_value = max_capi_gpio_port_get_value,
	.port_set_raw_value = max_capi_gpio_port_set_raw_value,
	.port_get_raw_value = max_capi_gpio_port_get_raw_value,
	.port_toggle = max_capi_gpio_port_toggle,
	.pin_set_direction = max_capi_gpio_pin_set_direction,
	.pin_get_direction = max_capi_gpio_pin_get_direction,
	.pin_set_value = max_capi_gpio_pin_set_value,
	.pin_get_value = max_capi_gpio_pin_get_value,
	.pin_set_raw_value = max_capi_gpio_pin_set_raw_value,
	.pin_get_raw_value = max_capi_gpio_pin_get_raw_value,
	.pin_toggle = max_capi_gpio_pin_toggle,
};
