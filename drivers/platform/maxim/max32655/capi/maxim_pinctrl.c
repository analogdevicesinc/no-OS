/***************************************************************************//**
 *   @file   maxim_pinctrl.c
 *   @brief  Source file of the pin control driver for the maxim platform
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "capi_pinctrl.h"
#include "maxim_pinctrl.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

#include "gpio.h"
#include "gpio_regs.h"
#include "max32655.h"
#include "mxc_errors.h"

/**
 * @brief Get GPIO registers pointer from port number
 */
static mxc_gpio_regs_t *get_gpio_regs(uint32_t port)
{
	switch (port) {
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
 * @brief Convert MAXIM pin function flags to mxc_gpio_func_t
 */
static mxc_gpio_func_t convert_function_flags(uint32_t flags)
{
	uint32_t func = flags & MAXIM_PIN_FUNC_MASK;

	switch (func) {
	case MAXIM_PIN_FUNC_IN:
		return MXC_GPIO_FUNC_IN; /* Default to input for GPIO */
	case MAXIM_PIN_FUNC_ALT1:
		return MXC_GPIO_FUNC_ALT1;
	case MAXIM_PIN_FUNC_ALT2:
		return MXC_GPIO_FUNC_ALT2;
	case MAXIM_PIN_FUNC_ALT3:
		return MXC_GPIO_FUNC_ALT3;
	case MAXIM_PIN_FUNC_ALT4:
		return MXC_GPIO_FUNC_ALT4;
	default:
		return MXC_GPIO_FUNC_IN;
	}
}

/**
 * @brief Convert MAXIM pin pad flags to mxc_gpio_pad_t
 */
static mxc_gpio_pad_t convert_pad_flags(uint32_t flags)
{
	uint32_t pad = (flags & MAXIM_PIN_PAD_MASK) >> 3;

	switch (pad) {
	case 0: /* MAXIM_PIN_PAD_NONE */
		return MXC_GPIO_PAD_NONE;
	case 1: /* MAXIM_PIN_PAD_PULL_UP */
		return MXC_GPIO_PAD_PULL_UP;
	case 2: /* MAXIM_PIN_PAD_PULL_DOWN */
		return MXC_GPIO_PAD_PULL_DOWN;
	case 3: /* MAXIM_PIN_PAD_WEAK_PULL_UP */
		return MXC_GPIO_PAD_WEAK_PULL_UP;
	case 4: /* MAXIM_PIN_PAD_WEAK_PULL_DOWN */
		return MXC_GPIO_PAD_WEAK_PULL_DOWN;
	default:
		return MXC_GPIO_PAD_NONE;
	}
}

/**
 * @brief Convert MAXIM pin voltage selection flags to mxc_gpio_vssel_t
 */
static mxc_gpio_vssel_t convert_vssel_flags(uint32_t flags)
{
	uint32_t vssel = (flags & MAXIM_PIN_VSSEL_MASK) >> 6;

	switch (vssel) {
	case 0: /* MAXIM_PIN_VSSEL_VDDIO */
		return MXC_GPIO_VSSEL_VDDIO;
	case 1: /* MAXIM_PIN_VSSEL_VDDIOH */
		return MXC_GPIO_VSSEL_VDDIOH;
	default:
		return MXC_GPIO_VSSEL_VDDIO;
	}
}

/**
 * @brief Convert MAXIM pin drive strength flags to mxc_gpio_drvstr_t
 */
static mxc_gpio_drvstr_t convert_drvstr_flags(uint32_t flags)
{
	uint32_t drvstr = (flags & MAXIM_PIN_DRVSTR_MASK) >> 8;

	switch (drvstr) {
	case 0: /* MAXIM_PIN_DRVSTR_0 */
		return MXC_GPIO_DRVSTR_0;
	case 1: /* MAXIM_PIN_DRVSTR_1 */
		return MXC_GPIO_DRVSTR_1;
	case 2: /* MAXIM_PIN_DRVSTR_2 */
		return MXC_GPIO_DRVSTR_2;
	case 3: /* MAXIM_PIN_DRVSTR_3 */
		return MXC_GPIO_DRVSTR_3;
	default:
		return MXC_GPIO_DRVSTR_0;
	}
}

/**
 * @brief Initialize the pin controller
 */
static int maxim_capi_pinctrl_init(struct capi_pinctrl_handle **handle,
				   const struct capi_pinctrl_config *config)
{
	struct maxim_capi_pinctrl_handle *maxim_handle;
	int ret;

	if (!handle || !config)
		return -EINVAL;

	if (!*handle) {
		maxim_handle = no_os_calloc(1, sizeof(*maxim_handle));
		if (!maxim_handle)
			return -ENOMEM;

		maxim_handle->capi_handle.init_allocated = true;
	} else {
		maxim_handle = (struct maxim_capi_pinctrl_handle *)*handle;
		maxim_handle->capi_handle.init_allocated = false;
	}

	/* Set up CAPI handle */
	maxim_handle->capi_handle.ops = config->ops;
	maxim_handle->capi_handle.identifier = config->identifier;
	maxim_handle->capi_handle.extra = config->extra;

	/* Initialize GPIO ports as needed */
	maxim_handle->port_mask = 0;
	maxim_handle->extra = config->extra;

	*handle = &maxim_handle->capi_handle;

	return 0;
}

/**
 * @brief Deinitialize the pin controller
 */
static int maxim_capi_pinctrl_deinit(struct capi_pinctrl_handle **handle)
{
	struct maxim_capi_pinctrl_handle *maxim_handle;

	if (!handle || !*handle)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_pinctrl_handle *)*handle;

	if (maxim_handle->capi_handle.init_allocated)
		no_os_free(maxim_handle);

	*handle = NULL;

	return 0;
}

/**
 * @brief Configure a single pin
 */
static int maxim_capi_pinctrl_set_pin(const struct capi_pinctrl_handle *handle,
				      const struct capi_pinctrl_descriptor *pin)
{
	struct maxim_capi_pinctrl_handle *maxim_handle;
	mxc_gpio_cfg_t gpio_cfg;
	mxc_gpio_regs_t *gpio_regs;
	uint32_t port, pin_num;
	int ret;

	if (!handle || !pin)
		return -EINVAL;

	maxim_handle = (struct maxim_capi_pinctrl_handle *)handle;

	/* Extract port and pin number */
	port = MAXIM_PIN_GET_PORT(pin->number);
	pin_num = MAXIM_PIN_GET_PIN(pin->number);

	if (port > 3 || pin_num > 31)
		return -EINVAL;

	/* Get GPIO registers for this port */
	gpio_regs = get_gpio_regs(port);
	if (!gpio_regs)
		return -EINVAL;

	/* Initialize GPIO port if not already done */
	if (!(maxim_handle->port_mask & NO_OS_BIT(port))) {
		ret = MXC_GPIO_Init(NO_OS_BIT(port));
		if (ret != E_SUCCESS)
			return -EIO;
		maxim_handle->port_mask |= NO_OS_BIT(port);
	}

	/* Configure GPIO pin */
	gpio_cfg.port = gpio_regs;
	gpio_cfg.mask = NO_OS_BIT(pin_num);
	gpio_cfg.func = convert_function_flags(pin->pinmux_flags);
	gpio_cfg.pad = convert_pad_flags(pin->pinmux_flags);
	gpio_cfg.vssel = convert_vssel_flags(pin->pinmux_flags);
	gpio_cfg.drvstr = convert_drvstr_flags(pin->pinmux_flags);

	/* Handle direction based on pn_flags */
	if (pin->pn_flags & CAPI_PIN_OUTPUT) {
		gpio_cfg.func = MXC_GPIO_FUNC_OUT;
	} else if (pin->pn_flags & CAPI_PIN_INPUT) {
		gpio_cfg.func = MXC_GPIO_FUNC_IN;
	}

	/* Override pad configuration based on pn_flags */
	if (pin->pn_flags & CAPI_PIN_PULL_UP) {
		gpio_cfg.pad = MXC_GPIO_PAD_PULL_UP;
	} else if (pin->pn_flags & CAPI_PIN_PULL_DOWN) {
		gpio_cfg.pad = MXC_GPIO_PAD_PULL_DOWN;
	}

	/* Configure open drain if specified */
	if (pin->pn_flags & CAPI_PIN_OPEN_DRAIN) {
		/* Open drain is typically implemented using weak pull-up */
		gpio_cfg.pad = MXC_GPIO_PAD_WEAK_PULL_UP;
	}

	ret = MXC_GPIO_Config(&gpio_cfg);
	if (ret != E_SUCCESS)
		return -EIO;

	return 0;
}

/**
 * @brief Configure multiple pins
 */
static int maxim_capi_pinctrl_set_pins(const struct capi_pinctrl_handle *handle,
				       const struct capi_pinctrl_descriptor *pins,
				       uint32_t num_pins)
{
	int ret;
	uint32_t i;

	if (!handle || !pins || num_pins == 0)
		return -EINVAL;

	/* Configure each pin individually */
	for (i = 0; i < num_pins; i++) {
		ret = maxim_capi_pinctrl_set_pin(handle, &pins[i]);
		if (ret != 0)
			return ret;
	}

	return 0;
}

/**
 * @brief MAX32655 pinctrl CAPI operations structure
 */
const struct capi_pinctrl_ops maxim_pinctrl_ops = {
	.pinctrl_init = maxim_capi_pinctrl_init,
	.pinctrl_deinit = maxim_capi_pinctrl_deinit,
	.pinctrl_set_pin = maxim_capi_pinctrl_set_pin,
	.pinctrl_set_pins = maxim_capi_pinctrl_set_pins,
};