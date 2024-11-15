/***************************************************************************//**
 *   @file   altera/altera_i2c.h
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ALTERA_I2C_H_
#define ALTERA_I2C_H_

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum i2c_type
 * @brief Altera platform architecture types
 */
enum i2c_type {
	/** Nios II architecture */
	NIOS_II_I2C
} i2c_type;

/**
 * @struct altera_i2c_init_param
 * @brief Structure holding the initialization parameters for Altera
 * platform specific I2C parameters.
 */
struct altera_i2c_init_param {
	/** Altera architecture type */
	enum i2c_type	type;
	/** Device ID */
	uint32_t	id;
} altera_i2c_init_param;

/**
 * @struct altera_i2c_desc
 * @brief Altera platform specific I2C descriptor
 */
struct altera_i2c_desc {
	/** Altera architecture type */
	enum i2c_type	type;
	/** Device ID */
	uint32_t	id;
} altera_i2c_desc;

/**
 * @brief Altera platform specific i2c platform ops structure
 */
extern const struct no_os_i2c_platform_ops altera_i2c_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the I2C communication peripheral. */
int32_t altera_i2c_init(struct no_os_i2c_desc **desc,
			const struct no_os_i2c_init_param *param);

/* Free the resources allocated by no_os_i2c_init(). */
int32_t altera_i2c_remove(struct no_os_i2c_desc *desc);

/* I2C Write data */
int32_t altera_i2c_write(struct no_os_i2c_desc *desc, uint8_t *data,
			 uint8_t bytes_number, uint8_t stop_bit);

/* I2C Read data. */
int32_t altera_i2c_read(struct no_os_i2c_desc *desc, uint8_t *data,
			uint8_t bytes_number, uint8_t stop_bit);

#endif /* ALTERA_I2C_H_ */
