/***************************************************************************//**
 *   @file   iio_ad713x.c
 *   @brief  Implementation of iio_ad713x.c.
 *   @author Cristian Pop (cristian.pop@analog.com)
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

#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "error.h"
#include "util.h"
#include "iio.h"
#include "iio_ad713x.h"
#include "spi_engine.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Transfer data from device into RAM.
 * @param iio_inst - Physical instance of a iio_axi_adc device.
 * @param bytes_count - Number of bytes to transfer.
 * @param ch_mask - Opened channels mask.
 * return bytes_count or negative value in case of error.
 */
ssize_t iio_ad713x_transfer_dev_to_mem(void *iio_inst, size_t bytes_count,
				       uint32_t ch_mask)
{
	struct iio_ad713x *iio_713x_inst;
	ssize_t ret, bytes;

	if (!iio_inst)
		return FAILURE;

	iio_713x_inst = (struct iio_ad713x *)iio_inst;
	bytes = (bytes_count * iio_713x_inst->dev_descriptor.num_ch);

	ret = spi_engine_offload_transfer(iio_713x_inst->spi_eng_desc,
					  *(iio_713x_inst->spi_engine_offload_message), bytes);
	if (ret < 0)
		return ret;

	if (iio_713x_inst->dcache_invalidate_range)
		iio_713x_inst->dcache_invalidate_range(
			iio_713x_inst->spi_engine_offload_message->tx_addr, bytes);

	return bytes_count;
}

/**
 * @brief Read chunk of data from RAM to pbuf.
 * Call "iio_ad713x_transfer_dev_to_mem" first.
 * This function is probably called multiple times by libtinyiiod after a
 * "iio_ad713x_transfer_dev_to_mem" call, since we can only read "bytes_count"
 * bytes at a time.
 * @param iio_inst - Physical instance of a device.
 * @param pbuf - Buffer where value is stored.
 * @param offset - Offset to the remaining data after reading n chunks.
 * @param bytes_count - Number of bytes to read.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
ssize_t iio_ad713x_read_dev(void *iio_inst, char *pbuf, size_t offset,
			    size_t bytes_count, uint32_t ch_mask)
{
	struct iio_ad713x *iio_713x_inst;
	uint32_t i, j = 0, current_ch = 0, offload_data;
	uint16_t *pbuf16;
	size_t samples;

	if (!iio_inst)
		return FAILURE;

	iio_713x_inst = (struct iio_ad713x *)iio_inst;
	pbuf16 = (uint16_t*)pbuf;
	samples = (bytes_count * iio_713x_inst->dev_descriptor.num_ch) / hweight8(
			  ch_mask);
	samples /= 2; /* because of uint16_t *pbuf16 = (uint16_t*)pbuf; */
	offset = (offset * iio_713x_inst->dev_descriptor.num_ch) / hweight8(ch_mask);

	for (i = 0; i < samples; i++) {
		if (ch_mask & BIT(current_ch)) {
			offload_data = *(uint32_t*)(iio_713x_inst->spi_engine_offload_message->rx_addr +
						    offset + i * 4);
			offload_data <<= 1;
			offload_data &= 0xffffff00;
			offload_data >>= 8;
			pbuf16[j] = offload_data;
			j++;
		}

		if (current_ch < (uint8_t)(iio_713x_inst->dev_descriptor.num_ch - 1))
			current_ch++;
		else
			current_ch = 0;
	}

	return bytes_count;
}

/**
 * @brief Get iio device descriptor.
 * @param desc - Descriptor.
 * @param dev_descriptor - iio device descriptor.
 */
void iio_ad713x_get_dev_descriptor(struct iio_ad713x *desc,
				   struct iio_device **dev_descriptor)
{
	*dev_descriptor = &desc->dev_descriptor;
}

/**
 * @brief Init for reading/writing and parameterization of a
 * ad9361 device.
 * @param desc - Descriptor.
 * @param param - Configuration structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_ad713x_init(struct iio_ad713x **desc,
			struct iio_ad713x_init_par *param)
{
	struct iio_ad713x *iio_ad713x;

	iio_ad713x = (struct iio_ad713x *)calloc(1, sizeof(struct iio_ad713x));
	if (!iio_ad713x)
		return FAILURE;
	iio_ad713x->spi_eng_desc = param->spi_eng_desc;
	iio_ad713x->spi_engine_offload_message = param->spi_engine_offload_message;
	iio_ad713x->dcache_invalidate_range = param->dcache_invalidate_range;

	iio_ad713x->dev_descriptor.num_ch = param->num_channels;
	iio_ad713x->dev_descriptor.channels = NULL;
	iio_ad713x->dev_descriptor.attributes = NULL;
	iio_ad713x->dev_descriptor.debug_attributes = NULL;
	iio_ad713x->dev_descriptor.buffer_attributes = NULL;
	iio_ad713x->dev_descriptor.transfer_dev_to_mem = iio_ad713x_transfer_dev_to_mem;
	iio_ad713x->dev_descriptor.transfer_mem_to_dev = NULL;
	iio_ad713x->dev_descriptor.read_data = iio_ad713x_read_dev;
	iio_ad713x->dev_descriptor.write_data = NULL;
	*desc = iio_ad713x;

	return SUCCESS;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_ad713x_remove(struct iio_ad713x *desc)
{
	if (!desc)
		return FAILURE;

	free(desc);

	return SUCCESS;
}
