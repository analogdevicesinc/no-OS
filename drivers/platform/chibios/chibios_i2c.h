/***************************************************************************//**
 *   @file   chibios/chibios_i2c.h
 *   @brief  Header file for the chibios i2c driver.
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
#ifndef CHIBIOS_I2C_H_
#define CHIBIOS_I2C_H_

#include <stdint.h>
#include "no_os_i2c.h"
#include "hal.h"

#if (HAL_USE_I2C==TRUE)

/**
 * @struct chibios_i2c_desc
 * @brief chibios platform specific I2C descriptor iwth config and slave address parameters
 */
struct chibios_i2c_desc {
	/** I2C instance */
	I2CDriver *hi2c;
	/** I2C init specific parameter*/
	I2CConfig *i2ccfg;
	/**I2C address*/
	uint16_t i2caddr;
};

/**
 * @struct chibios_i2c_init_param
 * @brief ChibiOS i2c param struct
 *
 */
struct chibios_i2c_init_param {
	/** I2C instance*/
	I2CDriver *hi2c;
	/** I2C Config*/
	I2CConfig *i2ccfg;
};

/**
 * @brief chibios specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops chibios_i2c_ops;

/**
 * @brief global variable for temporary buffers for transmit and receive
 *
 */
static uint8_t* chI2CBuffer;
static uint16_t buffSize;

#endif // HAL_USE_I2C==TRUE

#endif // CHIBIOS_I2C_H_
