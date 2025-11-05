/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _CAPI_GPIO_H_
#define _CAPI_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

/**
 * GPIO direction options.
 */
#define CAPI_GPIO_OUTPUT 0x0
#define CAPI_GPIO_INPUT  0x1

/**
 * GPIO pin value options.
 */
#define CAPI_GPIO_LOW  0x0
#define CAPI_GPIO_HIGH 0x1

/**
 * GPIO pin flags options.
 */
#define CAPI_GPIO_ACTIVE_HIGH 0x00
#define CAPI_GPIO_ACTIVE_LOW  0x01

/**
 * GPIO port handle.
 */
struct capi_gpio_port_handle {
	/** Set if init() has allocated the required memory. */
	bool init_allocated;
	/** GPIO port specific operations. */
	const struct capi_gpio_ops *ops;
	/** GPIO port identifier:
	 * - Base address for internal GPIO controllers.
	 * - Device specific address for external GPIO controllers (e.g., GPIO
	 *   expanders).
	 */
	uint64_t identifier;
	/** Reference to GPIO port private information. */
	void *priv;
};

/**
 * GPIO port configuration.
 */
struct capi_gpio_port_config {
	/** GPIO port specific operations. */
	const struct capi_gpio_ops *ops;
	/** GPIO port identifier:
	 * - Base address for internal GPIO controllers.
	 * - Device specific address for external GPIO controllers (e.g., GPIO
	 *   expanders).
	 */
	uint64_t identifier;
	/** Number of GPIO pins. */
	uint8_t num_pins;
	/** GPIO pins flags. */
	uint32_t *flags;
	/** Reference to GPIO port private information. */
	void *priv;
};

/**
 * GPIO pin configuration/descriptor.
 */
struct capi_gpio_pin {
	/** Corresponding GPIO port handle. */
	struct capi_gpio_port_handle *port_handle;
	/** GPIO pin number. */
	int number;
	/** GPIO pin flags. */
	uint32_t flags;
};

/**
 * @brief Initialize the GPIO port.
 *
 * @param [in,out] handle If NULL, the function must allocate the required
 *                        memory. Otherwise, init() will use the preallocated
 *                        structure pointed to by the handle.
 * @param [in] config The GPIO port configuration.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_port_init(struct capi_gpio_port_handle **handle,
			const struct capi_gpio_port_config *config);

/**
 * @brief Deinitialize the GPIO port.
 *
 * @param [in] handle The GPIO port handle. If allocated by the init()
 *                    function, deinit() must free the memory.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_port_deinit(struct capi_gpio_port_handle **handle);

/**
 * @brief Set the direction of all the pins belonging to the specified port.
 *
 * @param [in] handle The GPIO port.
 * @param [in] direction_bitmask The direction (bitmask) to be set for the
 *                               GPIO pins.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_port_set_direction(struct capi_gpio_port_handle *handle, uint64_t direction_bitmask);

/**
 * @brief Get the current direction of all the pins belonging to the
 *        specified port.
 *
 * @param [in] handle The GPIO port.
 * @param [out] direction_bitmask The current direction (bitmask) of the
 *                                GPIO pins.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_port_get_direction(struct capi_gpio_port_handle *handle, uint64_t *direction_bitmask);

/**
 * @brief Set the value of all the pins belonging to the specified port.
 *
 * @param [in] handle The GPIO port.
 * @param [in] value_bitmask The value (bitmask) to be set for the GPIO pins.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_port_set_value(struct capi_gpio_port_handle *handle, uint64_t value_bitmask);

/**
 * @brief Get the current value of all the pins belonging to the
 *        specified port.
 *
 * @param [in] handle The GPIO port.
 * @param [out] value_bitmask The current value (bitmask) of the GPIO pins.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_port_get_value(struct capi_gpio_port_handle *handle, uint64_t *value_bitmask);

/**
 * @brief Set the raw value of all the pins belonging to the specified port.
 *
 * @param [in] handle The GPIO port.
 * @param [in] value_bitmask The raw value (bitmask) to be set for the GPIO
 *                           pins, regardless of the ACTIVE_LOW flag.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_port_set_raw_value(struct capi_gpio_port_handle *handle, uint64_t value_bitmask);

/**
 * @brief Get the current raw value of all the pins belonging to the
 *        specified port.
 *
 * @param [in] handle The GPIO port.
 * @param [out] value_bitmask The current raw value (bitmask) of the GPIO pins,
 *                            regardless of the ACTIVE_LOW flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_port_get_raw_value(struct capi_gpio_port_handle *handle, uint64_t *value_bitmask);

/**
 * @brief Set the direction of the specified GPIO pin.
 *
 * @param [in] pin The GPIO pin.
 * @param [in] direction The direction to be set for the GPIO pin.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_pin_set_direction(struct capi_gpio_pin *pin, uint8_t direction);

/**
 * @brief Get the current direction of the specified GPIO pin.
 *
 * @param [in] pin The GPIO pin.
 * @param [out] direction The current direction of the GPIO pin.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_pin_get_direction(struct capi_gpio_pin *pin, uint8_t *direction);

/**
 * @brief Set the value of the specified GPIO pin.
 *
 * @param [in] pin The GPIO pin.
 * @param [in] value The value to be set for the GPIO pin.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_pin_set_value(struct capi_gpio_pin *pin, uint8_t value);

/**
 * @brief Get the current value of the specified GPIO pin.
 *
 * @param [in] pin The GPIO pin.
 * @param [out] value The current value of the GPIO pin.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_pin_get_value(struct capi_gpio_pin *pin, uint8_t *value);

/**
 * @brief Set the raw value of the specified GPIO pin.
 *
 * @param [in] pin The GPIO pin.
 * @param [in] value The raw value to be set for the GPIO pin,
 *                   regardless of the ACTIVE_LOW flag.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_pin_set_raw_value(struct capi_gpio_pin *pin, uint8_t value);

/**
 * @brief Get the current raw value of all the pins belonging to the
 *        specified port.
 *
 * @param [in] pin The GPIO pin.
 * @param [out] value The current raw value of the GPIO pin,
 *                    regardless of the ACTIVE_LOW flag.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int capi_gpio_pin_get_raw_value(struct capi_gpio_pin *pin, uint8_t *value);

/**
 * Container for the GPIO specific operations.
 */
struct capi_gpio_ops {
	/** See capi_gpio_port_init() */
	int (*port_init)(struct capi_gpio_port_handle **handle,
			 const struct capi_gpio_port_config *config);
	/** See capi_gpio_port_deinit() */
	int (*port_deinit)(struct capi_gpio_port_handle **handle);
	/** See capi_gpio_port_set_direction() */
	int (*port_set_direction)(struct capi_gpio_port_handle *handle, uint64_t direction_bitmask);
	/** See capi_gpio_port_get_direction() */
	int (*port_get_direction)(struct capi_gpio_port_handle *handle,
				  uint64_t *direction_bitmask);
	/** See capi_gpio_port_set_value() */
	int (*port_set_value)(struct capi_gpio_port_handle *handle, uint64_t value_bitmask);
	/** See capi_gpio_port_get_value() */
	int (*port_get_value)(struct capi_gpio_port_handle *handle, uint64_t *value_bitmask);
	/** See capi_gpio_port_set_raw_value() */
	int (*port_set_raw_value)(struct capi_gpio_port_handle *handle, uint64_t value_bitmask);
	/** See capi_gpio_port_get_raw_value() */
	int (*port_get_raw_value)(struct capi_gpio_port_handle *handle, uint64_t *value_bitmask);
	/** See capi_gpio_pin_set_direction() */
	int (*pin_set_direction)(struct capi_gpio_pin *pin, uint8_t direction);
	/** See capi_gpio_pin_get_direction() */
	int (*pin_get_direction)(struct capi_gpio_pin *pin, uint8_t *direction);
	/** See capi_gpio_pin_set_value() */
	int (*pin_set_value)(struct capi_gpio_pin *pin, uint8_t value);
	/** See capi_gpio_pin_get_value() */
	int (*pin_get_value)(struct capi_gpio_pin *pin, uint8_t *value);
	/** See capi_gpio_pin_set_raw_value() */
	int (*pin_set_raw_value)(struct capi_gpio_pin *pin, uint8_t value);
	/** See capi_gpio_pin_get_raw_value() */
	int (*pin_get_raw_value)(struct capi_gpio_pin *pin, uint8_t *value);
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _CAPI_GPIO_H_ */
