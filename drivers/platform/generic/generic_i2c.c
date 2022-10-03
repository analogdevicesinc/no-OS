/***************************************************************************//**
 *   @file   generic/generic_i2c.c
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
#include "no_os_i2c.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_i2c_init(struct no_os_i2c_desc **desc,
			 const struct no_os_i2c_init_param *param)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(param);

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_i2c_remove(struct no_os_i2c_desc *desc)
{
	NO_OS_UNUSED_PARAM(desc);

	return 0;
}

/**
 * @brief Write data to a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_i2c_write(struct no_os_i2c_desc *desc,
			  uint8_t *data,
			  uint8_t bytes_number,
			  uint8_t stop_bit)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(data);
	NO_OS_UNUSED_PARAM(bytes_number);
	NO_OS_UNUSED_PARAM(stop_bit);

	return 0;
}

/**
 * @brief Read data from a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that will store the received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_i2c_read(struct no_os_i2c_desc *desc,
			 uint8_t *data,
			 uint8_t bytes_number,
			 uint8_t stop_bit)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(data);
	NO_OS_UNUSED_PARAM(bytes_number);
	NO_OS_UNUSED_PARAM(stop_bit);

	return 0;
}

/**
 * @brief Generic platform I2C ops
 */
const struct no_os_i2c_platform_ops generic_i2c_ops = {
	.i2c_ops_init = &generic_i2c_init,
	.i2c_ops_write = &generic_i2c_write,
	.i2c_ops_read = &generic_i2c_read,
	.i2c_ops_remove = &generic_i2c_remove
};
