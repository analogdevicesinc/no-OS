/***************************************************************************//**
 *   @file   no_os_i2c.c
 *   @brief  Implementation of the I2C Interface
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include <inttypes.h>
#include "no_os_i2c.h"
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_mutex.h"
#include "no_os_alloc.h"

/**
 * @brief i2c_table contains the pointers towards the i2c buses
*/
static void *i2c_table[I2C_MAX_BUS_NUMBER + 1];

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_i2c_init(struct no_os_i2c_desc **desc,
		       const struct no_os_i2c_init_param *param)
{
	int32_t ret;

	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->i2c_ops_init)
		return -ENOSYS;
	if ( param->device_id > I2C_MAX_BUS_NUMBER)
		return -EINVAL;
	// Initializing BUS descriptor
	if (i2c_table[param->device_id] == NULL) {
		ret = no_os_i2cbus_init(param);
		if (ret)
			return ret;
	}
	// Initilize I2C descriptor
	ret = param->platform_ops->i2c_ops_init(desc, param);
	if (ret)
		return ret;
	(*desc)->bus = i2c_table[param->device_id];
	(*desc)->bus->slave_number++;
	(*desc)->platform_ops = param->platform_ops;

	return 0;
}

/**
 * @brief Initialize the i2c bus communication peripheral.
 * @param param - The structure that containes the i2c bus parameters
 * @return 0 in case of success, error code otherwise
*/
int32_t no_os_i2cbus_init(const struct no_os_i2c_init_param *param)
{
	struct no_os_i2cbus_desc *bus = (struct no_os_i2cbus_desc *)no_os_calloc(1,
					sizeof(struct no_os_i2cbus_desc));

	if (!bus)
		return -ENOMEM;

	no_os_mutex_init(&(bus->mutex));

	bus->slave_number = 0;
	bus->device_id = param->device_id;
	bus->max_speed_hz = param->max_speed_hz;
	bus->platform_ops = param->platform_ops;
	bus->extra = param->extra;

	i2c_table[param->device_id] = bus;

	return 0;
}


/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_i2c_remove(struct no_os_i2c_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (desc->bus)
		no_os_i2cbus_remove(desc->bus->device_id);

	if (!desc->platform_ops->i2c_ops_remove)
		return -ENOSYS;

	return desc->platform_ops->i2c_ops_remove(desc);
}

/**
 * @brief Removes i2c bus instance
 * @param bus_number - i2c bus number
*/
void no_os_i2cbus_remove(uint32_t bus_number)
{
	struct no_os_i2cbus_desc *bus = (struct no_os_i2cbus_desc *)
					i2c_table[bus_number];

	if (bus->slave_number > 0)
		bus->slave_number--;

	if (bus->slave_number == 0) {
		no_os_mutex_remove(bus->mutex);

		if (bus != NULL) {
			no_os_free(bus);
			i2c_table[bus_number] = NULL;
		}
	}
}

/**
 * @brief I2C Write data to slave device.
 * @param desc - The I2C descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop conditional control.
 *                   Example: 0 - A stop condition will not be generated.
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_i2c_write(struct no_os_i2c_desc *desc,
			uint8_t *data,
			uint8_t bytes_number,
			uint8_t stop_bit)
{
	int32_t ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i2c_ops_write)
		return -ENOSYS;

	no_os_mutex_lock(desc->bus->mutex);
	ret = desc->platform_ops->i2c_ops_write(desc, data, bytes_number,
						stop_bit);
	no_os_mutex_unlock(desc->bus->mutex);

	return ret;
}

/**
 * @brief I2C Read data from slave device.
 * @param desc - The i2c descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Stop conditional control.
 *                   Example: 0 - A stop condition will not be generated.
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_i2c_read(struct no_os_i2c_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number,
		       uint8_t stop_bit)
{
	int32_t ret;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->i2c_ops_read)
		return -ENOSYS;

	no_os_mutex_lock(desc->bus->mutex);
	ret = desc->platform_ops->i2c_ops_read(desc, data, bytes_number,
					       stop_bit);
	no_os_mutex_unlock(desc->bus->mutex);

	return ret;
}
