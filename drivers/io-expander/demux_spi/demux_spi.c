/***************************************************************************//**
 *   @file   demux_spi.c
 *   @brief  Implementation of the SPI Demux Interface
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "demux_spi.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/**************************** Types Definitions *******************************/
/******************************************************************************/

/**
 * @brief Demux specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops demux_spi_platform_ops = {
	.init = demux_spi_init,
	.remove = demux_spi_remove,
	.write_and_read = demux_spi_write_and_read
};

/**
 * @brief Initialize the SPI demux layer.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t demux_spi_init(struct no_os_spi_desc **desc,
		       const struct no_os_spi_init_param *param)
{
	int32_t ret;

	struct no_os_spi_desc *descriptor;
	struct no_os_spi_desc *spi_dev_desc;
	struct no_os_spi_init_param *spi_dev_param;

	if (!param)
		return -1;

	descriptor = (struct no_os_spi_desc *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -1;

	descriptor->chip_select = param->chip_select;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->mode = param->mode;

	spi_dev_param = param->extra;

	ret = no_os_spi_init(&spi_dev_desc, spi_dev_param);
	if (ret != 0) {
		no_os_free(descriptor);
		return -1;
	}

	(descriptor->extra) = spi_dev_desc;

	*desc = descriptor;

	return ret;
}

/**
 * @brief Free the resources allocated by demux_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t demux_spi_remove(struct no_os_spi_desc *desc)
{
	if (!desc)
		return -1;

	if (no_os_spi_remove(desc->extra))
		return -1;

	no_os_free(desc);

	return 0;
}

/**
 * @brief Write and read data to/from SPI demux layer.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t demux_spi_write_and_read(struct no_os_spi_desc *desc, uint8_t *data,
				 uint16_t bytes_number)
{
	int32_t ret;
	uint8_t cs;
	uint8_t *buff;

	struct no_os_spi_desc *spi_dev;

	if (!desc)
		return -1;

	buff = no_os_malloc(sizeof(*buff) * (bytes_number+1));
	if (!buff)
		return -1;

	spi_dev = desc->extra;
	cs = CS_OFFSET | desc->chip_select;

	buff[0] = cs;
	memcpy((buff+1), data, bytes_number);

	ret = no_os_spi_write_and_read(spi_dev, buff, bytes_number+1);

	memcpy(data, buff+1, bytes_number);

	no_os_free(buff);

	return ret;
}

