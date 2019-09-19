#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <xil_io.h>
#include "tinyiiod.h"
#include "tinyiiod_util.h"
#include "util.h"
static tinyiiod_devices *tinyiiod_devs = NULL;

/**
 * Get channel number
 * @param *ch channel name + channel number
 * @return channel number Ex: for "altvoltage0" return 0, for "voltage2" return 2
 */
static int32_t get_channel_number(const char *ch)
{
	char *p = (char*)ch;
	int32_t ch_num = 0;

	while(*p) {
		if(isdigit(*p))
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
static int16_t get_attribute_id(const char *attr, const attribute_map* map)
{
	int16_t i = 0;

	if(!map)
		return -EINVAL;

	while(map[i].attr_name)
	{
		if (strequal(attr, map[i].attr_name))
			return i;
		i++;
	}

	return -ENODEV;
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
static ssize_t read_all_attr(char *buf, size_t len,
			     const struct channel_info *channel, const attribute_map* map)
{
	int16_t i = 0, j = 0;
	char local_buf[0x1000];
	while(map[i].attr_name)
	{
		int16_t attr_length = map[i].exec((local_buf), len, channel);
		int32_t *len = (int32_t *)(buf + j);
		*len = Xil_EndianSwap32(attr_length);

		j += 4;
		if(attr_length >= 0) {
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
static ssize_t write_all_attr(char *buf, size_t len,
			      const struct channel_info *channel, const attribute_map* map)
{
	int16_t i = 0, j = 0;

	while(map[i].attr_name)
	{
		int16_t attr_length = Xil_EndianSwap32((uint32_t)(buf + j));
		j += 4;
		map[i].exec((buf + j), attr_length, channel);
		j += attr_length;
		if (j & 0x3)
			j = ((j >> 2) + 1) << 2;
		i++;
	}

	return len;
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
ssize_t rd_wr_attribute(element_info *el_info, char *buf, size_t len, attribute_map *map, bool is_write)
{
	int16_t attribute_id;

	if(!map)
		return -ENOENT;

	if(el_info->crnt_level == CHANNEL_EL && strequal(el_info->name[CHANNEL_EL], ""))
		el_info->crnt_level++;

	attribute_id = get_attribute_id(el_info->name[el_info->crnt_level], map);

	if(attribute_id < 0 && el_info->crnt_level == ATTRIBUTE_EL && strequal(el_info->name[el_info->crnt_level], "")) /* element not fond, but try to rd/wr all if element is ATTRIBUTE_EL and name is "" */
	{
		const struct channel_info channel_info = {
			get_channel_number(el_info->name[CHANNEL_EL]),
			el_info->ch_out
		};
		if(is_write)
			return write_all_attr(buf, len, &channel_info, map);
		else
			return read_all_attr(buf, len, &channel_info, map);
	}
	if(attribute_id >= 0)  /* element fond */
	{
		if(map[attribute_id].exec)
		{
			const struct channel_info channel_info = {
					get_channel_number(el_info->name[CHANNEL_EL]),
					el_info->ch_out
				};
			return map[attribute_id].exec((char*)buf, len, &channel_info);
		}
		else
		{
			el_info->crnt_level++;
			return rd_wr_attribute(el_info, buf, len, el_info->ch_out ? map[attribute_id].map_out : map[attribute_id].map_in, is_write);
		}
	}

	return -ENOENT;
}

ssize_t tinyiiod_register_device(void* device_address, const char *device_name, uint16_t number_of_channels, ssize_t (*get_device_xml)(char** xml,  char *device_name, uint8_t ch_no)) {
	if (!(tinyiiod_devs)) {
		tinyiiod_devs = malloc(sizeof(tinyiiod_devices*));
		tinyiiod_devs->number_of_dev = 1;
		tinyiiod_devs->devices = malloc(sizeof(tinyiiod_device*));
	}
	else {
		tinyiiod_devs->number_of_dev++;
		tinyiiod_devs->devices = realloc(tinyiiod_devs->devices, tinyiiod_devs->number_of_dev * sizeof(tinyiiod_device*));
	}
	tinyiiod_device *device = malloc(sizeof(tinyiiod_device));
	device->pointer = device_address;
	device->name = malloc(strlen(device_name) + 1);
	strcpy(device->name, device_name);
	device->number_of_channels = number_of_channels;
	device->get_device_xml = get_device_xml;
	tinyiiod_devs->devices[tinyiiod_devs->number_of_dev - 1] = device;

	return 0;
}

ssize_t get_xml(char **outxml)
{
	char *xml, *tmp_xml;
	uint32_t length;

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

	char header2[] = "</context>";

	xml = malloc(strlen(header) + 1);
	strcpy(xml, header);

	for (uint16_t i = 0; i < tinyiiod_devs->number_of_dev; i++) {
		tinyiiod_devs->devices[i]->get_device_xml(&tmp_xml, tinyiiod_devs->devices[i]->name, tinyiiod_devs->devices[i]->number_of_channels);
		length = strlen(xml);
		xml = realloc(xml, strlen(xml) + strlen(tmp_xml) + 1);
		strcpy((xml + length), tmp_xml);
	}

	length = strlen(xml);
	xml = realloc(xml, strlen(xml) + strlen(header2) + 1);
	strcpy((xml + length), header2);

	*outxml = xml;

	return 0;
}
