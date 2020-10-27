/***************************************************************************//**
 *   @file   iio_buffer.c
 *   @brief  Implementation of iio_buffer
 *   This is a generic module for interfacing with iio core, read data from
 *   device and send them to tinyiiod library/iio client
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
#include "iio_buffer.h"
#include "util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Transfer data from device into RAM.
 * @param iio_inst - Physical instance of a iio_buffer_desc device.
 * @param bytes_count - Number of bytes to transfer.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
static ssize_t iio_buffer_transfer_dev_to_mem(void *iio_inst,
		size_t bytes_count,
		uint32_t ch_mask)
{
	struct iio_buffer_desc *iio_buffer_desc;
	uint16_t num_ch;
	uint16_t samples;
	uint8_t sample_bytes;

	if (!iio_inst)
		return FAILURE;

	iio_buffer_desc = (struct iio_buffer_desc*)iio_inst;
	num_ch = iio_buffer_desc->dev_descriptor.num_ch;
	sample_bytes = iio_buffer_desc->dev_descriptor.channels[find_first_set_bit(
				ch_mask)]->scan_type->storagebits / 8;
	samples = (bytes_count * num_ch) / (hweight8(ch_mask) * sample_bytes);

	return iio_buffer_desc->read_data(iio_buffer_desc->dev_instance,
					  (uint32_t *)iio_buffer_desc->ddr_base,
					  samples);
}

/**
 * @brief Read chunk of data from RAM to pbuf.
 * Call "iio_demo_transfer_dev_to_mem" first.
 * This function is probably called multiple times by libtinyiiod after a
 * "iio_demo_transfer_dev_to_mem" call, since we can only read "bytes_count"
 * bytes at a time.
 * @param iio_inst - Physical instance of a device.
 * @param pbuf - Buffer where value is stored.
 * @param offset - Offset to the remaining data after reading n chunks.
 * @param bytes_count - Number of bytes to read.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
static ssize_t iio_buffer_read_dev(void *iio_inst, char *pbuf, size_t offset,
				   size_t bytes_count, uint32_t ch_mask)
{
	struct iio_buffer_desc *iio_buffer_desc;
	uint32_t i, j = 0, current_ch = 0;
	uint16_t samples, num_ch;
	uint8_t sample_bytes;
	uint8_t *pmem;

	if (!iio_inst)
		return FAILURE;

	if (!pbuf)
		return FAILURE;

	iio_buffer_desc = (struct iio_buffer_desc*)iio_inst;
	num_ch = iio_buffer_desc->dev_descriptor.num_ch;
	sample_bytes = iio_buffer_desc->dev_descriptor.channels[find_first_set_bit(
				ch_mask)]->scan_type->storagebits / 8;
	samples = (bytes_count * num_ch) / (hweight8(ch_mask) * sample_bytes);
	pmem = (uint8_t*)iio_buffer_desc->ddr_base + (offset * (1 + num_ch - hweight8(
				ch_mask)));

	for (i = 0; i < samples; i++) {

		if (ch_mask & BIT(current_ch)) {
			memcpy(pbuf + (j * sample_bytes), pmem + (i * sample_bytes), sample_bytes);
			j++;
		}

		if (current_ch + 1 < num_ch)
			current_ch++;
		else
			current_ch = 0;
	}

	return bytes_count;
}

/**
 * @brief Delete iio_device.
 * @param iio_device - Structure describing a device, channels and attributes.
 * @return SUCCESS in case of success or negative value otherwise.
 */
static ssize_t iio_buffer_delete_device_descriptor(
	struct iio_device *iio_device)
{
	uint16_t i = 0;

	if (!iio_device)
		return FAILURE;

	if (iio_device->channels) {
		while (iio_device->channels[i]) {
			if (iio_device->channels[i]->name)
				free(iio_device->channels[i]->name);
			free(iio_device->channels[i]);
			i++;
		}
		free(iio_device->channels);
	}

	return SUCCESS;
}

/**
 * @brief Create structure describing a device, channels
 * and attributes.
 * @param num_channels - number of channels.
 * @param scan_type - scan type.
 * @param iio_device - iio device.
 * @return iio_device or NULL, in case of failure.
 */
static int32_t iio_buffer_create_device_descriptor(
	uint8_t	num_channels, struct scan_type *scan_type,
	struct iio_device *iio_device)
{
	static struct scan_type scan;
	static struct iio_channel default_channel;
	int32_t i;
	int32_t ret;

	memcpy(&scan, scan_type, sizeof(scan));
	default_channel = (struct iio_channel) {
		.ch_type = IIO_VOLTAGE,
		.scan_type = &scan,
		.attributes = NULL,
		.ch_out = false,
	};

	iio_device->num_ch = num_channels;
	iio_device->attributes = NULL; /* no device attribute */
	iio_device->channels = calloc(iio_device->num_ch + 1,
				      sizeof(struct iio_channel *));
	if (!iio_device->channels)
		goto error;

	for (i = 0; i < iio_device->num_ch; i++) {
		iio_device->channels[i] = calloc(1, sizeof(struct iio_channel));
		if (!iio_device->channels[i])
			goto error;
		*(iio_device->channels[i]) = default_channel;
		iio_device->channels[i]->name = calloc(5, 1);
		if (!iio_device->channels[i]->name)
			goto error;
		iio_device->channels[i]->scan_index = i;
		ret = sprintf(iio_device->channels[i]->name, "voltage%"PRIi32"", i);
		if (ret < 0)
			goto error;
	}
	iio_device->channels[i] = NULL;

	iio_device->transfer_dev_to_mem = iio_buffer_transfer_dev_to_mem;
	iio_device->transfer_mem_to_dev = NULL;
	iio_device->read_data = iio_buffer_read_dev;
	iio_device->write_data = NULL;

	return SUCCESS;
error:
	iio_buffer_delete_device_descriptor(iio_device);

	return FAILURE;
}
/**
 * @brief Get iio device descriptor.
 * @param desc - Descriptor.
 * @param dev_descriptor - iio device descriptor.
 */
void iio_buffer_get_dev_descriptor(struct iio_buffer_desc *desc,
				   struct iio_device **dev_descriptor)
{
	*dev_descriptor = &desc->dev_descriptor;
}

/**
 * @brief Init iio buffer
 * @param desc - Descriptor.
 * @param init - Configuration structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_buffer_init(struct iio_buffer_desc **desc,
			struct iio_buffer_init_param *init)
{
	struct iio_buffer_desc *iio_buffer_inst;
	int32_t status;

	iio_buffer_inst = (struct iio_buffer_desc *)calloc(1,
			  sizeof(struct iio_buffer_desc));
	if (!iio_buffer_inst)
		return FAILURE;

	iio_buffer_inst->dev_descriptor.num_ch = init->num_channels;
	iio_buffer_inst->ddr_base = init->ddr_base;
	iio_buffer_inst->dev_instance = init->dev_instance;
	iio_buffer_inst->read_data = init->read_data;

	status = iio_buffer_create_device_descriptor(init->num_channels,
			init->scan_type,
			&iio_buffer_inst->dev_descriptor);
	if (IS_ERR_VALUE(status)) {
		free(iio_buffer_inst);
		return status;
	}
	*desc = iio_buffer_inst;

	return SUCCESS;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_buffer_remove(struct iio_buffer_desc *desc)
{
	if (!desc)
		return FAILURE;

	free(desc);

	return SUCCESS;
}
