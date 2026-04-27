#ifndef STM32_CAPI_I2C_H_
#define STM32_CAPI_I2C_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32_hal.h"
#include "capi/capi_i2c.h"

/**
 * @struct stm32_i2c_extra_config
 * @brief STM32 platform specific I2C configuration for CAPI interface
 */
struct stm32_i2c_extra_config {
	/** I2C Handle - can be NULL for auto-init based on identifier */
	I2C_HandleTypeDef *hi2c;
	/** I2C Timing (for STM32 families that use timing register) */
	uint32_t i2c_timing;
};

/**
 * @struct stm32_i2c_priv_handle
 * @brief STM32 platform specific I2C private handle
 */
struct stm32_i2c_priv_handle {
	/** I2C HAL Handle */
	I2C_HandleTypeDef hi2c;
	/** CAPI callback and context */
	capi_i2c_callback callback;
	void *callback_arg;
	/** Async transfer state */
	bool async_in_progress;
	/** Current transfer for async operations */
	struct capi_i2c_transfer *current_transfer;
};

/**
 * @brief STM32 CAPI I2C operations structure
 */
extern const struct capi_i2c_ops stm32_capi_i2c_ops;

#endif /* STM32_CAPI_I2C_H_ */
