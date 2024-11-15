/*******************************************************************************
 *   @file   maxim_i2c.h
 *   @brief  Header containing types used in the i2c driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MAXIM_I2C_H_
#define MAXIM_I2C_H_

#include <stdint.h>
#include "i2c_regs.h"
#include "max78000.h"
#include "gpio.h"
#include "no_os_i2c.h"

#ifndef MXC_I2C_GET_I2C
#define MXC_I2C_GET_I2C(i)	((i) == 0 ? MXC_I2C0 :		\
                                (i) == 1 ? MXC_I2C1 :		\
                                (i) == 2 ? MXC_I2C2 : 0)
#endif

/**
 * @struct max_i2c_extra
 * @brief MAXIM specific I2C handler structure
 */
struct max_i2c_extra {
	/** Pointer to the abstracted register structure */
	mxc_i2c_regs_t *handler;
	/** Pointer to the write data in case of
	 *  write-repeated start-read operation */
	uint8_t *prologue_data;
	/** Size of the write data in case of
	 *  write-repeated start-read operation */
	uint8_t prologue_size;
};

struct max_i2c_init_param {
	mxc_gpio_vssel_t vssel;
};

/**
 * @enum max_i2c_speed
 * @brief MAXIM I2C SCL frequency options
 */
enum max_i2c_speed {
	MAX_I2C_STD_MODE = 100000,
	MAX_I2C_FAST_MODE = 400000,
	MAX_I2C_FAST_PLUS_MODE = 1000000,
	MAX_I2C_HS_MODE = 3400000
};

/**
 * @brief MAXIM specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops max_i2c_ops;

#endif /* MAXIM_I2C_H_ */
