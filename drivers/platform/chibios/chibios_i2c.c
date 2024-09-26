/***************************************************************************//**
 *   @file   chibios/chibios_i2c.c
 *   @brief  Implementation of chibios i2c driver.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_i2c.h"
#include "chibios_i2c.h"

#if (HAL_USE_I2C==TRUE)

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int chibios_i2c_init(struct no_os_i2c_desc **desc,
		     const struct no_os_i2c_init_param *param)
{
	int ret;
	struct no_os_i2c_desc *descriptor;
	struct chibios_i2c_desc *xdesc;

	if (!desc || !param)
		return -EINVAL;

	descriptor = (struct no_os_i2c_desc *)no_os_calloc(1,
			sizeof(struct no_os_i2c_desc));
	if (!descriptor)
		return -ENOMEM;

	xdesc = (struct chibios_i2c_desc *)no_os_calloc(1,
			sizeof(struct chibios_i2c_desc));
	if (!xdesc) {
		ret = -ENOMEM;
		goto error_1;
	}

	descriptor->extra = xdesc;
	descriptor->device_id = param->device_id;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->slave_address = param->slave_address;

	xdesc->hi2c = ((struct chibios_i2c_init_param *)(param->extra))->hi2c;
	xdesc->i2ccfg = ((struct chibios_i2c_init_param *)(param->extra))->i2ccfg;
	xdesc->i2caddr = param->slave_address;

	ret = i2cStart(xdesc->hi2c, xdesc->i2ccfg);
	if (ret != I2C_NO_ERROR) {
		ret = -EIO;
		goto error_2;
	}

	*desc = descriptor;

	return 0;
error_2:
	no_os_free(xdesc);
error_1:
	no_os_free(descriptor);
	return ret;
}

/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int chibios_i2c_remove(struct no_os_i2c_desc *desc)
{
	struct chibios_i2c_desc *sdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	sdesc = desc->extra;
	i2cStop(sdesc->hi2c);
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief I2C write transaction as master.
 * @param desc - The I2C descriptor.
 * @param data - The buffer with the data to transmit.
 * @param bytes_number - Number of bytes in the buffer.
 * @param stop_bit - Specifis whether to end the transaction with a stop bit.
 * @return 0 in case of success, -1 otherwise.
 */
int chibios_i2c_write(struct no_os_i2c_desc *desc,
		      uint8_t *data,
		      uint8_t bytes_number,
		      uint8_t stop_bit)
{
	int32_t ret;
	struct chibios_i2c_desc *xdesc;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	xdesc = desc->extra;

	if (!stop_bit) {
		chI2CBuffer = (uint8_t *)no_os_malloc(bytes_number*sizeof(uint8_t));
		memcpy(chI2CBuffer, data, bytes_number);
		buffSize = bytes_number;
		ret = I2C_NO_ERROR;
	} else {
		ret = i2cMasterTransmitTimeout(xdesc->hi2c,  desc->slave_address, data,
					       bytes_number, NULL, 0, TIME_INFINITE);
	}

	if (ret != I2C_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief I2C read transaction as master.
 * @param desc - The I2C descriptor.
 * @param data - The buffer where received data is to be stored.
 * @param bytes_number - Number of bytes to receive.
 * @param stop_bit - Specifis whether to end the transaction with a stop bit.
 * @return 0 in case of success, -1 otherwise.
 */

int chibios_i2c_read(struct no_os_i2c_desc *desc,
		     uint8_t *data,
		     uint8_t bytes_number,
		     uint8_t stop_bit)
{
	int32_t ret;
	struct chibios_i2c_desc *xdesc;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	xdesc = desc->extra;

	if (!stop_bit) {
//		no current implementation, (from no-os drivers only adxl372 uses consequtive reads without stop bit)
		ret = I2C_NO_ERROR;
	} else {
		ret = i2cMasterTransmitTimeout(xdesc->hi2c,  desc->slave_address, chI2CBuffer,
					       buffSize, data, bytes_number, TIME_INFINITE);
		no_os_free(chI2CBuffer);
		buffSize=0;
	}

	if (ret != I2C_NO_ERROR)
		return -EIO;

	return 0;
}

/**
 * @brief chibios platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops chibios_i2c_ops = {
	.i2c_ops_init = &chibios_i2c_init,
	.i2c_ops_write = &chibios_i2c_write,
	.i2c_ops_read = &chibios_i2c_read,
	.i2c_ops_remove = &chibios_i2c_remove
};

#endif // HAL_USE_I2C
