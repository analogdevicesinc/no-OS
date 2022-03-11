/***************************************************************************//**
 *   @file   iio_ad463x.c
 *   @brief  Implementation of iio_ad463x.c.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#ifdef IIO_SUPPORT

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "ad463x.h"
#include "iio_ad463x.h"
#include "no_os_error.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

#define BITS_PER_SAMPLE 32

static struct scan_type adc_scan_type = {
	.sign = 'u',
	.realbits = BITS_PER_SAMPLE,
	.storagebits = BITS_PER_SAMPLE,
	.shift = 0,
	.is_big_endian = false
};

#define IIO_AD463x_CHANNEL(_idx) {\
	.name = "ch" # _idx,\
	.ch_type = IIO_VOLTAGE,\
	.channel = _idx,\
	.scan_index = _idx,\
	.indexed = true,\
	.scan_type = &adc_scan_type,\
	.ch_out = false,\
}

static struct iio_channel iio_adc_channels[] = {
	IIO_AD463x_CHANNEL(0),
	IIO_AD463x_CHANNEL(1),
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static int32_t _iio_ad463x_prepare_transfer(struct iio_ad463x *desc,
		uint32_t mask)
{
	if (!desc)
		return -EINVAL;

	desc->mask = mask;

	return 0;
}

static int32_t _iio_ad463x_read_dev(struct iio_ad463x *desc, uint32_t *buff,
				    uint32_t nb_samples)
{
	int ret;
	uint32_t data[nb_samples], i, j, ch;

	if (!desc)
		return -EINVAL;

	ret = ad463x_read_data(desc->ad463x_desc, data, nb_samples);
	if(ret)
		return ret;

	for(i = 0, j = 0; i < nb_samples; i++)
		for (ch = 0; ch < desc->iio_dev_desc.num_ch; ch++)
			if (desc->mask & NO_OS_BIT(ch)) {
				buff[j++] = data[i];
			}

	return nb_samples;
}

/**
 * @brief Init for reading/writing and parameterization of a
 * ad463x device.
 * @param desc - Descriptor.
 * @param param - Configuration structure.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t iio_ad463x_init(struct iio_ad463x **desc,
			struct ad463x_dev *dev)
{
	struct iio_ad463x *iio_ad463x;

	iio_ad463x = (struct iio_ad463x *)calloc(1, sizeof(struct iio_ad463x));
	if (!iio_ad463x)
		return -1;

	iio_ad463x->ad463x_desc = dev;
	iio_ad463x->iio_dev_desc = ad463x_iio_desc;

	*desc = iio_ad463x;

	return 0;
}

/**
 * @brief Release resources.
 * @param desc - Descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t iio_ad463x_remove(struct iio_ad463x *desc)
{
	if (!desc)
		return -1;

	free(desc);

	return 0;
}

struct iio_device ad463x_iio_desc = {
	.channels = iio_adc_channels,
	.num_ch = 2,
	.pre_enable = (int32_t (*)())_iio_ad463x_prepare_transfer,
	.read_dev = (int32_t (*)())_iio_ad463x_read_dev
};

#endif /* IIO_SUPPORT */
