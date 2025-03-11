/*******************************************************************************
 *   @file   zephyr/zephyr_i2c.c
 *   @brief  Implementation of zephyr i2c driver.
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include "zephyr_i2c.h"

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int zephyr_i2c_init(struct no_os_i2c_desc **desc,
		    const struct no_os_i2c_init_param *param)
{
	struct zephyr_i2c_desc *xdevice;

	if (NO_OS_DRIVER_DYNAMIC_ALLOC)
	{
		(*desc) = (struct no_os_i2c_desc *)no_os_malloc(sizeof(*(*desc)));
		if (!(*desc))
		{
			return -ENOMEM;
		}
		xdevice = no_os_calloc(1, sizeof(*xdevice));
		if (!xdevice)
		{
			no_os_free(*desc);
			return -ENOMEM;
		}
	}
	else
	{
		if (!(*desc))
			return -ENOMEM;
		if (!((*desc)->extra))
			return -ENOMEM;
		xdevice = (struct zephyr_i2c_desc *)((*desc)->extra);
	}
	xdevice->i2c_desc = ((struct zephyr_i2c_init_param *)(param->extra))->i2c_desc;
	(*desc)->extra = xdevice;
	(*desc)->platform_ops = &zephyr_i2c_ops;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int zephyr_i2c_remove(struct no_os_i2c_desc *desc)
{
	if (NO_OS_DRIVER_DYNAMIC_ALLOC)
	{
		if (!desc)
			return -EINVAL;
		if (!desc->extra)
			return -EINVAL;
		no_os_free(desc->extra);
		no_os_free(desc);
	}

	return 0;
}

/**
 * @brief I2C write transaction as master.
 * @param desc - The I2C descriptor.
 * @param data - The buffer with the data to transmit.
 * @param bytes_number - Number of bytes in the buffer.
 * @param stop_bit - Specifis whether to end the transaction with a stop bit.
 * @return 0 in case of success, Zephyr i2c error code otherwise.
 */
int zephyr_i2c_write(struct no_os_i2c_desc *desc,
		     uint8_t *data,
		     uint8_t bytes_number,
		     uint8_t stop_bit)
{
	struct i2c_msg msg;
	struct zephyr_i2c_desc xdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	xdesc = *((struct zephyr_i2c_desc *)(desc->extra));

	msg.buf = (uint8_t *)data;
	msg.len = bytes_number;
	msg.flags = I2C_MSG_WRITE | (stop_bit ? I2C_MSG_STOP : 0);

	return i2c_transfer(xdesc.i2c_desc->bus, &msg, 1, xdesc.i2c_desc->addr);
}

/**
 * @brief I2C read transaction as master.
 * @param desc - The I2C descriptor.
 * @param data - The buffer where received data is to be stored.
 * @param bytes_number - Number of bytes to receive.
 * @param stop_bit - Specifis whether to end the transaction with a stop bit.
 * @return 0 in case of success, Zephyr i2c error code otherwise.
 */

int zephyr_i2c_read(struct no_os_i2c_desc *desc,
		    uint8_t *data,
		    uint8_t bytes_number,
		    uint8_t stop_bit)
{
	struct i2c_msg msg;
	struct zephyr_i2c_desc xdesc;

	if (!desc || !desc->extra)
		return -EINVAL;

	xdesc = *((struct zephyr_i2c_desc *)(desc->extra));

	msg.buf = (uint8_t *)data;
	msg.len = bytes_number;
	msg.flags = I2C_MSG_READ | (stop_bit ? I2C_MSG_STOP : 0);

	return i2c_transfer(xdesc.i2c_desc->bus, &msg, 1, xdesc.i2c_desc->addr);
}

/**
 * @brief zephyr platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops zephyr_i2c_ops = {
    .i2c_ops_init = &zephyr_i2c_init,
    .i2c_ops_write = &zephyr_i2c_write,
    .i2c_ops_read = &zephyr_i2c_read,
    .i2c_ops_remove = &zephyr_i2c_remove};
