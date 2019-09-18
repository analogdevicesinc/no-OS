/***************************************************************************//**
 *   @file   ad9361_tinyiiod.c
 *   @brief  Implementation of ad9361_tinyiiod
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
#include <ad9361_tinyiiod_phy.h>
#include "ad9361.h"
#include "ad9361_tinyiiod.h"
#include "ad9361_parameters.h"
#include "config.h"
#include "tinyiiod_util.h"
#include "tinyiiod_dac.h"
#include "tinyiiod_adc.h"

#ifdef UART_INTERFACE
#include "serial.h"
#endif /* UART_INTERFACE */

static uint32_t request_mask;
/* mask for cf-ad9361-lpc 0x0F, it has 4 channels */
static const uint32_t input_channel_mask = 0x0F;

/**
 * Check device
 * @param *device
 * @return TRUE if valid device is found
 */
static bool supporter_dev(const char *device)
{
	return strequal(device, "ad9361-phy")
	       || strequal(device, "cf-ad9361-lpc")
	       || strequal(device, "cf-ad9361-dds-core-lpc");
}

static attrtibute_map read_attr_map[] = {
	{"ad9361-phy", NULL, NULL},
	{"cf-ad9361-dds-core-lpc", NULL, NULL},
	{"cf-ad9361-lpc", NULL, NULL},
	{NULL, NULL},
};

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
	return rd_wr_attribute(&el_info, buf, len, read_attr_map, 0);
}

static attrtibute_map write_attr_map[] = {
	{"ad9361-phy", NULL, NULL},
	{"cf-ad9361-dds-core-lpc", NULL, NULL},
	{"cf-ad9361-lpc", NULL, NULL},
	{NULL, NULL},
};

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
	return rd_wr_attribute(&el_info, (char*)buf, len, write_attr_map, 1);
}

static attrtibute_map ch_read_attr_map[] = {
	{"ad9361-phy", NULL, NULL, NULL},
	{"cf-ad9361-dds-core-lpc", NULL, NULL, NULL},
	{"cf-ad9361-lpc", NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
};

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
	return rd_wr_attribute(&el_info, buf, len, ch_read_attr_map, 0);
}

static attrtibute_map ch_write_attr_map[] = {
	{"ad9361-phy", NULL, NULL},
	{"cf-ad9361-dds-core-lpc", NULL, NULL},
	{"cf-ad9361-lpc", NULL, NULL},
	{NULL, NULL},
};

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
	return rd_wr_attribute(&el_info, buf, len, ch_write_attr_map, 1);
}

static int32_t open_dev(const char *device, size_t sample_size, uint32_t mask)
{
	if (!supporter_dev(device))
		return -ENODEV;

	if (mask & ~input_channel_mask)
		return -ENOENT;

	request_mask = mask;

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
	*mask = input_channel_mask; /*  this way client has to do demux of data */

	return 0;
}

const struct tinyiiod_ops ops = {
	/* communication */
#ifdef UART_INTERFACE
	.read = serial_read,
	.read_line = serial_read_line,
	.write = serial_write_data,
#endif /* UART_INTERFACE */

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

ssize_t ad9361_tinyiiod_create(struct ad9361_rf_phy *phy, struct tinyiiod **iiod)
{
	ssize_t ret;

	ch_read_attr_map[0].map_in = get_ch_read_phy_attr_map();
	ch_read_attr_map[1].map_in = get_ch_read_dac_attr_map();
	ch_read_attr_map[2].map_in = get_ch_read_adc_attr_map();
	ch_read_attr_map[0].map_out = ch_read_attr_map[0].map_in;
	ch_read_attr_map[1].map_out = ch_read_attr_map[1].map_in;
	ch_read_attr_map[2].map_out = ch_read_attr_map[2].map_in;

	ch_write_attr_map[0].map_in = get_ch_write_phy_attr_map();
	ch_write_attr_map[1].map_in = get_ch_write_dac_attr_map();
	ch_write_attr_map[2].map_in = get_ch_write_adc_attr_map();
	ch_write_attr_map[0].map_out = ch_write_attr_map[0].map_in;
	ch_write_attr_map[1].map_out = ch_write_attr_map[1].map_in;
	ch_write_attr_map[2].map_out = ch_write_attr_map[2].map_in;

	read_attr_map[0].map_in = get_read_phy_attr_map();
	write_attr_map[0].map_in = get_write_phy_attr_map();

	ret = tinyiiod_adc_configure(phy->rx_adc, phy->rx_dmac, ADC_DDR_BASEADDR);
	if(ret < 0)
		return ret;

	ret = tinyiiod_dac_configure(phy->tx_dac, phy->tx_dmac, DAC_DDR_BASEADDR);
	if(ret < 0)
		return ret;

	ret = tinyiiod_phy_configure(phy);
	if(ret < 0)
		return ret;
	char *xml, *tmp_xml;
	uint32_t length;
	printf("\n\n\n");
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

	get_dac_xml(&tmp_xml, 4);
	length = strlen(xml);
	xml = realloc(xml, strlen(xml) + strlen(tmp_xml) + 1);
	strcpy((xml + length), tmp_xml);

	get_adc_xml(&tmp_xml, 4);
	length = strlen(xml);
	xml = realloc(xml, strlen(xml) + strlen(tmp_xml) + 1);
	strcpy((xml + length), tmp_xml);

	get_phy_xml(&tmp_xml, 4);
	length = strlen(xml);
	xml = realloc(xml, strlen(xml) + strlen(tmp_xml) + 1);
	strcpy((xml + length), tmp_xml);

	length = strlen(xml);
	xml = realloc(xml, strlen(xml) + strlen(header2) + 1);
	strcpy((xml + length), header2);
	printf("xml = %s\n\n\n", xml);
	*iiod = tinyiiod_create(xml, &ops);

	return 0;
}
