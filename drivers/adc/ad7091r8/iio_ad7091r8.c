/***************************************************************************//**
 *   @file   iio_ad7091r8.c
 *   @brief  Implementation of IIO AD7091R8 Driver.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_ad7091r8.h"
#include "ad7091r8.h"
#include "no_os_units.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int ad7091r8_iio_read_reg(struct ad7091r8_iio_dev *dev, uint32_t reg,
				 uint32_t *readval);
static int ad7091r8_iio_write_reg(struct ad7091r8_iio_dev *dev, uint32_t reg,
				  uint32_t writeval);
static int ad7091r8_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int ad7091r8_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);
static int ad7091r8_buffer_preenable(void* dev, uint32_t mask);
static int32_t ad7091r8_trigger_handler(struct iio_device_data *iio_dev_data);

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
static struct iio_attribute ad7091r8_iio_attrs[] = {
	{
		.name = "raw",
		.show = ad7091r8_iio_read_raw,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = ad7091r8_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type ad7091r8_iio_scan_type = {
	.sign = 'u',
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

#define AD7091R8_CHANNEL(index) {			\
	.ch_type = IIO_VOLTAGE,				\
	.indexed = 1,					\
	.channel = index,				\
	.scan_type = &ad7091r8_iio_scan_type,		\
	.scan_index = index,				\
	.attributes = ad7091r8_iio_attrs,		\
	.ch_out = false					\
}

static struct iio_channel ad7091r2_channels[] = {
	AD7091R8_CHANNEL(0),
	AD7091R8_CHANNEL(1),
};

static struct iio_channel ad7091r4_channels[] = {
	AD7091R8_CHANNEL(0),
	AD7091R8_CHANNEL(1),
	AD7091R8_CHANNEL(2),
	AD7091R8_CHANNEL(3),
};

static struct iio_channel ad7091r8_channels[] = {
	AD7091R8_CHANNEL(0),
	AD7091R8_CHANNEL(1),
	AD7091R8_CHANNEL(2),
	AD7091R8_CHANNEL(3),
	AD7091R8_CHANNEL(4),
	AD7091R8_CHANNEL(5),
	AD7091R8_CHANNEL(6),
	AD7091R8_CHANNEL(7),
};

static struct iio_device ad7091r2_iio_device = ad7091r8_iio_device(
			ad7091r2_channels);
static struct iio_device ad7091r4_iio_device = ad7091r8_iio_device(
			ad7091r4_channels);
static struct iio_device ad7091r8_iio_device = ad7091r8_iio_device(
			ad7091r8_channels);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static int ad7091r8_iio_read_reg(struct ad7091r8_iio_dev *dev, uint32_t reg,
				 uint32_t *readval)
{
	uint16_t read_data;
	int ret;

	return ad7091r8_spi_reg_read(dev->ad7091r8_dev, reg, &read_data);
	if (ret)
		return ret;

	*readval = read_data;
	return 0;
}

static int ad7091r8_iio_write_reg(struct ad7091r8_iio_dev *dev, uint32_t reg,
				  uint32_t writeval)
{
	return ad7091r8_spi_reg_write(dev->ad7091r8_dev, reg, writeval);
}

/***************************************************************************//**
 * @brief Handles the read request for raw attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 *		    In case of success, the size of the read data is returned.
*******************************************************************************/
static int ad7091r8_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad7091r8_iio_dev *iio_ad7091r8;
	struct ad7091r8_dev *ad7091r8_dev;
	int32_t read_val_32;
	uint16_t read_val;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_ad7091r8 = (struct ad7091r8_iio_dev *)dev;
	ad7091r8_dev = iio_ad7091r8->ad7091r8_dev;
	ret = ad7091r8_read_one(ad7091r8_dev, channel->ch_num, &read_val);
	if (ret)
		return ret;

	if ((int16_t)no_os_field_get(AD7091R8_REG_RESULT_CH_ID_MASK, read_val)
	    != channel->ch_num)
		return -EIO;

	read_val_32 = no_os_field_get(AD7091R8_REG_RESULT_DATA_MASK, read_val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &read_val_32);
}

/***************************************************************************//**
 * @brief Handles the read request for scale attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 		    In case of success, the size of the read data is returned.
*******************************************************************************/
static int ad7091r8_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad7091r8_iio_dev *iio_ad7091r8;
	struct ad7091r8_dev *ad7091r8_dev;
	int32_t vals[2];

	if (!dev)
		return -EINVAL;

	iio_ad7091r8 = (struct ad7091r8_iio_dev *)dev;
	ad7091r8_dev = iio_ad7091r8->ad7091r8_dev;
	vals[0] = ad7091r8_dev->vref_mv;
	vals[1] = 12;
	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
}

/***************************************************************************//**
 * @brief Prepares the device for buffered capture mode.
 *
 * @param dev     - The iio device structure.
 * @param mask    - Mask of enabled/disabled channels.
 *
 * @return ret    - Result of the pre enable setup.
 * 		    Zero in case of success, errno otherwise.
*******************************************************************************/
static int ad7091r8_buffer_preenable(void* dev, uint32_t mask)
{
	struct ad7091r8_iio_dev *iio_ad7091r8;
	struct ad7091r8_dev *ad7091r8;
	uint16_t dummy;
	int ret;

	if (!dev)
		return -ENODEV;

	iio_ad7091r8 = dev;
	ad7091r8 = iio_ad7091r8->ad7091r8_dev;

	/* Dummy read cycle to pulse CONVST */
	ret = ad7091r8_pulse_convst(ad7091r8);
	if (ret)
		return ret;

	/* Set enabled channels in the channel register */
	ret = ad7091r8_spi_reg_write(ad7091r8, AD7091R8_REG_CHANNEL, mask);
	if (ret)
		return ret;

	/* Dummy read cycle to let the conversion sequence get updated */
	return ad7091r8_sequenced_read(ad7091r8, &dummy);
}

/***************************************************************************//**
 * @brief Prepares the device for buffered capture mode.
 *
 * @param iio_dev_data    - Object with pointers to ad7091r8_iio_dev and buffer.
 *
 * @return ret            - Result of the trigger handler routine.
 * 		            Zero in case of success, errno otherwise.
*******************************************************************************/
static int32_t ad7091r8_trigger_handler(struct iio_device_data *iio_dev_data)
{
	struct ad7091r8_iio_dev *iio_ad7091r8;
	struct ad7091r8_dev *ad7091r8;
	uint16_t data_buff[8]; /* Data buffer to store one sample-set. */
	uint16_t read_val;
	int i, k = 0;
	int ret;

	if (!iio_dev_data)
		return -EINVAL;

	iio_ad7091r8 = (struct ad7091r8_iio_dev *)iio_dev_data->dev;

	if (!iio_ad7091r8)
		return -EINVAL;

	ad7091r8 = iio_ad7091r8->ad7091r8_dev;

	/* For each enabled channel do a sequenced read and push result to buff */
	for (i = 0; i < AD7091R_NUM_CHANNELS(ad7091r8->device_id); i++) {
		if (iio_dev_data->buffer->active_mask & NO_OS_BIT(i)) {
			ret = ad7091r8_sequenced_read(ad7091r8, &read_val);
			if (ret)
				return ret;

			data_buff[k++] = read_val;
		}
	}

	return iio_buffer_push_scan(iio_dev_data->buffer, data_buff);
}

/***************************************************************************//**
 * @brief Initializes the AD7091R8 IIO driver
 *
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int ad7091r8_iio_init(struct ad7091r8_iio_dev **iio_dev,
		      struct ad7091r8_iio_dev_init_param *init_param)
{
	int ret;
	struct ad7091r8_iio_dev *desc;

	if (!init_param || !init_param->ad7091r8_dev_init)
		return -EINVAL;

	desc = (struct ad7091r8_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	switch (init_param->ad7091r8_dev_init->device_id) {
	case AD7091R2:
		desc->iio_dev = &ad7091r2_iio_device;
		break;
	case AD7091R4:
		desc->iio_dev = &ad7091r4_iio_device;
		break;
	case AD7091R8:
		desc->iio_dev = &ad7091r8_iio_device;
		break;
	default:
		ret = -EINVAL;
		goto error_ad7091r8_init;
	}

	// Initialize ad7091r8 driver
	ret = ad7091r8_init(&desc->ad7091r8_dev, init_param->ad7091r8_dev_init);
	if (ret)
		goto error_ad7091r8_init;

	*iio_dev = desc;

	return 0;

error_ad7091r8_init:
	no_os_free(desc);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7091r8_iio_init().
 *
 * @param desc - The IIO device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int ad7091r8_iio_remove(struct ad7091r8_iio_dev *desc)
{
	int ret;

	ret = ad7091r8_remove(desc->ad7091r8_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
