/***************************************************************************//**
 *   @file   linux/linux_uart.c
 *   @brief  Implementation of Linux platform UART Driver.
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
#include "no_os_alloc.h"
#include "linux_uart.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct linux_uart_desc
 * @brief Linux platform specific UART descriptor
 */
struct linux_uart_desc {
	/** /dev/"device_id" file descriptor */
	int fd;
	/** structure containing the terminal flags/settings */
	struct termios *terminal;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t linux_uart_init(struct no_os_uart_desc **desc,
			       struct no_os_uart_init_param *param)
{
	struct linux_uart_init_param *linux_init;
	struct linux_uart_desc *linux_desc;
	struct no_os_uart_desc *descriptor;
	speed_t speed;
	char path[64];
	int ret;

	descriptor = no_os_malloc(sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	linux_desc = (struct linux_uart_desc*) no_os_malloc(sizeof(
				struct linux_uart_desc));
	if (!linux_desc) {
		ret = -ENOMEM;
		goto free_desc;
	}

	linux_desc->terminal = (struct termios*) no_os_malloc(sizeof(struct termios));
	if (!linux_desc->terminal) {
		ret = -ENOMEM;
		goto free_linux_desc;
	}

	descriptor->extra = linux_desc;
	linux_init = param->extra;

	ret = snprintf(path, sizeof(path), "/dev/%s", linux_init->device_id);
	if (ret < 0 || ret >= (int)sizeof(path)) {
		ret = -ENOMEM;
		goto free_linux_desc;
	}

	linux_desc->fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (linux_desc->fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, path);
		ret = -ENOENT;
		goto free_terminal;
	}

	tcgetattr(linux_desc->fd, linux_desc->terminal);

	cfmakeraw(linux_desc->terminal);

	switch(param->baud_rate) {
	case 50:
		speed = B50;
		break;
	case 75:
		speed = B75;
		break;
	case 110:
		speed = B110;
		break;
	case 134:
		speed = B134;
		break;
	case 150:
		speed = B150;
		break;
	case 200:
		speed = B200;
		break;
	case 300:
		speed = B300;
		break;
	case 600:
		speed = B600;
		break;
	case 1200:
		speed = B1200;
		break;
	case 1800:
		speed = B1800;
		break;
	case 2400:
		speed = B2400;
		break;
	case 4800:
		speed = B4800;
		break;
	case 9600:
		speed = B9600;
		break;
	case 19200:
		speed = B19200;
		break;
	case 38400:
		speed = B38400;
		break;
	default:
		ret = -EINVAL;
		goto free;
	}
	cfsetispeed(linux_desc->terminal, speed);
	cfsetospeed(linux_desc->terminal, speed);

	linux_desc->terminal->c_cflag &= ~CSIZE;
	switch(param->size) {
	case NO_OS_UART_CS_5:
		linux_desc->terminal->c_cflag |= CS5;
		break;
	case NO_OS_UART_CS_6:
		linux_desc->terminal->c_cflag |= CS6;
		break;
	case NO_OS_UART_CS_7:
		linux_desc->terminal->c_cflag |= CS7;
		break;
	case NO_OS_UART_CS_8:
		linux_desc->terminal->c_cflag |= CS8;
		break;
	default:
		ret = -EINVAL;
		goto free;
	}

	linux_desc->terminal->c_cflag &= ~PARENB;
	linux_desc->terminal->c_cflag &= ~PARODD;
	switch(param->parity) {
	case NO_OS_UART_PAR_NO:
		break;
	case NO_OS_UART_PAR_ODD:
		linux_desc->terminal->c_cflag |= PARODD;
		break;
	case NO_OS_UART_PAR_EVEN:
		linux_desc->terminal->c_cflag |= PARENB;
		break;
	default:
		ret = -EINVAL;
		goto free;
	}

	if (param->stop == NO_OS_UART_STOP_1_BIT)
		linux_desc->terminal->c_cflag &= ~CSTOPB;
	else
		linux_desc->terminal->c_cflag |= CSTOPB;

	linux_desc->terminal->c_cflag |= CREAD;

	tcsetattr(linux_desc->fd, TCSANOW, linux_desc->terminal);

	tcflush(linux_desc->fd, TCIOFLUSH);

	*desc = descriptor;

	return 0;

free:
	close(linux_desc->fd);
free_terminal:
	no_os_free(linux_desc->terminal);
free_linux_desc:
	no_os_free(linux_desc);
free_desc:
	no_os_free(descriptor);

	return ret;
};

/**
 * @brief Free the resources allocated by linux_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t linux_uart_remove(struct no_os_uart_desc *desc)
{
	struct linux_uart_desc *linux_desc;
	int32_t ret;

	linux_desc = desc->extra;

	ret = close(linux_desc->fd);
	if (ret < 0)
		printf("%s: Can't close device\n\r", __func__);

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
};

/**
 * @brief Write data to UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t linux_uart_write(struct no_os_uart_desc *desc,
				const uint8_t *data,
				uint32_t bytes_number)
{
	struct linux_uart_desc *linux_desc;
	uint32_t count = 0;
	int32_t ret;

	linux_desc = desc->extra;

	while (count < bytes_number) {
		ret = write(linux_desc->fd, data, bytes_number);
		if (ret > 0)
			count += ret;
	}

	return 0;
};

/**
 * @brief Read data from UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t linux_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			       uint32_t bytes_number)
{
	struct linux_uart_desc *linux_desc;
	uint32_t count = 0;
	int ret;

	linux_desc = desc->extra;

	while (count < bytes_number) {
		ret = read(linux_desc->fd, &data[count], bytes_number - count);
		if (ret > 0)
			count += ret;
	}

	return 0;
};

/**
 * @brief Linux platform specific UART platform ops structure
 */
const struct no_os_uart_platform_ops linux_uart_ops = {
	.init = &linux_uart_init,
	.read = &linux_uart_read,
	.write = &linux_uart_write,
	.remove = &linux_uart_remove
};
