/***************************************************************************//**
 *   @file   iio_adc_demo.h
 *   @brief  Header file of ADC Demo iio.
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

#ifndef IIO_DEMO_ADC
#define IIO_DEMO_ADC

#include <stdlib.h>
#include "adc_demo.h"
#include "iio.h"

#define MAX_NR_CHANNELS 10

#define ADC_DEMO_DEV(_numch) {\
	.num_ch = _numch, \
	.attributes = adc_global_attributes,	\
	.debug_attributes = NULL,	\
	.buffer_attributes = NULL,	\
	.prepare_transfer = init_adc_channels,	\
	.end_transfer = close_adc_channels,	\
	.read_dev = (uint32_t (*)())adc_read_samples,	\
	.debug_reg_read = (int32_t (*)()) adc_demo_reg_read,	\
	.debug_reg_write = (int32_t (*)()) adc_demo_reg_write	\
}

#define ADC_DEMO_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = get_adc_demo_attr,\
	.store = set_adc_demo_attr\
}

enum iio_adc_demo_attributes {
	ADC_CHANNEL_ATTR,
	ADC_GLOBAL_ATTR,
};

extern struct iio_attribute adc_channel_attributes[];

extern struct iio_attribute adc_global_attributes[];

extern struct iio_device adc_demo_iio_descriptor;

int32_t init_adc_channels(void* dev, uint32_t mask);

#endif /* IIO_DEMO_ADC */
