/***************************************************************************//**
 *   @file   maxim/maxim_spi.c
 *   @brief  Implementation of SPI driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
/************************* Include Files **************************************/
/******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include "spi.h"
#include "spi_extra.h"
#include "no-os/spi.h"
#include "no-os/util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_init(struct spi_desc **desc, const struct spi_init_param *param)
{
	int32_t ret;
	struct spi_desc *descriptor;

	if (!param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));

	if (!descriptor)
		return -ENOMEM;

	descriptor->device_id = param->device_id;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->chip_select = param->chip_select;
	descriptor->mode = param->mode;
	descriptor->bit_order = param->bit_order;
	descriptor->platform_ops = &max_spi_ops;

	if (descriptor->device_id == 0)
		ret = SPI_Init(SPI0A, descriptor->mode, param->max_speed_hz);
#ifdef MXC_SPI1A
	else if (descriptor->device_id == 1)
		ret = SPI_Init(SPI1A, descriptor->mode, param->max_speed_hz);
#endif
	else {
		ret = -EINVAL;
		goto err;
	}

	if (ret == E_BAD_PARAM) {
		ret = -EINVAL;
		goto err;
	}
	if (ret == E_NO_DEVICE) {
		ret = -ENODEV;
		goto err;
	}

	*desc = descriptor;

	return 0;
err:
	free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_remove(struct spi_desc *desc)
{
	if (!desc)
		return -EINVAL;

	free(desc);

	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_write_and_read(struct spi_desc *desc,
			       uint8_t *data,
			       uint16_t bytes_number)
{
	int32_t ret;
	uint8_t *tx_buffer;
	uint8_t *rx_buffer;
	spi_req_t req;

	if (!desc || !data)
		return -EINVAL;

	rx_buffer = data;
	tx_buffer = data;

	req.bits = 8;
	req.ssel = desc->chip_select;
	req.tx_data = tx_buffer;
	req.tx_num = 0;
	req.rx_data = rx_buffer;
	req.rx_num = 0;
	req.deass = 1;
	req.width = SPI17Y_WIDTH_1;
	req.ssel_pol = SPI17Y_POL_LOW;
	req.len = bytes_number;

	if (desc->device_id == 0)
		ret = SPI_MasterTrans(SPI0A, &req);
#ifdef MXC_SPI1
	else if (desc->device_id == 1)
		ret = SPI_MasterTrans(SPI1A, &req);
#endif
	else
		return -EINVAL;

	if (ret == E_BAD_PARAM)
		return -EINVAL;
	if (ret == E_BAD_STATE)
		return -EBUSY;

	return 0;
}

/**
 * @brief Write/read multiple messages to/from SPI.
 * @param desc - The SPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_transfer(struct spi_desc *desc, struct spi_msg *msgs,
			 uint32_t len)
{
	spi_req_t req;
	int32_t ret;

	if (!desc || !desc->extra || !msgs)
		return -EINVAL;

	req.bits = 8;
	req.ssel = desc->chip_select;
	req.ssel_pol = SPI17Y_POL_LOW;

	for (uint32_t i = 0; i < len; i++) {
		req.tx_data = msgs[i].tx_buff;
		req.rx_data = msgs[i].rx_buff;
		req.tx_num = 0;
		req.rx_num = 0;
		req.deass = msgs[i].cs_change;
		req.len = msgs[i].bytes_number;

		if (desc->device_id == 0)
			ret = SPI_MasterTrans(SPI0A, &req);
#ifdef MXC_SPI1
		else if (desc->device_id == 1)
			ret = SPI_MasterTrans(SPI1A, &req);
#endif
		else
			return -EINVAL;

		if (ret == E_BAD_PARAM)
			return -EINVAL;
		if (ret == E_BAD_STATE)
			return -EBUSY;
	}

	return 0;
}

/**
 * @brief maxim platform specific SPI platform ops structure
 */
const struct spi_platform_ops max_spi_ops = {
	.init = &max_spi_init,
	.write_and_read = &max_spi_write_and_read,
	.transfer = &max_spi_transfer,
	.remove = &max_spi_remove
};
