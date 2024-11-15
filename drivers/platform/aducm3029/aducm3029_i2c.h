/*******************************************************************************
 *   @file   aducm3029_i2c.h
 *   @brief  Header containing types used in the i2c driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ADUCM3029_I2C_H_
#define ADUCM3029_I2C_H_

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief ADuCM3029 specific IRQ platform ops structure
 */
extern const struct no_os_i2c_platform_ops aducm_i2c_ops;

#endif // ADUCM3029_I2C_H_
