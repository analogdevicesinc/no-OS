/*******************************************************************************
 *   @file   ftd2xx_i2c.h
 *   @brief  Implementation of I2C driver for ftd2xx platform.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#ifndef FTD2XX_I2C_H_
#define FTD2XX_I2C_H_

#include <stdbool.h>

#include "no_os_i2c.h"
#include "mpsse.h"
#include "ftd2xx.h"

/**
 * @struct ftd2xx_i2c_init
 * @brief Structure holding the initialization parameters for ftd2xx platform
 * specific I2C parameters.
 */
struct ftd2xx_i2c_init {
	/** Selected device specific PIN configuration.
	 *  BIT7-BIT0: Initial direction of the pins
	 *  BIT15-BIT8: Initial values of the pins
	 *  BIT23-BIT16: Final direction of the pins
	 *  BIT31-BIT24: Final values of the pins
	 */
	uint32_t channel_config_pin;
	/** Enable specific PIN configuration if set to true. */
	bool channel_config;
	/** Enable extra GPIO pin configuration at I2C init. */
	bool extra_pins;
	/** Extra pin values, bit 0 for low, bit 1 for high. */
	uint8_t extra_pins_val;
	/** Extra pin direction, bit 0 for in, bit 1 for out. */
	uint8_t extra_pins_dir;
};

/**
 * @struct ftd2xx_i2c_desc
 * @brief ftd2xx platform specific I2C descriptor
 */
struct ftd2xx_i2c_desc {
	/** Specific device handle */
	FT_HANDLE ftHandle;
};

/**
 * @brief ftd2xx platform specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops ftd2xx_i2c_ops;

#endif
