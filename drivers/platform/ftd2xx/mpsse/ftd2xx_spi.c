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

	if (!desc || !param)
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

	if (param->max_speed_hz > 30000000) {
		ret = -EINVAL;
		goto error;
	}
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

	status = SPI_OpenChannel(param->device_id, &extra_desc->ftHandle);
	if (status != FT_OK) {
		pr_err("SPI_OpenChannel() failed (%d)\n", status);
		ret = status;
		goto error;
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

int ftd2xx_spi_remove(struct no_os_spi_desc *desc)
{
	if (!desc)
		return -EINVAL;

	no_os_free(desc);

	return 0;
}

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

const struct no_os_spi_platform_ops ftd2xx_spi_ops = {
	.init = &ftd2xx_spi_init,
	.remove = &ftd2xx_spi_remove,
	.write_and_read = &ftd2xx_spi_write_and_read,
};
