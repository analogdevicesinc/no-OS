/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx PS GPIO driver (XGpioPs)
 *
 * Supports Zynq-7000, ZynqMP, Versal PS GPIO.
 * Pin numbering uses global XGpioPs pin IDs. EMIO starts at 54 on Zynq-7000
 * and at 78 on ZynqMP.
 * Port operations use base_pin + bit index to map bitmask to physical pins.
 * Bank boundaries are validated at init via XGpioPs_GetBankPin().
 */
#include "capi_gpio.h"
#include <xilinx_capi_gpio_priv.h>
#include <capi_alloc.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

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

struct capi_gpio_ops capi_gpio_xilinx_ps_ops = {
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
	.pin_get_raw_value = capi_gpio_xilinx_pin_get_raw_value,
};

static uint64_t logical_to_raw(uint64_t logical_value, uint64_t active_low_mask)
{
	return logical_value ^ active_low_mask;
}

static uint64_t raw_to_logical(uint64_t raw_value, uint64_t active_low_mask)
{
	return raw_value ^ active_low_mask;
}

static uint32_t capi_gpio_get_max_pins(const XGpioPs *instance)
{
	return instance->MaxPinNum;
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
 * @brief Initialize a PS GPIO port.
 * @note PS: XGpioPs_LookupConfig()/XGpioPs_CfgInitialize().
 *
 * Uses XGpioPs_LookupConfig() and XGpioPs_CfgInitialize().
 * Validates that base_pin + num_pins stays within one bank boundary.
 *
 * @param[out] handle GPIO port handle pointer.
 * @param[in] config GPIO port configuration.
 * @return 0 on success, -EINVAL, -ENODEV, -ENOMEM, or -EIO.
 */
static int capi_gpio_xilinx_port_init(struct capi_gpio_port_handle **handle,
				      const struct capi_gpio_port_config *config)
{
	if (handle == NULL || config == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_ps_config *xcfg =
		(struct capi_gpio_xilinx_ps_config *)config->extra;

	uint8_t base_pin = xcfg ? xcfg->base_pin : 0;

	/* active_low_mask is uint64_t, so num_pins is bounded to 64. */
	if (config->num_pins == 0 || config->num_pins > 64)
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
	xhandle->base_pin = base_pin;
	xhandle->num_pins = config->num_pins;

	xhandle->pin_flags = capi_malloc(config->num_pins * sizeof(uint32_t));
	if (xhandle->pin_flags == NULL) {
		if (alloc_handle)
			xilinx_gpio_free_allocated_handle(handle);
		else
			xilinx_gpio_clear_app_handle(h);
		return -ENOMEM;
	}
	if (config->flags != NULL)
		memcpy(xhandle->pin_flags, config->flags, config->num_pins * sizeof(uint32_t));
	else
		memset(xhandle->pin_flags, 0, config->num_pins * sizeof(uint32_t));

	for (int i = 0; i < config->num_pins; i++) {
		if (xhandle->pin_flags[i] & CAPI_GPIO_ACTIVE_LOW)
			xhandle->active_low_mask |= (1ULL << i);
	}

	XGpioPs_Config *ps_cfg = XGpioPs_LookupConfig((UINTPTR)config->identifier);
	if (ps_cfg == NULL) {
		capi_free(xhandle->pin_flags);
		xhandle->pin_flags = NULL;
		if (alloc_handle)
			xilinx_gpio_free_allocated_handle(handle);
		else
			xilinx_gpio_clear_app_handle(h);
		return -ENODEV;
	}

	XGpioPs *ps_inst = capi_malloc(sizeof(XGpioPs));
	if (ps_inst == NULL) {
		capi_free(xhandle->pin_flags);
		xhandle->pin_flags = NULL;
		if (alloc_handle)
			xilinx_gpio_free_allocated_handle(handle);
		else
			xilinx_gpio_clear_app_handle(h);
		return -ENOMEM;
	}
	memset(ps_inst, 0, sizeof(XGpioPs));
	int status = XGpioPs_CfgInitialize(ps_inst, ps_cfg, ps_cfg->BaseAddr);
	if (status != XST_SUCCESS) {
		capi_free(ps_inst);
		capi_free(xhandle->pin_flags);
		xhandle->pin_flags = NULL;
		if (alloc_handle)
			xilinx_gpio_free_allocated_handle(handle);
		else
			xilinx_gpio_clear_app_handle(h);
		return -EIO;
	}

	if ((uint32_t)base_pin + config->num_pins >
	    capi_gpio_get_max_pins(ps_inst)) {
		capi_free(ps_inst);
		capi_free(xhandle->pin_flags);
		xhandle->pin_flags = NULL;
		if (alloc_handle)
			xilinx_gpio_free_allocated_handle(handle);
		else
			xilinx_gpio_clear_app_handle(h);
		return -EINVAL;
	}

	/* Validate the requested port remains within one physical GPIO bank. */
	u8 start_bank, start_pin_in_bank;
	u8 end_bank, end_pin_in_bank;
	/*
	 * Versal's XGpioPs_GetBankPin() signature takes the instance pointer;
	 * Zynq/ZynqMP BSPs expose the older static-helper signature.
	 */
#ifdef versal
	XGpioPs_GetBankPin(ps_inst, (u8)base_pin,
			   &start_bank, &start_pin_in_bank);
	XGpioPs_GetBankPin(ps_inst,
			   (u8)(base_pin + config->num_pins - 1),
			   &end_bank, &end_pin_in_bank);
#else
	XGpioPs_GetBankPin((u8)base_pin, &start_bank, &start_pin_in_bank);
	XGpioPs_GetBankPin((u8)(base_pin + config->num_pins - 1),
			   &end_bank, &end_pin_in_bank);
#endif
	if (start_bank != end_bank) {
		capi_free(ps_inst);
		capi_free(xhandle->pin_flags);
		xhandle->pin_flags = NULL;
		if (alloc_handle)
			xilinx_gpio_free_allocated_handle(handle);
		else
			xilinx_gpio_clear_app_handle(h);
		return -EINVAL;
	}

	xhandle->instance = ps_inst;
	return 0;
}

/**
 * @brief Deinitialize a PS GPIO port.
 * @note PS: Frees XGpioPs instance.
 *
 * @param[in,out] handle GPIO port handle pointer, set to NULL on success.
 * @return 0 on success, -EINVAL if handle is NULL.
 */
static int capi_gpio_xilinx_port_deinit(struct capi_gpio_port_handle **handle)
{
	if (handle == NULL || *handle == NULL)
		return -EINVAL;

	struct capi_gpio_port_handle *h = *handle;
	struct capi_gpio_xilinx_port_handle *xhandle = h->priv;
	if (xhandle == NULL)
		return -EINVAL;

	if (xhandle->instance != NULL) {
		capi_free(xhandle->instance);
		xhandle->instance = NULL;
	}
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
 * @brief Set PS GPIO port direction.
 * @note PS: XGpioPs_SetDirectionPin()/XGpioPs_SetOutputEnablePin().
 *
 * Iterates pins, maps bitmask bit i to physical pin base_pin+i.
 * XGpioPs direction: 0=input, 1=output (same as CAPI convention is inverted:
 * CAPI 1=input, 0=output, so we invert when calling SetDirectionPin).
 * Output enable is set here once per output pin, not on every value write.
 *
 * @param[in] handle GPIO port handle.
 * @param[in] direction_bitmask 1=input, 0=output per bit.
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_port_set_direction(struct capi_gpio_port_handle
		*handle,
		uint64_t direction_bitmask)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	xhandle->direction = direction_bitmask & ((1ULL << xhandle->num_pins) - 1);

	XGpioPs *inst = (XGpioPs *)xhandle->instance;
	for (int i = 0; i < xhandle->num_pins; i++) {
		u32 physical_pin = xhandle->base_pin + i;
		int is_input = (direction_bitmask >> i) & 1; /* CAPI: 1=input */
		/* XGpioPs: 0=input, 1=output */
		XGpioPs_SetDirectionPin(inst, physical_pin, is_input ? 0 : 1);
		/* Enable output buffer only for output pins */
		XGpioPs_SetOutputEnablePin(inst, physical_pin, is_input ? 0 : 1);
	}
	return 0;
}

/**
 * @brief Get PS GPIO port direction (cached).
 * @note PS: Returns cached direction.
 *
 * @param[in] handle GPIO port handle.
 * @param[out] direction_bitmask 1=input, 0=output per bit.
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_port_get_direction(struct capi_gpio_port_handle
		*handle,
		uint64_t *direction_bitmask)
{
	if (handle == NULL || direction_bitmask == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	*direction_bitmask = xhandle->direction;
	return 0;
}

/**
 * @brief Set PS GPIO port raw output value (no ACTIVE_LOW inversion).
 * @note PS: XGpioPs_WritePin().
 *
 * Skips input pins. Uses XGpioPs_WritePin() per output pin.
 *
 * @param[in] handle GPIO port handle.
 * @param[in] value_bitmask Raw value per bit.
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_port_set_raw_value(struct capi_gpio_port_handle
		*handle,
		uint64_t value_bitmask)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	XGpioPs *inst = (XGpioPs *)xhandle->instance;
	for (int i = 0; i < xhandle->num_pins; i++) {
		if (xhandle->direction & (1ULL << i))
			continue; /* skip input pins */

		u32 physical_pin = xhandle->base_pin + i;
		XGpioPs_WritePin(inst, physical_pin, (value_bitmask >> i) & 1);
	}
	return 0;
}

/**
 * @brief Set PS GPIO port output value (applies ACTIVE_LOW inversion).
 * @note PS: Active-low map then XGpioPs_WritePin().
 *
 * @param[in] handle GPIO port handle.
 * @param[in] value_bitmask Logical value per bit.
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_port_set_value(struct capi_gpio_port_handle *handle,
		uint64_t value_bitmask)
{
	if (handle == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	return capi_gpio_xilinx_port_set_raw_value(
		       handle, logical_to_raw(value_bitmask, xhandle->active_low_mask));
}

/**
 * @brief Get PS GPIO port raw value (no ACTIVE_LOW inversion).
 * @note PS: XGpioPs_ReadPin().
 *
 * Reads each pin via XGpioPs_ReadPin(), assembles bitmask.
 *
 * @param[in] handle GPIO port handle.
 * @param[out] value_bitmask Raw value per bit.
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

	XGpioPs *inst = (XGpioPs *)xhandle->instance;
	*value_bitmask = 0;
	for (int i = 0; i < xhandle->num_pins; i++) {
		u32 physical_pin = xhandle->base_pin + i;
		*value_bitmask |= ((uint64_t)(XGpioPs_ReadPin(inst, physical_pin) & 1) << i);
	}
	return 0;
}

/**
 * @brief Get PS GPIO port value (applies ACTIVE_LOW inversion).
 * @note PS: XGpioPs_ReadPin() plus active-low map.
 *
 * @param[in] handle GPIO port handle.
 * @param[out] value_bitmask Logical value per bit.
 * @return 0 on success, -EINVAL.
 */
static int capi_gpio_xilinx_port_get_value(struct capi_gpio_port_handle *handle,
		uint64_t *value_bitmask)
{
	if (handle == NULL || value_bitmask == NULL)
		return -EINVAL;

	struct capi_gpio_xilinx_port_handle *xhandle =
			handle->priv;

	uint64_t raw;
	int ret = capi_gpio_xilinx_port_get_raw_value(handle, &raw);
	if (ret != 0)
		return ret;

	*value_bitmask = raw_to_logical(raw, xhandle->active_low_mask);
	return 0;
}

/**
 * @brief Set PS GPIO pin direction.
 * @note PS: XGpioPs_SetDirectionPin()/XGpioPs_SetOutputEnablePin().
 *
 * pin->number is used directly as the absolute XGpioPs global pin ID.
 * XGpioPs: 0=input, 1=output. Output enable set/cleared here.
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

	XGpioPs *inst = (XGpioPs *)xhandle->instance;
	u32 physical_pin = xhandle->base_pin + pin->number;
	int is_input = (direction == CAPI_GPIO_INPUT);
	XGpioPs_SetDirectionPin(inst, physical_pin, is_input ? 0 : 1);
	XGpioPs_SetOutputEnablePin(inst, physical_pin, is_input ? 0 : 1);
	return 0;
}

/**
 * @brief Get PS GPIO pin direction (cached).
 * @note PS: Returns cached pin direction.
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

	*direction =
		(xhandle->direction & (1ULL << pin->number)) ? CAPI_GPIO_INPUT :
		CAPI_GPIO_OUTPUT;
	return 0;
}

/**
 * @brief Set PS GPIO pin value (applies ACTIVE_LOW inversion).
 * @note PS: XGpioPs_WritePin().
 *
 * Uses XGpioPs_WritePin() with pin->number as absolute global pin ID.
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

	uint8_t raw = (gpio_pin_flags(xhandle,
				      pin) & CAPI_GPIO_ACTIVE_LOW) ? !value : value;
	XGpioPs_WritePin((XGpioPs *)xhandle->instance, xhandle->base_pin + pin->number,
			 raw);
	return 0;
}

/**
 * @brief Get PS GPIO pin value (applies ACTIVE_LOW inversion).
 * @note PS: XGpioPs_ReadPin() plus active-low map.
 *
 * Uses XGpioPs_ReadPin() with pin->number as absolute global pin ID.
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

	uint8_t raw =
		XGpioPs_ReadPin((XGpioPs *)xhandle->instance,
				xhandle->base_pin + pin->number) & 1;
	*value = (gpio_pin_flags(xhandle, pin) & CAPI_GPIO_ACTIVE_LOW) ? !raw : raw;
	return 0;
}

/**
 * @brief Set PS GPIO pin raw output value (no ACTIVE_LOW inversion).
 * @note PS: XGpioPs_WritePin().
 *
 * Uses XGpioPs_WritePin() with pin->number as absolute global pin ID.
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

	XGpioPs_WritePin((XGpioPs *)xhandle->instance, xhandle->base_pin + pin->number,
			 value & 1);
	return 0;
}

/**
 * @brief Get PS GPIO pin raw value (no ACTIVE_LOW inversion).
 * @note PS: XGpioPs_ReadPin().
 *
 * Uses XGpioPs_ReadPin() with pin->number as absolute global pin ID.
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

	*value = XGpioPs_ReadPin((XGpioPs *)xhandle->instance,
				 xhandle->base_pin + pin->number) & 1;
	return 0;
}
