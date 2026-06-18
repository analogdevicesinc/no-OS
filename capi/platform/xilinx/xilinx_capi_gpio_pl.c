/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform GPIO driver implementation
 *
 * Supports XGpio (PL GPIO).
 */
#include "capi_gpio.h"
#include <xilinx_capi_gpio_priv.h>
#include <capi_alloc.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

/* AXI GPIO exposes one 32-bit data/tristate register per channel. */
#define CAPI_GPIO_PL_MAX_PINS	32U

/* Function prototypes */
static int capi_gpio_xilinx_port_init(struct capi_gpio_port_handle **handle,
				      const struct capi_gpio_port_config *config);
static int capi_gpio_xilinx_port_deinit(struct capi_gpio_port_handle **handle);
static int capi_gpio_xilinx_port_set_direction(struct capi_gpio_port_handle
		*handle,
		uint64_t direction_bitmask);
static int capi_gpio_xilinx_port_get_direction(struct capi_gpio_port_handle
		*handle,
		uint64_t *direction_bitmask);
static int capi_gpio_xilinx_port_set_value(struct capi_gpio_port_handle *handle,
		uint64_t value_bitmask);
static int capi_gpio_xilinx_port_get_value(struct capi_gpio_port_handle *handle,
		uint64_t *value_bitmask);
static int capi_gpio_xilinx_port_set_raw_value(struct capi_gpio_port_handle
		*handle,
		uint64_t value_bitmask);
static int capi_gpio_xilinx_port_get_raw_value(struct capi_gpio_port_handle
		*handle,
		uint64_t *value_bitmask);
static int capi_gpio_xilinx_pin_set_direction(struct capi_gpio_pin *pin,
		uint8_t direction);
static int capi_gpio_xilinx_pin_get_direction(struct capi_gpio_pin *pin,
		uint8_t *direction);
static int capi_gpio_xilinx_pin_set_value(struct capi_gpio_pin *pin,
		uint8_t value);
static int capi_gpio_xilinx_pin_get_value(struct capi_gpio_pin *pin,
		uint8_t *value);
static int capi_gpio_xilinx_pin_set_raw_value(struct capi_gpio_pin *pin,
		uint8_t value);
static int capi_gpio_xilinx_pin_get_raw_value(struct capi_gpio_pin *pin,
		uint8_t *value);

/* GPIO operations structure. */
struct capi_gpio_ops capi_gpio_xilinx_pl_ops = {
	.port_init = capi_gpio_xilinx_port_init,
	.port_deinit = capi_gpio_xilinx_port_deinit,
	.port_set_direction = capi_gpio_xilinx_port_set_direction,
	.port_get_direction = capi_gpio_xilinx_port_get_direction,
	.port_set_value = capi_gpio_xilinx_port_set_value,
	.port_get_value = capi_gpio_xilinx_port_get_value,
	.port_set_raw_value = capi_gpio_xilinx_port_set_raw_value,
	.port_get_raw_value = capi_gpio_xilinx_port_get_raw_value,
	.pin_set_direction = capi_gpio_xilinx_pin_set_direction,
	.pin_get_direction = capi_gpio_xilinx_pin_get_direction,
	.pin_set_value = capi_gpio_xilinx_pin_set_value,
	.pin_get_value = capi_gpio_xilinx_pin_get_value,
	.pin_set_raw_value = capi_gpio_xilinx_pin_set_raw_value,
	.pin_get_raw_value = capi_gpio_xilinx_pin_get_raw_value
};

static uint64_t logical_to_raw(uint64_t logical_value, uint64_t active_low_mask)
{
	return logical_value ^ active_low_mask;
}

static uint64_t raw_to_logical(uint64_t raw_value, uint64_t active_low_mask)
{
	return raw_value ^ active_low_mask;
}

static uint32_t capi_gpio_get_max_pins(void)
{
	return CAPI_GPIO_PL_MAX_PINS;
}

static void xilinx_gpio_free_allocated_handle(
	struct capi_gpio_port_handle **handle)
{
	if (handle == NULL || *handle == NULL)
		return;

	capi_free((*handle)->priv);
	capi_free(*handle);
	*handle = NULL;
}

static void xilinx_gpio_clear_app_handle(struct capi_gpio_port_handle *handle)
{
	if (handle != NULL)
		handle->ops = NULL;
}

/* Dispatcher already checks pin != NULL and port_handle != NULL before calling us */
static int gpio_validate_pin(const struct capi_gpio_xilinx_port_handle *xhandle,
			     const struct capi_gpio_pin *pin)
{
	if (pin->number >= xhandle->num_pins)
		return -EINVAL;

	return 0;
}

static uint32_t gpio_pin_flags(const struct capi_gpio_xilinx_port_handle
			       *xhandle,
			       const struct capi_gpio_pin *pin)
{
	uint32_t flags = pin->flags;

	if (xhandle->pin_flags != NULL)
		flags |= xhandle->pin_flags[pin->number];

	return flags;
}

/**
 * @brief Initialize a GPIO port.
 * @note PL: XGpio_Initialize().
 *
 * @param[out] handle GPIO port handle pointer.
 * @param[in] config GPIO port configuration.
 * @return 0 on success, -EINVAL, -ENODEV, -ENOMEM, or -EIO.
 */
static int capi_gpio_xilinx_port_init(struct capi_gpio_port_handle **handle,
				      const struct capi_gpio_port_config *config)
{
	if (handle == NULL || config == NULL) {
		return -EINVAL;
	}
	struct capi_gpio_xilinx_pl_config *xcfg =
		(struct capi_gpio_xilinx_pl_config *)config->extra;
	if (xcfg == NULL)
		return -EINVAL;

	if (xcfg->channel == 0 || xcfg->channel > 2)
		return -EINVAL;

	if (config->num_pins == 0 ||
	    config->num_pins > capi_gpio_get_max_pins())
		return -EINVAL;

	bool alloc_handle = (*handle == NULL);
	struct capi_gpio_port_handle *h = *handle;
	struct capi_gpio_xilinx_port_handle *xhandle;

	if (alloc_handle) {
		h = capi_malloc(sizeof(*h));
		if (h == NULL)
			return -ENOMEM;
		memset(h, 0, sizeof(*h));

		xhandle = capi_malloc(sizeof(*xhandle));
		if (xhandle == NULL) {
			capi_free(h);
			return -ENOMEM;
		}
		h->priv = xhandle;
		*handle = h;
	} else {
		xhandle = h->priv;
		if (xhandle == NULL)
			return -EINVAL;
	}

	memset(xhandle, 0, sizeof(*xhandle));
	h->init_allocated = alloc_handle;
	h->ops = config->ops;
	h->priv = xhandle;
	xhandle->channel = xcfg->channel;
	xhandle->num_pins = config->num_pins;

	/* Allocate and copy pin flags */
	xhandle->pin_flags = capi_malloc(config->num_pins * sizeof(uint32_t));
	if (xhandle->pin_flags == NULL) {
		if (alloc_handle)
			xilinx_gpio_free_allocated_handle(handle);
		else
			xilinx_gpio_clear_app_handle(h);
		return -ENOMEM;
	}
	if (config->flags != NULL) {
		memcpy(xhandle->pin_flags, config->flags, config->num_pins * sizeof(uint32_t));
	} else {
		memset(xhandle->pin_flags, 0, config->num_pins * sizeof(uint32_t));
	}

	/* Compute active-low mask once at init (eliminates iteration per value call) */
	xhandle->active_low_mask = 0;
	for (int i = 0; i < config->num_pins; i++) {
		if (xhandle->pin_flags[i] & CAPI_GPIO_ACTIVE_LOW) {
			xhandle->active_low_mask |= (1ULL << i);
		}
	}

	XGpio *pl_inst = capi_malloc(sizeof(XGpio));
	if (pl_inst == NULL) {
		capi_free(xhandle->pin_flags);
		xhandle->pin_flags = NULL;
		if (alloc_handle)
			xilinx_gpio_free_allocated_handle(handle);
		else
			xilinx_gpio_clear_app_handle(h);
		return -ENOMEM;
	}
	memset(pl_inst, 0, sizeof(XGpio));
	int status = XGpio_Initialize(pl_inst, (UINTPTR)config->identifier);
	if (status != XST_SUCCESS) {
		capi_free(pl_inst);
		capi_free(xhandle->pin_flags);
		xhandle->pin_flags = NULL;
		if (alloc_handle)
			xilinx_gpio_free_allocated_handle(handle);
		else
			xilinx_gpio_clear_app_handle(h);
		return -EIO;
	}
	xhandle->instance = pl_inst;

	return 0;
}

/**
 * @brief Deinitialize a GPIO port.
 * @note PL: Frees XGpio instance.
 *
 * @param[in,out] handle GPIO port handle pointer, set to NULL on success.
 * @return 0 on success, -EINVAL if handle is NULL.
 */
static int capi_gpio_xilinx_port_deinit(struct capi_gpio_port_handle **handle)
{
	if (handle == NULL || *handle == NULL) {
		return -EINVAL;
	}
	struct capi_gpio_port_handle *h = *handle;
	struct capi_gpio_xilinx_port_handle *xhandle = h->priv;
	if (xhandle == NULL)
		return -EINVAL;

	/* Free the Xilinx driver instance */
	if (xhandle->instance != NULL) {
		capi_free(xhandle->instance);
		xhandle->instance = NULL;
	}
	/* Free dynamically allocated pin flags */
	if (xhandle->pin_flags != NULL) {
		capi_free(xhandle->pin_flags);
		xhandle->pin_flags = NULL;
	}
	if (h->init_allocated) {
		capi_free(xhandle);
		capi_free(h);
		*handle = NULL;
	} else {
		h->ops = NULL;
	}
	return 0;
}

/**
 * @brief Set GPIO port direction.
 * @note PL: XGpio_SetDataDirection().
 *
 * XGpio_SetDataDirection uses inverted polarity (1=output, 0=input) vs CAPI (1=input, 0=output).
 *
 * @param[in] handle GPIO port handle.
 * @param[in] direction_bitmask 1=input, 0=output per pin.
 * @return 0 on success, -EINVAL.
 */
int capi_gpio_xilinx_port_set_direction(struct capi_gpio_port_handle *handle,
					uint64_t direction_bitmask)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	uint64_t mask = (1ULL << xhandle->num_pins) - 1;
	xhandle->direction = direction_bitmask & mask;

	XGpio *inst = (XGpio *)xhandle->instance;
	XGpio_SetDataDirection(inst, xhandle->channel, (uint32_t)direction_bitmask);
	return 0;
}

/**
 * @brief Get GPIO port direction (cached).
 * @note PL: Returns cached direction.
 *
 * @param[in] handle GPIO port handle.
 * @param[out] direction_bitmask 1=input, 0=output per pin.
 * @return 0 on success, -EINVAL if NULL.
 */
static int capi_gpio_xilinx_port_get_direction(struct capi_gpio_port_handle
		*handle,
		uint64_t *direction_bitmask)
{
	if (handle == NULL || direction_bitmask == NULL) {
		return -EINVAL;
	}
	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	/* Return cached direction - hardware may not support readback */
	*direction_bitmask = xhandle->direction;
	return 0;
}
/**
 * @brief Set GPIO port raw output value (no ACTIVE_LOW inversion).
 *
 * @note PL: XGpio_DiscreteWrite().
 *
 * @param[in] handle GPIO port handle.
 * @param[in] value_bitmask Raw value per pin.
 * @return 0 on success, -EINVAL, or -ENOTSUP.
 */
static int capi_gpio_xilinx_port_set_raw_value(struct capi_gpio_port_handle
		*handle,
		uint64_t value_bitmask)
{
	if (handle == NULL) {
		return -EINVAL;
	}
	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	XGpio *inst = (XGpio *)xhandle->instance;
	XGpio_DiscreteWrite(inst, xhandle->channel, (uint32_t)value_bitmask);
	return 0;
}

/**
 * @brief Set GPIO port output value (applies ACTIVE_LOW inversion).
 * @note PL: Active-low map then XGpio_DiscreteWrite().
 *
 * @param[in] handle GPIO port handle.
 * @param[in] value_bitmask Logical value per pin.
 * @return 0 on success, -EINVAL, or -ENOTSUP.
 */
static int capi_gpio_xilinx_port_set_value(struct capi_gpio_port_handle *handle,
		uint64_t value_bitmask)
{
	if (handle == NULL) {
		return -EINVAL;
	}
	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	/* Use cached active-low mask (computed once at init) */
	uint64_t raw_value = logical_to_raw(value_bitmask, xhandle->active_low_mask);

	return capi_gpio_xilinx_port_set_raw_value(handle, raw_value);
}

/**
 * @brief Get GPIO port raw value (no ACTIVE_LOW inversion).
 * @note PL: XGpio_DiscreteRead().
 *
 * Uses XGpio_DiscreteRead() and masks to num_pins.
 *
 * @param[in] handle GPIO port handle.
 * @param[out] value_bitmask Raw value per pin.
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_port_get_raw_value(struct capi_gpio_port_handle
		*handle,
		uint64_t *value_bitmask)
{
	if (handle == NULL || value_bitmask == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	XGpio *inst = (XGpio *)xhandle->instance;
	uint32_t raw = XGpio_DiscreteRead(inst, xhandle->channel);

	*value_bitmask = (uint64_t)raw & ((1ULL << xhandle->num_pins) - 1);
	return 0;
}

/**
 * @brief Get GPIO port value (applies ACTIVE_LOW inversion).
 * @note PL: XGpio_DiscreteRead() plus active-low map.
 *
 * @param[in] handle GPIO port handle.
 * @param[out] value_bitmask Logical value per pin.
 * @return 0 on success, -EINVAL, or -ENOTSUP.
 */
static int capi_gpio_xilinx_port_get_value(struct capi_gpio_port_handle *handle,
		uint64_t *value_bitmask)
{
	if (handle == NULL || value_bitmask == NULL) {
		return -EINVAL;
	}
	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	uint64_t raw_value;
	int ret = capi_gpio_xilinx_port_get_raw_value(handle, &raw_value);
	if (ret != 0) {
		return ret;
	}

	/* Use cached active-low mask (computed once at init) */
	*value_bitmask = raw_to_logical(raw_value, xhandle->active_low_mask);
	return 0;
}

/**
 * @brief Set GPIO pin direction.
 * @note PL: Cache then XGpio_SetDataDirection().
 *
 * Updates cached direction then writes all pins via XGpio_SetDataDirection().
 * XGpio polarity is inverted (1=output, 0=input) so cached value is inverted on write.
 *
 * @param[in] pin GPIO pin descriptor.
 * @param[in] direction CAPI_GPIO_INPUT or CAPI_GPIO_OUTPUT.
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_pin_set_direction(struct capi_gpio_pin *pin,
		uint8_t direction)
{
	struct capi_gpio_xilinx_port_handle *xhandle =
			pin->port_handle->priv;

	if (gpio_validate_pin(xhandle, pin) != 0)
		return -EINVAL;

	if (direction == CAPI_GPIO_INPUT)
		xhandle->direction |= (1ULL << pin->number);
	else
		xhandle->direction &= ~(1ULL << pin->number);

	XGpio *inst = (XGpio *)xhandle->instance;
	XGpio_SetDataDirection(inst, xhandle->channel, (uint32_t)xhandle->direction);
	return 0;
}

/**
 * @brief Get GPIO pin direction (cached).
 * @note PL: Returns cached pin direction.
 *
 * @param[in] pin GPIO pin descriptor.
 * @param[out] direction CAPI_GPIO_INPUT or CAPI_GPIO_OUTPUT.
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_pin_get_direction(struct capi_gpio_pin *pin,
		uint8_t *direction)
{
	if (direction == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_port_handle *xhandle =
			pin->port_handle->priv;

	if (gpio_validate_pin(xhandle, pin) != 0)
		return -EINVAL;

	*direction = (xhandle->direction >> pin->number) & 1;
	return 0;
}

/**
 * @brief Set GPIO pin value (applies ACTIVE_LOW inversion).
 * @note PL: XGpio_DiscreteSet()/XGpio_DiscreteClear().
 *
 * Uses XGpio_DiscreteSet() / XGpio_DiscreteClear() for atomic single-pin writes.
 *
 * @param[in] pin GPIO pin descriptor.
 * @param[in] value Logical value (0 or 1).
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_pin_set_value(struct capi_gpio_pin *pin,
		uint8_t value)
{
	struct capi_gpio_xilinx_port_handle *xhandle =
			pin->port_handle->priv;
	if (gpio_validate_pin(xhandle, pin) != 0)
		return -EINVAL;

	uint8_t raw_value = value;
	if (gpio_pin_flags(xhandle, pin) & CAPI_GPIO_ACTIVE_LOW) {
		raw_value = !value;
	}
	XGpio *inst = (XGpio *)xhandle->instance;
	uint32_t mask = (1U << pin->number);

	/* Use atomic set/clear - faster and safer than read-modify-write */
	if (raw_value) {
		XGpio_DiscreteSet(inst, xhandle->channel, mask);
	} else {
		XGpio_DiscreteClear(inst, xhandle->channel, mask);
	}
	return 0;
}

/**
 * @brief Get GPIO pin value (applies ACTIVE_LOW inversion).
 * @note PL: XGpio_DiscreteRead() plus active-low map.
 *
 * Reads via XGpio_DiscreteRead() and applies active-low inversion.
 *
 * @param[in] pin GPIO pin descriptor.
 * @param[out] value Logical value (0 or 1).
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_pin_get_value(struct capi_gpio_pin *pin,
		uint8_t *value)
{
	if (value == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_port_handle *xhandle =
			pin->port_handle->priv;
	if (gpio_validate_pin(xhandle, pin) != 0)
		return -EINVAL;

	uint8_t raw_value = 0;

	XGpio *inst = (XGpio *)xhandle->instance;
	uint32_t reg = XGpio_DiscreteRead(inst, xhandle->channel);
	raw_value = (reg >> pin->number) & 1;

	/* Apply ACTIVE_LOW inversion */
	if (gpio_pin_flags(xhandle, pin) & CAPI_GPIO_ACTIVE_LOW) {
		*value = !raw_value;
	} else {
		*value = raw_value;
	}
	return 0;
}

/**
 * @brief Set GPIO pin raw output value (no ACTIVE_LOW inversion).
 * @note PL: XGpio_DiscreteSet()/XGpio_DiscreteClear().
 *
 * Uses XGpio_DiscreteSet() / XGpio_DiscreteClear() for atomic single-pin writes.
 *
 * @param[in] pin GPIO pin descriptor.
 * @param[in] value Raw value (0 or 1).
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_pin_set_raw_value(struct capi_gpio_pin *pin,
		uint8_t value)
{
	struct capi_gpio_xilinx_port_handle *xhandle =
			pin->port_handle->priv;
	if (gpio_validate_pin(xhandle, pin) != 0)
		return -EINVAL;

	XGpio *inst = (XGpio *)xhandle->instance;
	uint32_t mask = (1U << pin->number);

	/* Use atomic set/clear - faster and safer than read-modify-write */
	if (value) {
		XGpio_DiscreteSet(inst, xhandle->channel, mask);
	} else {
		XGpio_DiscreteClear(inst, xhandle->channel, mask);
	}
	return 0;
}

/**
 * @brief Get GPIO pin raw value (no ACTIVE_LOW inversion).
 * @note PL: XGpio_DiscreteRead().
 *
 * Reads via XGpio_DiscreteRead(), extracts single bit at pin->number.
 *
 * @param[in] pin GPIO pin descriptor.
 * @param[out] value Raw value (0 or 1).
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_pin_get_raw_value(struct capi_gpio_pin *pin,
		uint8_t *value)
{
	if (value == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_port_handle *xhandle =
			pin->port_handle->priv;
	if (gpio_validate_pin(xhandle, pin) != 0)
		return -EINVAL;

	XGpio *inst = (XGpio *)xhandle->instance;
	uint32_t reg = XGpio_DiscreteRead(inst, xhandle->channel);
	*value = (reg >> pin->number) & 1;
	return 0;
}
