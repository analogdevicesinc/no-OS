#ifndef STM32_CAPI_GPIO_H_
#define STM32_CAPI_GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"
#include "capi/capi_gpio.h"

/**
 * STM32-specific GPIO pin flags (can be OR'd with CAPI_GPIO_* flags)
 */
#define STM32_GPIO_OPEN_DRAIN  0x02

/**
 * @struct stm32_capi_gpio_port_config
 * @brief STM32-specific GPIO port configuration for CAPI interface
 */
struct stm32_capi_gpio_port_config {
	/** Default output mode for pins (GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP, etc.) */
	uint32_t mode;
	/** Default speed grade for pins */
	uint32_t speed;
	/** Default alternate functionality (for AF modes) */
	uint32_t alternate;
	/** Default pull configuration (GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN) */
	uint32_t pull;
};

/**
 * @struct stm32_capi_gpio_port_priv
 * @brief STM32 platform specific GPIO port private data
 */
struct stm32_capi_gpio_port_priv {
	/** STM32 GPIO port peripheral (GPIOA, GPIOB, etc.) */
	GPIO_TypeDef *port;
	/** Number of pins in this port */
	uint8_t num_pins;
	/** Current direction configuration for all pins (1=input, 0=output) */
	uint16_t direction_mask;
	/** Default configuration */
	struct stm32_capi_gpio_port_config default_config;
};

/**
 * @brief STM32 platform specific GPIO operations for CAPI
 */
extern const struct capi_gpio_ops stm32_capi_gpio_ops;

/**
 * @brief Initialize a GPIO port
 * @param handle - Pointer to port handle pointer
 * @param config - Port configuration
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_port_init(struct capi_gpio_port_handle **handle,
			      const struct capi_gpio_port_config *config);

/**
 * @brief Deinitialize a GPIO port
 * @param handle - Pointer to port handle pointer
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_port_deinit(struct capi_gpio_port_handle **handle);

/**
 * @brief Set direction for all pins in the port
 * @param handle - Port handle
 * @param direction_bitmask - Direction bitmask (1=input, 0=output)
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_port_set_direction(struct capi_gpio_port_handle *handle,
				       uint64_t direction_bitmask);

/**
 * @brief Get direction of all pins in the port
 * @param handle - Port handle
 * @param direction_bitmask - Pointer to store direction bitmask
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_port_get_direction(struct capi_gpio_port_handle *handle,
				       uint64_t *direction_bitmask);

/**
 * @brief Set value for all pins in the port
 * @param handle - Port handle
 * @param value_bitmask - Value bitmask
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_port_set_value(struct capi_gpio_port_handle *handle,
				   uint64_t value_bitmask);

/**
 * @brief Get value of all pins in the port
 * @param handle - Port handle
 * @param value_bitmask - Pointer to store value bitmask
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_port_get_value(struct capi_gpio_port_handle *handle,
				   uint64_t *value_bitmask);

/**
 * @brief Set raw value for all pins (ignoring ACTIVE_LOW flag)
 * @param handle - Port handle
 * @param value_bitmask - Value bitmask
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_port_set_raw_value(struct capi_gpio_port_handle *handle,
				       uint64_t value_bitmask);

/**
 * @brief Get raw value of all pins (ignoring ACTIVE_LOW flag)
 * @param handle - Port handle
 * @param value_bitmask - Pointer to store value bitmask
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_port_get_raw_value(struct capi_gpio_port_handle *handle,
				       uint64_t *value_bitmask);

/**
 * @brief Set direction for a single pin
 * @param pin - Pin descriptor
 * @param direction - Direction (CAPI_GPIO_INPUT or CAPI_GPIO_OUTPUT)
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_pin_set_direction(struct capi_gpio_pin *pin, uint8_t direction);

/**
 * @brief Get direction of a single pin
 * @param pin - Pin descriptor
 * @param direction - Pointer to store direction
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_pin_get_direction(struct capi_gpio_pin *pin, uint8_t *direction);

/**
 * @brief Set value for a single pin
 * @param pin - Pin descriptor
 * @param value - Value (CAPI_GPIO_HIGH or CAPI_GPIO_LOW)
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_pin_set_value(struct capi_gpio_pin *pin, uint8_t value);

/**
 * @brief Get value of a single pin
 * @param pin - Pin descriptor
 * @param value - Pointer to store value
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_pin_get_value(struct capi_gpio_pin *pin, uint8_t *value);

/**
 * @brief Set raw value for a single pin (ignoring ACTIVE_LOW flag)
 * @param pin - Pin descriptor
 * @param value - Value
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_pin_set_raw_value(struct capi_gpio_pin *pin, uint8_t value);

/**
 * @brief Get raw value of a single pin (ignoring ACTIVE_LOW flag)
 * @param pin - Pin descriptor
 * @param value - Pointer to store value
 * @return 0 on success, negative error code otherwise
 */
int stm32_capi_gpio_pin_get_raw_value(struct capi_gpio_pin *pin, uint8_t *value);

#endif /* STM32_CAPI_GPIO_H_ */
