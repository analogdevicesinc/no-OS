/***************************************************************************//**
 *   @file   altera/altera_i2c.h
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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
