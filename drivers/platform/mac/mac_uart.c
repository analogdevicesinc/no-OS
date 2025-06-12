/***************************************************************************//**
 *   @file   mac_uart.c
 *   @brief  Implementation of Mac platform UART Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "mac_uart.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

/**
 * @struct mac_uart_desc
 * @brief Mac platform specific UART descriptor
 */
struct mac_uart_desc {
	/** /dev/"device_id" file descriptor */
	int fd;
	/** structure containing the terminal flags/settings */
	struct termios *terminal;
};

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t mac_uart_init(struct no_os_uart_desc **desc,
			     struct no_os_uart_init_param *param)
{
	struct mac_uart_init_param *mac_init;
	struct mac_uart_desc *mac_desc;
	struct no_os_uart_desc *descriptor;
	speed_t speed;
	char path[64];
	int ret;

	descriptor = no_os_malloc(sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	mac_desc = (struct mac_uart_desc*) no_os_malloc(sizeof(
				struct mac_uart_desc));
	if (!mac_desc) {
		ret = -ENOMEM;
		goto free_desc;
	}

	mac_desc->terminal = (struct termios*) no_os_malloc(sizeof(struct termios));
	if (!mac_desc->terminal) {
		ret = -ENOMEM;
		goto free_mac_desc;
	}

	descriptor->extra = mac_desc;
	mac_init = param->extra;

	ret = snprintf(path, sizeof(path), "/dev/%s", mac_init->device_id);
	if (ret < 0 || ret >= (int)sizeof(path)) {
		ret = -ENOMEM;
		goto free_mac_desc;
	}

	mac_desc->fd = open(path, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (mac_desc->fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, path);
		ret = -ENOENT;
		goto free_terminal;
	}

	tcgetattr(mac_desc->fd, mac_desc->terminal);

	cfmakeraw(mac_desc->terminal);

	switch (param->baud_rate) {
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
	cfsetispeed(mac_desc->terminal, speed);
	cfsetospeed(mac_desc->terminal, speed);

	mac_desc->terminal->c_cflag &= ~CSIZE;
	switch (param->size) {
	case NO_OS_UART_CS_5:
		mac_desc->terminal->c_cflag |= CS5;
		break;
	case NO_OS_UART_CS_6:
		mac_desc->terminal->c_cflag |= CS6;
		break;
	case NO_OS_UART_CS_7:
		mac_desc->terminal->c_cflag |= CS7;
		break;
	case NO_OS_UART_CS_8:
		mac_desc->terminal->c_cflag |= CS8;
		break;
	default:
		ret = -EINVAL;
		goto free;
	}

	mac_desc->terminal->c_cflag &= ~PARENB;
	mac_desc->terminal->c_cflag &= ~PARODD;
	switch (param->parity) {
	case NO_OS_UART_PAR_NO:
		break;
	case NO_OS_UART_PAR_ODD:
		mac_desc->terminal->c_cflag |= PARODD;
		break;
	case NO_OS_UART_PAR_EVEN:
		mac_desc->terminal->c_cflag |= PARENB;
		break;
	default:
		ret = -EINVAL;
		goto free;
	}

	if (param->stop == NO_OS_UART_STOP_1_BIT)
		mac_desc->terminal->c_cflag &= ~CSTOPB;
	else
		mac_desc->terminal->c_cflag |= CSTOPB;

	mac_desc->terminal->c_cflag |= CREAD;

	tcsetattr(mac_desc->fd, TCSANOW, mac_desc->terminal);

	tcflush(mac_desc->fd, TCIOFLUSH);

	*desc = descriptor;

	return 0;

free:
	close(mac_desc->fd);
free_terminal:
	no_os_free(mac_desc->terminal);
free_mac_desc:
	no_os_free(mac_desc);
free_desc:
	no_os_free(descriptor);

	return ret;
};

/**
 * @brief Free the resources allocated by mac_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t mac_uart_remove(struct no_os_uart_desc *desc)
{
	struct mac_uart_desc *mac_desc;
	int32_t ret;

	mac_desc = desc->extra;

	ret = close(mac_desc->fd);
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
static int32_t mac_uart_write(struct no_os_uart_desc *desc,
			      const uint8_t *data,
			      uint32_t bytes_number)
{
	struct mac_uart_desc *mac_desc;
	uint32_t count = 0;
	int32_t ret;

	mac_desc = desc->extra;

	while (count < bytes_number) {
		ret = write(mac_desc->fd, data, bytes_number);
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
static int32_t mac_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			     uint32_t bytes_number)
{
	struct mac_uart_desc *mac_desc;
	uint32_t count = 0;
	int ret;

	mac_desc = desc->extra;

	while (count < bytes_number) {
		ret = read(mac_desc->fd, &data[count], bytes_number - count);
		if (ret > 0)
			count += ret;
	}

	return 0;
};

/**
 * @brief mac platform specific UART platform ops structure
 */
const struct no_os_uart_platform_ops mac_uart_ops = {
	.init = &mac_uart_init,
	.read = &mac_uart_read,
	.write = &mac_uart_write,
	.remove = &mac_uart_remove
};
