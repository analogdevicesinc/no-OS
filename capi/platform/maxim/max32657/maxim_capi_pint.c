/***************************************************************************//**
 *   @file   maxim_capi_pint.c
 *   @brief  Implementation of the Maxim CAPI PINT (pin interrupt) driver.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <string.h>
#include <errno.h>
#include "maxim_capi_pint_priv.h"
#include "maxim_capi_pint.h"
#include "max32657.h"
#include "gpio.h"
#include "capi_alloc.h"
#include "capi_irq.h"

/** Static variables **********************************************************/

static struct capi_pint_port_handle *pint[MXC_CFG_GPIO_INSTANCES] = {NULL};

/** Helper functions **********************************************************/

/**
 * @brief MSDK-to-CAPI callback adapter registered per pin.
 *
 * MSDK callbacks receive a single void* argument. The owning per-pin context
 * is passed as that argument so we can recover the pin index and the user's
 * CAPI-shaped callback.
 * @param cbdata Pointer to the pin's max_capi_pint_pin_ctx
 */
static void _max_capi_pint_msdk_adapter(void *cbdata)
{
	struct max_capi_pint_pin_ctx *ctx = cbdata;
	struct max_capi_pint_entry *entry;

	if (!ctx || !ctx->priv)
		return;

	entry = &ctx->priv->entries[ctx->pin_index];
	/* Honor the CAPI enabled state even if a flag stayed latched. */
	if (entry->enabled && entry->callback)
		entry->callback(ctx->pin_index, entry->user_data);
}

/**
 * @brief capi_irq line callback that routes into the CAPI pint handler.
 * @param arg - the owning capi_pint_port_handle
 */
static void _max_capi_pint_irq_callback(void *arg)
{
	capi_pint_handle_interrupt((struct capi_pint_port_handle *)arg);
}

/**
 * @brief Initialize a PINT (pin interrupt) port
 * @param handle The PINT port handle
 * @param config The PINT port configuration
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_pint_port_init(struct capi_pint_port_handle **handle,
				   const struct capi_pint_port_config *config)
{
	struct capi_pint_port_handle *pint_handle;
	struct max_capi_pint_port_priv *pint_priv;
	int ret;

	if (!handle || !config || !config->ops)
		return -EINVAL;

	if (config->identifier >= MXC_CFG_GPIO_INSTANCES)
		return -EINVAL;

	if (config->num_pins == 0 || config->num_pins > MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	if (*handle == NULL) {
		pint_handle = capi_calloc(1, sizeof(*pint_handle));
		if (!pint_handle)
			return -ENOMEM;

		pint_priv = capi_calloc(1, sizeof(*pint_priv));
		if (!pint_priv) {
			capi_free(pint_handle);
			return -ENOMEM;
		}

		pint_handle->priv = pint_priv;
		pint_handle->init_allocated = true;
	} else {
		pint_handle = *handle;
		if (!pint_handle->priv)
			return -EINVAL;
		pint_priv = pint_handle->priv;
		pint_handle->init_allocated = false;
	}

	pint_handle->ops = config->ops;

	/* The GPIO port is assumed to already be initialized via capi_gpio. */
	pint_priv->id = (uint32_t)config->identifier;
	pint_priv->port = MXC_GPIO_GET_GPIO(pint_priv->id);
	pint_priv->num_pins = config->num_pins;
	memset(pint_priv->entries, 0, sizeof(pint_priv->entries));
	memset(pint_priv->pin_ctx, 0, sizeof(pint_priv->pin_ctx));

	ret = capi_irq_connect(MXC_GPIO_GET_IRQ(pint_priv->id),
			       _max_capi_pint_irq_callback, pint_handle);
	if (ret) {
		if (pint_handle->init_allocated) {
			capi_free(pint_priv);
			capi_free(pint_handle);
		}
		return ret;
	}

	ret = capi_irq_enable(MXC_GPIO_GET_IRQ(pint_priv->id));
	if (ret) {
		if (pint_handle->init_allocated) {
			capi_free(pint_priv);
			capi_free(pint_handle);
		}
		return ret;
	}

	pint[pint_priv->id] = pint_handle;
	*handle = pint_handle;

	return 0;
}

/**
 * @brief Deinitialize a PINT port
 * @param handle The PINT port handle
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_pint_port_deinit(struct capi_pint_port_handle **handle)
{
	struct capi_pint_port_handle *pint_handle;
	struct max_capi_pint_port_priv *pint_priv;
	uint8_t i;
	uint32_t id;

	if (!handle || !*handle || !(*handle)->priv)
		return -EINVAL;

	pint_handle = *handle;
	pint_priv = pint_handle->priv;
	id = pint_priv->id;

	for (i = 0; i < pint_priv->num_pins; i++) {
		MXC_GPIO_DisableInt(pint_priv->port, (1U << i));
		pint_priv->entries[i].enabled = false;
		pint_priv->entries[i].callback = NULL;
		pint_priv->entries[i].user_data = NULL;
	}

	capi_irq_disable(MXC_GPIO_GET_IRQ(id));

	if (pint_handle->init_allocated) {
		capi_free(pint_priv);
		capi_free(pint_handle);
	}

	pint[id] = NULL;
	*handle = NULL;

	return 0;
}

/**
 * @brief Configure a pin's interrupt trigger and enable state
 * @param handle The PINT port handle
 * @param pin_index The pin to configure
 * @param config The pin configuration (trigger type, enable/disable)
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_pint_configure_pin(struct capi_pint_port_handle *handle,
				       uint8_t pin_index,
				       const struct capi_pint_pin_config *config)
{
	struct max_capi_pint_port_priv *pint_priv;
	mxc_gpio_int_pol_t pol;
	mxc_gpio_cfg_t cfg;

	if (!handle || !handle->priv || !config)
		return -EINVAL;

	pint_priv = handle->priv;
	if (pin_index >= pint_priv->num_pins)
		return -EINVAL;

	switch (config->trigger_type) {
	case CAPI_PINT_TRIGGER_EDGE_RISING:
		pol = MXC_GPIO_INT_RISING;
		break;
	case CAPI_PINT_TRIGGER_EDGE_FALLING:
		pol = MXC_GPIO_INT_FALLING;
		break;
	case CAPI_PINT_TRIGGER_LEVEL_LOW:
		pol = MXC_GPIO_INT_LOW;
		break;
	case CAPI_PINT_TRIGGER_LEVEL_HIGH:
		pol = MXC_GPIO_INT_HIGH;
		break;
	default:
		/* NOTE: capi_pint has no EDGE_BOTH; nothing maps to it. */
		return -EINVAL;
	}

	cfg = (mxc_gpio_cfg_t) {
		.port = pint_priv->port,
		.mask = (1U << pin_index),
		.func = MXC_GPIO_FUNC_IN,
	};
	MXC_GPIO_IntConfig(&cfg, pol);

	if (config->enabled) {
		MXC_GPIO_EnableInt(pint_priv->port, (1U << pin_index));
		pint_priv->entries[pin_index].enabled = true;
	} else {
		MXC_GPIO_DisableInt(pint_priv->port, (1U << pin_index));
		pint_priv->entries[pin_index].enabled = false;
	}

	return 0;
}

/**
 * @brief Enable the interrupt on a specific pin
 * @param handle The PINT port handle
 * @param pin_index The pin to enable
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_pint_enable_pin(struct capi_pint_port_handle *handle,
				    uint8_t pin_index)
{
	struct max_capi_pint_port_priv *pint_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	pint_priv = handle->priv;
	if (pin_index >= pint_priv->num_pins)
		return -EINVAL;

	MXC_GPIO_EnableInt(pint_priv->port, (1U << pin_index));
	pint_priv->entries[pin_index].enabled = true;

	return 0;
}

/**
 * @brief Disable the interrupt on a specific pin
 * @param handle The PINT port handle
 * @param pin_index The pin to disable
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_pint_disable_pin(struct capi_pint_port_handle *handle,
				     uint8_t pin_index)
{
	struct max_capi_pint_port_priv *pint_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	pint_priv = handle->priv;
	if (pin_index >= pint_priv->num_pins)
		return -EINVAL;

	MXC_GPIO_DisableInt(pint_priv->port, (1U << pin_index));
	pint_priv->entries[pin_index].enabled = false;

	return 0;
}

/**
 * @brief Register a callback for a specific pin
 * @param handle The PINT port handle
 * @param pin_index The pin to register the callback for
 * @param callback The callback to invoke on interrupt
 * @param user_data User data forwarded to the callback
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_pint_register_callback(struct capi_pint_port_handle *handle,
		uint8_t pin_index,
		capi_pint_callback_t callback,
		void *user_data)
{
	struct max_capi_pint_port_priv *pint_priv;
	mxc_gpio_cfg_t cfg;

	if (!handle || !handle->priv || !callback)
		return -EINVAL;

	pint_priv = handle->priv;
	if (pin_index >= pint_priv->num_pins)
		return -EINVAL;

	pint_priv->entries[pin_index].callback = callback;
	pint_priv->entries[pin_index].user_data = user_data;

	pint_priv->pin_ctx[pin_index].priv = pint_priv;
	pint_priv->pin_ctx[pin_index].pin_index = pin_index;

	cfg = (mxc_gpio_cfg_t) {
		.port = pint_priv->port,
		.mask = (1U << pin_index),
	};
	MXC_GPIO_RegisterCallback(&cfg, _max_capi_pint_msdk_adapter,
				  &pint_priv->pin_ctx[pin_index]);

	return 0;
}

/**
 * @brief Unregister a pin's callback
 * @param handle The PINT port handle
 * @param pin_index The pin to unregister
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_pint_unregister_callback(struct capi_pint_port_handle
		*handle,
		uint8_t pin_index)
{
	struct max_capi_pint_port_priv *pint_priv;
	mxc_gpio_cfg_t cfg;

	if (!handle || !handle->priv)
		return -EINVAL;

	pint_priv = handle->priv;
	if (pin_index >= pint_priv->num_pins)
		return -EINVAL;

	cfg = (mxc_gpio_cfg_t) {
		.port = pint_priv->port,
		.mask = (1U << pin_index),
	};
	MXC_GPIO_RegisterCallback(&cfg, NULL, NULL);

	pint_priv->entries[pin_index].callback = NULL;
	pint_priv->entries[pin_index].user_data = NULL;
	pint_priv->entries[pin_index].enabled = false;

	return 0;
}

/**
 * @brief Clear a pin's pending interrupt flag
 * @param handle The PINT port handle
 * @param pin_index The pin to clear
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_pint_clear_pending(struct capi_pint_port_handle *handle,
				       uint8_t pin_index)
{
	struct max_capi_pint_port_priv *pint_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	pint_priv = handle->priv;
	if (pin_index >= pint_priv->num_pins)
		return -EINVAL;

	MXC_GPIO_ClearFlags(pint_priv->port, (1U << pin_index));

	return 0;
}

/**
 * @brief Get the port's pending interrupt bitmask
 * @param handle The PINT port handle
 * @param pending Pointer where the pending bitmask is stored
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_pint_get_pending(struct capi_pint_port_handle *handle,
				     uint32_t *pending)
{
	struct max_capi_pint_port_priv *pint_priv;

	if (!handle || !handle->priv || !pending)
		return -EINVAL;

	pint_priv = handle->priv;
	*pending = MXC_GPIO_GetFlags(pint_priv->port);

	return 0;
}

/**
 * @brief Handle the port interrupt (call from the GPIO ISR)
 * @param handle The PINT port handle
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_pint_handle_interrupt(struct capi_pint_port_handle *handle)
{
	struct max_capi_pint_port_priv *pint_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	pint_priv = handle->priv;

	/* MSDK dispatches to the per-pin adapters and clears flags. */
	MXC_GPIO_Handler(MXC_GPIO_GET_IDX(pint_priv->port));

	return 0;
}

const struct capi_pint_ops max_capi_pint_ops = {
	.port_init = max_capi_pint_port_init,
	.port_deinit = max_capi_pint_port_deinit,
	.configure_pin = max_capi_pint_configure_pin,
	.enable_pin = max_capi_pint_enable_pin,
	.disable_pin = max_capi_pint_disable_pin,
	.register_callback = max_capi_pint_register_callback,
	.unregister_callback = max_capi_pint_unregister_callback,
	.clear_pending = max_capi_pint_clear_pending,
	.get_pending = max_capi_pint_get_pending,
	.handle_interrupt = max_capi_pint_handle_interrupt,
};
