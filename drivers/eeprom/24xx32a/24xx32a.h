/***************************************************************************//**
 *   @file   24xx32a.h
 *   @brief  Header file of 24AA32A/24LC32A Interface
 *   @author Mahesh Phalke (mahesh.phalke@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _24XX32A_H_
#define _24XX32A_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_i2c.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

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
