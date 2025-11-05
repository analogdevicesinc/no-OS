/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CAPI_PINCTRL_H_
#define _CAPI_PINCTRL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

/**
 * Common pin attributes.
 */
#define CAPI_PIN_INPUT      (1U)
#define CAPI_PIN_OUTPUT     (2U)
#define CAPI_PIN_PULL_UP    (4U)
#define CAPI_PIN_PULL_DOWN  (8U)
#define CAPI_PIN_ENABLE     (16U)
#define CAPI_PIN_OPEN_DRAIN (32U)

/**
 * Pin Control handle.
 */
struct capi_pinctrl_handle {
	/** Set if init() has allocated the required memory. */
	bool init_allocated;
	/** Pin Control specific operations. */
	const struct capi_pinctrl_ops *ops;
	/** Pin Control identifier. */
	uint64_t identifier;
	/** Reference to pin control private information. */
	void *extra;
};

/**
 * Pin configuration.
 */
struct capi_pinctrl_config {
	/** Pin control specific operations. */
	const struct capi_pinctrl_ops *ops;
	/** Pin control instance. */
	uint64_t identifier;
	/** Pin control flags. */
	uint32_t *flags;
	/** Reference to extra information. */
	void *extra;
};

/**
 * Pin configuration/descriptor.
 */
struct capi_pinctrl_descriptor {
	/** Pin number which can be a number and group based on
	    the underlying implementation.
	*/
	int number;
	/** Pin flags based on implementation. */
	uint32_t pn_flags;
	/** Pin Mux flags based on implementation. */
	uint32_t pinmux_flags;
};

/**
 * @brief Initialize the Pin Controller.
 *
 * @param [in,out] handle If NULL, the function must allocate the required
 *                        memory. Otherwise, init() will use the preallocated
 *                        structure pointed to by the handle.
 * @param [in] config The pin controller configuration.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pinctrl_init(struct capi_pinctrl_handle **handle,
		      const struct capi_pinctrl_config *config);

/**
 * @brief Deinitialize the Pin Controller and place all pins in default position.
 *
 * @param [in] handle The pin controller handle. If allocated by the init()
 *                    function, deinit() must free the memory.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pinctrl_deinit(struct capi_pinctrl_handle **handle);

/**
 * @brief Set the pin configuration for one pin.
 *
 * @param [in] handle The pin controller handle.
 * @param [in] pin Defines the pin and its attributes. Attributes can be direction,
 *                 pullup, pulldown, state, and others if implemented. The
 *                 configuration may also contain pin multiplexer flags that set
 *                 the source/sink for the specified pin.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pinctrl_set_pin(const struct capi_pinctrl_handle *handle,
			 const struct capi_pinctrl_descriptor *pin);

/**
 * @brief Set the configuration for a group of pin.
 *
 * @param [in] handle The pin controller handle.
 * @param [in] pins Defines each pin and its attributes. Attributes can be direction,
 *                 pullup, pulldown, state, and others if implemented. The
 *                 configuration may also contain pin multiplexer flags that set
 *                 the source/sink for the specified pin.
 * @param [in] num_pins Defines the number of pins to configure.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_pinctrl_set_pins(const struct capi_pinctrl_handle *handle,
			  const struct capi_pinctrl_descriptor *pins, uint32_t num_pins);

/**
 * Container for the GPIO specific operations.
 */
struct capi_pinctrl_ops {
	/** See capi_pinctrl_init() */
	int (*pinctrl_init)(struct capi_pinctrl_handle **handle,
			    const struct capi_pinctrl_config *config);
	/** See capi_pinctrl_deinit() */
	int (*pinctrl_deinit)(struct capi_pinctrl_handle **handle);
	/** See capi_pinctrl_set_pin() */
	int (*pinctrl_set_pin)(const struct capi_pinctrl_handle *handle,
			       const struct capi_pinctrl_descriptor *pin);
	/** See capi_pinctrl_set_pins() */
	int (*pinctrl_set_pins)(const struct capi_pinctrl_handle *handle,
				const struct capi_pinctrl_descriptor *pins, uint32_t num_pins);
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CAPI_PINCTRL_H_ */
