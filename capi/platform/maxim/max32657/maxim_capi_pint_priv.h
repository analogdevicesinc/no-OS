/***************************************************************************//**
 *   @file   maxim_capi_pint_priv.h
 *   @brief  Private handle for the CAPI PINT (pin interrupt) driver.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_PINT_PRIV_H_
#define MAXIM_CAPI_PINT_PRIV_H_

#include <stdint.h>
#include "capi_pint.h"
#include "max32657.h"
#include "gpio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_pint_entry
 * @brief  Per-pin callback bookkeeping.
 */
struct max_capi_pint_entry {
	/** User callback (CAPI shape) */
	capi_pint_callback_t callback;
	/** User data forwarded to the callback */
	void *user_data;
	/** True when the pin interrupt is enabled */
	bool enabled;
};

/**
 * @struct max_capi_pint_pin_ctx
 * @brief  Context handed to MSDK as the per-pin callback argument.
 *
 * MSDK callbacks receive a single void*, so this carries what the adapter
 * needs to rebuild the CAPI-shaped call. Embedded in the port priv so its
 * address is stable for the port's lifetime and scales per port.
 */
struct max_capi_pint_pin_ctx {
	struct max_capi_pint_port_priv *priv;
	uint8_t pin_index;
};

/**
 * @struct max_capi_pint_port_priv
 * @brief  Private structure for a single PINT (GPIO) port.
 */
struct max_capi_pint_port_priv {
	/** GPIO port id (index into the pint[] table) */
	uint32_t id;
	/** MSDK GPIO port registers */
	mxc_gpio_regs_t *port;
	/** Number of pins on the port */
	uint8_t num_pins;
	/** Per-pin callback table */
	struct max_capi_pint_entry entries[MXC_CFG_GPIO_PINS_PORT];
	/** Per-pin MSDK callback contexts */
	struct max_capi_pint_pin_ctx pin_ctx[MXC_CFG_GPIO_PINS_PORT];
};

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_PINT_PRIV_H_ */
