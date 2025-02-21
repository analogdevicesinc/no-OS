/*******************************************************************************
 *   @file   xilinx/xilinx_i2c.h
 *   @brief  Header containing types used in the i2c driver.
 *   @author scuciurean (sergiu.cuciurean@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef XILINX_I2C_H_
#define XILINX_I2C_H_

#include <stdint.h>

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
