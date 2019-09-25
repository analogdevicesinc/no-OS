/***************************************************************************//**
 *   @file   tinyiiod_axi_adc.c
 *   @brief  Implementation of tinyiiod_axi_adc
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

#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <tinyiiod_axi_adc.h>
#include <xil_cache.h>
#include "platform_drivers.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"
#include "xml.h"
#include "util.h"

ssize_t tinyiiod_axi_adc_init(tinyiiod_adc **tinyiiod_adc, tinyiiod_adc_init_par *init) {
    *tinyiiod_adc = malloc(sizeof(*tinyiiod_adc));
    if (!(*tinyiiod_adc))
        return FAILURE;
    (*tinyiiod_adc)->adc = init->adc;
    (*tinyiiod_adc)->dmac = init->dmac;
    (*tinyiiod_adc)->adc_ddr_base = init->adc_ddr_base;

    return SUCCESS;
}

ssize_t tinyiiod_axi_adc_remove(tinyiiod_adc *tinyiiod_adc)
{

	free(tinyiiod_adc);

	return SUCCESS;
}

/**
 * get_cf_calibphase
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_cf_calibphase(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    int32_t val, val2;
    int32_t i = 0;
    struct tinyiiod_adc *iiod_adc = (tinyiiod_adc *)device;
    ssize_t ret = axi_adc_get_calib_phase(iiod_adc->adc, channel->ch_num, &val,
                                          &val2);


    if(ret < 0)
        return ret;
    if(val2 < 0 && val >= 0) {
        snprintf(buf, len, "-");
        i++;
    }

    return i + snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val, labs(val2));
}

/**
 * get_cf_calibbias
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_cf_calibbias(void *device, char *buf, size_t len,
                                const struct channel_info *channel)
{
    int32_t val;
    struct tinyiiod_adc *iiod_adc = (tinyiiod_adc *)device;
    axi_adc_get_calib_bias(iiod_adc->adc,
                           channel->ch_num,
                           &val,
                           NULL);

    return snprintf(buf, len, "%"PRIi32"", val);
}

/**
 * get_cf_calibscale
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_cf_calibscale(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    int32_t val, val2;
    int32_t i = 0;
    struct tinyiiod_adc *iiod_adc = (tinyiiod_adc *)device;
    ssize_t ret = axi_adc_get_calib_scale(iiod_adc->adc, channel->ch_num, &val,
                                          &val2);
    if(ret < 0)
        return ret;
    if(val2 < 0 && val >= 0) {
        ret = (ssize_t) snprintf(buf, len, "-");
        i++;
    }
    ret = i + (ssize_t) snprintf(&buf[i], len, "%"PRIi32".%.6"PRIi32"", val,
                                 labs(val2));

    return ret;
}

/**
 * get_cf_samples_pps
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_cf_samples_pps(void *device, char *buf, size_t len,
                                  const struct channel_info *channel)
{
    return -ENODEV;
}

/**
 * get_cf_sampling_frequency
 * @param *buff where value is stored
 * @param len maximum length of value to be stored in buf
 * @param *channel channel properties
 * @return length of chars written in buf, or negative value on failure
 */
static ssize_t get_cf_sampling_frequency(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    uint64_t sampling_freq_hz;
    struct tinyiiod_adc *iiod_adc = (tinyiiod_adc *)device;
    ssize_t ret = axi_adc_get_sampling_freq(iiod_adc->adc, channel->ch_num, &sampling_freq_hz);
    if(ret < 0)
        return ret;

    return snprintf(buf, len, "%"PRIi64"", sampling_freq_hz);
}

/**
 * set_cf_calibphase
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_cf_calibphase(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    float calib = strtof(buf, NULL);
    int32_t val = (int32_t)calib;
    int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
    struct tinyiiod_adc *iiod_adc = (tinyiiod_adc *)device;
    axi_adc_set_calib_phase(iiod_adc->adc, channel->ch_num, val, val2);

    return len;
}

/**
 * set_cf_calibbias
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_cf_calibbias(void *device, char *buf, size_t len,
                                const struct channel_info *channel)
{
    int32_t val = read_value(buf);
    struct tinyiiod_adc *iiod_adc = (tinyiiod_adc *)device;
    axi_adc_set_calib_bias(iiod_adc->adc,
                           channel->ch_num,
                           val,
                           0);

    return len;
}

/**
 * set_cf_calibscale
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_cf_calibscale(void *device, char *buf, size_t len,
                                 const struct channel_info *channel)
{
    float calib= strtof(buf, NULL);
    int32_t val = (int32_t)calib;
    int32_t val2 = (int32_t)(calib* 1000000) % 1000000;
    struct tinyiiod_adc *iiod_adc = (tinyiiod_adc *)device;
    axi_adc_set_calib_scale(iiod_adc->adc, channel->ch_num, val, val2);

    return len;
}

/**
 * set_cf_samples_pps
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_cf_samples_pps(void *device, char *buf, size_t len,
                                  const struct channel_info *channel)
{
    return -ENODEV;
}

/**
 * set_cf_sampling_frequency
 * @param *buff value to be written to attribute
 * @param len of the value
 * @param *channel channel properties
 * @return length of chars written to attribute, or negative value on failure
 */
static ssize_t set_cf_sampling_frequency(void *device, char *buf, size_t len,
        const struct channel_info *channel)
{
    return -ENODEV;
}

static attribute_map cf_voltage_read_attrtibute_map[] = {
    {.name = "calibphase", .exec = get_cf_calibphase},
    {.name = "calibbias", .exec = get_cf_calibbias},
    {.name = "calibscale", .exec = get_cf_calibscale},
    {.name = "samples_pps", .exec = get_cf_samples_pps},
    {.name = "sampling_frequency", .exec = get_cf_sampling_frequency},
    {NULL},
};

static attribute_map ch_read_adc_attr_map[] = {
    {.name = "voltage0", .exec = NULL, .map_in = cf_voltage_read_attrtibute_map, NULL},
    {.name = "voltage1", .exec = NULL, .map_in = cf_voltage_read_attrtibute_map, NULL},
    {.name = "voltage2", .exec = NULL, .map_in = cf_voltage_read_attrtibute_map, NULL},
    {.name = "voltage3", .exec = NULL, .map_in = cf_voltage_read_attrtibute_map, NULL},
    {.name = NULL},
};

static attribute_map cf_voltage_write_attrtibute_map[] = {
    {.name = "calibphase", .exec = set_cf_calibphase},
    {.name = "calibbias", .exec = set_cf_calibbias},
    {.name = "calibscale", .exec = set_cf_calibscale},
    {.name = "samples_pps", .exec = set_cf_samples_pps},
    {.name = "sampling_frequency", .exec = set_cf_sampling_frequency},
    {.name = NULL},
};

static attribute_map ch_write_adc_attr_map[] = {
    {.name = "voltage0", .exec = NULL, .map_in = cf_voltage_write_attrtibute_map, NULL},
    {.name = "voltage1", .exec = NULL, .map_in = cf_voltage_write_attrtibute_map, NULL},
    {.name = "voltage2", .exec = NULL, .map_in = cf_voltage_write_attrtibute_map, NULL},
    {.name = "voltage3", .exec = NULL, .map_in = cf_voltage_write_attrtibute_map, NULL},
    {.name = NULL},
};

static attribute_map adc_attr_map[] = {
    {.name = "NULL", .exec = NULL, .map_in = ch_read_adc_attr_map, .map_out = ch_write_adc_attr_map},
    {.name = NULL},
};

ssize_t get_adc_xml(char** xml, const char *device_name, uint8_t ch_no) {
    char buff[256];
    xml_node *device = NULL;
    xml_node *channel = NULL;
    xml_node *attribute = NULL;
    xml_attribute *att = NULL;
    xml_document *document = NULL;
    ssize_t ret;

    ret = xml_create_node(&device, "device");
    if (ret < 0)
        goto error;
    ret = xml_create_attribute(&att, "id", device_name);
    if (ret < 0)
        goto error;
    ret = xml_add_attribute(device, att);
    if (ret < 0)
        goto error;
    ret = xml_create_attribute(&att, "name", device_name);
    if (ret < 0)
        goto error;
    ret = xml_add_attribute(device, att);
    if (ret < 0)
        goto error;

    for (uint8_t i = 0; i < ch_no; i++) {
        ret = xml_create_node(&channel, "channel");
        if (ret < 0)
            goto error;
        sprintf(buff, "voltage%d", i);
        ret = xml_create_attribute(&att, "id", buff);
        if (ret < 0)
            goto error;
        ret = xml_add_attribute(channel, att);
        if (ret < 0)
            goto error;
        ret = xml_create_attribute(&att, "type", "input");
        if (ret < 0)
            goto error;
        ret = xml_add_attribute(channel, att);
        if (ret < 0)
            goto error;


        ret = xml_create_node(&attribute, "scan-element");
        if (ret < 0)
            goto error;
        sprintf(buff, "%d", i);
        ret = xml_create_attribute(&att, "index", buff);
        if (ret < 0)
            goto error;
        ret = xml_add_attribute(attribute, att);
        if (ret < 0)
            goto error;
        ret = xml_create_attribute(&att, "format", "le:S12/16&gt;&gt;0"); // todo
        if (ret < 0)
            goto error;
        ret = xml_add_attribute(attribute, att);
        if (ret < 0)
            goto error;
        xml_add_node(channel, attribute);

        for (uint8_t j = 0; cf_voltage_read_attrtibute_map[j].name != NULL; j++) {
            ret = xml_create_node(&attribute, "attribute");
            if (ret < 0)
                goto error;
            ret = xml_create_attribute(&att, "name", cf_voltage_read_attrtibute_map[j].name);
            if (ret < 0)
                goto error;
            ret = xml_add_attribute(attribute, att);
            if (ret < 0)
                goto error;
            sprintf(buff, "in_voltage%d_%s", i, cf_voltage_read_attrtibute_map[j].name);
            ret = xml_create_attribute(&att, "filename", buff);
            if (ret < 0)
                goto error;
            ret = xml_add_attribute(attribute, att);
            if (ret < 0)
                goto error;
            xml_add_node(channel, attribute);
        }
        xml_add_node(device, channel);
    }

    ret = xml_create_document(&document, device);
    if(ret < 0)
    {
        if(document)
            xml_delete_document(&document);
        goto error;
    }
    *xml = document->buff;

error:
    if(device)
        xml_delete_node(&device);

    return ret;
}

/**
 * get_ch_read_dac_attr_map
 * get map between attribute name and corresponding function
 * @return map
 */
attribute_map *get_adc_attr_map(const char *device_name)
{
    adc_attr_map[0].name = device_name;
    return adc_attr_map;
}

/**
 * transfer_dev_to_mem data from ADC into RAM
 * @param *device name
 * @param bytes_count
 * @return bytes_count
 */
ssize_t adc_transfer_dev_to_mem(struct axi_dmac	*rx_dmac, uint32_t address, size_t bytes_count)
{
    rx_dmac->flags = 0;
    axi_dmac_transfer(rx_dmac,
                      address, bytes_count);
    Xil_DCacheInvalidateRange(address,	bytes_count);

    return bytes_count;
}

/**
 * read data from RAM to pbuf, use "capture()" first
 * @param *device name
 * @param *buff where data's are stored
 * @param *offset to the remaining data
 * @param bytes_count
 * @return bytes_count
 */
ssize_t adc_read_dev(char *adc_ddr_baseaddr, char *pbuf, size_t offset,
                     size_t bytes_count)
{
    memcpy(pbuf, adc_ddr_baseaddr + offset, bytes_count);

    return bytes_count;
}
