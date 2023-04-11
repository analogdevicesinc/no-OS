/***************************************************************************//**
 *   @file   max31875.c
 *   @brief  Implementation of MAX31875 Driver.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_alloc.h"
#include "max31875.h"

/******************************************************************************/
/************************* Functions Definition *******************************/
/******************************************************************************/

/**
 * @brief Read a device register through the I2C interface.
 * @param [in] dev - Driver handler pointer.
 * @param [in] reg - Address of the register to be read.
 * @param [out] readval - Pointer to the register value.
 * @return 0 in case of success, error code otherwise.
 */
int32_t max31875_reg_read(struct max31875_dev *dev,
			  uint32_t reg,
			  uint32_t *readval)
{
	int32_t ret;
	uint8_t data[2];

	if (!dev || !readval)
		return -EINVAL;

	data[0] = reg;

	ret = no_os_i2c_write(dev->i2c_desc, data, 1, 0);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, data, 2, 1);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	*readval = (data[0] << 8) | data[1];

	return 0;
}

/**
 * @brief Write a device register through the I2C interface.
 * @param [in] dev - Driver handler pointer.
 * @param [in] reg - Address of the register to be written.
 * @param [in] writeval - New value for the register.
 * @return 0 in case of success, error code otherwise.
 */
int32_t max31875_reg_write(struct max31875_dev *dev,
			   uint32_t reg,
			   uint32_t writeval)
{
	uint8_t data[3];

	if (!dev)
		return -EINVAL;

	data[0] = reg;
	data[1] = (writeval >> 8) & 0xff;
	data[2] = writeval & 0xff;

	return no_os_i2c_write(dev->i2c_desc, data, 3, 1);
}

/**
 * @brief Read-modify-write operation on a field of a register.
 * @param [in] dev - Driver handler pointer.
 * @param [in] reg - Address of the register.
 * @param [in] val - New value for the bit field.
 * @param [in] mask - Mask of the bit field.
 * @return 0 in case of success, error code otherwise.
 */
int32_t max31875_reg_write_mask(struct max31875_dev *dev,
				uint32_t reg,
				uint32_t val,
				uint32_t mask)
{
	int32_t ret;
	uint32_t regval;

	if (!dev)
		return -EINVAL;

	ret = max31875_reg_read(dev, reg, &regval);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	regval &= ~no_os_field_prep(mask, 0xffff);
	regval |= no_os_field_prep(mask, val);

	return max31875_reg_write(dev, reg, regval);
}

/**
 * @brief Initialize the device driver befre use.
 * @param [out] device - Driver handler pointer.
 * @param [in] init_param - Pointer to the initialization structure.
 * @return 0 in case of success, error code otherwise.
 */
int32_t max31875_init(struct max31875_dev **device,
		      struct max31875_init_param *init_param)
{

	int32_t ret;
	struct max31875_dev *dev;

	if (!device || !init_param)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto error;

	*device = dev;

	return 0;
error:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free memory allocated by max31875_init().
 * @param [in] dev - Device driver handler.
 * @return 0 in case of success, error code otherwise.
 */
int32_t max31875_remove(struct max31875_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	no_os_free(dev);

	return 0;
}

