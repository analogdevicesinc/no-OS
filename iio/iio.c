/***************************************************************************//**
 *   @file   iio.c
 *   @brief  Implementation of iio
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include "iio.h"
#include "ctype.h"
#include "tinyiiod.h"
#include "util.h"
#include "error.h"
#include "iio_axi_adc.h"
#include "iio_axi_dac.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

struct iio_interface {
	const char *name;
	uint16_t num_channels;
	uint32_t ch_mask;
	void *dev_instance;
	struct iio_device *iio;
	ssize_t (*get_device_xml)(char **xml, const char *device_name, uint8_t ch_no);
};

struct iio_interfaces {
	struct iio_interface **interfaces;
	uint8_t num_interfaces;
};

struct element_info {
	const char *device_name;
	const char *channel_name;
	const char *attribute_name;
	bool ch_out;
};

/**
 * iio_read_attr(), iio_write_attr() functions, they need to know about iio_interfaces
 */
static struct iio_interfaces *iio_interfaces = NULL;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * Get channel number
 * @param *ch channel name + channel number
 * @return channel number Ex: for "altvoltage0" return 0, for "voltage2" return 2
 */
static int32_t iio_get_channel_number(const char *ch)
{
	char *p = (char*)ch;
	int32_t ch_num = 0;

	while (*p) {
		if (isdigit(*p))
			ch_num = strtol(p, &p, 10);
		else
			p++;
	}

	return ch_num;
}

/**
 * Get attribute ID from map based on attribute name
 * @param attr* attribute name
 * @param *map pointer to map terminated with NULL element
 * @param map_size map_size element numbers of the map
 * @return attribute ID, or negative value if attribute is not found
 */
static int16_t iio_get_channel_id(const char *channel,
				  struct iio_channel **channels)
{
	int16_t i = 0;

	if (!(*channels))
		return -EINVAL;

	while (channels[i]) {
		if (!strcmp(channel, channels[i]->name))
			return i;
		i++;
	}

	return -ENOENT;
}

/**
 * Get attribute ID from map based on attribute name
 * @param attr* attribute name
 * @param *map pointer to map terminated with NULL element
 * @param map_size map_size element numbers of the map
 * @return attribute ID, or negative value if attribute is not found
 */
static int16_t iio_get_attribute_id(const char *attr,
				    struct iio_attribute **attributes)
{
	int16_t i = 0;

	if (!(*attributes))
		return -EINVAL;

	while (attributes[i]) {
		if (!strcmp(attr, attributes[i]->name))
			return i;
		i++;
	}

	return -ENOENT;
}

/**
 * Find interface with "device_name".
 * @param @device_name.
 * @param iio_interfaces - list of interfaces.
 * @return interface pointer if interface is found, NULL otherwise.
 */
static struct iio_interface *iio_get_interface(const char *device_name,
		struct iio_interfaces *iio_interfaces)
{
	int16_t i;

	for (i = 0; i < iio_interfaces->num_interfaces; i++) {
		if (!strcmp(device_name, iio_interfaces->interfaces[i]->name))
			return iio_interfaces->interfaces[i];
	}

	return NULL;
}

/**
 * read all attributes from map
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @param *map map with coresponding attributes
 * @param map_size
 * @return length of chars written in buf
 */
static ssize_t iio_read_all_attr(void *device, char *buf, size_t len,
				 const struct iio_ch_info *channel, struct iio_attribute **attributes)
{
	int16_t i = 0, j = 0;
	char local_buf[256];
	ssize_t attr_length;
	uint32_t *pattr_length;

	while (attributes[i]) {
		attr_length = attributes[i]->show(device, local_buf, len, channel);
		pattr_length = (uint32_t *)(buf + j);
		*pattr_length = bswap_constant_32(attr_length);
		j += 4;
		if (attr_length >= 0) {
			sprintf(buf + j, "%s", local_buf);
			if (attr_length & 0x3) /* multiple of 4 */
				attr_length = ((attr_length >> 2) + 1) << 2;
			j += attr_length;
		}
		i++;
	}

	return j;
}

/**
 * write all attributes from map
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @param *map map with coresponding attributes
 * @param map_size
 * @return length of chars written in buf
 */
static ssize_t iio_write_all_attr(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel, struct iio_attribute **attributes)
{
	int16_t i = 0, j = 0;
	int16_t attr_length;

	while (attributes[i]) {
		attr_length = bswap_constant_32((uint32_t)(buf + j));
		j += 4;
		attributes[i]->store(device, (buf + j), attr_length, channel);
		j += attr_length;
		if (j & 0x3)
			j = ((j >> 2) + 1) << 2;
		i++;
	}

	return len;
}

/**
 * read/write channel attribute.
 * @param *el_info.
 * @param *buf, read / write value.
 * @param len.
 * @param *channel.
 * @param is_write
 * @return length of chars written in buf or negative value in case of error
 */
static ssize_t iio_rd_wr_channel_attribute(struct element_info *el_info,
		char *buf, size_t len,
		struct iio_channel *channel, bool is_write)
{
	int16_t attribute_id;
	struct iio_interface *device = iio_get_interface(el_info->device_name,
				       iio_interfaces);
	const struct iio_ch_info channel_info = {
		iio_get_channel_number(el_info->channel_name),
		el_info->ch_out
	};

	if (!strcmp(el_info->attribute_name, "")) {
		/* read / write all channel attributes */
		if (is_write)
			return iio_write_all_attr(device->dev_instance, buf, len, &channel_info,
						  channel->attributes);
		else
			return iio_read_all_attr(device->dev_instance, buf, len, &channel_info,
						 channel->attributes);
	} else {
		/* read / write single channel attribute, if attribute found */
		attribute_id = iio_get_attribute_id(el_info->attribute_name,
						    channel->attributes);
		if (attribute_id >= 0) {
			if (is_write)
				return channel->attributes[attribute_id]->store(device->dev_instance,
						(char*)buf, len, &channel_info);
			else
				return channel->attributes[attribute_id]->show(device->dev_instance, (char*)buf,
						len, &channel_info);
		}
	}

	return -ENOENT;
}

/**
 * read write attribute
 * @param *device name
 * @param *channel name
 * @param *ch_out type: input/output
 * @param *attr name
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @return length of chars written in buf
 */
static ssize_t iio_rd_wr_attribute(struct element_info *el_info, char *buf,
				   size_t len,
				   struct iio_device *iio_device, bool is_write)
{
	int16_t channel_id;
	int16_t attribute_id;
	struct iio_interface *device;

	if (!iio_device)
		return -ENOENT;

	if (!strcmp(el_info->channel_name, "")) {
		/* it is attribute of a device */
		device = iio_get_interface(el_info->device_name, iio_interfaces);

		if (!strcmp(el_info->attribute_name, "")) {
			/* read / write all device attributes */
			if (is_write)
				return iio_write_all_attr(device->dev_instance, buf, len, NULL,
							  iio_device->attributes);
			else
				return iio_read_all_attr(device->dev_instance, buf, len, NULL,
							 iio_device->attributes);
		} else {
			/* read / write single device attribute, if attribute found */
			attribute_id = iio_get_attribute_id(el_info->attribute_name,
							    iio_device->attributes);
			if (attribute_id < 0)
				return -ENOENT;
			if (is_write)
				return iio_device->attributes[attribute_id]->store(device->dev_instance,
						(char*)buf, len, NULL);
			else
				return iio_device->attributes[attribute_id]->show(device->dev_instance,
						(char*)buf, len, NULL);
		}
	} else {
		/* it is attribute of a channel */
		channel_id = iio_get_channel_id(el_info->channel_name, iio_device->channels);
		return iio_rd_wr_channel_attribute(el_info, buf, len,
						   iio_device->channels[channel_id], is_write);
	}

	return -ENOENT;
}

/**
 * Check device
 * @param *device
 * @return TRUE if valid device is found
 */
static bool iio_supporter_dev(const char *device)
{
	return (NULL != iio_get_interface(device, iio_interfaces));
}

/**
 * read global attribute
 * @param *device name
 * @param *attr name
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param debug
 * @return length of chars written in buf
 */
static ssize_t iio_read_attr(const char *device, const char *attr, char *buf,
			     size_t len, bool debug)
{
	struct iio_interface *iiod_device;
	struct element_info el_info;

	if (!iio_supporter_dev(device))
		return FAILURE;

	el_info.device_name = device;
	el_info.channel_name = "";
	el_info.attribute_name = attr;
	el_info.ch_out = 0; 	/* set to 0, there is no channel here */

	iiod_device = iio_get_interface(device, iio_interfaces);
	if (!iiod_device)
		return FAILURE;

	return iio_rd_wr_attribute(&el_info, buf, len, iiod_device->iio, 0);
}

/**
 * write global attribute
 * @param *device name
 * @param *attr name
 * @param *buff value to write
 * @param len maximum length of value to be stored in buf
 * @param debug
 * @return length of chars written in buf
 */
static ssize_t iio_write_attr(const char *device, const char *attr,
			      const char *buf,
			      size_t len, bool debug)
{
	struct element_info el_info;
	struct iio_interface *iiod_device;

	if (!iio_supporter_dev(device))
		return -ENODEV;

	el_info.device_name = device;
	el_info.channel_name = "";
	el_info.attribute_name = attr;
	el_info.ch_out = 0;		/* set to 0, there is no channel here */

	iiod_device = iio_get_interface(device, iio_interfaces);
	if (!iiod_device)
		return FAILURE;

	return iio_rd_wr_attribute(&el_info, (char*)buf, len, iiod_device->iio, 1);
}

/**
 * read channel attribute
 * @param *device name
 * @param *channel name
 * @param *ch_out type: input/output
 * @param *attr name
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @return length of chars written in buf
 */
static ssize_t iio_ch_read_attr(const char *device, const char *channel,
				bool ch_out, const char *attr, char *buf, size_t len)
{
	struct element_info el_info;
	struct iio_interface *iiod_device;

	if (!iio_supporter_dev(device))
		return FAILURE;

	el_info.device_name = device;
	el_info.channel_name = channel;
	el_info.attribute_name = attr;
	el_info.ch_out = ch_out;

	iiod_device = iio_get_interface(device, iio_interfaces);
	if (!device)
		return FAILURE;

	return iio_rd_wr_attribute(&el_info, buf, len, iiod_device->iio, 0);
}

///**
// * write channel attribute
// * @param *device name
// * @param *channel name
// * @param *ch_out type: input/output
// * @param *attr name
// * @param *buff where value is stored
// * @param len length of the value
// * @return length of chars written to attribute, negative value in case of failure
// */
static ssize_t iio_ch_write_attr(const char *device, const char *channel,
				 bool ch_out, const char *attr, const char *buf, size_t len)
{
	struct element_info el_info;
	struct iio_interface *iiod_device;

	if (!iio_supporter_dev(device))
		return -ENODEV;

	el_info.device_name = device;
	el_info.channel_name = channel;
	el_info.attribute_name = attr;
	el_info.ch_out = ch_out;
	iiod_device = iio_get_interface(device, iio_interfaces);

	if (!iiod_device)
		return -ENOENT;

	return iio_rd_wr_attribute(&el_info, (char*)buf, len, iiod_device->iio, 1);
}

/**
 * open device
 * @param *device name.
 * @param *sample_size.
 * @param *mask of channels to be opened.
 * @return SUCCESS, negative value in case of failure.
 */
static int32_t iio_open_dev(const char *device, size_t sample_size,
			    uint32_t mask)
{
	struct iio_interface *dev;
	uint32_t ch_mask;

	if (!iio_supporter_dev(device))
		return -ENODEV;

	dev = iio_get_interface(device, iio_interfaces);
	ch_mask = 0xFFFFFFFF >> (32 - dev->num_channels);

	if (mask & ~ch_mask)
		return -ENOENT;

	dev->ch_mask = ch_mask;

	return SUCCESS;
}

/**
 * close device
 * @param *device name
 * @return 0 on success, otherwise negative value
 */
static int32_t iio_close_dev(const char *device)
{
	return iio_supporter_dev(device) ? SUCCESS : FAILURE;
}

/**
 * get device mask, this specifies the channels that are used
 * @param *device name
 * @param *device mask
 * @return 0 on success, otherwise negative value
 */
static int32_t iio_get_mask(const char *device, uint32_t *mask)
{
	struct iio_interface *dev;
	uint32_t ch_mask;

	if (!iio_supporter_dev(device))
		return -ENODEV;

	dev = iio_get_interface(device, iio_interfaces);
	ch_mask = 0xFFFFFFFF >> (32 - dev->num_channels);
	*mask = ch_mask;

	return SUCCESS;
}
/**
 * transfer_dev_to_mem data from ADC into RAM
 * @param *device name
 * @param bytes_count
 * @return bytes_count
 */
static ssize_t iio_transfer_dev_to_mem(const char *device, size_t bytes_count)
{
	struct iio_interface *iiod_device = iio_get_interface(device, iio_interfaces);
	struct iio_axi_adc *iiod_adc;

	if (!iiod_device)
		return -ENOENT;
	iiod_adc = (struct iio_axi_adc *)(iiod_device->dev_instance);

	return iio_axi_adc_transfer_dev_to_mem(iiod_adc, bytes_count);
}

/**
 * read data from RAM to pbuf, use "capture()" first
 * @param *device name
 * @param *buff where data's are stored
 * @param *offset to the remaining data
 * @param bytes_count
 * @return bytes_count
 */
static ssize_t iio_read_dev(const char *device, char *pbuf, size_t offset,
			    size_t bytes_count)
{
	struct iio_interface *iiod_device = iio_get_interface(device, iio_interfaces);
	struct iio_axi_adc *iiod_adc;

	if (!iiod_device)
		return -ENOENT;

	iiod_adc = (struct iio_axi_adc *)(iiod_device->dev_instance);

	return iio_axi_adc_read_dev((char*)iiod_adc->adc_ddr_base, pbuf, offset,
				    bytes_count);
}

/**
 * transfer_mem_to_dev write data to DAC
 * @param *device name
 * @param *buff
 * @param bytes_count
 * @return bytes_count
 */
static ssize_t iio_transfer_mem_to_dev(const char *device, size_t bytes_count)
{
	struct iio_interface *iiod_device = iio_get_interface(device, iio_interfaces);
	struct iio_axi_dac *iiod_dac;

	if (!device)
		return -ENOENT;

	iiod_dac = (struct iio_axi_dac *)(iiod_device->dev_instance);

	return iio_axi_dac_transfer_mem_to_dev(iiod_dac->dmac, iiod_dac->dac_ddr_base,
					       bytes_count);
}

/**
 * write data to RAM
 * @param *device name
 * @param *buff
 * @param *offset in memory, used if some data have been already written
 * @param bytes_count
 * @return bytes_count
 */
static ssize_t iio_write_dev(const char *device, const char *buf,
			     size_t offset,  size_t bytes_count)
{
	struct iio_interface *iiod_device = iio_get_interface(device, iio_interfaces);
	struct iio_axi_dac *iiod_dac;

	if (!device)
		return -ENOENT;

	iiod_dac = (struct iio_axi_dac *)(iiod_device->dev_instance);

	return iio_axi_dac_write_dev(iiod_dac, buf, offset, bytes_count);
}

/**
 * @brief Get a merged xml containing all devices.
 * @param devs - physical device instance.
 * @param outxml - device name.
 * @return SUCCESS in case of success or negative value otherwise.
 */
static ssize_t iio_get_xml(char **outxml)
{
	char *xml, *tmp_xml, *tmp_xml2;
	uint32_t length;
	uint16_t i;

	char header[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
			"<!DOCTYPE context ["
			"<!ELEMENT context (device | context-attribute)*>"
			"<!ELEMENT context-attribute EMPTY>"
			"<!ELEMENT device (channel | attribute | debug-attribute | buffer-attribute)*>"
			"<!ELEMENT channel (scan-element?, attribute*)>"
			"<!ELEMENT attribute EMPTY>"
			"<!ELEMENT scan-element EMPTY>"
			"<!ELEMENT debug-attribute EMPTY>"
			"<!ELEMENT buffer-attribute EMPTY>"
			"<!ATTLIST context name CDATA #REQUIRED description CDATA #IMPLIED>"
			"<!ATTLIST context-attribute name CDATA #REQUIRED value CDATA #REQUIRED>"
			"<!ATTLIST device id CDATA #REQUIRED name CDATA #IMPLIED>"
			"<!ATTLIST channel id CDATA #REQUIRED type (input|output) #REQUIRED name CDATA #IMPLIED>"
			"<!ATTLIST scan-element index CDATA #REQUIRED format CDATA #REQUIRED scale CDATA #IMPLIED>"
			"<!ATTLIST attribute name CDATA #REQUIRED filename CDATA #IMPLIED>"
			"<!ATTLIST debug-attribute name CDATA #REQUIRED>"
			"<!ATTLIST buffer-attribute name CDATA #REQUIRED>"
			"]>"
			"<context name=\"xml\" description=\"Linux analog 4.9.0-g2398d50 #189 SMP PREEMPT Tue Jun 26 09:52:32 IST 2018 armv7l\" >"
			"<context-attribute name=\"local,kernel\" value=\"4.9.0-g2398d50\" />";
	char header_end[] = "</context>";

	xml = calloc(1, strlen(header) + 1);
	if (!xml)
		return FAILURE;

	strcpy(xml, header);
	for (i = 0; i < iio_interfaces->num_interfaces; i++) {
		iio_interfaces->interfaces[i]->get_device_xml(&tmp_xml,
				iio_interfaces->interfaces[i]->name,
				iio_interfaces->interfaces[i]->num_channels);
		length = strlen(xml);
		tmp_xml2 = realloc(xml, strlen(xml) + strlen(tmp_xml) + 1);
		if (!tmp_xml2)
			goto error;

		xml = tmp_xml2;
		strcpy((xml + length), tmp_xml);
	}

	length = strlen(xml);
	tmp_xml = realloc(xml, strlen(xml) + strlen(header_end) + 1);
	if (!tmp_xml)
		goto error;

	xml = tmp_xml;
	strcpy((xml + length), header_end);

	*outxml = xml;

	return SUCCESS;
error:
	free(xml);

	return FAILURE;
}

/**
 * @brief Connect "iio_dev" and "get_device_xml" to a "dev_instance" physical device.
 * @param dev_instance - physical device instance.
 * @param dev_name - device name.
 * @param num_ch - number of channels.
 * @param get_device_xml - get corresponding xml.
 * @param iio_device.
 * @return SUCCESS in case of success or negative value otherwise.
 */
ssize_t iio_register(void* dev_instance, const char *dev_name, uint16_t num_ch,
		     ssize_t (*get_device_xml)(char** xml, const char *dev_name, uint8_t num_ch),
		     struct iio_device *iio_device)
{
	struct iio_interface *iio_interface;

	if (!(iio_interfaces)) {
		iio_interfaces = calloc(1, sizeof(struct iio_interfaces));
		if (!iio_interfaces)
			return -ENOMEM;

		iio_interfaces->num_interfaces = 1;
		iio_interfaces->interfaces = calloc(1, sizeof(struct iio_interface*));
		if (!iio_interfaces->interfaces)
			return -ENOMEM;
	} else {
		iio_interfaces->num_interfaces++;
		iio_interfaces->interfaces = realloc(iio_interfaces->interfaces,
						     iio_interfaces->num_interfaces * sizeof(struct iio_interface*));
		if (!iio_interfaces->interfaces)
			return -ENOMEM;
	}
	iio_interface = calloc(1, sizeof(struct iio_interface));
	if (!iio_interface)
		return -ENOMEM;

	iio_interface->dev_instance = dev_instance;
	iio_interface->name = dev_name;
	iio_interface->num_channels = num_ch;
	iio_interface->get_device_xml = get_device_xml;
	iio_interface->iio = iio_device;

	iio_interfaces->interfaces[iio_interfaces->num_interfaces - 1] = iio_interface;

	return SUCCESS;
}

/**
 * @brief Set communication ops and read/write ops that will be called from
 * "libtinyiiod"
 * @param iiod - pointer to iiod.
 * @param iio_server_ops - read/write ops (Ex: read/write to UART).
 * @return SUCCESS in case of success or negative value otherwise.
 */
ssize_t iio_init(struct tinyiiod **iiod, struct iio_server_ops *iio_server_ops)
{
	struct tinyiiod_ops *ops = calloc(1, sizeof(struct tinyiiod_ops));

	if (!ops)
		return FAILURE;

	/* device operations */
	ops->read_attr = iio_read_attr;
	ops->write_attr = iio_write_attr;
	ops->ch_read_attr = iio_ch_read_attr;
	ops->ch_write_attr = iio_ch_write_attr;
	ops->transfer_dev_to_mem = iio_transfer_dev_to_mem;
	ops->read_data = iio_read_dev;
	ops->transfer_mem_to_dev = iio_transfer_mem_to_dev;
	ops->write_data = iio_write_dev;

	ops->open = iio_open_dev;
	ops->close = iio_close_dev;
	ops->get_mask = iio_get_mask;

	ops->read = iio_server_ops->read;
	ops->write = iio_server_ops->write;
	ops->get_xml = iio_get_xml;

	*iiod = tinyiiod_create(ops);
	if (!(*iiod))
		goto error_free_ops;

	return SUCCESS;

error_free_ops:
	free(ops);

	return FAILURE;
}

/**
 * @brief Free the resources allocated by iio_init().
 * @param iiod - pointer to iiod.
 * @return SUCCESS in case of success or negative value otherwise.
 */
ssize_t iio_remove(struct tinyiiod *iiod)
{
	uint8_t i;

	for (i = 0; i < iio_interfaces->num_interfaces; i++) {
		free(iio_interfaces->interfaces[i]);
	}
	free(iio_interfaces);
	tinyiiod_destroy(iiod);

	return SUCCESS;
}
