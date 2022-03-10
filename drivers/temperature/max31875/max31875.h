/***************************************************************************//**
 *   @file   max31875.h
 *   @brief  Header file of MAX31875 Driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
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
#ifndef __MAX31875_H__
#define __MAX31875_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_i2c.h"
#include "no_os_util.h"

/******************************************************************************/
/************************** MAX31875 Definitions ******************************/
/******************************************************************************/

/** Register addresses */
#define MAX31875_TEMPERATURE_REG	0x0
#define MAX31875_CONFIGURATION_REG	0x1
#define MAX31875_THYST_REG		0x2
#define MAX31875_TOS_REG		0x3

/** MAX31875_TEMPERATURE_REG */
#define MAX31875_OVER_TEMP_STATUS_MASK	NO_OS_BIT(15)
#define MAX31875_FAULT_QUEUE_MASK	NO_OS_GENMASK(12, 11)
#define MAX31875_COMP_INT_MASK		NO_OS_BIT(9)
#define MAX31875_SHUTDOWN_MASK		NO_OS_BIT(8)
#define MAX31875_DATA_FORMAT_MASK	NO_OS_BIT(7)
#define MAX31875_RESOLUTION_MASK	NO_OS_GENMASK(6, 5)
#define MAX31875_TIMEOUT_MASK		NO_OS_BIT(4)
#define MAX31875_PEC_MASK		NO_OS_BIT(3)
#define MAX31875_CONVERSION_RATE_MASK	NO_OS_GENMASK(2, 1)
#define MAX31875_ONESHOT_MASK		NO_OS_BIT(0)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum max31875_i2c_addr
 * @brief Defines the potential device I2C addresses
 */
enum max31875_i2c_addr {
	MAX31875R0 = 0x48,
	MAX31875R1 = 0x49,
	MAX31875R2 = 0x4A,
	MAX31875R3 = 0x4B,
	MAX31875R4 = 0x4C,
	MAX31875R5 = 0x4D,
	MAX31875R6 = 0x4E,
	MAX31875R7 = 0x4F
};

/**
 * @struct max31875_dev
 * @brief MAX31875 handler structure
 */
struct max31875_dev {
	/* I2C */
	struct no_os_i2c_desc *i2c_desc;
};

/**
 * @struct max31875_init_param
 * @brief MAX31875 driver initailization structure
 */
struct max31875_init_param {
	/* I2C */
	struct no_os_i2c_init_param i2c_init;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Read a device register through the I2C interface. */
int32_t max31875_reg_read(struct max31875_dev *dev,
			  uint32_t reg,
			  uint32_t *readval);

/** Write a device register through the I2C interface. */
int32_t max31875_reg_write(struct max31875_dev *dev,
			   uint32_t reg,
			   uint32_t writeval);

/** Read-modify-write operation on a field of a register. */
int32_t max31875_reg_write_mask(struct max31875_dev *dev,
				uint32_t reg,
				uint32_t val,
				uint32_t mask);

/** Initialize the device driver befre use. */
int32_t max31875_init(struct max31875_dev **device,
		      struct max31875_init_param *init_param);

/** Free memory allocated by max31875_init(). */
int32_t max31875_remove(struct max31875_dev *dev);

#endif	/* __MAX31875_H__ */
