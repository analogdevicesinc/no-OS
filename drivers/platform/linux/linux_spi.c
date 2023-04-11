/***************************************************************************//**
 *   @file   linux/linux_spi.c
 *   @brief  Implementation of Linux platform SPI Driver.
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
#include "no_os_spi.h"
#include "no_os_alloc.h"
#include "linux_spi.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>

#warning SPI cs_delay_first and cs_delay_last delays are not supported on the linux platform

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct linux_spi_desc
 * @brief Linux platform specific SPI descriptor
 */
struct linux_spi_desc {
	/** /dev/spidev"device_id"."chip_select" file descriptor */
	int spidev_fd;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_spi_init(struct no_os_spi_desc **desc,
		       const struct no_os_spi_init_param *param)
{
	struct linux_spi_desc *linux_desc;
	struct no_os_spi_desc *descriptor;
	uint8_t bits = 8;
	char path[64];
	int ret;

	descriptor = no_os_malloc(sizeof(*descriptor));
	if (!descriptor)
		return -1;

	linux_desc = (struct linux_spi_desc*) no_os_malloc(sizeof(
				struct linux_spi_desc));
	if (!linux_desc)
		goto free_desc;

	descriptor->extra = linux_desc;

	snprintf(path, sizeof(path), "/dev/spidev%d.%d",
		 param->device_id, param->chip_select);

	linux_desc->spidev_fd = open(path, O_RDWR);
	if (linux_desc->spidev_fd < 0) {
		printf("%s: Can't open %s\n\r", __func__, path);
		goto free;
	}

	ret = ioctl(linux_desc->spidev_fd, SPI_IOC_WR_MODE,
		    &param->mode);
	if (ret == -1) {
		printf("%s: Can't set SPI mode\n\r", __func__);
		goto free;
	}

	ret = ioctl(linux_desc->spidev_fd, SPI_IOC_WR_BITS_PER_WORD,
		    &bits);
	if (ret == -1) {
		printf("%s: Can't set SPI bits per word\n\r", __func__);
		goto free;
	}

	ret = ioctl(linux_desc->spidev_fd, SPI_IOC_WR_MAX_SPEED_HZ,
		    &param->max_speed_hz);
	if (ret == -1) {
		printf("%s: Can't set SPI max speed hz\n\r", __func__);
		goto free;
	}

	*desc = descriptor;

	return 0;
free:
	no_os_free(linux_desc);
free_desc:
	no_os_free(descriptor);

	return -1;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_spi_write_and_read(struct no_os_spi_desc *desc,
				 uint8_t *data,
				 uint16_t bytes_number)
{
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)data,
		.rx_buf = (unsigned long)data,
		.len = bytes_number,
	};
	struct linux_spi_desc *linux_desc;
	int ret;

	linux_desc = desc->extra;

	ret = ioctl(linux_desc->spidev_fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret == 1) {
		printf("%s: Can't send spi message\n\r", __func__);
		return -1;
	}

	return 0;
}

/**
 * @brief Free the resources allocated by linux_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t linux_spi_remove(struct no_os_spi_desc *desc)
{
	struct linux_spi_desc *linux_desc;
	int32_t ret;

	linux_desc = desc->extra;

	ret = close(linux_desc->spidev_fd);
	if (ret < 0) {
		printf("%s: Can't close device\n\r", __func__);
		return -1;
	}

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

static int32_t linux_spi_transfer(struct no_os_spi_desc *desc,
				  struct no_os_spi_msg *msgs,
				  uint32_t len)

{
	struct spi_ioc_transfer *tr;
	struct linux_spi_desc	*linux_desc;
	int			ret;
	uint32_t		i;

	linux_desc = desc->extra;

	tr = (struct spi_ioc_transfer *)no_os_calloc(len, sizeof(*tr));
	if (!tr)
		return -ENOMEM;

	for (i = 0; i < len; i++) {
		tr[i].tx_buf = (unsigned long) msgs[i].tx_buff;
		tr[i].rx_buf = (unsigned long) msgs[i].rx_buff;
		tr[i].len = msgs[i].bytes_number;
		tr[i].cs_change = msgs[i].cs_change;
		tr[i].word_delay_usecs = msgs[i].cs_change_delay;
	}

	ret = ioctl(linux_desc->spidev_fd, SPI_IOC_MESSAGE(len), tr);

	no_os_free(tr);

	if (ret < 0) {
		printf("%s: Can't send spi message (%d)\n\r", __func__, errno);
		return ret;
	}

	return 0;
}
/**
 * @brief Linux platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops linux_spi_ops = {
	.init = &linux_spi_init,
	.write_and_read = &linux_spi_write_and_read,
	.remove = &linux_spi_remove,
	.transfer = &linux_spi_transfer
};
