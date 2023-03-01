/***************************************************************************//**
 *   @file   linux/linux_i2c.c
 *   @brief  Implementation of Linux platform I2C Driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_error.h"
#include "no_os_i2c.h"
#include "no_os_alloc.h"
#include "linux_i2c.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct linux_i2c_desc
 * @brief Linux platform specific I2C descriptor
 */
struct linux_i2c_desc {
	/** /dev/i2c-"device_id" file descriptor */
	int fd;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_i2c_init(struct no_os_i2c_desc **desc,
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
int32_t linux_i2c_remove(struct no_os_i2c_desc *desc)
{
	struct linux_i2c_desc *linux_desc;
	int32_t ret;

	linux_desc = desc->extra;

	ret = close(linux_desc->fd);
	if (ret < 0) {
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
int32_t linux_i2c_write(struct no_os_i2c_desc *desc,
			uint8_t *data,
			uint8_t bytes_number,
			uint8_t stop_bit)
{
	struct linux_i2c_desc *linux_desc;
	int32_t ret;

	linux_desc = desc->extra;

	ret = ioctl(linux_desc->fd, I2C_SLAVE, desc->slave_address);
	if (ret < 0) {
		printf("%s: Can't select device\n\r", __func__);
		return -1;
	}

	ret = write(linux_desc->fd, data, bytes_number);
	if (ret < 0) {
		printf("%s: Can't write to file\n\r", __func__);
		return -1;
	}

	if (stop_bit) {
		// Unused variable - fix compiler warning
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
int32_t linux_i2c_read(struct no_os_i2c_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number,
		       uint8_t stop_bit)
{
	struct linux_i2c_desc *linux_desc;
	int32_t ret;

	linux_desc = desc->extra;

	ret = ioctl(linux_desc->fd, I2C_SLAVE, desc->slave_address);
	if (ret < 0) {
		printf("%s: Can't select device\n\r", __func__);
		return -1;
	}

	ret = read(linux_desc->fd, data, bytes_number);
	if (ret < 0) {
		printf("%s: Can't read from file\n\r", __func__);
		return -1;
	}

	if (stop_bit) {
		// Unused variable - fix compiler warning
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
