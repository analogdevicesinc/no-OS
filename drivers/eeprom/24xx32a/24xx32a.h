/***************************************************************************//**
 *   @file   24xx32a.h
 *   @brief  Header file of 24AA32A/24LC32A Interface
 *   @author Mahesh Phalke (mahesh.phalke@analog.com)
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

#ifndef _24XX32A_H_
#define _24XX32A_H_

#include <stdint.h>
#include "no_os_i2c.h"

/**
* @struct eeprom_24xx32a_init_param
* @brief 24XX32A EEPROM init params structure
*/
struct eeprom_24xx32a_init_param {
	/** I2C initialization parameters */
	struct no_os_i2c_init_param *i2c_init;
};

/**
* @struct eeprom_24xx32a_dev
* @brief 24XX32A EEPROM device structure
*/
struct eeprom_24xx32a_dev {
	/** I2C descriptor*/
	struct no_os_i2c_desc *i2c_desc;
};

/**
 * @brief 24XX32A EEPROM specific platform ops structure
 */
extern const struct no_os_eeprom_platform_ops eeprom_24xx32a_ops;

#endif	/* end of _24XX32A_H_ */
