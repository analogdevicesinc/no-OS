/***************************************************************************//**
 *   @file   tinyiiod_util.c
 *   @brief  Implementation of tinyiiod_util
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

#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <xil_io.h>
#include "tinyiiod.h"
#include "tinyiiod_util.h"
#include "axi_dmac.h"
#include "util.h"
#include "tinyiiod_axi_adc.h"
#include "tinyiiod_axi_dac.h"
#include "serial.h"

typedef struct tinyiiod_device {
    const char *name;
    uint16_t number_of_channels;
    uint32_t opened_ch_mask;
    void *pointer;
    attribute_map *attr_map;
    ssize_t (*get_device_xml)(char** xml, const char *device_name, uint8_t ch_no);
} tinyiiod_device;

typedef struct tinyiiod_devices {
    tinyiiod_device **devices;
    uint8_t number_of_dev;
} tinyiiod_devices;

enum elem_level {
    DEVICE_EL,
    CHANNEL_EL,
    ATTRIBUTE_EL,
    MAX_EL
};

typedef struct element_info {
    const char *name[MAX_EL];
    bool ch_out;
    enum elem_level crnt_level;
} element_info;

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

    while(map[i].name)
    {
        if (strequal(attr, map[i].name))
            return i;
        i++;
    }

    return -ENODEV;
}

static tinyiiod_device *get_device(const char *device_name, tinyiiod_devices *tinyiiod_devs)
{
    int16_t i = 0;
    for (i = 0; i < tinyiiod_devs->number_of_dev; i++) {
        if(strequal(device_name, tinyiiod_devs->devices[i]->name)) {
            return tinyiiod_devs->devices[i];
        }
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
static ssize_t read_all_attr(void *device, char *buf, size_t len,
                             const struct channel_info *channel, const attribute_map* map)
{
    int16_t i = 0, j = 0;
    char local_buf[0x1000];
    while(map[i].name)
    {
        int16_t attr_length = map[i].exec(device, (local_buf), len, channel);
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
static ssize_t write_all_attr(void *device, char *buf, size_t len,
                              const struct channel_info *channel, const attribute_map* map)
{
    int16_t i = 0, j = 0;

    while(map[i].name)
    {
        int16_t attr_length = Xil_EndianSwap32((uint32_t)(buf + j));
        j += 4;
        map[i].exec(device, (buf + j), attr_length, channel);
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

    attribute_id = get_attribute_id(el_info->name[el_info->crnt_level], map);

    if(el_info->crnt_level == DEVICE_EL &&
            strequal(el_info->name[CHANNEL_EL], "")) /* element not fond, but try to rd/wr all if element is ATTRIBUTE_EL and name is "" */
    {
        if (strequal(el_info->name[ATTRIBUTE_EL], ""))
        {
            const struct channel_info channel_info = {
                get_channel_number(el_info->name[CHANNEL_EL]),
                el_info->ch_out
            };
            tinyiiod_device *device = get_device(el_info->name[DEVICE_EL], tinyiiod_devs);
            if(!device)
                return -ENOENT;


            if(is_write)
                return write_all_attr(device->pointer, buf, len, &channel_info, map->map_out_global);
            else
                return read_all_attr(device->pointer, buf, len, &channel_info, map->map_in_global);
        }
        else
        {
            map = el_info->ch_out ? map->map_out_global : map->map_in_global;
            attribute_id = get_attribute_id(el_info->name[ATTRIBUTE_EL], map);
        }
    }

    if(el_info->crnt_level == CHANNEL_EL &&
            strequal(el_info->name[ATTRIBUTE_EL], "")) {
        const struct channel_info channel_info = {
            get_channel_number(el_info->name[CHANNEL_EL]),
            el_info->ch_out
        };
        tinyiiod_device *device = get_device(el_info->name[DEVICE_EL], tinyiiod_devs);
        if(!device)
            return -ENOENT;
        if(is_write)
            return write_all_attr(device->pointer, buf, len, &channel_info, map->map_out);
        else
            return read_all_attr(device->pointer, buf, len, &channel_info, map->map_in);

    }

    if(attribute_id >= 0)  /* element fond */
    {
        if(map[attribute_id].exec)
        {
            const struct channel_info channel_info = {
                get_channel_number(el_info->name[CHANNEL_EL]),
                el_info->ch_out
            };
            tinyiiod_device *device = get_device(el_info->name[DEVICE_EL], tinyiiod_devs);
            if(!device)
                return -ENOENT;

            return map[attribute_id].exec(device->pointer, (char*)buf, len, &channel_info);
        }
        else
        {
            el_info->crnt_level++;
            if(el_info->crnt_level == CHANNEL_EL)
                return rd_wr_attribute(el_info, buf, len, is_write ? map[attribute_id].map_out : map[attribute_id].map_in, is_write);
            if(el_info->crnt_level == ATTRIBUTE_EL)
                return rd_wr_attribute(el_info, buf, len, el_info->ch_out ? map[attribute_id].map_out : map[attribute_id].map_in, is_write);
        }
    }

    return -ENOENT;
}

ssize_t tinyiiod_register_device(void* device_address, const char *device_name, uint16_t number_of_channels,
                                 ssize_t (*get_device_xml)(char** xml, const char *device_name, uint8_t ch_no),
                                 attribute_map *attr_map) {

    tinyiiod_device *device;

    if (!(tinyiiod_devs)) {
        tinyiiod_devs = malloc(sizeof(tinyiiod_devices*));
        if(!tinyiiod_devs)
            return -ENOMEM;
        tinyiiod_devs->number_of_dev = 1;
        tinyiiod_devs->devices = malloc(sizeof(tinyiiod_device*));
        if(!tinyiiod_devs->devices)
            return -ENOMEM;
    }
    else {
        tinyiiod_devs->number_of_dev++;
        tinyiiod_devs->devices = realloc(tinyiiod_devs->devices, tinyiiod_devs->number_of_dev * sizeof(tinyiiod_device*));
        if(!tinyiiod_devs->devices)
            return -ENOMEM;
    }
    device = malloc(sizeof(tinyiiod_device));
    if(!device)
        return -ENOMEM;

    device->pointer = device_address;
    device->name = device_name;
    device->number_of_channels = number_of_channels;
    device->get_device_xml = get_device_xml;
    device->attr_map = attr_map;

    tinyiiod_devs->devices[tinyiiod_devs->number_of_dev - 1] = device;

    return 0;
}

ssize_t tinyiiod_remove(void)
{
	uint8_t i;
	for (i = 0; i < tinyiiod_devs->number_of_dev; i++)
	{
		free(tinyiiod_devs->devices[i]);
	}
	free(tinyiiod_devs);

	return 0;
}

static ssize_t get_xml(tinyiiod_devices *devs, char **outxml)
{
    char *xml, *tmp_xml, *tmp_xml2;
    uint32_t length;
    ssize_t error = 0;

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
    if(!xml)
    	return -ENOMEM;

    strcpy(xml, header);

    for (uint16_t i = 0; i < devs->number_of_dev; i++) {
        devs->devices[i]->get_device_xml(&tmp_xml, devs->devices[i]->name, devs->devices[i]->number_of_channels);
        length = strlen(xml);
        tmp_xml2 = realloc(xml, strlen(xml) + strlen(tmp_xml) + 1);
        if(!tmp_xml2)
        	error = -EACCES;
        else
        	xml = tmp_xml2;
        strcpy((xml + length), tmp_xml);
    }

    length = strlen(xml);
    tmp_xml = realloc(xml, strlen(xml) + strlen(header2) + 1);
    if(!tmp_xml) {
    	free(tmp_xml);
        return -ENOMEM;
    }
    xml = tmp_xml;
    strcpy((xml + length), header2);

    *outxml = xml;

    return error;
}

/**
 * Check device
 * @param *device
 * @return TRUE if valid device is found
 */
static bool supporter_dev(const char *device)
{
	return (NULL != get_device(device, tinyiiod_devs));
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
static ssize_t read_attr(const char *device, const char *attr, char *buf, size_t len, bool debug)
{
    if (!supporter_dev(device))
        return -ENODEV;
    element_info el_info;
    el_info.name[DEVICE_EL] = device;
    el_info.name[CHANNEL_EL] = "";
    el_info.name[ATTRIBUTE_EL] = attr;
    el_info.crnt_level = DEVICE_EL;
    el_info.ch_out = 0;
    tinyiiod_device *iiod_device = get_device(device, tinyiiod_devs);
    if(!iiod_device)
        return -ENOENT;

    return rd_wr_attribute(&el_info, buf, len, iiod_device->attr_map, 0);
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
static ssize_t write_attr(const char *device, const char *attr, const char *buf, size_t len, bool debug)
{
    if (!supporter_dev(device))
        return -ENODEV;
    element_info el_info;
    el_info.name[DEVICE_EL] = device;
    el_info.name[CHANNEL_EL] = "";
    el_info.name[ATTRIBUTE_EL] = attr;
    el_info.crnt_level = DEVICE_EL;
    el_info.ch_out = 0;

    tinyiiod_device *iiod_device = get_device(device, tinyiiod_devs);
    if(!iiod_device)
        return -ENOENT;

    return rd_wr_attribute(&el_info, (char*)buf, len, iiod_device->attr_map, 1);
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
static ssize_t ch_read_attr(const char *device, const char *channel,
                            bool ch_out, const char *attr, char *buf, size_t len)
{
    if (!supporter_dev(device))
        return -ENODEV;
    element_info el_info;
    el_info.name[DEVICE_EL] = device;
    el_info.name[CHANNEL_EL] = channel;
    el_info.name[ATTRIBUTE_EL] = attr;
    el_info.crnt_level = DEVICE_EL;
    el_info.ch_out = ch_out;

    tinyiiod_device *iiod_device = get_device(device, tinyiiod_devs);
    if(!device)
        return -ENOENT;

    return rd_wr_attribute(&el_info, buf, len, iiod_device->attr_map, 0);
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
static ssize_t ch_write_attr(const char *device, const char *channel,
                             bool ch_out, const char *attr, char *buf, size_t len)
{
    if (!supporter_dev(device))
        return -ENODEV;
    element_info el_info;
    el_info.name[DEVICE_EL] = device;
    el_info.name[CHANNEL_EL] = channel;
    el_info.name[ATTRIBUTE_EL] = attr;
    el_info.crnt_level = DEVICE_EL;
    el_info.ch_out = ch_out;
    tinyiiod_device *iiod_device = get_device(device, tinyiiod_devs);
    if(!iiod_device)
        return -ENOENT;

    return rd_wr_attribute(&el_info, buf, len, iiod_device->attr_map, 1);
}

static int32_t open_dev(const char *device, size_t sample_size, uint32_t mask)
{
    if (!supporter_dev(device))
        return -ENODEV;

    tinyiiod_device * dev = get_device(device, tinyiiod_devs);
    uint32_t ch_mask = 0xFFFFFFFF >> (32 - dev->number_of_channels);

    if (mask & ~ch_mask)
        return -ENOENT;

    dev->opened_ch_mask = ch_mask;

    return 0;
}

/**
 * close device
 * @param *device name
 * @return 0 on success, otherwise negative value
 */
static int32_t close_dev(const char *device)
{
    return supporter_dev(device) ? 0 : -ENODEV;
}

/**
 * get device mask, this specifies the channels that are used
 * @param *device name
 * @param *device mask
 * @return 0 on success, otherwise negative value
 */
static int32_t get_mask(const char *device, uint32_t *mask)
{
    if (!supporter_dev(device))
        return -ENODEV;
    tinyiiod_device * dev = get_device(device, tinyiiod_devs);
    uint32_t ch_mask = 0xFFFFFFFF >> (32 - dev->number_of_channels);
    *mask = ch_mask; /*  this way client has to do demux of data */

    return 0;
}
/**
 * transfer_dev_to_mem data from ADC into RAM
 * @param *device name
 * @param bytes_count
 * @return bytes_count
 */
static ssize_t transfer_dev_to_mem(const char *device, size_t bytes_count)
{
    tinyiiod_device *iiod_device = get_device(device, tinyiiod_devs);
    if(!iiod_device)
        return -ENOENT;
    tinyiiod_adc *iiod_adc = (tinyiiod_adc *)(iiod_device->pointer);

    return adc_transfer_dev_to_mem(iiod_adc->dmac, iiod_adc->adc_ddr_base, bytes_count);
}

/**
 * read data from RAM to pbuf, use "capture()" first
 * @param *device name
 * @param *buff where data's are stored
 * @param *offset to the remaining data
 * @param bytes_count
 * @return bytes_count
 */
static ssize_t read_dev(const char *device, char *pbuf, size_t offset,
                        size_t bytes_count)
{
    tinyiiod_device *iiod_device = get_device(device, tinyiiod_devs);
    if(!iiod_device)
        return -ENOENT;
    tinyiiod_adc *iiod_adc = (tinyiiod_adc *)(iiod_device->pointer);

    return adc_read_dev((char*)iiod_adc->adc_ddr_base, pbuf, offset, bytes_count);
}

/**
 * transfer_mem_to_dev write data to DAC
 * @param *device name
 * @param *buff
 * @param bytes_count
 * @return bytes_count
 */
static ssize_t transfer_mem_to_dev(const char *device, size_t bytes_count)
{
    tinyiiod_device *iiod_device = get_device(device, tinyiiod_devs);
    if(!device)
        return -ENOENT;
    tinyiiod_dac *iiod_dac = (tinyiiod_dac *)(iiod_device->pointer);

    return dac_transfer_mem_to_dev(iiod_dac->dmac, iiod_dac->dac_ddr_base, bytes_count);
}

/**
 * write data to RAM
 * @param *device name
 * @param *buff
 * @param *offset in memory, used if some data have been already written
 * @param bytes_count
 * @return bytes_count
 */
static ssize_t write_dev(const char *device, const char *buf,
                  size_t offset,  size_t bytes_count)
{
    tinyiiod_device *iiod_device = get_device(device, tinyiiod_devs);
    if(!device)
        return -ENOENT;
    tinyiiod_dac *iiod_dac = (tinyiiod_dac *)(iiod_device->pointer);

    return dac_write_dev(iiod_dac, buf, offset, bytes_count);
}


struct tinyiiod_ops ops = {
    /* communication */
    .read = NULL,
    .read_line = NULL,
    .write = NULL,

    /* device operations */
    .read_attr = read_attr,
    .write_attr = write_attr,
    .ch_read_attr = ch_read_attr,
    .ch_write_attr = ch_write_attr,
    .transfer_dev_to_mem = transfer_dev_to_mem,
    .read_data = read_dev,
    .transfer_mem_to_dev = transfer_mem_to_dev,
    .write_data = write_dev,

    .open = open_dev,
    .close = close_dev,
    .get_mask = get_mask,
};

ssize_t iiod_create(struct tinyiiod **iiod, tinyiiod_comm_ops *comm_ops)
{
    ssize_t ret;
    char *xml;

    ops.read = comm_ops->read;
    ops.read_line = comm_ops->read_line;
    ops.write = comm_ops->write;

    ret = get_xml(tinyiiod_devs, &xml);
    if(ret < 0)
        return ret;
    *iiod = tinyiiod_create(xml, &ops);
    if(!(*iiod))
    	return -ENOMEM;

    return 0;
}

ssize_t iiod_remove(struct tinyiiod *iiod)
{
	// free(iiod->xml); // todo
	tinyiiod_destroy(iiod);

	return 0;
}
