/***************************************************************************//**
 *   @file   iio_demo.c
 *   @brief  Implementation of iio_demo.c.
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
#include <stdlib.h>
#include <string.h>
#include "iio_demo.h"
#include "error.h"
#include "xml.h"
#include "util.h"
#include "iio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

static uint32_t demo_channel_attr = 0;
static uint32_t demo_global_attr = 0;

static const char demo_xml_output[] =
	"<device id=\"demo_device_output\" name=\"demo_device_output\" >"
	"<channel id=\"altvoltage0\" name=\"TX1_I_F1\" type=\"output\" >"
	"<scan-element index=\"0\" format=\"le:S12/16&gt;&gt;0\" />"
	"</channel>"
	"<channel id=\"altvoltage1\" name=\"TX1_I_F2\" type=\"output\" >"
	"<scan-element index=\"1\" format=\"le:S12/16&gt;&gt;0\" />"
	"</channel>"
	"<channel id=\"altvoltage2\" name=\"TX1_Q_F1\" type=\"output\" >"
	"<scan-element index=\"2\" format=\"le:S12/16&gt;&gt;0\" />"
	"</channel>"
	"<channel id=\"altvoltage3\" name=\"TX1_Q_F2\" type=\"output\" >"
	"<scan-element index=\"3\" format=\"le:S12/16&gt;&gt;0\" />"
	"</channel>"
	"</device>"
	;

static const char demo_xml_input[] =
	"<device id=\"demo_device_input\" name=\"demo_device_input\" >"
	"<channel id=\"voltage0\" type=\"input\" >"
	"<scan-element index=\"0\" format=\"le:S12/16&gt;&gt;0\" />"
	"</channel>"
	"<channel id=\"voltage1\" type=\"input\" >"
	"<scan-element index=\"1\" format=\"le:S12/16&gt;&gt;0\" />"
	"</channel>"
	"<channel id=\"voltage2\" type=\"input\" >"
	"<scan-element index=\"2\" format=\"le:S12/16&gt;&gt;0\" />"
	"</channel>"
	"<channel id=\"voltage3\" type=\"input\" >"
	"<scan-element index=\"3\" format=\"le:S12/16&gt;&gt;0\" />"
	"<attribute name=\"demo_channel_attr\" filename=\"in_demo_attr\" />"
	"</channel>"
	"<attribute name=\"demo_global_attr\" filename=\"in_demo_attr\" />"
	"</device>"
	;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief get_demo_channel_attr().
 * @param device- Physical instance of a iio_demo_device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static ssize_t get_demo_channel_attr(void *device, char *buf, size_t len,
				     const struct iio_ch_info *channel)
{
	return snprintf(buf, len, "%"PRIu32"", demo_channel_attr);
}

/**
 * @brief set_demo_channel_attr().
 * @param device - Physical instance of a iio_demo_device.
 * @param buf - Value to be written to attribute.
 * @param len -	Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_demo_channel_attr(void *device, char *buf, size_t len,
				     const struct iio_ch_info *channel)
{
	demo_channel_attr = srt_to_uint32(buf);

	return len;
}

/**
 * @brief get_demo_global_attr().
 * @param device- Physical instance of a iio_demo_device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @return Length of chars written in buf, or negative value on failure.
 */
static ssize_t get_demo_global_attr(void *device, char *buf, size_t len,
				    const struct iio_ch_info *channel)
{
	return snprintf(buf, len, "%"PRIu32"", demo_global_attr);
}

/**
 * @brief set_demo_global_attr().
 * @param device - Physical instance of a iio_demo_device.
 * @param buf - Value to be written to attribute.
 * @param len -	Length of the data in "buf".
 * @param channel - Channel properties.
 * @return: Number of bytes written to device, or negative value on failure.
 */
static ssize_t set_demo_global_attr(void *device, char *buf, size_t len,
				    const struct iio_ch_info *channel)
{
	demo_global_attr = srt_to_uint32(buf);

	return len;
}

static struct iio_attribute iio_attr_demo_channel = {
	.name = "demo_channel_attr",
	.show = get_demo_channel_attr,
	.store = set_demo_channel_attr,
};

static struct iio_attribute iio_attr_demo_global = {
	.name = "demo_global_attr",
	.show = get_demo_global_attr,
	.store = set_demo_global_attr,
};

static struct iio_attribute *demo_channel_attributes[] = {
	&iio_attr_demo_channel,
	NULL,
};

static struct iio_channel iio_demo_channel_voltage0_in = {
	.name = "voltage3",
	.attributes = demo_channel_attributes,
	.ch_out = false,
};

static struct iio_channel *iio_demo_channels[] = {
	&iio_demo_channel_voltage0_in,
	NULL,
};

static struct iio_attribute *iio_demo_global_attributes[] = {
	&iio_attr_demo_global,
	NULL,
};

/**
 * @brief Create structure describing a device, channels and attributes.
 * @param device_name - Device name.
 * @param num_ch - Number of channels that the device has.
 * @return iio_device or NULL, in case of failure.
 */
static struct iio_device *iio_demo_create_device(const char *device_name,
		uint16_t num_ch)
{
	struct iio_device *iio_device;

	iio_device = (struct iio_device *)calloc(1, sizeof(*iio_device));
	if (!iio_device)
		return NULL;

	iio_device->name = device_name;
	iio_device->num_ch = num_ch;
	iio_device->attributes = iio_demo_global_attributes;
	iio_device->channels = iio_demo_channels;

	return iio_device;
}

/**
 * @brief Delete iio_device.
 * @param iio_device - Structure describing a device, channels and attributes.
 * @return SUCCESS in case of success or negative value otherwise.
 */
static ssize_t iio_demo_delete_device(struct iio_device *iio_device)
{
	if(iio_device)
		free(iio_device);

	return SUCCESS;
}

/**
 * @brief Transfer data from RAM to device.
 * @param iio_inst - Physical instance of a iio_demo_dac device.
 * @param bytes_count - Number of bytes to transfer.
 * @param ch_mask - Opened channels mask.
 * @return Number of bytes transfered, or negative value in case of failure.
 */
static ssize_t iio_demo_transfer_mem_to_dev(void *iio_inst,
		size_t bytes_count,
		uint32_t ch_mask)
{
	struct iio_demo_device *demo_device;
	demo_device = (struct iio_demo_device *)iio_inst;
	if (!demo_device)
		return FAILURE;

	return bytes_count;
}

/**
 * @brief Transfer data from device into RAM.
 * @param iio_inst - Physical instance of a device.
 * @param bytes_count - Number of bytes to transfer.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
static ssize_t iio_demo_transfer_dev_to_mem(void *iio_inst,
		size_t bytes_count,
		uint32_t ch_mask)
{
	struct iio_demo_device *demo_device;
	demo_device = (struct iio_demo_device *)iio_inst;
	if (!demo_device)
		return FAILURE;

	/* In some cases a transfer is necessary before a "iio_demo_read_dev"
	 * function is called. In this case an implementation should be provided
	 * in this location. For an example check "iio_axi_adc" module, where
	 * data is transfered from device to RAM memory by DMA.
	 */
	return bytes_count;
}

/**
 * @brief Write chunk of data into RAM.
 * This function is probably called multiple times by libtinyiiod before a
 * "iio_transfer_mem_to_dev" call, since we can only write "bytes_count" bytes
 * at a time.
 * @param iio_inst - Physical instance of a iio_demo_dac device.
 * @param buf - Values to write.
 * @param offset - Offset in memory after the nth chunk of data.
 * @param bytes_count - Number of bytes to write.
 * @param ch_mask - Opened channels mask.
 * @return bytes_count or negative value in case of error.
 */
static ssize_t iio_demo_write_dev(void *iio_inst, char *buf,
				  size_t offset,  size_t bytes_count, uint32_t ch_mask)
{
	struct iio_demo_device *demo_device;
	uint32_t index, addr;
	uint16_t *buf16;

	if (!iio_inst)
		return FAILURE;

	if (!buf)
		return FAILURE;

	buf16 = (uint16_t *)buf;
	demo_device = (struct iio_demo_device *)iio_inst;
	addr = demo_device->ddr_base_addr + offset;
	for(index = 0; index < bytes_count; index += 2) {
		uint32_t *local_addr = (uint32_t *)(addr +
						    (index * 2) % demo_device->ddr_base_size);
		*local_addr = (buf16[index + 1] << 16) | buf16[index];
	}


	return bytes_count;
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
static ssize_t iio_demo_read_dev(void *iio_inst, char *pbuf, size_t offset,
				 size_t bytes_count, uint32_t ch_mask)
{
	struct iio_demo_device *demo_device;
	uint32_t i, j = 0, current_ch = 0;
	uint16_t *pbuf16;
	size_t samples;

	if (!iio_inst)
		return FAILURE;

	if (!pbuf)
		return FAILURE;

	demo_device = (struct iio_demo_device *)iio_inst;
	pbuf16 = (uint16_t*)pbuf;
	samples = (bytes_count * demo_device->num_channels) / hweight8(
			  ch_mask);
	samples /= 2; /* because of uint16_t *pbuf16 = (uint16_t*)pbuf; */
	offset = (offset * demo_device->num_channels) / hweight8(ch_mask);

	for (i = 0; i < samples; i++) {

		if (ch_mask & BIT(current_ch)) {
			pbuf16[j] = *(uint16_t*)(demo_device->ddr_base_addr +
						 (offset + i * 2) % demo_device->ddr_base_size);

			j++;
		}

		if (current_ch + 1 < demo_device->num_channels)
			current_ch++;
		else
			current_ch = 0;
	}

	return bytes_count;
}

/**
 * @brief Get xml corresponding to device.
 * @param xml - Xml containing description of a device.
 * @param iio_dev - Structure describing a device, channels and attributes.
 * @return SUCCESS in case of success or negative value otherwise.
 */
static ssize_t iio_demo_get_xml(char** xml, struct iio_device *iio_dev)
{
	const char *dev_xml;

	if (strstr(demo_xml_input, iio_dev->name))
		dev_xml = demo_xml_input;
	else if (strstr(demo_xml_output, iio_dev->name))
		dev_xml = demo_xml_output;
	else
		return FAILURE;

	*xml = (char*)calloc(1, strlen(dev_xml) + 1);
	if (!(*xml))
		return FAILURE;

	memcpy(*xml, dev_xml, strlen(dev_xml));

	return SUCCESS;
}

/**
 * @brief iio demo init function, registers a demo .
 * @param desc - Descriptor.
 * @param init - Configuration structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_demo_init(struct iio_demo_desc **desc,
		      struct iio_demo_init_param *init)
{
	struct iio_interface *iio_interface;
	struct iio_device * iio_device;
	int32_t status;
	struct iio_demo_device *iio_demo_device_inst;

	iio_demo_device_inst = (struct iio_demo_device *)calloc(
				       1,
				       sizeof(*iio_demo_device_inst));
	if (!iio_demo_device_inst)
		return FAILURE;

	iio_demo_device_inst->name = init->name;
	iio_demo_device_inst->num_channels = init->num_channels;
	iio_demo_device_inst->ddr_base_addr = init->ddr_base_addr;
	iio_demo_device_inst->ddr_base_size = init->ddr_base_size;

	iio_device = iio_demo_create_device(init->name,
					    iio_demo_device_inst->num_channels);
	if (!iio_device)
		goto error_free_iio_demo_inst;

	iio_interface = (struct iio_interface *)calloc(1,
			sizeof(*iio_interface));
	if (!iio_interface)
		goto error_free_iio_demo_delete_dev;

	*iio_interface = (struct iio_interface) {
		.name = init->name,
		.dev_instance = iio_demo_device_inst,
		.iio = iio_device,
		.get_xml = iio_demo_get_xml,
		.transfer_dev_to_mem = iio_demo_transfer_dev_to_mem,
		.read_data = iio_demo_read_dev,
		.transfer_mem_to_dev = iio_demo_transfer_mem_to_dev,
		.write_data = iio_demo_write_dev,
	};

	status = iio_register(init->iio_desc, iio_interface);
	if (status < 0)
		goto error_free_iio_interface;

	return SUCCESS;

error_free_iio_interface:
	free(iio_interface);
error_free_iio_demo_delete_dev:
	iio_demo_delete_device(iio_device);
error_free_iio_demo_inst:
	free(iio_demo_device_inst);

	return FAILURE;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t iio_demo_remove(struct iio_demo_desc *desc)
{
	int32_t status;

	if (!desc)
		return FAILURE;

	status = iio_unregister(desc->iio_desc, desc->iio_interface);
	if(status < 0)
		return FAILURE;

	status = iio_demo_delete_device(desc->iio_interface->iio);
	if (status < 0)
		return FAILURE;

	free(desc->iio_interface->dev_instance);
	free(desc->iio_interface);
	free(desc);

	return SUCCESS;
}
