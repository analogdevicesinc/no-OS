/***************************************************************************//**
 *   @file   iio.c
 *   @brief  Implementation of iio.
 *   This module implements read/write ops, required by libtinyiiod and further
 *   calls show/store functions, corresponding to device/channel/attribute.
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

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * struct iio_interface - Links a physical device instance "void *dev_instance"
 * with a "iio_device *iio" that describes capabilities of the device.
 * @name:			Device name.
 * @ch_mask:			Opened channels.
 * @dev_instance:		Physical instance of a device.
 * @iio:			Device descriptor(describes channels and
 *				attributes).
 * @get_xml:			Generate device xml.
 * @transfer_dev_to_mem:	Transfer data from device into RAM.
 * @read_data:			Read data from RAM to pbuf. It should be called
 *				after "transfer_dev_to_mem".
 * @transfer_mem_to_dev:	Transfer data from RAM to device.
 * @write_data:			Write data to RAM. It should be called before
 *				"transfer_mem_to_dev".
 */
struct iio_interface {
	const char *name;
	uint32_t ch_mask;
	void *dev_instance;
	struct iio_device *iio;
	ssize_t (*get_xml)(char **xml, struct iio_device *iio);
	ssize_t (*transfer_dev_to_mem)(void *dev_instance, size_t bytes_count,
				       uint32_t ch_mask);
	ssize_t (*read_data)(void *dev_instance, char *pbuf, size_t offset,
			     size_t bytes_count, uint32_t ch_mask);
	ssize_t (*transfer_mem_to_dev)(void *dev_instance, size_t bytes_count,
				       uint32_t ch_mask);
	ssize_t (*write_data)(void *dev_instance, char *pbuf, size_t offset,
			      size_t bytes_count, uint32_t ch_mask);
};

/**
 * struct iio_interfaces - Structure containing all interfaces.
 * @interfaces:		List containing all interfaces.
 * @num_interfaces:	Number of Interfaces.
 */
struct iio_interfaces {
	struct iio_interface **interfaces;
	uint8_t num_interfaces;
};

/**
 * struct element_info - Structure informations about a specific parameter.
 * @device_name:	Device name.
 * @channel_name:	Channel name.
 * @attribute_name:	Attribute name.
 * @ch_out:		If set, is an output channel.
 */
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
 * iio_get_channel_number() - Get channel number.
 * @ch: String containing channel name + channel number.
 * Return: Channel number. Ex: for "altvoltage0" return 0, for "voltage2"
 * return 2.
 */
static int32_t iio_get_channel_number(const char *ch)
{
	char *p = (char*)ch;
	int32_t ch_num = FAILURE;

	while (*p) {
		if (isdigit(*p))
			ch_num = strtol(p, &p, 10);
		else
			p++;
	}

	return ch_num;
}

/**
 * iio_get_channel_id() - Get channel ID from a list of channels.
 * @channel:	Channel name.
 * @channels:	List of channels.
 * @ch_out:	If "true" is output channel, if "false" is input channel.
 * Return: Channel ID, or negative value if attribute is not found.
 */
static int16_t iio_get_channel_id(const char *channel,
				  struct iio_channel **channels, bool ch_out)
{
	int16_t i = 0;

	if (!(*channels))
		return -EINVAL;

	while (channels[i]) {
		if (!strcmp(channel, channels[i]->name) && (channels[i]->ch_out == ch_out))
			return i;
		i++;
	}

	return -ENOENT;
}

/**
 * iio_get_attribute_id() - Get attribute ID from a list of attributes.
 * @attr:	Attribute name.
 * @attributes:	List of attributes.
 * Return: Attribute ID, or negative value if attribute is not found.
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
 * iio_get_interface() - Find interface with "device_name".
 * @device_name:	Device name.
 * @iio_interfaces:	List of interfaces.
 * Return: Interface pointer if interface is found, NULL otherwise.
 */
static struct iio_interface *iio_get_interface(const char *device_name,
		struct iio_interfaces *iio_interfaces)
{
	int16_t i;

	if (!iio_interfaces)
		return NULL;

	for (i = 0; i < iio_interfaces->num_interfaces; i++) {
		if (!strcmp(device_name, iio_interfaces->interfaces[i]->name))
			return iio_interfaces->interfaces[i];
	}

	return NULL;
}

/**
 * iio_read_all_attr() - Read all attributes from an attribute list.
 * @device:		Physical instance of a device.
 * @buf:		Buffer where values are read.
 * @len:		Maximum length of value to be stored in buf.
 * @channel:		Channel properties.
 * @attributes:		List of attributes to be read.
 * Return: Number of bytes read or negative value in case of error.
 */
static ssize_t iio_read_all_attr(void *device, char *buf, size_t len,
				 const struct iio_ch_info *channel, struct iio_attribute **attributes)
{
	int16_t i = 0, j = 0;
	char local_buf[256];
	ssize_t attr_length;
	uint32_t *pattr_length;

	if (!attributes)
		return FAILURE;

	if (!buf)
		return FAILURE;

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
 * iio_write_all_attr() - Write all attributes from an attribute list.
 * @device:	Physical instance of a device.
 * @buf:	Values to be written.
 * @len:	Length of buf.
 * @channel:	Channel properties.
 * @attributes:	List of attributes to be written.
 * Return: Number of written bytes or negative value in case of error.
 */
static ssize_t iio_write_all_attr(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel, struct iio_attribute **attributes)
{
	int16_t i = 0, j = 0;
	int16_t attr_length;

	if (!attributes)
		return FAILURE;

	if (!buf)
		return FAILURE;

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
 * iio_rd_wr_channel_attribute() - Read/write channel attribute.
 * @el_info:	Structure describing element to be written.
 * @buf:	Read/write value.
 * @len:	Length of data in "buf" parameter.
 * @channel: 	Structure describing channel attributes.
 * @is_write:	If it has value "1", writes attribute, otherwise reads
 * 		attribute.
 * Return: Length of chars written/read or negative value in case of error.
 */
static ssize_t iio_rd_wr_channel_attribute(struct element_info *el_info,
		char *buf, size_t len,
		struct iio_channel *channel, bool is_write)
{
	int16_t attribute_id;
	struct iio_interface *iface = iio_get_interface(el_info->device_name,
				      iio_interfaces);
	const struct iio_ch_info channel_info = {
		iio_get_channel_number(el_info->channel_name),
		el_info->ch_out
	};

	if (!strcmp(el_info->attribute_name, "")) {
		/* read / write all channel attributes */
		if (is_write)
			return iio_write_all_attr(iface->dev_instance, buf, len, &channel_info,
						  channel->attributes);
		else
			return iio_read_all_attr(iface->dev_instance, buf, len, &channel_info,
						 channel->attributes);
	} else {
		/* read / write single channel attribute, if attribute found */
		attribute_id = iio_get_attribute_id(el_info->attribute_name,
						    channel->attributes);
		if (attribute_id >= 0) {
			if (is_write)
				return channel->attributes[attribute_id]->store(iface->dev_instance,
						(char*)buf, len, &channel_info);
			else
				return channel->attributes[attribute_id]->show(iface->dev_instance, (char*)buf,
						len, &channel_info);
		}
	}

	return -ENOENT;
}

/**
 * iio_rd_wr_attribute() - Read/write attribute.
 * @el_info:	Structure describing element to be written.
 * @buf:	Read/write value.
 * @len:	Length of data in "buf" parameter.
 * @iio_device:	Physical instance of a device.
 * @is_write:	If it has value "1", writes attribute, otherwise reads
 * 		attribute.
 * Return: Length of chars written/read or negative value in case of error.
 */
static ssize_t iio_rd_wr_attribute(struct element_info *el_info, char *buf,
				   size_t len,
				   struct iio_device *iio_device, bool is_write)
{
	int16_t channel_id;
	int16_t attribute_id;
	struct iio_interface *iface;

	if (!iio_device)
		return -ENOENT;

	if (!strcmp(el_info->channel_name, "")) {
		/* it is attribute of a device */
		iface = iio_get_interface(el_info->device_name, iio_interfaces);

		if (!strcmp(el_info->attribute_name, "")) {
			/* read / write all device attributes */
			if (is_write)
				return iio_write_all_attr(iface->dev_instance, buf, len, NULL,
							  iio_device->attributes);
			else
				return iio_read_all_attr(iface->dev_instance, buf, len, NULL,
							 iio_device->attributes);
		} else {
			/* read / write single device attribute, if attribute found */
			attribute_id = iio_get_attribute_id(el_info->attribute_name,
							    iio_device->attributes);
			if (attribute_id < 0)
				return -ENOENT;
			if (is_write)
				return iio_device->attributes[attribute_id]->store(iface->dev_instance,
						(char*)buf, len, NULL);
			else
				return iio_device->attributes[attribute_id]->show(iface->dev_instance,
						(char*)buf, len, NULL);
		}
	} else {
		/* it is attribute of a channel */
		channel_id = iio_get_channel_id(el_info->channel_name, iio_device->channels,
						el_info->ch_out);
		return iio_rd_wr_channel_attribute(el_info, buf, len,
						   iio_device->channels[channel_id], is_write);
	}

	return -ENOENT;
}

/**
 * iio_supported_dev() - Check if device is supported.
 * @device:	Device name.
 * Return: TRUE if device is supported, FALSE otherwise.
 */
static bool iio_supported_dev(const char *device)
{
	return (NULL != iio_get_interface(device, iio_interfaces));
}

/**
 * iio_read_attr() - Read global attribute of a device.
 * @device:	String containing device name.
 * @attr:	String containing attribute name.
 * @buf:	Buffer where value is read.
 * @len:	Maximum length of value to be stored in buf.
 * @debug:	Read raw value if set.
 * Return: Number of bytes read.
 */
static ssize_t iio_read_attr(const char *device, const char *attr, char *buf,
			     size_t len, bool debug)
{
	struct iio_interface *iio_device;
	struct element_info el_info;

	if (!iio_supported_dev(device))
		return FAILURE;

	el_info.device_name = device;
	el_info.channel_name = "";	/* there is no channel here */
	el_info.attribute_name = attr;

	iio_device = iio_get_interface(device, iio_interfaces);
	if (!iio_device)
		return FAILURE;

	return iio_rd_wr_attribute(&el_info, buf, len, iio_device->iio, 0);
}

/**
 * iio_write_attr() - Write global attribute of a device.
 * @device:	String containing device name.
 * @attr:	String containing attribute name.
 * @buf:	Value to be written.
 * @len:	Length of data.
 * @debug:	Write raw value if set.
 * Return: Number of written bytes.
 */
static ssize_t iio_write_attr(const char *device, const char *attr,
			      const char *buf,
			      size_t len, bool debug)
{
	struct element_info el_info;
	struct iio_interface *iio_interface;

	if (!iio_supported_dev(device))
		return -ENODEV;

	el_info.device_name = device;
	el_info.channel_name = "";	/* there is no channel here */
	el_info.attribute_name = attr;

	iio_interface = iio_get_interface(device, iio_interfaces);
	if (!iio_interface)
		return FAILURE;

	return iio_rd_wr_attribute(&el_info, (char*)buf, len, iio_interface->iio, 1);
}

/**
 * iio_ch_read_attr() - Read channel attribute.
 * @device:	String containing device name.
 * @channel:	String containing channel name.
 * @ch_out:	Channel type input/output.
 * @attr:	String containing attribute name.
 * @buf:	Buffer where value is stored.
 * @len:	Maximum length of value to be stored in buf.
 * Return: Number of bytes read.
 */
static ssize_t iio_ch_read_attr(const char *device, const char *channel,
				bool ch_out, const char *attr, char *buf, size_t len)
{
	struct element_info el_info;
	struct iio_interface *iio_interface;

	if (!iio_supported_dev(device))
		return FAILURE;

	el_info.device_name = device;
	el_info.channel_name = channel;
	el_info.attribute_name = attr;
	el_info.ch_out = ch_out;

	iio_interface = iio_get_interface(device, iio_interfaces);
	if (!device)
		return FAILURE;

	return iio_rd_wr_attribute(&el_info, buf, len, iio_interface->iio, 0);
}

/**
 * iio_ch_write_attr() - Write channel attribute.
 * @device:	String containing device name.
 * @channel:	String containing channel name.
 * @ch_out:	Channel type input/output.
 * @attr:	String containing attribute name.
 * @buf:	Value to be written.
 * @len:	Length of data in "buf" parameter.
 * Return: Number of written bytes.
 */
static ssize_t iio_ch_write_attr(const char *device, const char *channel,
				 bool ch_out, const char *attr, const char *buf, size_t len)
{
	struct element_info el_info;
	struct iio_interface *iio_interface;

	if (!iio_supported_dev(device))
		return -ENODEV;

	el_info.device_name = device;
	el_info.channel_name = channel;
	el_info.attribute_name = attr;
	el_info.ch_out = ch_out;
	iio_interface = iio_get_interface(device, iio_interfaces);

	if (!iio_interface)
		return -ENOENT;

	return iio_rd_wr_attribute(&el_info, (char*)buf, len, iio_interface->iio, 1);
}

/**
 * iio_open_dev() - Open device.
 * @device:		String containing device name.
 * @sample_size:	Sample size.
 * @mask:		Channels to be opened.
 * Return: SUCCESS, negative value in case of failure.
 */
static int32_t iio_open_dev(const char *device, size_t sample_size,
			    uint32_t mask)
{
	struct iio_interface *iface;
	uint32_t ch_mask;

	if (!iio_supported_dev(device))
		return -ENODEV;

	iface = iio_get_interface(device, iio_interfaces);
	ch_mask = 0xFFFFFFFF >> (32 - iface->iio->num_ch);

	if (mask & ~ch_mask)
		return -ENOENT;

	iface->ch_mask = mask;

	return SUCCESS;
}

/**
 * iio_close_dev() - Close device.
 * @device:	String containing device name.
 * Return: SUCCESS, negative value in case of failure.
 */
static int32_t iio_close_dev(const char *device)
{
	struct iio_interface *iface;

	if (!iio_supported_dev(device))
		return FAILURE;
	iface = iio_get_interface(device, iio_interfaces);
	iface->ch_mask = 0;

	return SUCCESS;
}

/**
 * iio_get_mask() - Get device mask, this specifies the channels that are used.
 * @device:	String containing device name.
 * @mask:	Channels that are opened.
 * Return: SUCCESS, negative value in case of failure.
 */
static int32_t iio_get_mask(const char *device, uint32_t *mask)
{
	struct iio_interface *iface;

	if (!iio_supported_dev(device))
		return -ENODEV;

	iface = iio_get_interface(device, iio_interfaces);
	*mask = iface->ch_mask;

	return SUCCESS;
}

/**
 * iio_transfer_dev_to_mem() - Transfer data from device into RAM.
 * @device:		String containing device name.
 * @bytes_count:	Number of bytes.
 * Return: Bytes_count or negative value in case of error.
 */
static ssize_t iio_transfer_dev_to_mem(const char *device, size_t bytes_count)
{
	struct iio_interface *iio_interface = iio_get_interface(device, iio_interfaces);

	if (iio_interface->transfer_dev_to_mem)
		return iio_interface->transfer_dev_to_mem(iio_interface->dev_instance,
				bytes_count, iio_interface->ch_mask);

	return -ENOENT;
}

/**
 * iio_read_dev() - Read chunk of data from RAM to pbuf. Call
 * "iio_transfer_dev_to_mem()" first.
 * This function is probably called multiple times by libtinyiiod after a
 * "iio_transfer_dev_to_mem" call, since we can only read "bytes_count" bytes.
 * @device:		String containing device name.
 * @pbuf:		Buffer where value is stored.
 * @offset:		Offset to the remaining data after reading n chunks.
 * @bytes_count:	Number of bytes to read.
 * Return: Bytes_count or negative value in case of error.
 */
static ssize_t iio_read_dev(const char *device, char *pbuf, size_t offset,
			    size_t bytes_count)
{
	struct iio_interface *iio_interface = iio_get_interface(device, iio_interfaces);

	if (iio_interface->read_data)
		return iio_interface->read_data(iio_interface->dev_instance, pbuf, offset,
						bytes_count, iio_interface->ch_mask);

	return -ENOENT;
}

/**
 * iio_transfer_mem_to_dev() - Transfer memory to device.
 * @device:		String containing device name.
 * @bytes_count:	Number of bytes to transfer.
 * Return: Bytes_count or negative value in case of error.
 */
static ssize_t iio_transfer_mem_to_dev(const char *device, size_t bytes_count)
{
	struct iio_interface *iio_interface = iio_get_interface(device, iio_interfaces);

	if (iio_interface->transfer_mem_to_dev)
		return iio_interface->transfer_mem_to_dev(iio_interface->dev_instance,
				bytes_count, iio_interface->ch_mask);

	return -ENOENT;
}

/**
 * iio_write_dev() - Write chunk of data into RAM.
 * This function is probably called multiple times by libtinyiiod before a
 * "iio_transfer_mem_to_dev" call, since we can only write "bytes_count" bytes
 * at a time.
 * @device:		String containing device name.
 * @buf:		Values to write.
 * @offset:		Offset in memory after the nth chunk of data.
 * @bytes_count:	Number of bytes to write.
 * Return: Bytes_count or negative value in case of error.
 */
static ssize_t iio_write_dev(const char *device, const char *buf,
			     size_t offset, size_t bytes_count)
{
	struct iio_interface *iio_interface = iio_get_interface(device, iio_interfaces);
	if(iio_interface->write_data)
		return iio_interface->write_data(iio_interface->dev_instance, (char*)buf,
						 offset, bytes_count, iio_interface->ch_mask);

	return -ENOENT;
}

/**
 * iio_get_xml() - Get a merged xml containing all devices.
 * @outxml:	Generated xml.
 * Return: SUCCESS in case of success or negative value otherwise.
 */
static ssize_t iio_get_xml(char **outxml)
{
	char *xml, *tmp_xml, *tmp_xml2;
	uint32_t length;
	uint16_t i;
	ssize_t ret;

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
			"<context name=\"xml\" description=\"no-OS analog 1.1.0-g0000000 #1 Tue Nov 26 09:52:32 IST 2019 armv7l\" >"
			"<context-attribute name=\"no-OS\" value=\"1.1.0-g0000000\" />";
	char header_end[] = "</context>";

	if (!outxml)
		return FAILURE;

	xml = (char *)calloc(1, strlen(header) + 1);
	if (!xml)
		return FAILURE;

	strcpy(xml, header);
	for (i = 0; i < iio_interfaces->num_interfaces; i++) {
		ret = iio_interfaces->interfaces[i]->get_xml(&tmp_xml,
				iio_interfaces->interfaces[i]->iio);
		if (ret < 0)
			goto error;

		length = strlen(xml);
		tmp_xml2 = (char *)realloc(xml, strlen(xml) + strlen(tmp_xml) + 1);
		if (!tmp_xml2)
			goto error;

		xml = tmp_xml2;
		strcpy((xml + length), tmp_xml);
	}

	length = strlen(xml);
	tmp_xml = (char *)realloc(xml, strlen(xml) + strlen(header_end) + 1);
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
 * iio_register() - Register interface.
 * @init_par:	Structure containing physical device instance and device
 * 		descriptor.
 * Return: SUCCESS in case of success or negative value otherwise.
 */
ssize_t iio_register(struct iio_interface_init_par *init_par)
{
	struct iio_interface *iio_interface;
	struct iio_interface **temp_interfaces;

	if (!(iio_interfaces)) {
		iio_interfaces = (struct iio_interfaces *)calloc(1,
				 sizeof(struct iio_interfaces));
		if (!iio_interfaces)
			return -ENOMEM;

		iio_interfaces->num_interfaces = 1;
		iio_interfaces->interfaces = (struct iio_interface **)calloc(1,
					     sizeof(struct iio_interface*));
		if (!iio_interfaces->interfaces)
			return -ENOMEM;
	} else {
		iio_interfaces->num_interfaces++;
		temp_interfaces = (struct iio_interface **)realloc(iio_interfaces->interfaces,
				  iio_interfaces->num_interfaces * sizeof(struct iio_interface*));
		if (!temp_interfaces) {
			free(iio_interfaces->interfaces);
			return -ENOMEM;
		}
		iio_interfaces->interfaces = temp_interfaces;
	}
	iio_interface = (struct iio_interface *)calloc(1, sizeof(struct iio_interface));
	if (!iio_interface)
		return -ENOMEM;

	iio_interface->dev_instance = init_par->dev_instance;
	iio_interface->name = init_par->dev_name;
	iio_interface->iio = init_par->iio_device;
	iio_interface->get_xml = init_par->get_xml;
	iio_interface->transfer_dev_to_mem = init_par->transfer_dev_to_mem;
	iio_interface->transfer_mem_to_dev = init_par->transfer_mem_to_dev;
	iio_interface->read_data = init_par->read_data;
	iio_interface->write_data = init_par->write_data;

	iio_interfaces->interfaces[iio_interfaces->num_interfaces - 1] = iio_interface;

	return SUCCESS;
}

/**
 * iio_unregister() - Unregister interface.
 * @device_name: String containing device name.
 * Return: SUCCESS in case of success or negative value otherwise.
 */
ssize_t iio_unregister(const char *device_name)
{
	struct iio_interface *iio_interface = iio_get_interface(device_name,
					      iio_interfaces);
	struct iio_interfaces *interfaces;
	int16_t i, deleted = 0;

	if (!iio_interface)
		return FAILURE;

	interfaces = (struct iio_interfaces *)calloc(1, sizeof(struct iio_interfaces));
	if (!interfaces)
		return FAILURE;

	interfaces->interfaces = (struct iio_interface **)calloc(
					 iio_interfaces->num_interfaces - 1,
					 sizeof(struct iio_interface*));
	if (!interfaces->interfaces) {
		free(interfaces);
		return FAILURE;
	}

	for(i = 0; i < iio_interfaces->num_interfaces; i++) {
		if (!strcmp(device_name, iio_interfaces->interfaces[i]->name)) {
			free(iio_interfaces->interfaces[i]);
			deleted = 1;
			continue;
		}
		interfaces->interfaces[i - deleted] = iio_interfaces->interfaces[i];
	}

	interfaces->num_interfaces = iio_interfaces->num_interfaces - 1;
	free(iio_interfaces);
	iio_interfaces = interfaces;

	return SUCCESS;
}

/**
 * iio_init() - Set communication ops and read/write ops that will be called
 * from "libtinyiiod".
 * @*iiod:		Structure containing new tinyiiod instance.
 * @iio_server_ops:	Structure containing read/write ops (Ex: read/write to
 * 			UART).
 * Return: SUCCESS in case of success or negative value otherwise.
 */
ssize_t iio_init(struct tinyiiod **iiod, struct iio_server_ops *iio_server_ops)
{
	struct tinyiiod_ops *ops = (struct tinyiiod_ops *)calloc(1,
				   sizeof(struct tinyiiod_ops));

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
	if (!(*iiod)) {
		free(ops);
		return FAILURE;
	} else {
		return SUCCESS;
	}
}

/**
 * iio_remove() - Free the resources allocated by "iio_init()".
 * @iiod: Structure containing tinyiiod instance.
 * Return: SUCCESS in case of success or negative value otherwise.
 */
ssize_t iio_remove(struct tinyiiod *iiod)
{
	uint8_t i;

	for (i = 0; i < iio_interfaces->num_interfaces; i++)
		free(iio_interfaces->interfaces[i]);

	free(iio_interfaces);
	tinyiiod_destroy(iiod);

	return SUCCESS;
}
