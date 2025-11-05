/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_pinctrl.h
 * @brief MAX32655 platform-specific pin control CAPI implementation header
 * @author Claude Code (noreply@anthropic.com)
 */

#ifndef MAXIM_CAPI_PINCTRL_H_
#define MAXIM_CAPI_PINCTRL_H_

#include "capi_pinctrl.h"
#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maxim-specific pin flags for MAX32655
 *
 * These flags map to the Maxim GPIO configuration parameters and can be
 * used in the pinmux_flags field of capi_pinctrl_descriptor
 */

/* Function selection flags (bits 0-2) */
#define MAXIM_PIN_FUNC_IN	(0U << 0)  /**< GPIO function */
#define MAXIM_PIN_FUNC_ALT1     (1U << 0)  /**< Alternate function 1 */
#define MAXIM_PIN_FUNC_ALT2     (2U << 0)  /**< Alternate function 2 */
#define MAXIM_PIN_FUNC_ALT3     (3U << 0)  /**< Alternate function 3 */
#define MAXIM_PIN_FUNC_ALT4     (4U << 0)  /**< Alternate function 4 */
#define MAXIM_PIN_FUNC_MASK     (7U << 0)  /**< Function mask */

/* Pad configuration flags (bits 3-5) */
#define MAXIM_PIN_PAD_NONE          (0U << 3)  /**< No pull-up or pull-down */
#define MAXIM_PIN_PAD_PULL_UP       (1U << 3)  /**< Strong pull-up */
#define MAXIM_PIN_PAD_PULL_DOWN     (2U << 3)  /**< Strong pull-down */
#define MAXIM_PIN_PAD_WEAK_PULL_UP  (3U << 3)  /**< Weak pull-up */
#define MAXIM_PIN_PAD_WEAK_PULL_DOWN (4U << 3) /**< Weak pull-down */
#define MAXIM_PIN_PAD_MASK          (7U << 3)  /**< Pad mask */

/* Voltage selection flags (bits 6-7) */
#define MAXIM_PIN_VSSEL_VDDIO   (0U << 6)  /**< VDDIO voltage */
#define MAXIM_PIN_VSSEL_VDDIOH  (1U << 6)  /**< VDDIOH voltage */
#define MAXIM_PIN_VSSEL_MASK    (1U << 6)  /**< Voltage select mask */

/* Drive strength flags (bits 8-9) */
#define MAXIM_PIN_DRVSTR_0      (0U << 8)  /**< Drive strength 0 */
#define MAXIM_PIN_DRVSTR_1      (1U << 8)  /**< Drive strength 1 */
#define MAXIM_PIN_DRVSTR_2      (2U << 8)  /**< Drive strength 2 */
#define MAXIM_PIN_DRVSTR_3      (3U << 8)  /**< Drive strength 3 */
#define MAXIM_PIN_DRVSTR_MASK   (3U << 8)  /**< Drive strength mask */

/**
 * @brief Private handle structure for MAX32655 pinctrl CAPI implementation
 */
struct maxim_capi_pinctrl_handle {
	struct capi_pinctrl_handle capi_handle;  /**< Must be first field */
	uint32_t port_mask;                      /**< Bitmask of enabled GPIO ports */
	void *extra;                             /**< Extra private data */
};

/**
 * @brief Convenience macro to create a pin descriptor for common configurations
 *
 * @param port GPIO port number (0-3)
 * @param pin Pin number within port (0-31)
 * @param func Function selection (MAXIM_PIN_FUNC_*)
 * @param pad Pad configuration (MAXIM_PIN_PAD_*)
 * @param vssel Voltage selection (MAXIM_PIN_VSSEL_*)
 * @param drvstr Drive strength (MAXIM_PIN_DRVSTR_*)
 */
#define MAXIM_PIN_DESCRIPTOR(port, pin, func, pad, vssel, drvstr) \
	{ \
		.number = ((port) << 5) | (pin), \
		.pn_flags = (CAPI_PIN_ENABLE) | (pad), \
		.pinmux_flags = (func) | (pad) | (vssel) | (drvstr) \
	}

/**
 * @brief Extract port number from pin descriptor number
 */
#define MAXIM_PIN_GET_PORT(number) (((number) >> 5) & 0x7)

/**
 * @brief Extract pin number from pin descriptor number
 */
#define MAXIM_PIN_GET_PIN(number) ((number) & 0x1F)

/**
 * @brief MAX32655 pinctrl CAPI operations structure
 *
 * This structure contains function pointers for all pinctrl CAPI operations
 * specific to the MAX32655 platform.
 */
extern const struct capi_pinctrl_ops maxim_capi_pinctrl_ops;

#ifdef __cplusplus
}
#endif

#endif /* MAXIM_CAPI_PINCTRL_H_ */