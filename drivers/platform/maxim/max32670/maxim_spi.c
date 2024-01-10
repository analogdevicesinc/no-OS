/***************************************************************************//**
 *   @file   maxim_spi.c
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
#include "mxc_errors.h"
#include "mxc_pins.h"
#include "maxim_spi.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

#define SPI_MASTER_MODE	1
#define SPI_SINGLE_MODE	0

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static int _max_spi_config(struct no_os_spi_desc *desc)
{
	int32_t ret;
	struct max_spi_init_param *eparam;

	eparam = desc->extra;

	ret = MXC_SPI_Init(MXC_SPI_GET_SPI(desc->device_id), SPI_MASTER_MODE,
			   SPI_SINGLE_MODE,
			   eparam->num_slaves, eparam->polarity, desc->max_speed_hz);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	ret = MXC_SPI_SetMode(MXC_SPI_GET_SPI(desc->device_id),
			      (mxc_spi_mode_t)desc->mode);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	ret = MXC_SPI_SetWidth(MXC_SPI_GET_SPI(desc->device_id), SPI_WIDTH_STANDARD);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	ret = MXC_SPI_SetDataSize(MXC_SPI_GET_SPI(desc->device_id), 8);
	if (ret) {
		ret = -EINVAL;
		goto err_init;
	}

	return 0;
err_init:
	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(desc->device_id));

	return ret;
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_init(struct no_os_spi_desc **desc,
		     const struct no_os_spi_init_param *param)
{
	int32_t ret;
	struct no_os_spi_desc *descriptor;
	struct max_spi_init_param *eparam;

	if (!param || !param->extra)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));

	if (!descriptor)
		return -ENOMEM;

	eparam = param->extra;
	descriptor->device_id = param->device_id;
	descriptor->max_speed_hz = param->max_speed_hz;
	descriptor->chip_select = param->chip_select;
	descriptor->mode = param->mode;
	descriptor->bit_order = param->bit_order;
	descriptor->extra = eparam;

	if (descriptor->device_id >= MXC_SPI_INSTANCES) {
		ret = -EINVAL;
		goto err;
	}

	ret = _max_spi_config(descriptor);
	if (ret)
		return ret;

	*desc = descriptor;

	return 0;
err_init:
	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(descriptor->device_id));
err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_remove(struct no_os_spi_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_SPI_Shutdown(MXC_SPI_GET_SPI(desc->device_id));
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write/read multiple messages to/from SPI.
 * @param desc - The SPI descriptor.
 * @param msgs - The messages array.
 * @param len - Number of messages.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_transfer(struct no_os_spi_desc *desc,
			 struct no_os_spi_msg *msgs,
			 uint32_t len)
{
	static uint32_t last_slave_id[MXC_SPI_INSTANCES];
	mxc_spi_req_t req;
	uint32_t slave_id;
	int32_t ret;

	if (!desc || !msgs)
		return -EINVAL;

	slave_id = desc->chip_select;
	if (slave_id != last_slave_id[desc->device_id]) {
		ret = _max_spi_config(desc);
		if (ret)
			return ret;

		last_slave_id[desc->device_id] = slave_id;
	}

	req.spi = MXC_SPI_GET_SPI(desc->device_id);
	req.ssIdx = desc->chip_select;
	for (uint32_t i = 0; i < len; i++) {
		req.txData = msgs[i].tx_buff;
		req.rxData = msgs[i].rx_buff;
		req.txCnt = 0;
		req.rxCnt = 0;
		req.ssDeassert = msgs[i].cs_change;
		req.txLen = req.txData ? msgs[i].bytes_number : 0;
		req.rxLen = req.rxData ? msgs[i].bytes_number : 0;

		ret = MXC_SPI_MasterTransaction(&req);

		if (ret == E_BAD_PARAM)
			return -EINVAL;
		if (ret == E_BAD_STATE)
			return -EBUSY;
	}

	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t max_spi_write_and_read(struct no_os_spi_desc *desc,
			       uint8_t *data,
			       uint16_t bytes_number)
{
	struct no_os_spi_msg xfer = {
		.rx_buff = data,
		.tx_buff = data,
		.bytes_number = bytes_number,
		.cs_change = 1,
	};

	return max_spi_transfer(desc, &xfer, 1);
}

/**
 * @brief maxim platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops max_spi_ops = {
	.init = &max_spi_init,
	.write_and_read = &max_spi_write_and_read,
	.transfer = &max_spi_transfer,
	.remove = &max_spi_remove
};
