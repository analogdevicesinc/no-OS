/***************************************************************************//**
 *   @file   pico/pico_i2c.h
 *   @brief  Header file for the pico i2c driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef _PICO_I2C_H_
#define _PICO_I2C_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_i2c.h"
#include "hardware/i2c.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct pico_i2c_desc
 * @brief pico platform specific I2C descriptor
 */
struct pico_i2c_desc {
	/** I2C instance */
	i2c_inst_t *i2c_instance;
};

/**
 * @brief Available GP config for I2c SDA
 */
enum i2c_sda_gp {
	/* I2C0 available configurations */
	I2C0_SDA_GP0 = 0,
	I2C0_SDA_GP4 = 4,
	I2C0_SDA_GP8 = 8,
	I2C0_SDA_GP12 = 12,
	I2C0_SDA_GP16 = 16,
	I2C0_SDA_GP20 = 20,
	/* I2C1 available configurations */
	I2C1_SDA_GP2 = 2,
	I2C1_SDA_GP6 = 6,
	I2C1_SDA_GP10 = 10,
	I2C1_SDA_GP14 = 14,
	I2C1_SDA_GP18 = 18,
	I2C1_SDA_GP26 = 26
};

/**
 * @brief Available GP config for I2C SCL
 */
enum i2c_scl_gp {
	/* I2C0 available configurations */
	I2C0_SCL_GP1 = 1,
	I2C0_SCL_GP5 = 5,
	I2C0_SCL_GP9 = 9,
	I2C0_SCL_GP13 = 13,
	I2C0_SCL_GP17 = 17,
	I2C0_SCL_GP21 = 21,
	/* I2C1 available configurations */
	I2C1_SCL_GP3 = 3,
	I2C1_SCL_GP7 = 7,
	I2C1_SCL_GP11 = 11,
	I2C1_SCL_GP15 = 15,
	I2C1_SCL_GP19 = 19,
	I2C1_SCL_GP27 = 27
};

/**
 * @brief Additional I2C config parameters
 */
struct pico_i2c_init_param {
	/** I2C SDA pin configuration */
	enum i2c_sda_gp i2c_sda_pin;
	/** I2C SCL pin configuration */
	enum i2c_scl_gp i2c_scl_pin;
};

/**
 * @brief pico specific I2C platform ops structure
 */
extern const struct no_os_i2c_platform_ops pico_i2c_ops;

#endif
