/***************************************************************************//**
 *   @file   iio_adxrs290.h
 *   @brief  Implementation of ADXRS290 iio.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef IIO_ADXRS290_H
#define IIO_ADXRS290_H

#include "iio_types.h"

ssize_t get_adxrs290_iio_ch_raw(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel, intptr_t priv);
ssize_t get_adxrs290_iio_ch_scale(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
ssize_t set_adxrs290_iio_ch_hpf(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel, intptr_t priv);
ssize_t get_adxrs290_iio_ch_hpf(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel, intptr_t priv);
ssize_t set_adxrs290_iio_ch_lpf(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel, intptr_t priv);
ssize_t get_adxrs290_iio_ch_lpf(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel, intptr_t priv);
int32_t adxrs290_update_active_channels(void *dev, uint32_t mask);
int32_t	adxrs290_read_samples(void *device, uint16_t *buff,
			      uint32_t nb_samples);

static struct iio_attribute adxrs290_iio_vel_attrs[] = {
	{
		.name = "filter_high_pass_3db_frequency",
		.show = get_adxrs290_iio_ch_hpf,
		.store = set_adxrs290_iio_ch_hpf
	},
	{
		.name = "filter_low_pass_3db_frequency",
		.show = get_adxrs290_iio_ch_lpf,
		.store = set_adxrs290_iio_ch_lpf
	},
	{
		.name = "raw",
		.show = get_adxrs290_iio_ch_raw,
		.store = NULL
	},
	{
		.name = "scale",
		.show = get_adxrs290_iio_ch_scale,
		.store = NULL
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adxrs290_iio_temp_attrs[] = {
	{
		.name = "raw",
		.show = get_adxrs290_iio_ch_raw,
		.store = NULL
	},
	{
		.name = "scale",
		.show = get_adxrs290_iio_ch_scale,
		.store = NULL
	},
	END_ATTRIBUTES_ARRAY,
};

static struct scan_type scan_type_gyro = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

static struct scan_type scan_type_temp = {
	.sign = 's',
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

static struct iio_channel adxrs290_iio_channels[] = {
	{
		.ch_type = IIO_ANGL_VEL,
		.modified=1,
		.channel2=IIO_MOD_X,
		.scan_index = 0,
		.scan_type = &scan_type_gyro,
		.attributes = adxrs290_iio_vel_attrs,
		.ch_out = false,
	},
	{
		.ch_type = IIO_ANGL_VEL,
		.modified=1,
		.channel2=IIO_MOD_Y,
		.scan_index = 1,
		.scan_type = &scan_type_gyro,
		.attributes = adxrs290_iio_vel_attrs,
		.ch_out = false,
	},
	{
		.ch_type = IIO_TEMP,
		.scan_index = 2,
		.scan_type = &scan_type_temp,
		.attributes = adxrs290_iio_temp_attrs,
		.ch_out = false,
	}
};

//extern struct iio_device adxrs290_iio_descriptor ;
struct iio_device adxrs290_iio_descriptor = {
	.num_ch = 3,
	.channels = adxrs290_iio_channels,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.prepare_transfer = adxrs290_update_active_channels,
	.end_transfer = NULL,
	.read_dev = (int32_t (*)())adxrs290_read_samples,
	.debug_reg_read = (int32_t (*)())adxrs290_reg_read,
	.debug_reg_write = (int32_t (*)())adxrs290_reg_write,
};

#endif
