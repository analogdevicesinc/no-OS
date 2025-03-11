/*******************************************************************************
 *   @file   zephyr/zephyr_spi.c
 *   @brief  Implementation of zephyr spi driver.
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
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_alloc.h"
#include "zephyr_spi.h"

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t zephyr_spi_init(struct no_os_spi_desc **desc,
			const struct no_os_spi_init_param *param)
{
	struct zephyr_spi_desc *xdevice;

	if (!param)
		return -EINVAL;

	if (NO_OS_DRIVER_DYNAMIC_ALLOC) {
		(*desc) = (struct no_os_spi_desc *)no_os_malloc(sizeof(*(*desc)));
		if (!(*desc))
			return -ENOMEM;
		xdevice = no_os_calloc(1, sizeof(*xdevice));
		if (!xdevice) {
			no_os_free(*desc);
			return -ENOMEM;
		}
	} else {
		if (!(*desc))
			return -ENOMEM;
		if (!((*desc)->extra))
			return -ENOMEM;
		xdevice = (struct zephyr_spi_desc *)((*desc)->extra);
	}
	xdevice->bus = ((struct zephyr_spi_init_param *)(param->extra))->bus;
	xdevice->config = ((struct zephyr_spi_init_param *)(param->extra))->config;
	(*desc)->extra = xdevice;
	(*desc)->platform_ops = &zephyr_spi_ops;

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t zephyr_spi_remove(struct no_os_spi_desc *desc)
{
	if (NO_OS_DRIVER_DYNAMIC_ALLOC) {
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
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t zephyr_spi_write_and_read(struct no_os_spi_desc *desc,
				  uint8_t *data,
				  uint16_t bytes_number)
{
	int ret;
	struct device *bus_tmp;
	struct spi_buf_set tx, rx;
	struct spi_buf buff_tx, buff_rx;
	uint8_t rx_data[bytes_number];
	struct spi_config *config_tmp;

	bus_tmp = ((struct zephyr_spi_desc *)(desc->extra))->bus;
	config_tmp = ((struct zephyr_spi_desc *)(desc->extra))->config;

	buff_tx.buf = data;
	buff_tx.len = bytes_number;
	tx.buffers = &buff_tx;
	tx.count = 1;

	buff_rx.buf = rx_data;
	buff_rx.len = bytes_number;
	rx.buffers = &buff_rx;
	rx.count = 1;

	ret = spi_transceive(bus_tmp, config_tmp, &tx, &rx);
	if (ret) {
		printk("SPI transceive WRITE failed with error code: %d\n", ret);
		return ret;
	}

	memcpy(data, rx_data, bytes_number);

	return 0;
}

static int32_t zephyr_spi_transfer(struct no_os_spi_desc *desc,
				   struct no_os_spi_msg *msgs,
				   uint32_t len)
{
	int ret;
	struct device *bus_tmp;
	struct spi_buf_set tx, rx;
	struct spi_buf *buff_tx, *buff_rx;
	struct spi_config *config_tmp;
	uint32_t i;

	bus_tmp = ((struct zephyr_spi_desc *)(desc->extra))->bus;
	config_tmp = ((struct zephyr_spi_desc *)(desc->extra))->config;

	buff_tx = no_os_calloc(len, sizeof(*buff_tx));
	if (!buff_tx)
		return -ENOMEM;

	buff_rx = no_os_calloc(len, sizeof(*buff_rx));
	if (!buff_rx) {
		no_os_free(buff_tx);
		return -ENOMEM;
	}

	for (i = 0; i < len; i++) {
		buff_tx[i].buf = msgs[i].tx_buff;
		buff_tx[i].len = msgs[i].bytes_number;
		buff_rx[i].buf = msgs[i].rx_buff;
		buff_rx[i].len = msgs[i].bytes_number;
	}

	tx.buffers = buff_tx;
	tx.count = len;
	rx.buffers = buff_rx;
	rx.count = len;

	ret = spi_transceive(bus_tmp, config_tmp, &tx, &rx);

	no_os_free(buff_tx);
	no_os_free(buff_rx);

	if (ret) {
		printk("SPI transceive failed with error code: %d\n", ret);
		return ret;
	}

	return 0;
}

/**
 * @brief zephyr platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops zephyr_spi_ops = {
	.init = &zephyr_spi_init,
	.write_and_read = &zephyr_spi_write_and_read,
	.remove = &zephyr_spi_remove,
	.transfer = &zephyr_spi_transfer
};
