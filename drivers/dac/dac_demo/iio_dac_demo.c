/***************************************************************************//**
 *   @file   iio_dac_demo.c
 *   @brief  Implementation of iio_dac_demo.
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

#include "iio_dac_demo.h"

#define DAC_DEMO_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = get_dac_demo_attr,\
	.store = set_dac_demo_attr\
}

#define IIO_DEMO_DAC_CHANNEL(_idx) {\
	.name = "dac_out_ch" # _idx,\
	.ch_type = IIO_VOLTAGE,\
	.channel = _idx,\
	.scan_index = _idx,\
	.indexed = true,\
	.scan_type = &dac_scan_type,\
	.attributes = dac_channel_attributes,\
	.ch_out = true,\
}

struct scan_type dac_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

struct iio_attribute dac_channel_attributes[] = {
	DAC_DEMO_ATTR("dac_channel_attr", DAC_CHANNEL_ATTR),
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute dac_global_attributes[] = {
	DAC_DEMO_ATTR("dac_global_attr", DAC_GLOBAL_ATTR),
	END_ATTRIBUTES_ARRAY,
};

static struct iio_channel iio_dac_channels[] = {
	IIO_DEMO_DAC_CHANNEL(0),
	IIO_DEMO_DAC_CHANNEL(1),
	IIO_DEMO_DAC_CHANNEL(2),
	IIO_DEMO_DAC_CHANNEL(3),
	IIO_DEMO_DAC_CHANNEL(4),
	IIO_DEMO_DAC_CHANNEL(5),
	IIO_DEMO_DAC_CHANNEL(6),
	IIO_DEMO_DAC_CHANNEL(7),
	IIO_DEMO_DAC_CHANNEL(8),
	IIO_DEMO_DAC_CHANNEL(9),
	IIO_DEMO_DAC_CHANNEL(10),
	IIO_DEMO_DAC_CHANNEL(11),
	IIO_DEMO_DAC_CHANNEL(12),
	IIO_DEMO_DAC_CHANNEL(13),
	IIO_DEMO_DAC_CHANNEL(14),
	IIO_DEMO_DAC_CHANNEL(15)
};

struct iio_device const dac_demo_iio_descriptor = {
	.num_ch = TOTAL_DAC_CHANNELS,
	.channels = iio_dac_channels,
	.attributes = dac_global_attributes,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.prepare_transfer = (int32_t (*)())update_dac_channels,
	.end_transfer = close_dac_channels,
	.read_dev = (int32_t (*)())dac_write_samples,
	.debug_reg_read = (int32_t (*)()) dac_demo_reg_read,
	.debug_reg_write = (int32_t (*)()) dac_demo_reg_write
};

