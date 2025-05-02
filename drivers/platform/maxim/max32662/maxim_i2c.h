/*******************************************************************************
 *   @file   maxim_i2c.h
 *   @brief  Header containing types used in the i2c driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#ifndef MAXIM_I2C_H_
#define MAXIM_I2C_H_

#include <stdint.h>
#include "i2c_regs.h"
#include "max32662.h"
#include "gpio.h"
#include "no_os_i2c.h"

/**
 * @struct max_i2c_extra
 * @brief MAXIM specific I2C handler structure
 */
struct max_i2c_extra {
	/** Pointer to the abstracted register structure */
	mxc_i2c_regs_t *handler;
	/** Pointer to the write data in case of
	 *  write-repeated start-read operation */
	uint8_t *prologue_data;
	/** Size of the write data in case of
	 *  write-repeated start-read operation */
	uint8_t prologue_size;
};

struct max_i2c_init_param {
	mxc_gpio_vssel_t vssel;
};

/**
 * @enum max_i2c_speed
 * @brief MAXIM I2C SCL frequency options
 */
enum max_i2c_speed {
	MAX_I2C_STD_MODE = 100000,
	MAX_I2C_FAST_MODE = 400000,
	MAX_I2C_FAST_PLUS_MODE = 1000000,
	MAX_I2C_HS_MODE = 3400000
};

/**
 * @brief MAXIM specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops max_i2c_ops;

#endif /* MAXIM_I2C_H_ */
