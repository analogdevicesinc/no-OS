/*******************************************************************************
 *   @file   xilinx/xilinx_i2c.h
 *   @brief  Header containing types used in the i2c driver.
 *   @author scuciurean (sergiu.cuciurean@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef XILINX_I2C_H_
#define XILINX_I2C_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum xil_i2c_type
 * @brief Xilinx platform architecture sections
 */
enum xil_i2c_type {
	/** Programmable Logic */
	IIC_PL,
	/** Processing System */
	IIC_PS
};

/**
 * @struct xil_i2c_init
 * @brief Structure holding the initialization parameters for Xilinx platform
 * specific I2C parameters.
 */
struct xil_i2c_init_param {
	/** Xilinx architecture */
	enum xil_i2c_type	type;
	/** Device ID */
	uint32_t		device_id;
};

/**
 * @struct xil_i2c_desc
 * @brief Xilinx platform specific I2C descriptor
 */
struct xil_i2c_desc {
	/** Xilinx architecture */
	enum xil_i2c_type	type;
	/** Device ID */
	uint32_t		device_id;
	/** Xilinx I2C configuration */
	void			*config;
	/** Xilinx I2C Instance */
	void			*instance;
};

/**
 * @brief Xilinx platform specific i2c platform ops structure
 */
extern const struct no_os_i2c_platform_ops xil_i2c_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the I2C communication peripheral. */
int32_t xil_i2c_init(struct no_os_i2c_desc **desc,
		     const struct no_os_i2c_init_param *param);

/* Free the resources allocated by no_os_i2c_init(). */
int32_t xil_i2c_remove(struct no_os_i2c_desc *desc);

/* I2C Write data */
int32_t xil_i2c_write(struct no_os_i2c_desc *desc, uint8_t *data,
		      uint8_t bytes_number, uint8_t stop_bit);

/* I2C Read data. */
int32_t xil_i2c_read(struct no_os_i2c_desc *desc, uint8_t *data,
		     uint8_t bytes_number, uint8_t stop_bit);

#endif // XILINX_I2C_H_
