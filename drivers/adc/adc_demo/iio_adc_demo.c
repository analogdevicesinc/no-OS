/***************************************************************************//**
 *   @file   iio_adc_demo.c
 *   @brief  Source file of ADC Demo iio.
 *   @author RNechita (ramona.nechita@analog.com)
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


#include <stdio.h>
#include <string.h>
#include "iio_adc_demo.h"
#include "iio_types.h"

#include "system_ADuCM3029.h"

/*
#define ADC_DEMO_CHANNEL_DEF(name,ind)	{\
			.name = name,\
			.ch_type = IIO_VOLTAGE,	\
			.channel = ind,	\
			.scan_index = ind,	\
			.indexed = true,\
			.scan_type = &adc_scan_type,\
			.attributes = adc_channel_attributes,\
			.ch_out = false	\
}
*/

/*enum iio_adc_demo_attributes {
	ADC_CHANNEL_ATTR,
	ADC_GLOBAL_ATTR,
};*/

/*
#define ADC_DEMO_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	//.show = get_demo_attr,\
	//.store = set_demo_attr\
}
*/

/*static struct iio_attribute adc_channel_attributes[] = {
	ADC_DEMO_ATTR("adc_channel_attr", ADC_CHANNEL_ATTR),
	END_ATTRIBUTES_ARRAY,
};

static struct iio_attribute adc_global_attributes[] = {
	ADC_DEMO_ATTR("adc_global_attr", ADC_GLOBAL_ATTR),
	END_ATTRIBUTES_ARRAY,
};*/

struct iio_channel iio_adc_channels[MAX_NR_CHANNELS];

static struct scan_type adc_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};


/***************************************************************************//**
 * @brief initialize the device number of channels with the number of channels in the init_param
 * @param dev - physical instance of an adc device
 * @param mask - the new number of active channels
 * @return SUCCESS in case of success.
*******************************************************************************/
int32_t init_adc_channels(void* dev, uint32_t mask)
{
	struct adc_demo_desc *desc = dev;

	for(int i = 0; i <desc->active_ch; i++)
	{
		char buff[20];
		sprintf(buff,"adc_channel_%d",i);
		struct iio_channel ch = {
				.name = buff,
				.ch_type = IIO_VOLTAGE,
				.channel = i,
				.scan_index = i,
				.indexed = true,
				.scan_type = &adc_scan_type,
				.attributes = NULL,
				.ch_out = false
		};
		iio_adc_channels[i] = ch;
	}


	return SUCCESS;
}

/*struct iio_device adc_demo_iio_descriptor = {
	.channels = iio_adc_channels,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.prepare_transfer = init_adc_channels,
	.end_transfer = close_adc_channels,
	.read_dev = (int32_t (*)())adc_read_samples,
	.debug_reg_read = (int32_t (*)()) adc_demo_reg_read,
	.debug_reg_write = (int32_t (*)()) adc_demo_reg_write
};*/




