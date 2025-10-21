/***************************************************************************//**
 *   @file   bitbang_i2c.h
 *   @brief  Header file for bitbang I2C driver
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#ifndef BITBANG_I2C_H_
#define BITBANG_I2C_H_

#include "no_os_i2c.h"
#include "no_os_gpio.h"

/**
 * @struct bitbang_i2c_init_param
 * @brief Structure holding the parameters for bitbang I2C initialization.
 */
struct bitbang_i2c_init_param {
	/** GPIO descriptor for SDA line */
	struct no_os_gpio_desc *sda_gpio;
	/** GPIO descriptor for SCL line */
	struct no_os_gpio_desc *scl_gpio;
};

/**
 * @struct bitbang_i2c_desc
 * @brief Structure holding bitbang I2C descriptor.
 */
struct bitbang_i2c_desc {
	/** GPIO descriptor for SDA line */
	struct no_os_gpio_desc *sda_gpio;
	/** GPIO descriptor for SCL line */
	struct no_os_gpio_desc *scl_gpio;
	/** I2C frequency in Hz */
	uint32_t frequency;
};

/* Bitbang I2C platform operations */
extern const struct no_os_i2c_platform_ops bitbang_i2c_ops;

#endif /* BITBANG_I2C_H_ */