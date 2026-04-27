/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <errno.h>
#include "capi_alloc.h"
#include "stm32_capi_gpio_priv.h"

/** Maximum number of pins per STM32 GPIO port */
#define STM32_GPIO_PINS_PER_PORT 16

/** Bit shift to access the reset bits in the BSRR register */
#define STM32_BSRR_RESET_SHIFT  16

/** Default direction mask - all pins configured as inputs */
#define STM32_GPIO_ALL_INPUTS    0xFFFF


static int stm32_capi_gpio_port_set_raw_value(struct capi_gpio_port_handle
		*handle,
		uint64_t value_bitmask);
static int stm32_capi_gpio_port_get_raw_value(struct capi_gpio_port_handle
		*handle,
		uint64_t *value_bitmask);
static int stm32_capi_gpio_pin_set_raw_value(struct capi_gpio_pin *pin,
		uint8_t value);
static int stm32_capi_gpio_pin_get_raw_value(struct capi_gpio_pin *pin,
		uint8_t *value);

/**
 * @brief Get STM32 GPIO port from identifier.
 * @param identifier - Port identifier (0=GPIOA, 1=GPIOB, etc. or base address).
 * @return Pointer to the GPIO port peripheral, or NULL if invalid.
 */
static GPIO_TypeDef *get_gpio_port_from_identifier(uint64_t identifier)
{
	/* If identifier is a valid GPIO peripheral base address, use it directly */
	if (IS_GPIO_ALL_INSTANCE((GPIO_TypeDef *)(uintptr_t)identifier))
		return (GPIO_TypeDef *)(uintptr_t)identifier;

	/* Otherwise, map port number to GPIO peripheral */
	switch (identifier) {
	case 0:
		return GPIOA;
#ifdef GPIOB
	case 1:
		return GPIOB;
#endif
#ifdef GPIOC
	case 2:
		return GPIOC;
#endif
#ifdef GPIOD
	case 3:
		return GPIOD;
#endif
#ifdef GPIOE
	case 4:
		return GPIOE;
#endif
#ifdef GPIOF
	case 5:
		return GPIOF;
#endif
#ifdef GPIOG
	case 6:
		return GPIOG;
#endif
#ifdef GPIOH
	case 7:
		return GPIOH;
#endif
#ifdef GPIOI
	case 8:
		return GPIOI;
#endif
#ifdef GPIOJ
	case 9:
		return GPIOJ;
#endif
#ifdef GPIOK
	case 10:
		return GPIOK;
#endif
	default:
		return NULL;
	}
}

/**
 * @brief Enable GPIO port clock in RCC.
 * @param identifier - Port identifier (number or base address).
 * @return 0 on success, negative error code otherwise.
 */
static int enable_gpio_port_clock(uint64_t identifier)
{
	/* Handle base address case */
	if (IS_GPIO_ALL_INSTANCE((GPIO_TypeDef *)(uintptr_t)identifier)) {
		GPIO_TypeDef *port = (GPIO_TypeDef *)(uintptr_t)identifier;

		if (port == GPIOA) {
			__HAL_RCC_GPIOA_CLK_ENABLE();
			return 0;
		}
#ifdef GPIOB
		if (port == GPIOB) {
			__HAL_RCC_GPIOB_CLK_ENABLE();
			return 0;
		}
#endif
#ifdef GPIOC
		if (port == GPIOC) {
			__HAL_RCC_GPIOC_CLK_ENABLE();
			return 0;
		}
#endif
#ifdef GPIOD
		if (port == GPIOD) {
			__HAL_RCC_GPIOD_CLK_ENABLE();
			return 0;
		}
#endif
#ifdef GPIOE
		if (port == GPIOE) {
			__HAL_RCC_GPIOE_CLK_ENABLE();
			return 0;
		}
#endif
#ifdef GPIOF
		if (port == GPIOF) {
			__HAL_RCC_GPIOF_CLK_ENABLE();
			return 0;
		}
#endif
#ifdef GPIOG
		if (port == GPIOG) {
			__HAL_RCC_GPIOG_CLK_ENABLE();
			return 0;
		}
#endif
#ifdef GPIOH
		if (port == GPIOH) {
			__HAL_RCC_GPIOH_CLK_ENABLE();
			return 0;
		}
#endif
#ifdef GPIOI
		if (port == GPIOI) {
			__HAL_RCC_GPIOI_CLK_ENABLE();
			return 0;
		}
#endif
#ifdef GPIOJ
		if (port == GPIOJ) {
			__HAL_RCC_GPIOJ_CLK_ENABLE();
			return 0;
		}
#endif
#ifdef GPIOK
		if (port == GPIOK) {
			__HAL_RCC_GPIOK_CLK_ENABLE();
			return 0;
		}
#endif
		return -EINVAL;
	}

	/* Handle port number case */
	switch (identifier) {
	case 0:
		__HAL_RCC_GPIOA_CLK_ENABLE();
		break;
#ifdef GPIOB
	case 1:
		__HAL_RCC_GPIOB_CLK_ENABLE();
		break;
#endif
#ifdef GPIOC
	case 2:
		__HAL_RCC_GPIOC_CLK_ENABLE();
		break;
#endif
#ifdef GPIOD
	case 3:
		__HAL_RCC_GPIOD_CLK_ENABLE();
		break;
#endif
#ifdef GPIOE
	case 4:
		__HAL_RCC_GPIOE_CLK_ENABLE();
		break;
#endif
#ifdef GPIOF
	case 5:
		__HAL_RCC_GPIOF_CLK_ENABLE();
		break;
#endif
#ifdef GPIOG
	case 6:
		__HAL_RCC_GPIOG_CLK_ENABLE();
		break;
#endif
#ifdef GPIOH
	case 7:
		__HAL_RCC_GPIOH_CLK_ENABLE();
		break;
#endif
#ifdef GPIOI
	case 8:
		__HAL_RCC_GPIOI_CLK_ENABLE();
		break;
#endif
#ifdef GPIOJ
	case 9:
		__HAL_RCC_GPIOJ_CLK_ENABLE();
		break;
#endif
#ifdef GPIOK
	case 10:
		__HAL_RCC_GPIOK_CLK_ENABLE();
		break;
#endif
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Apply logical value considering ACTIVE_LOW flag.
 * @param raw_value - Raw hardware value.
 * @param flags - Pin flags.
 * @return Logical value after applying ACTIVE_LOW inversion.
 */
static inline uint8_t apply_active_low(uint8_t raw_value, uint32_t flags)
{
	if (flags & CAPI_GPIO_ACTIVE_LOW)
		return raw_value ? CAPI_GPIO_LOW : CAPI_GPIO_HIGH;
	return raw_value;
}

/**
 * @brief Convert logical value to raw considering ACTIVE_LOW flag.
 * @param logical_value - Logical value.
 * @param flags - Pin flags.
 * @return Raw hardware value after applying ACTIVE_LOW inversion.
 */
static inline uint8_t to_raw_value(uint8_t logical_value, uint32_t flags)
{
	if (flags & CAPI_GPIO_ACTIVE_LOW)
		return logical_value ? CAPI_GPIO_LOW : CAPI_GPIO_HIGH;
	return logical_value;
}

/**
 * @brief Initialize a GPIO port.
 * @param handle - Pointer to port handle pointer. If *handle is NULL, memory
 *                 is allocated by the driver.
 * @param config - Pointer to the GPIO port configuration.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_port_init(struct capi_gpio_port_handle **handle,
				     const struct capi_gpio_port_config *config)
{
	struct capi_gpio_port_handle *port_handle;
	struct stm32_capi_gpio_port_priv *priv;
	struct stm32_capi_gpio_port_config *extra_config;
	GPIO_TypeDef *port;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	/* Get GPIO port from identifier */
	port = get_gpio_port_from_identifier(config->identifier);
	if (!port)
		return -EINVAL;

	/* Enable port clock */
	ret = enable_gpio_port_clock(config->identifier);
	if (ret)
		return ret;

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

	/* Initialize private data */
	priv->port = port;
	priv->num_pins = config->num_pins ? config->num_pins : STM32_GPIO_PINS_PER_PORT;
	priv->direction_mask = STM32_GPIO_ALL_INPUTS;

	/* Apply extra configuration if provided */
	if (config->extra) {
		extra_config = (struct stm32_capi_gpio_port_config *)config->extra;
		priv->default_config.mode = extra_config->mode;
		priv->default_config.speed = extra_config->speed;
		priv->default_config.alternate = extra_config->alternate;
		priv->default_config.pull = extra_config->pull;
	} else {
		/* Default configuration */
		priv->default_config.mode = GPIO_MODE_INPUT;
		priv->default_config.speed = GPIO_SPEED_FREQ_LOW;
		priv->default_config.alternate = 0;
		priv->default_config.pull = GPIO_NOPULL;
	}

	*handle = port_handle;
	return 0;
}

/**
 * @brief Deinitialize a GPIO port.
 * @param handle - Pointer to port handle pointer. Set to NULL on return.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_port_deinit(struct capi_gpio_port_handle **handle)
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
static int stm32_capi_gpio_port_set_direction(struct capi_gpio_port_handle
		*handle,
		uint64_t direction_bitmask)
{
	struct stm32_capi_gpio_port_priv *priv;
	GPIO_InitTypeDef gpio_config;
	uint16_t i;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	/* Configure each pin according to direction bitmask */
	for (i = 0; i < priv->num_pins; i++) {
		gpio_config.Pin = (1 << (i));
		gpio_config.Pull = priv->default_config.pull;
		gpio_config.Speed = priv->default_config.speed;

		if (direction_bitmask & (1 << (i))) {
			/* Input */
			gpio_config.Mode = GPIO_MODE_INPUT;
		} else {
			/* Output */
			gpio_config.Mode = GPIO_MODE_OUTPUT_PP;
		}

		HAL_GPIO_Init(priv->port, &gpio_config);
	}

	priv->direction_mask = (uint16_t)direction_bitmask;
	return 0;
}

/**
 * @brief Get direction of all pins in the port.
 * @param handle - Pointer to the GPIO port handle.
 * @param direction_bitmask - Pointer to store direction bitmask.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_port_get_direction(struct capi_gpio_port_handle
		*handle,
		uint64_t *direction_bitmask)
{
	struct stm32_capi_gpio_port_priv *priv;

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
static int stm32_capi_gpio_port_set_value(struct capi_gpio_port_handle *handle,
		uint64_t value_bitmask)
{
	/* For port-level operations without per-pin flags, use raw value */
	return stm32_capi_gpio_port_set_raw_value(handle, value_bitmask);
}

/**
 * @brief Get value of all pins in the port (considering ACTIVE_LOW flags).
 * @param handle - Pointer to the GPIO port handle.
 * @param value_bitmask - Pointer to store value bitmask.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_port_get_value(struct capi_gpio_port_handle *handle,
		uint64_t *value_bitmask)
{
	/* For port-level operations without per-pin flags, use raw value */
	return stm32_capi_gpio_port_get_raw_value(handle, value_bitmask);
}

/**
 * @brief Set raw value for all pins (ignoring ACTIVE_LOW flag).
 * @param handle - Pointer to the GPIO port handle.
 * @param value_bitmask - Raw value bitmask to set.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_port_set_raw_value(struct capi_gpio_port_handle
		*handle,
		uint64_t value_bitmask)
{
	struct stm32_capi_gpio_port_priv *priv;
	uint16_t set_pins;
	uint16_t reset_pins;

	if (!handle || !handle->priv)
		return -EINVAL;

	priv = handle->priv;

	/* Calculate which pins to set and reset */
	set_pins = (uint16_t)value_bitmask;
	reset_pins = (uint16_t)(~value_bitmask);

	/* Use BSRR register for atomic set/reset */
	priv->port->BSRR = ((uint32_t)reset_pins << STM32_BSRR_RESET_SHIFT) | set_pins;

	return 0;
}

/**
 * @brief Get raw value of all pins (ignoring ACTIVE_LOW flag).
 * @param handle - Pointer to the GPIO port handle.
 * @param value_bitmask - Pointer to store raw value bitmask.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_port_get_raw_value(struct capi_gpio_port_handle
		*handle,
		uint64_t *value_bitmask)
{
	struct stm32_capi_gpio_port_priv *priv;

	if (!handle || !handle->priv || !value_bitmask)
		return -EINVAL;

	priv = handle->priv;

	/* Read input data register */
	*value_bitmask = priv->port->IDR & GPIO_PIN_All;

	return 0;
}

/**
 * @brief Set direction for a single pin.
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param direction - Direction (CAPI_GPIO_INPUT or CAPI_GPIO_OUTPUT).
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_pin_set_direction(struct capi_gpio_pin *pin,
		uint8_t direction)
{
	struct stm32_capi_gpio_port_priv *priv;
	GPIO_InitTypeDef gpio_config;
	uint8_t current_value;

	if (!pin || !pin->port_handle || !pin->port_handle->priv)
		return -EINVAL;

	if (pin->number >= STM32_GPIO_PINS_PER_PORT)
		return -EINVAL;

	priv = pin->port_handle->priv;

	gpio_config.Pin = (1 << (pin->number));
	gpio_config.Pull = priv->default_config.pull;
	gpio_config.Speed = priv->default_config.speed;

	if (direction == CAPI_GPIO_INPUT) {
		gpio_config.Mode = GPIO_MODE_INPUT;
		priv->direction_mask |= (1 << (pin->number));
	} else {
		/* Read current pin state and set output value before configuring
		 * direction to avoid glitches on the pin */
		current_value = (priv->port->IDR & (1 << (pin->number))) ? 1 : 0;
		HAL_GPIO_WritePin(priv->port, (1 << (pin->number)),
				  (GPIO_PinState)current_value);

		/* Use open-drain or push-pull based on pin flags */
		if (pin->flags & STM32_GPIO_OPEN_DRAIN)
			gpio_config.Mode = GPIO_MODE_OUTPUT_OD;
		else
			gpio_config.Mode = GPIO_MODE_OUTPUT_PP;
		priv->direction_mask &= ~(1 << (pin->number));
	}

	HAL_GPIO_Init(priv->port, &gpio_config);

	return 0;
}

/**
 * @brief Get direction of a single pin.
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param direction - Pointer to store the direction.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_pin_get_direction(struct capi_gpio_pin *pin,
		uint8_t *direction)
{
	struct stm32_capi_gpio_port_priv *priv;

	if (!pin || !pin->port_handle || !pin->port_handle->priv || !direction)
		return -EINVAL;

	if (pin->number >= STM32_GPIO_PINS_PER_PORT)
		return -EINVAL;

	priv = pin->port_handle->priv;

	if (priv->direction_mask & (1 << (pin->number)))
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
static int stm32_capi_gpio_pin_set_value(struct capi_gpio_pin *pin,
		uint8_t value)
{
	uint8_t raw_value;

	if (!pin)
		return -EINVAL;

	/* Apply ACTIVE_LOW flag */
	raw_value = to_raw_value(value, pin->flags);

	return stm32_capi_gpio_pin_set_raw_value(pin, raw_value);
}

/**
 * @brief Get value of a single pin (considering ACTIVE_LOW flag).
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param value - Pointer to store the logical value.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_pin_get_value(struct capi_gpio_pin *pin,
		uint8_t *value)
{
	uint8_t raw_value;
	int ret;

	if (!pin || !value)
		return -EINVAL;

	ret = stm32_capi_gpio_pin_get_raw_value(pin, &raw_value);
	if (ret)
		return ret;

	/* Apply ACTIVE_LOW flag */
	*value = apply_active_low(raw_value, pin->flags);

	return 0;
}

/**
 * @brief Set raw value for a single pin (ignoring ACTIVE_LOW flag).
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param value - Raw value to set.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_pin_set_raw_value(struct capi_gpio_pin *pin,
		uint8_t value)
{
	struct stm32_capi_gpio_port_priv *priv;

	if (!pin || !pin->port_handle || !pin->port_handle->priv)
		return -EINVAL;

	if (pin->number >= STM32_GPIO_PINS_PER_PORT)
		return -EINVAL;

	priv = pin->port_handle->priv;

	/* Use BSRR for atomic set/reset */
	if (value)
		priv->port->BSRR = (1 << (pin->number));
	else
		priv->port->BSRR = (1 << (pin->number)) << STM32_BSRR_RESET_SHIFT;

	return 0;
}

/**
 * @brief Get raw value of a single pin (ignoring ACTIVE_LOW flag).
 * @param pin - Pointer to the GPIO pin descriptor.
 * @param value - Pointer to store the raw value.
 * @return 0 on success, negative error code otherwise.
 */
static int stm32_capi_gpio_pin_get_raw_value(struct capi_gpio_pin *pin,
		uint8_t *value)
{
	struct stm32_capi_gpio_port_priv *priv;

	if (!pin || !pin->port_handle || !pin->port_handle->priv || !value)
		return -EINVAL;

	if (pin->number >= STM32_GPIO_PINS_PER_PORT)
		return -EINVAL;

	priv = pin->port_handle->priv;

	/* Read from input data register */
	*value = (priv->port->IDR & (1 << (pin->number))) ? CAPI_GPIO_HIGH :
		 CAPI_GPIO_LOW;

	return 0;
}

/**
 * @brief STM32 platform specific GPIO operations for CAPI
 */
const struct capi_gpio_ops stm32_capi_gpio_ops = {
	.port_init = stm32_capi_gpio_port_init,
	.port_deinit = stm32_capi_gpio_port_deinit,
	.port_set_direction = stm32_capi_gpio_port_set_direction,
	.port_get_direction = stm32_capi_gpio_port_get_direction,
	.port_set_value = stm32_capi_gpio_port_set_value,
	.port_get_value = stm32_capi_gpio_port_get_value,
	.port_set_raw_value = stm32_capi_gpio_port_set_raw_value,
	.port_get_raw_value = stm32_capi_gpio_port_get_raw_value,
	.pin_set_direction = stm32_capi_gpio_pin_set_direction,
	.pin_get_direction = stm32_capi_gpio_pin_get_direction,
	.pin_set_value = stm32_capi_gpio_pin_set_value,
	.pin_get_value = stm32_capi_gpio_pin_get_value,
	.pin_set_raw_value = stm32_capi_gpio_pin_set_raw_value,
	.pin_get_raw_value = stm32_capi_gpio_pin_get_raw_value,
};
