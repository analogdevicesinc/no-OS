/***************************************************************************//**
 *   @file   zephyr/zephyr_i2c.h
 *   @brief  Header file for the zephyr i2c driver.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef ZEPHYR_I2C_H_
#define ZEPHYR_I2C_H_

#include <stdint.h>
#include "no_os_i2c.h"
#include <zephyr/drivers/i2c.h>

/**
 * @struct zephyr_i2c_desc
 * @brief zephyr platform specific I2C descriptor iwth config and slave address parameters
 */
struct zephyr_i2c_desc {
	/** I2C instance */
	struct i2c_dt_spec *i2c_desc;
};

/**
 * @struct zephyr_i2c_init_param
 * @brief Zephyr i2c param struct
 *
 */
struct zephyr_i2c_init_param {
	/** I2C instance*/
	struct i2c_dt_spec *i2c_desc;
	/** I2C devicetree specification */
};

/**
 * @brief zephyr specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops zephyr_i2c_ops;

#endif // ZEPHYR_I2C_H_
