/*******************************************************************************
 *   @file   maxim_capi_i2c.h
 *   @brief  Header file for I2C functions with CAPI
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_I2C_H_
#define MAXIM_CAPI_I2C_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "maxim_capi_gpio.h"
#include "capi_i2c.h"

/**
 * @enum max_capi_i2c_speed
 * @brief MAX32657 I2C speed modes
 */
enum max_capi_i2c_speed {
	/** 100 kHz */
	MAX_CAPI_I2C_SPEED_STANDARD =	 100000,
	/** 400 kHz */
	MAX_CAPI_I2C_SPEED_FAST = 	 400000,
	/** 1 MHz */
	MAX_CAPI_I2C_SPEED_FAST_PLUS =	1000000,
};

/**
 * @struct max_capi_i2c_extra
 * @brief MAX32657 platform-specific extra configuration
 */
struct max_capi_i2c_extra {
	/** Voltage level of the I3C peripheral */
	enum max_capi_gpio_vssel vssel;
	/** OPTIONAL - DMA config parameters */
	struct capi_dma_config *dma_config;
};

extern const struct capi_i2c_ops max_capi_i2c_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_I2C_H_ */
