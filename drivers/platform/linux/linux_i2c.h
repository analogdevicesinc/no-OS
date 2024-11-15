/*******************************************************************************
 *   @file   linux/linux_i2c.h
 *   @brief  Header containing extra types used by the I2C driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef LINUX_I2C_H_
#define LINUX_I2C_H_

/**
 * @struct linux_i2c_init_param
 * @brief Structure holding the initialization parameters for Linux platform
 * specific I2C parameters.
 */
struct linux_i2c_init_param {
	/** I2C bus ID (/dev/i2c-"device_id") */
	uint32_t device_id;
};

/**
 * @brief Linux specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops linux_i2c_ops;

#endif // LINUX_I2C_H_
