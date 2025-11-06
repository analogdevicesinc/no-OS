/*
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef I2C_BITBANG_H_
#define I2C_BITBANG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "capi_i2c.h"
#include "capi_gpio.h"

/* Default timeout for clock stretching (microseconds) */
#define I2C_BITBANG_TIMEOUT_US  100000  /* 100 ms */

/**
 * Pull-up configuration options
 */
enum i2c_bitbang_pull_type {
	/** External pull-ups present, use open-drain */
	I2C_BITBANG_PULL_EXTERNAL,
	/** Use internal pull-ups */
	I2C_BITBANG_PULL_INTERNAL,
	/** No pull-ups, drive high/low actively */
	I2C_BITBANG_PULL_NONE
};

/**
 * I2C bitbang extra configuration
 */
struct i2c_bitbang_extra {
	/** CAPI GPIO pin for SDA */
	struct capi_gpio_pin sda_pin;
	/** CAPI GPIO pin for SCL */
	struct capi_gpio_pin scl_pin;
	/** Pull-up configuration */
	enum i2c_bitbang_pull_type pull_type;
	/** Clock stretching timeout in microseconds (0 = use default) */
	uint32_t timeout_us;
};

/**
 * I2C bitbang controller handle (extends capi_i2c_controller_handle)
 */
struct i2c_bitbang_handle {
	/** Base I2C controller handle (must be first member) */
	struct capi_i2c_controller_handle base;
	/** SDA GPIO pin */
	struct capi_gpio_pin sda_pin;
	/** SCL GPIO pin */
	struct capi_gpio_pin scl_pin;
	/** Half period delay in microseconds */
	uint32_t half_period_us;
	/** Quarter period delay in microseconds */
	uint32_t quarter_period_us;
	/** Pull-up configuration */
	enum i2c_bitbang_pull_type pull_type;
	/** Clock stretching timeout */
	uint32_t timeout_us;
};

/**
 * I2C bitbang operations structure
 * This should be passed as the 'ops' field in capi_i2c_config
 */
extern const struct capi_i2c_ops i2c_bitbang_ops;

#ifdef __cplusplus
}
#endif

#endif /* I2C_BITBANG_H_ */
