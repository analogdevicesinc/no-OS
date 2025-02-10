/***************************************************************************//**
 *   @file   gmsl_reg_access.c
 *   @brief  Implementation of GMSL Register Access.
 *   @author Automotive Software and Systems team, Bangalore, India
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_util.h"
#include "no_os_i2c.h"
#include "gmsl_reg_access.h"
#include "gmsl_dbg.h"

/******************************************************************************/
/************************* Functions Definition *******************************/
/******************************************************************************/

/**
 * @brief Read a value from a GMSL register.
 *
 * @param i2c_desc - Pointer to the I2C descriptor.
 * @param reg - Register address to read from.
 * @param mask - Mask to apply to the read value.
 * @param readval - Pointer to store the read value.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t gmsl_reg_read(struct no_os_i2c_desc *i2c_desc,
			  uint16_t reg, uint8_t mask, uint8_t *readval)
{
	int32_t ret;
	uint8_t data[2];

	uint8_t rd_data;

	if (!i2c_desc || !readval)
		return -EINVAL;

	data[0] = (reg >> 8) & 0xff;
	data[1] = reg & 0xff;

	ret = no_os_i2c_write(i2c_desc, data, 2, 0);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	ret = no_os_i2c_read(i2c_desc, &rd_data, 1, 1);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	*readval =  ((rd_data & mask) >> no_os_find_first_set_bit(mask));
    
	GMSL_LOG_DEBUG("GMSL read - addr:0x%x reg:0x%x reg_val:0x%x mask:0x%x field_val=0x%x", i2c_desc->slave_address, reg, rd_data, mask, (uint8_t)*readval);

	return 0;
}

/**
 * @brief Write a value to a GMSL register.
 *
 * @param i2c_desc - Pointer to the I2C descriptor.
 * @param reg - Register address to write to.
 * @param writeval - Value to write to the register.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t gmsl_reg_write(struct no_os_i2c_desc *i2c_desc,
			   uint16_t reg,
			   uint8_t writeval)
{
	uint8_t data[3];
    
	if (!i2c_desc)
		return -EINVAL;

	data[0] = (reg >> 8) & 0xff;
	data[1] = reg & 0xff;
	data[2] = writeval & 0xff;

	GMSL_LOG_DEBUG("GMSL write - addr:0x%x reg:0x%x write_data=0x%x", i2c_desc->slave_address, reg, writeval);

	return no_os_i2c_write(i2c_desc, data, 3, 1);
}

/**
 * @brief Update a value in a GMSL register.
 *
 * @param i2c_desc - Pointer to the I2C descriptor.
 * @param reg - Register address to update.
 * @param val - Value to write to the register.
 * @param mask - Mask to apply to the value.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t gmsl_reg_update(struct no_os_i2c_desc *i2c_desc,
				uint16_t reg,
				uint8_t val,
				uint8_t mask)
{
	int32_t ret;
	uint8_t regval;

	if (!i2c_desc)
		return -EINVAL;

	ret = gmsl_reg_read(i2c_desc, reg, 0xFF, &regval);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

/** @todo Revert this after removal of explicit mask being used in mission mode api's */
#ifndef REGACCESS_GM_FLEX
	regval &= ~no_os_field_prep(mask, 0xffff);
	regval |= no_os_field_prep(mask, val);
#else
	regval &= ~mask;
    	regval |= val;
#endif

	GMSL_LOG_DEBUG("GMSL update - addr:0x%x reg:0x%x mask:0x%x write_data=0x%x", i2c_desc->slave_address, reg, mask, regval);

	return gmsl_reg_write(i2c_desc, reg, regval);
}
