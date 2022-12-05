/***************************************************************************//**
 *   @file   iio_adc_demo.c
 *   @brief  Implementation of iio_adc_demo.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_adc_demo.h"
#include "iio.h"

/**
 * @brief utility function for computing next upcoming channel
 * @param ch_mask - active channels .
 * @param last_idx -  previous index.
 * @param new_idx - upcoming channel index, return param.
 * @return 1 if there are more channels, 0 if done.
 */
static bool get_next_ch_idx(uint32_t ch_mask, uint32_t last_idx,
			    uint32_t *new_idx)
{
	last_idx++;
	ch_mask >>= last_idx;
	if (!ch_mask) {
		*new_idx = -1;
		return 0;
	}
	while (!(ch_mask & 1)) {
		last_idx++;
		ch_mask >>= 1;
	}
	*new_idx = last_idx;

	return 1;
}

/**
 * @brief get attributes for adc.
 * @param device- Physical instance of a iio_demo_device.
 * @param buf - Where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @param attr_id - Attribute ID
 * @return Length of chars written in buf, or negative value on failure.
 */
int get_adc_demo_attr(void *device, char *buf, uint32_t len,
		      const struct iio_ch_info *channel, intptr_t attr_id)
{
	struct adc_demo_desc *desc;

	if(!device)
		return -ENODEV;

	desc = device;

	switch(attr_id) {
	case ADC_GLOBAL_ATTR:
		return snprintf(buf,len,"%"PRIu32"",desc->adc_global_attr);
	case ADC_CHANNEL_ATTR:
		return snprintf(buf,len,"%"PRIu32"",desc->adc_ch_attr[channel->ch_num]);
	default:
		return -EINVAL;
	}

	return -EINVAL;
}

/**
 * @brief set attributes for adc.
 * @param device - Physical instance of a iio_demo_device.
 * @param buf - Value to be written to attribute.
 * @param len -	Length of the data in "buf".
 * @param channel - Channel properties.
 * @param attr_id - Attribute ID
 * @return: Number of bytes written to device, or negative value on failure.
 */
int set_adc_demo_attr(void *device, char *buf, uint32_t len,
		      const struct iio_ch_info *channel, intptr_t attr_id)
{
	struct adc_demo_desc *desc;
	uint32_t value = no_os_str_to_uint32(buf);

	if(!device)
		return -ENODEV;

	desc = device;

	switch(attr_id) {
	case ADC_GLOBAL_ATTR:
		desc->adc_global_attr = value;
		return len;
	case ADC_CHANNEL_ATTR:
		desc->adc_ch_attr[channel->ch_num] = value;
		return len;
	default:
		return -EINVAL;
	}

	return -EINVAL;
}

/**
 * @brief function for reading samples from the device.
 * @param dev_data  - The iio device data structure.
 * @return the number of read samples.
 */
int32_t adc_submit_samples(struct iio_device_data *dev_data)
{
	struct adc_demo_desc *desc;
	uint32_t k = 0;
	uint32_t ch = -1;
	uint16_t buff[TOTAL_ADC_CHANNELS];
	uint32_t i;
	uint16_t *ch_buf_ptr;

	if(!dev_data)
		return -ENODEV;

	desc = (struct adc_demo_desc *)dev_data->dev;

	if(desc->ext_buff == NULL) {
		int offset_per_ch = NO_OS_ARRAY_SIZE(sine_lut) / TOTAL_ADC_CHANNELS;
		for(i = 0; i < dev_data->buffer->size / dev_data->buffer->bytes_per_scan; i++) {
			while(get_next_ch_idx(desc->active_ch, ch, &ch))
				buff[k++] = sine_lut[(i + ch * offset_per_ch ) % NO_OS_ARRAY_SIZE(sine_lut)];
			k = 0;
			iio_buffer_push_scan(dev_data->buffer, buff);
		}
		return dev_data->buffer->size / dev_data->buffer->bytes_per_scan;
	}

	for(i = 0; i < dev_data->buffer->size / dev_data->buffer->bytes_per_scan; i++) {
		while(get_next_ch_idx(desc->active_ch, ch, &ch)) {
			ch_buf_ptr = (uint16_t*)desc->ext_buff + (ch * desc->ext_buff_len);
			buff[k++] = ch_buf_ptr[i];
		}
		k = 0;
		iio_buffer_push_scan(dev_data->buffer, buff);
	}
	return dev_data->buffer->size / dev_data->buffer->bytes_per_scan;
}


/**
 * @brief Handles trigger: reads one data-set and writes it to the buffer.
 *
 * @param dev_data  - The iio device data structure.
 *
 * @return ret - Result of the handling procedure. In case of success, the size
 * 				 of the written data is returned.
 */
int32_t adc_demo_trigger_handler(struct iio_device_data *dev_data)
{
	struct adc_demo_desc *desc;
	uint32_t k = 0;
	uint32_t ch = -1;
	uint16_t buff[TOTAL_ADC_CHANNELS];
	static uint32_t i = 0;
	uint16_t *ch_buf_ptr;

	if (!dev_data)
		return -EINVAL;

	desc = (struct adc_demo_desc *)dev_data->dev;

	if(desc->ext_buff == NULL) {
		int offset_per_ch = NO_OS_ARRAY_SIZE(sine_lut) / TOTAL_ADC_CHANNELS;
		while(get_next_ch_idx(desc->active_ch, ch, &ch))
			buff[k++] = sine_lut[(i + ch * offset_per_ch ) % NO_OS_ARRAY_SIZE(sine_lut)];
		if (i == NO_OS_ARRAY_SIZE(sine_lut))
			i = 0;
		else
			i++;

		return iio_buffer_push_scan(dev_data->buffer, buff);
	}

	while(get_next_ch_idx(desc->active_ch, ch, &ch)) {
		ch_buf_ptr = (uint16_t*)desc->ext_buff + (ch * desc->ext_buff_len);
		buff[k++] = ch_buf_ptr[i];
	}
	if (i == (desc->ext_buff_len - 1))
		i = 0;
	else
		i++;

	return iio_buffer_push_scan(dev_data->buffer, buff);
}

#define ADC_DEMO_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = get_adc_demo_attr,\
	.store = set_adc_demo_attr \
}

struct scan_type adc_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

struct iio_attribute adc_channel_attributes[] = {
	ADC_DEMO_ATTR("adc_channel_attr", ADC_CHANNEL_ATTR),
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute iio_adc_global_attributes[] = {
	ADC_DEMO_ATTR("adc_global_attr", ADC_GLOBAL_ATTR),
	END_ATTRIBUTES_ARRAY,
};

#define IIO_DEMO_ADC_CHANNEL(_idx) {\
	.name = "adc_in_ch" # _idx,\
	.ch_type = IIO_VOLTAGE,\
	.channel = _idx,\
	.scan_index = _idx,\
	.indexed = true,\
	.scan_type = &adc_scan_type,\
	.attributes = adc_channel_attributes,\
	.ch_out = false,\
}

static struct iio_channel iio_adc_channels[] = {
	IIO_DEMO_ADC_CHANNEL(0),
	IIO_DEMO_ADC_CHANNEL(1),
	IIO_DEMO_ADC_CHANNEL(2),
	IIO_DEMO_ADC_CHANNEL(3),
	IIO_DEMO_ADC_CHANNEL(4),
	IIO_DEMO_ADC_CHANNEL(5),
	IIO_DEMO_ADC_CHANNEL(6),
	IIO_DEMO_ADC_CHANNEL(7),
	IIO_DEMO_ADC_CHANNEL(8),
	IIO_DEMO_ADC_CHANNEL(9),
	IIO_DEMO_ADC_CHANNEL(10),
	IIO_DEMO_ADC_CHANNEL(11),
	IIO_DEMO_ADC_CHANNEL(12),
	IIO_DEMO_ADC_CHANNEL(13),
	IIO_DEMO_ADC_CHANNEL(14),
	IIO_DEMO_ADC_CHANNEL(15),
};

struct iio_device adc_demo_iio_descriptor = {
	.num_ch = TOTAL_ADC_CHANNELS,
	.channels = iio_adc_channels,
	.attributes = iio_adc_global_attributes,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = update_adc_channels,
	.post_disable = close_adc_channels,
	.trigger_handler = (int32_t (*)())adc_demo_trigger_handler,
	.submit = (int32_t (*)())adc_submit_samples,
	.debug_reg_read = (int32_t (*)()) adc_demo_reg_read,
	.debug_reg_write = (int32_t (*)()) adc_demo_reg_write
};

