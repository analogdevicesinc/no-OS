/***************************************************************************//**
 *   @file   linux/linux_i2c.c
 *   @brief  Implementation of Linux platform I2C Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
 *   @author Rene Arthur Necesito (Renearthur.Necesito@analog.com)
********************************************************************************
 * Copyright 2020, 2025(c) Analog Devices, Inc.
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

#include "no_os_error.h"
#include "no_os_i2c.h"
#include "no_os_alloc.h"
#include "linux_i2c.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

/**
 * @struct linux_i2c_desc
 * @brief Linux platform specific I2C descriptor
 */
struct linux_i2c_desc {
	/** /dev/i2c-"device_id" file descriptor */
	int fd;
	/** struct for write - repeated start - read operations */
	struct i2c_msg *messages;
	/** count of i2c messages in array */
	int len_messages;
};

/**
 * @brief Add I2C messages for a combined I2C R/W transaction.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that stores the received/transmission data.
 * @param bytes_number - Number of bytes to read/write.
 * @param read - 0 to write, otherwise read.
 * @return 0 in case of success.
 */
int linux_i2c_add_msg(struct no_os_i2c_desc *desc,
		      uint8_t *data,
		      uint8_t bytes_number,
		      uint8_t read)
{
	struct linux_i2c_desc *linux_desc;
	struct i2c_msg msg;
	void *ptr;

	linux_desc = desc->extra;

	msg.addr = desc->slave_address;
	msg.len = bytes_number;
	msg.buf = data;

	if (read)
		msg.flags = I2C_M_RD;
	else
		msg.flags = 0;  // Write


	if (linux_desc->messages) {
		ptr = realloc(linux_desc->messages, sizeof(struct i2c_msg) *
			      (linux_desc->len_messages + 1));
		if (!ptr)
			return -ENOMEM;
		linux_desc->messages = ptr;
	} else {
		linux_desc->messages = no_os_malloc(sizeof(struct i2c_msg));
		linux_desc->len_messages = 0;
		if (!linux_desc->messages)
			return -ENOMEM;
	}

	linux_desc->messages[linux_desc->len_messages] = msg;
	linux_desc->len_messages++;

	return 0;
}

/**
 * @brief Send a combined I2C R/W transaction with only one stop bit.
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int linux_i2c_send_msg(struct no_os_i2c_desc *desc)
{
	struct linux_i2c_desc *linux_desc;
	struct i2c_rdwr_ioctl_data packets;
	int32_t ret;

	linux_desc = desc->extra;

	packets.msgs = linux_desc->messages;
	packets.nmsgs = linux_desc->len_messages;

	ret = ioctl(linux_desc->fd, I2C_RDWR, &packets);
	if (ret <= 0)  // True if error or no messages sent.
		return -1;

	no_os_free(linux_desc->messages);
	linux_desc->len_messages = 0;
	linux_desc->messages = NULL;

	return 0;
}

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int linux_i2c_init(struct no_os_i2c_desc **desc,
		   const struct no_os_i2c_init_param *param)
{
	struct linux_i2c_init_param *linux_init;
	struct linux_i2c_desc *linux_desc;
	struct no_os_i2c_desc *descriptor;
	char path[64];

	descriptor = no_os_malloc(sizeof(*descriptor));
	if (!descriptor)
		return -1;

	linux_desc = (struct linux_i2c_desc*) no_os_malloc(sizeof(
				struct linux_i2c_desc));
	if (!linux_desc)
		goto free_desc;

	descriptor->extra = linux_desc;
	linux_init = param->extra;

	snprintf(path, sizeof(path), "/dev/i2c-%d", linux_init->device_id);

	linux_desc->fd = open(path, O_RDWR);
	if (linux_desc->fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, path);
		goto free;
	}

	descriptor->slave_address = param->slave_address;

	*desc = descriptor;

	return 0;

free:
	no_os_free(linux_desc);
free_desc:
	no_os_free(descriptor);

	return -1;
}

/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int linux_i2c_remove(struct no_os_i2c_desc *desc)
{
	struct linux_i2c_desc *linux_desc;
	int32_t ret;

	linux_desc = desc->extra;

	ret = close(linux_desc->fd);
	if (ret) {
		printf("%s: Can't close device\n\r", __func__);
		return -1;
	}

	no_os_free(desc->extra);
	no_os_free(desc);

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
int linux_i2c_write(struct no_os_i2c_desc *desc,
		    uint8_t *data,
		    uint8_t bytes_number,
		    uint8_t stop_bit)
{
	int32_t ret;

	ret = linux_i2c_add_msg(desc, data, bytes_number, 0);
	if (ret) {
		printf("%s: Can't allocate memory\n\r", __func__);
		return -1;
	}

	if (stop_bit) {
		ret = linux_i2c_send_msg(desc);
		if (ret) {
			printf("%s: Can't write to file\n\r", __func__);
			return -1;
		}
	}

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
int linux_i2c_read(struct no_os_i2c_desc *desc,
		   uint8_t *data,
		   uint8_t bytes_number,
		   uint8_t stop_bit)
{
	int32_t ret;

	ret = linux_i2c_add_msg(desc, data, bytes_number, 1);
	if (ret) {
		printf("%s: Can't allocate memory\n\r", __func__);
		return -1;
	}

	if (stop_bit) {
		ret = linux_i2c_send_msg(desc);
		if (ret) {
			printf("%s: Can't read from file\n\r", __func__);
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Linux platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops linux_i2c_ops = {
	.i2c_ops_init = &linux_i2c_init,
	.i2c_ops_write = &linux_i2c_write,
	.i2c_ops_read = &linux_i2c_read,
	.i2c_ops_remove = &linux_i2c_remove
};
