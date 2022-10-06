/***************************************************************************//**
 *   @file   mbed_i2c.cpp
 *   @brief  Implementation of I2C Mbed platform driver interfaces
********************************************************************************
 * Copyright (c) 2019-2022 Analog Devices, Inc.
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

#include <stdio.h>
#include <mbed.h>

// Platform drivers needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif //  _cplusplus

#include "no_os_error.h"
#include "no_os_i2c.h"
#include "mbed_i2c.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc[in,out] - The I2C descriptor.
 * @param param[in] - The structure that contains the I2C parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_i2c_init(struct no_os_i2c_desc **desc,
		      const struct no_os_i2c_init_param *param)
{
	mbed::I2C *i2c;		// pointer to new I2C instance
	struct mbed_i2c_desc *mbed_i2c_desc;	// pointer to mbed i2c desc
	struct no_os_i2c_desc *i2c_desc;

	if (!desc || !param)
		return -EINVAL;

	/* Create an i2c descriptor object for the device */
	i2c_desc = (struct no_os_i2c_desc *)calloc(1, sizeof(*i2c_desc));
	if (!i2c_desc)
		return -ENOMEM;

	// Address passed in parameter shifted left by 1 to form
	// 7-bit i2c slave address (7 MSBs) and the LSB acts as
	// r/w bit during i2c read/write operations
	i2c_desc->slave_address = ((param->slave_address) << 1);

	/* Configure and instantiate I2C protocol */
	i2c = new I2C(
		(PinName)(((struct mbed_i2c_init_param *)param->extra)->i2c_sda_pin),
		(PinName)(((struct mbed_i2c_init_param *)param->extra)->i2c_scl_pin));
	if (!i2c)
		goto err_i2c;

	/* Create the i2c mbed descriptor object to store new i2c instance */
	mbed_i2c_desc = (struct mbed_i2c_desc *)calloc(1, sizeof(*mbed_i2c_desc));
	if (!mbed_i2c_desc)
		goto err_mbed_i2c_desc;

	i2c->frequency(param->max_speed_hz);

	mbed_i2c_desc->i2c_port = i2c;
	i2c_desc->extra = mbed_i2c_desc;

	*desc = i2c_desc;

	return 0;

err_mbed_i2c_desc:
	free(i2c);
err_i2c:
	free(i2c_desc);

	return -ENOMEM;
}

/**
 * @brief Free the resources allocated by mbed_i2c_init().
 * @param desc[in] - The I2C descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_i2c_remove(struct no_os_i2c_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	/* Free the I2C port object */
	if ((I2C *)(((mbed_i2c_desc *)(desc->extra))->i2c_port))
		delete((I2C *)(((mbed_i2c_desc *)(desc->extra))->i2c_port));

	/* Free the I2C descriptor objects */
	free(desc->extra);
	free(desc);

	return 0;
}

/**
 * @brief Write data to a slave device.
 * @param desc[in] - The I2C descriptor.
 * @param data[in] - Buffer that stores the transmission data.
 * @param bytes_number[in] - Number of bytes to write.
 * @param stop_bit[in] - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_i2c_write(struct no_os_i2c_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number,
		       uint8_t stop_bit)
{
	mbed::I2C *i2c;

	if (!desc  || !desc->extra || !data)
		return -EINVAL;

	i2c = (I2C *)(((mbed_i2c_desc *)(desc->extra))->i2c_port);

	/**
	    The MBED I2C API is reversed for parameter 4
	    Instead of stop_bit - it has
	    @param repeated   - Repeated start, true - don't send stop at end default value is false.
	    Inverting here to keep the no-OS/platform_drivers API
	 */
	if (i2c->write(desc->slave_address, (char *)data, bytes_number, !stop_bit))
		return -EINVAL;

	return 0;
}

/**
 * @brief Read data from a slave device.
 * @param desc[in] - The I2C descriptor.
 * @param data[in,out] - Buffer that will store the received data.
 * @param bytes_number[in] - Number of bytes to read.
 * @param stop_bit[in] - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t mbed_i2c_read(struct no_os_i2c_desc *desc,
		      uint8_t *data,
		      uint8_t bytes_number,
		      uint8_t stop_bit)
{
	mbed::I2C *i2c;

	if (!desc  || !desc->extra || !data)
		return -EINVAL;

	i2c = (I2C *)(((mbed_i2c_desc *)(desc->extra))->i2c_port);

	/**
	    The MBED I2C API is reversed for parameter 4
	    Instead of stop_bit - it has
	    @param repeated   - Repeated start, true - don't send stop at end default value is false.
	    Inverting here to keep the no-OS/platform_drivers API
	 */
	if (i2c->read(desc->slave_address, (char *)data, bytes_number, !stop_bit))
		return -EINVAL;

	return 0;
}

/**
* @brief Mbed platform specific I2C platform ops structure
*/
const struct no_os_i2c_platform_ops mbed_i2c_ops = {
	.i2c_ops_init = &mbed_i2c_init,
	.i2c_ops_write = &mbed_i2c_write,
	.i2c_ops_read = &mbed_i2c_read,
	.i2c_ops_remove = &mbed_i2c_remove
};

#ifdef __cplusplus
}
#endif //  _cplusplus
