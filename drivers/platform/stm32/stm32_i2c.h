/***************************************************************************//**
 *   @file   stm32/stm32_i2c.h
 *   @brief  Header file for the stm32 i2c driver.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#ifndef STM32_I2C_H_
#define STM32_I2C_H_

#include <stdint.h>
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "stm32_hal.h"

/**
 * @struct stm32_i2c_desc
 * @brief stm32 platform specific I2C descriptor
 */
struct stm32_i2c_desc {
	/** I2C instance */
	I2C_HandleTypeDef hi2c;
};

/**
 * @struct stm32_i2c_init_param
 * @brief Structure holding the initialization parameters for stm32 platform
 * specific I2C parameters.
 */
struct stm32_i2c_init_param {
	/** I2C Timing */
	uint32_t i2c_timing;
	/* SCL GPIO init params */
	struct no_os_gpio_init_param *scl;
	/* SDA GPIO init params */
	struct no_os_gpio_init_param *sda;
};

/**
 * @brief stm32 specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops stm32_i2c_ops;

#endif // STM32_I2C_H_
