/***************************************************************************/ /*
*   @file   maxim_capi_gpio.c
*   @brief  Implementation of GPIO functions with CAPI.
*   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <errno.h>
#include "gpio.h"
#include "maxim_capi_gpio.h"
#include "mxc_errors.h"
#include "max32657.h"

/** Static variables **********************************************************/

static struct capi_gpio_port_handle *gpio[MXC_CFG_GPIO_INSTANCES] = {NULL};

/** GPIO functions implemenation **********************************************/
/**
 * @brief Initialize a GPIO port
 * @param handle - the GPIO port handle
 * @param config - the GPIO port configuration
 * @return 0 if successful, negative error code otherwise
 */
int max_capi_gpio_port_init(struct capi_gpio_port_handle **handle,
			    const struct capi_gpio_port_config *config)
{
	int ret;
	struct capi_gpio_port_handle *port_handle;
	struct max_capi_gpio_port_priv *priv;

	if (!handle || !config)
		return -EINVAL;

	if (config->identifier >= MXC_CFG_GPIO_INSTANCES)
		return -EINVAL;

	if (config->num_pins == 0 || config->num_pins > MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	// if (gpio[config->identifier] != NULL) {
	// 	*handle = gpio[config->identifier];
	// 	return 0;
	// }

	if (*handle == NULL) {
		port_handle = capi_calloc(1, sizeof(*port_handle));
		if (!port_handle)
			return -ENOMEM;
		port_handle->init_allocated = true;
	} else {
		port_handle = *handle;
		port_handle->init_allocated = false;
	}

	priv = capi_calloc(1, sizeof(*priv));
	if (!priv) {
		ret = -ENOMEM;
		goto free_handle;
	}

	port_handle->priv = priv;
	port_handle->ops = config->ops;

	priv->id = config->identifier;
	priv->port = MXC_GPIO_GET_GPIO(config->identifier);
	priv->num_pins = config->num_pins;
	priv->pin_mask = (1U << config->num_pins) - 1;
	priv->direction_mask = priv->pin_mask; /* All inputs by default */

	/* Copy user config or set defaults */
	if (config->extra) {
		priv->extra = *(struct max_capi_gpio_extra_config *)config->extra;
	} else {
		priv->extra.vssel = MAX_CAPI_GPIO_VSSEL_VDDIO;
		priv->extra.func = MAX_CAPI_GPIO_FUNC_IN;
		priv->extra.pad = MAX_CAPI_GPIO_PAD_NONE;
		priv->extra.drvstr = MAX_CAPI_GPIO_DRVSTR_0;
	}

	ret = MXC_GPIO_Init(config->identifier);
	if (ret != E_NO_ERROR)
		goto free_priv;

	gpio[config->identifier] = port_handle;
	*handle = port_handle;

	return 0;

free_priv:
	capi_free(priv);
free_handle:
	if (port_handle->init_allocated)
		capi_free(port_handle);

	gpio[config->identifier] = NULL;

	return ret;
}

/**
 * @brief Deinitialize a GPIO port
 * @param handle - the GPIO port handle
 * @return 0 if successful, negative error code otherwise
 */
int max_capi_gpio_port_deinit(struct capi_gpio_port_handle **handle)
{
	const struct max_capi_gpio_port_priv *gpio_priv;
	uint8_t id;

	if (!handle || !*handle || !(*handle)->priv)
		return -EINVAL;

	gpio_priv = (*handle)->priv;
	id = gpio_priv->id;

	capi_free((*handle)->priv);

	if ((*handle)->init_allocated)
		capi_free(*handle);

	gpio[id] = NULL;
	*handle = NULL;

	return 0;
}

static void set_enable(mxc_gpio_regs_t *regs, uint32_t mask, uint8_t is_enabled)
{
	if (is_enabled)
		regs->en0 |= mask;
	else
		regs->en0 &= ~mask;
}

/**
 * @brief Set direction for all pins in the port
 * @param handle - the GPIO handle
 * @param direction_bitmask - Direction bitmask (1=input, 0=output)
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_port_set_direction(struct capi_gpio_port_handle *handle,
				     uint64_t direction_bitmask)
{
	struct max_capi_gpio_port_priv *priv;
	mxc_gpio_cfg_t gpio_config;

	if (!handle)
		return -EINVAL;

	priv = handle->priv;

	/* Common configuration */
	gpio_config = (mxc_gpio_cfg_t) {
		.port = priv->port,
		.drvstr = (mxc_gpio_drvstr_t)priv->extra.drvstr,
		.pad = (mxc_gpio_pad_t)priv->extra.pad,
		.vssel = (mxc_gpio_vssel_t)priv->extra.vssel,
	};

	/* Set the output pins */
	gpio_config.func = MXC_GPIO_FUNC_OUT;
	gpio_config.mask = ~direction_bitmask & priv->pin_mask;
	MXC_GPIO_Config(&gpio_config);
	set_enable(priv->port, ~direction_bitmask & priv->pin_mask, true);
	MXC_GPIO_OutClr(priv->port, ~direction_bitmask & priv->pin_mask);

	/* Set the input pins */
	gpio_config.func = MXC_GPIO_FUNC_IN;
	gpio_config.mask = direction_bitmask & priv->pin_mask;
	MXC_GPIO_Config(&gpio_config);

	priv->direction_mask = (uint32_t)direction_bitmask & priv->pin_mask;
	return 0;
}

/**
 * @brief Get direction for all pins in the port
 * @param handle - the GPIO handle
 * @param direction_bitmask - pointer to where the bitmask will be stored
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_port_get_direction(struct capi_gpio_port_handle *handle,
				     uint64_t *direction_bitmask)
{
	const struct max_capi_gpio_port_priv *priv;

	if (!handle || !handle->priv || !direction_bitmask)
		return -EINVAL;

	priv = handle->priv;

	*direction_bitmask = priv->direction_mask;

	return 0;
}

/**
 * @brief Set raw value for all pins in the port (ignoring ACTIVE_LOW flag)
 * @param handle - the GPIO handle
 * @param value_bitmask - the value bitmask
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_port_set_raw_value(struct capi_gpio_port_handle *handle,
				     uint64_t value_bitmask)
{
	struct max_capi_gpio_port_priv *priv;
	uint32_t set_pins, reset_pins;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	set_pins = (uint32_t)value_bitmask & priv->pin_mask;
	reset_pins = (uint32_t)~value_bitmask & priv->pin_mask;

	set_enable(priv->port, priv->pin_mask, true);
	MXC_GPIO_OutSet(priv->port, set_pins);
	MXC_GPIO_OutClr(priv->port, reset_pins);

	return 0;
}

/**
 * @brief Get raw value for all pins in the port (ignoring ACTIVE_LOW flag)
 * @param handle - the GPIO handle
 * @param value_bitmask - pointer to where the value bitmask will be stored
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_port_get_raw_value(struct capi_gpio_port_handle *handle,
				     uint64_t *value_bitmask)
{
	struct max_capi_gpio_port_priv *priv;
	uint32_t in_pins, out_pins;

	if (!handle || !handle->priv || !value_bitmask)
		return -EINVAL;

	priv = handle->priv;

	in_pins = MXC_GPIO_InGet(priv->port, priv->pin_mask);
	out_pins = MXC_GPIO_OutGet(priv->port, priv->pin_mask);
	*value_bitmask = in_pins | out_pins;

	return 0;
}

/**
 * @brief Set value for all pins in the port
 * @param handle - the GPIO handle
 * @param value_bitmask - the value bitmask
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_port_set_value(struct capi_gpio_port_handle *handle,
				 uint64_t value_bitmask)
{
	return max_capi_gpio_port_set_raw_value(handle, value_bitmask);
}

/**
 * @brief Get value for all pins in the port
 * @param handle - the GPIO handle
 * @param value_bitmask - pointer to where the value bitmask will be stored
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_port_get_value(struct capi_gpio_port_handle *handle,
				 uint64_t *value_bitmask)
{
	return max_capi_gpio_port_get_raw_value(handle, value_bitmask);
}

/**
 * @brief Set direction for a single pin
 * @param pin - the pin handle
 * @param direction - the direction to set the pin to
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_pin_set_direction(struct capi_gpio_pin *pin,
				    uint8_t direction)
{
	struct capi_gpio_port_handle *handle;
	struct max_capi_gpio_port_priv *priv;
	mxc_gpio_cfg_t gpio_config;

	if (!pin || !pin->port_handle || !pin->port_handle->priv)
		return -EINVAL;

	priv = pin->port_handle->priv;

	if (pin->number >= priv->num_pins)
		return -EINVAL;

	handle = pin->port_handle;

	gpio_config = (mxc_gpio_cfg_t) {
		.port = priv->port,
		.mask = (1U << pin->number),
		.pad = (mxc_gpio_pad_t)priv->extra.pad,
		.vssel = (mxc_gpio_vssel_t)priv->extra.vssel,
		.drvstr = (mxc_gpio_drvstr_t)priv->extra.drvstr,
	};

	switch (direction) {
	case CAPI_GPIO_OUTPUT:
		gpio_config.func = MXC_GPIO_FUNC_OUT;
		MXC_GPIO_Config(&gpio_config);
		priv->direction_mask &= ~(1 << pin->number);
		break;
	case CAPI_GPIO_INPUT:
		gpio_config.func = MXC_GPIO_FUNC_IN;
		MXC_GPIO_Config(&gpio_config);
		priv->direction_mask |= (1 << pin->number);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Get direction for a single pin
 * @param pin - the pin handle
 * @param direction - pointer to where the direction will be stored
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_pin_get_direction(struct capi_gpio_pin *pin,
				    uint8_t *direction)
{
	const struct max_capi_gpio_port_priv *priv;

	if (!pin || !pin->port_handle || !pin->port_handle->priv || !direction)
		return -EINVAL;

	priv = pin->port_handle->priv;

	if (pin->number >= priv->num_pins)
		return -EINVAL;

	if (priv->direction_mask & ((uint32_t)(1 << pin->number)))
		*direction = CAPI_GPIO_INPUT;
	else
		*direction = CAPI_GPIO_OUTPUT;

	return 0;
}

/**
 * @brief Set the raw value of a single pin (ignoring the ACITVE_LOW flag)
 * @param pin - the pin handle
 * @param value - the value (CAPI_GPIO_LOW or CAPI_GPIO_HIGH)
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_pin_set_raw_value(struct capi_gpio_pin *pin, uint8_t value)
{
	struct max_capi_gpio_port_priv *priv;

	if (!pin || !pin->port_handle || !pin->port_handle->priv)
		return -EINVAL;

	priv = pin->port_handle->priv;

	if (pin->number >= priv->num_pins)
		return -EINVAL;

	set_enable(priv->port, (1 << pin->number), true);
	switch (value) {
	case CAPI_GPIO_LOW:
		MXC_GPIO_OutClr(priv->port, (1 << pin->number));
		break;
	case CAPI_GPIO_HIGH:
		MXC_GPIO_OutSet(priv->port, (1 << pin->number));
		break;
	/**
	 * In the maxim_gpio implementation, there is a HIGH_Z option that seems to
	 * be missing from CAPI. See: capi/capi_gpio.h.
	 */
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Get the raw value of a single pin (ignoring the ACTIVE_LOW flag)
 * @param pin - the pin handle
 * @param value - pointer to where value will be stored
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_pin_get_raw_value(struct capi_gpio_pin *pin, uint8_t *value)
{
	struct max_capi_gpio_port_priv *priv;

	if (!pin || !pin->port_handle || !pin->port_handle->priv || !value)
		return -EINVAL;

	priv = pin->port_handle->priv;

	if (pin->number >= priv->num_pins)
		return -EINVAL;

	set_enable(priv->port, (1 << pin->number), true);
	if (priv->extra.func == MAX_CAPI_GPIO_FUNC_IN)
		*value = MXC_GPIO_InGet(priv->port,
					(1U << pin->number)) >> pin->number;
	else
		*value = MXC_GPIO_OutGet(priv->port,
					 (1U << pin->number)) >> pin->number;

	return 0;
}

/**
 * @brief Apply logical value considering ACTIVE_LOW flag
 * @param raw_value - Raw hardware value
 * @param flags - Pin flags
 * @return Logical value
 */
static inline uint8_t apply_active_low(uint8_t raw_value, uint32_t flags)
{
	if (flags & CAPI_GPIO_ACTIVE_LOW)
		return raw_value ? CAPI_GPIO_LOW : CAPI_GPIO_HIGH;
	return raw_value;
}

/**
 * @brief Convert logical value to raw considering ACTIVE_LOW flag
 * @param logical_value - Logical value
 * @param flags - Pin flags
 * @return Raw hardware value
 */
static inline uint8_t to_raw_value(uint8_t logical_value, uint32_t flags)
{
	if (flags & CAPI_GPIO_ACTIVE_LOW)
		return logical_value ? CAPI_GPIO_LOW : CAPI_GPIO_HIGH;
	return logical_value;
}

/**
 * @brief Set the value of a single pin (considering the ACTIVE_LOW flag)
 * @param pin - the pin handle
 * @param value - the value (CAPI_GPIO_LOW or CAPI_GPIO_HIGH)
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_pin_set_value(struct capi_gpio_pin *pin, uint8_t value)
{
	uint8_t raw_value;

	if (!pin)
		return -EINVAL;

	raw_value = to_raw_value(value, pin->flags);

	return max_capi_gpio_pin_set_raw_value(pin, raw_value);
}

/**
 * @brief Get the value of a single pin (considering the ACTIVE_LOW flag)
 * @param pin - the pin handle
 * @param value - pointer where to store the value
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_pin_get_value(struct capi_gpio_pin *pin, uint8_t *value)
{
	uint8_t raw_value;
	int ret;

	if (!pin || !value)
		return -EINVAL;

	ret = max_capi_gpio_pin_get_raw_value(pin, &raw_value);
	if (ret)
		return ret;

	*value = apply_active_low(raw_value, pin->flags);

	return 0;
}

const struct capi_gpio_ops max_capi_gpio_ops = {
	.port_init = &max_capi_gpio_port_init,
	.port_deinit = &max_capi_gpio_port_deinit,
	.port_set_direction = &max_capi_gpio_port_set_direction,
	.port_get_direction = &max_capi_gpio_port_get_direction,
	.port_set_value = &max_capi_gpio_port_set_value,
	.port_get_value = &max_capi_gpio_port_get_value,
	.port_set_raw_value = &max_capi_gpio_port_set_raw_value,
	.port_get_raw_value = &max_capi_gpio_port_get_raw_value,
	.pin_set_direction = &max_capi_gpio_pin_set_direction,
	.pin_get_direction = &max_capi_gpio_pin_get_direction,
	.pin_set_value = &max_capi_gpio_pin_set_value,
	.pin_get_value = &max_capi_gpio_pin_get_value,
	.pin_set_raw_value = &max_capi_gpio_pin_set_raw_value,
	.pin_get_raw_value = &max_capi_gpio_pin_get_raw_value,
};
