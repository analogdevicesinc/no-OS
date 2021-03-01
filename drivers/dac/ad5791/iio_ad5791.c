/***************************************************************************//**
 *   @file   iio_ad5791.c
 *   @brief  Implementation of AD5791 IIO Driver.
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
 *
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <inttypes.h>
#include <stdio.h>
#include "iio_ad5791.h"
#include "error.h"

/**
 * @brief IIO get method to the 'raw' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static ssize_t ad5791_iio_get_raw(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad5791_dev *dev = (struct ad5791_dev *)device;
	int32_t ret;
	uint32_t value;

	ret = ad5791_get_register_value(dev,
					AD5791_REG_DAC,
					&value);
	if (ret != SUCCESS)
		return ret;

	return snprintf(buf, len, "%"PRIX32"", value);
}

/**
 * @brief IIO set method to the 'raw' attribute of the channel.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @param priv - Pointer
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static ssize_t ad5791_iio_set_raw(void *device, char *buf, size_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad5791_dev *dev = (struct ad5791_dev *)device;
	int32_t ret;
	uint32_t value;

	sscanf(buf, "%"PRIX32"", &value);

	ret = ad5791_set_dac_value(dev, value);
	if (ret != SUCCESS)
		return ret;

	ret = ad5791_soft_instruction(dev, AD5791_SOFT_CTRL_LDAC);
	if (ret != SUCCESS)
		return ret;

	return  len;
}

static struct scan_type const ad5791_iio_scan_type = {
	.sign = 's',
	.realbits = 20,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = true
};

static struct iio_attribute const channel_attributes[] = {
	{
		.name = "raw",
		.priv = 0,
		.show = ad5791_iio_get_raw,
		.store = ad5791_iio_set_raw
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel const ad5791_channels[] = {
	{
		.name = "ad5791_out",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.channel2 = 0,
		.scan_type = &ad5791_iio_scan_type,
		.attributes = channel_attributes,
		.ch_out = 1,
		.indexed = 1,
		.diferential = false
	}
};

struct iio_device const iio_ad5791_device = {
	.num_ch = AD5791_CH_NO,
	.channels = ad5791_channels,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.prepare_transfer = NULL,
	.end_transfer = NULL,
	.read_dev = NULL,
	.debug_reg_read = (int32_t (*)())ad5791_get_register_value,
	.debug_reg_write = (int32_t (*)())ad5791_set_register_value
};

