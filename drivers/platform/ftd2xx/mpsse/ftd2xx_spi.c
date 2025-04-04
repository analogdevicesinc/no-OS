/*******************************************************************************
 *   @file   ftd2xx_spi.c
 *   @brief  Implementation of SPI driver for ftd2xx platform.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
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
#include <errno.h>
#include "no_os_spi.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"

#include "ftd2xx_spi.h"
#include "ftd2xx_platform.h"
#include "libmpsse_spi.h"

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int ftd2xx_spi_init(struct no_os_spi_desc **desc,
		    const struct no_os_spi_init_param *param)
{
	struct ftd2xx_spi_desc *extra_desc;
	struct ftd2xx_spi_init *extra_init;
	struct no_os_spi_desc *spi_desc;
	ChannelConfig channelConf;
	DWORD *num_channels = 0;
	FT_STATUS status;
	int ret;

	if (!desc || !param || param->device_id >= FTD2XX_MAX_DEV_PER_CHIP
	    || param->max_speed_hz > 30000000)
		return -EINVAL;

	spi_desc = (struct no_os_spi_desc *)
		   no_os_calloc(1, sizeof(*spi_desc));
	if (!spi_desc)
		return -ENOMEM;

	extra_desc = (struct ftd2xx_spi_desc*)
		     no_os_calloc(1, sizeof(struct ftd2xx_spi_desc));
	if (!extra_desc) {
		ret = -ENOMEM;
		goto error;
	}

	spi_desc->extra = extra_desc;

	extra_init = param->extra;

	channelConf.ClockRate = param->max_speed_hz;
	channelConf.LatencyTimer = 10;
	channelConf.configOptions = param->mode |
				    SPI_CONFIG_OPTION_CS_DBUS3 |
				    SPI_CONFIG_OPTION_CS_ACTIVELOW;
	channelConf.Pin = extra_init->channel_config_pin;

	Init_libMPSSE();

	status = SPI_GetNumChannels(&num_channels);
	if ((status != FT_OK) || (num_channels == 0)) {
		pr_err("SPI_GetNumChannels() failed (%d, %d)\n",
		       status, num_channels);
		ret = -EIO;
		goto error;
	}

	if (ftHandle[param->device_id] == NULL) {
		status = SPI_OpenChannel(param->device_id, &extra_desc->ftHandle);
		if (status != FT_OK) {
			pr_err("SPI_OpenChannel() failed (%d)\n", status);
			ret = status;
			goto error;
		}
		ftHandle[param->device_id] = extra_desc->ftHandle;
	} else {
		extra_desc->ftHandle = ftHandle[param->device_id];
	}

	status = SPI_InitChannel(extra_desc->ftHandle, &channelConf);
	if (status != FT_OK) {
		pr_err("SPI_InitChannel() failed (%d)\n", status);
		ret = status;
		goto error;
	}

	*desc = spi_desc;

	return 0;

error:
	no_os_free(extra_desc);
	no_os_free(spi_desc);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int ftd2xx_spi_remove(struct no_os_spi_desc *desc)
{
	struct ftd2xx_spi_desc *extra_desc;

	if (!desc)
		return -EINVAL;

	extra_desc = desc->extra;
	SPI_CloseChannel(extra_desc->ftHandle);
	Cleanup_libMPSSE();
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.
 */
int ftd2xx_spi_write_and_read(struct no_os_spi_desc *desc,
			      uint8_t *data,
			      uint16_t bytes_number)
{
	struct ftd2xx_spi_desc *extra_desc = desc->extra;
	DWORD transferred;
	FT_STATUS status;

	if (!desc || !data)
		return -EINVAL;

	if (!bytes_number)
		return 0;

	status = SPI_ReadWrite(extra_desc->ftHandle, (UCHAR *)data,
			       (UCHAR *)data, bytes_number, &transferred,
			       SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
			       SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (status != FT_OK)
		return -EIO;

	return 0;
}

/**
 * @brief Write/read multiple messages to/from SPI.
 * @param desc - The SPI descriptor.
 * @param xfer - The messages array.
 * @param len - Number of messages.
 * @return 0 in case of success, errno codes otherwise.
 */
int ftd2xx_spi_transfer(struct no_os_spi_desc *desc, struct no_os_spi_msg *xfer,
			uint32_t len)
{
	struct ftd2xx_spi_desc *extra_desc = desc->extra;
	DWORD transferred;
	UCHAR *data;
	FT_STATUS status;

	if (!desc || !xfer)
		return -EINVAL;

	if (!len)
		return 0;

	status = SPI_Write(extra_desc->ftHandle, (UCHAR *)xfer->tx_buff,
			   xfer->bytes_number, &transferred,
			   SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
			   SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
	if (status != FT_OK)
		return -EIO;

	if (xfer->rx_buff) {
		status = SPI_Read(extra_desc->ftHandle, (UCHAR *)xfer->rx_buff,
				  xfer->bytes_number, &transferred,
				  SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE |
				  SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE);
		if (status != FT_OK)
			return -EIO;
	}

	return 0;
}

/**
 * @brief ftd2xx platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops ftd2xx_spi_ops = {
	.init = &ftd2xx_spi_init,
	.remove = &ftd2xx_spi_remove,
	.write_and_read = &ftd2xx_spi_write_and_read,
	.transfer = &ftd2xx_spi_transfer,
};
