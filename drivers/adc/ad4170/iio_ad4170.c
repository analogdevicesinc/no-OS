/***************************************************************************//**
 *   @file   iio_ad4170.c
 *   @brief  Implementation of IIO AD4170 Driver.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <errno.h>
#include "iio_ad4170.h"
#include "ad4170.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int32_t ad4170_iio_reg_read(struct ad4170_iio_device *dev,
				   uint32_t reg, uint32_t *readval);
static int32_t ad4170_iio_reg_write(struct ad4170_iio_device *dev,
				    uint32_t reg, uint32_t writeval);
static int ad4170_iio_read_raw(void *ddev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int ad4170_iio_read_scale(void *ddev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);

/******************************************************************************/
/************************ Variable Declarations *******************************/
/******************************************************************************/
static struct iio_attribute ad4170_iio_attrs[] = {
	{
		.name = "raw",
		.show = ad4170_iio_read_raw,
	},
	{
		.name   = "scale",
		.show   = ad4170_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type ad4170_signed_scan_type = {
	.sign = 's',
	.realbits = AD4170_PRECISION_BITS,
	.storagebits = 32,
	.shift = 8,
	.is_big_endian = true,
};

static struct scan_type ad4170_unsigned_scan_type = {
	.sign = 'u',
	.realbits = AD4170_PRECISION_BITS,
	.storagebits = 32,
	.shift = 8,
	.is_big_endian = true,
};

static struct iio_channel ad4170_channels[] = {
	{
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.channel = index,
		.scan_type = &ad4170_signed_scan_type,
		.scan_index = index,
		.attributes = ad4170_iio_attrs,
		.ch_out = false
	},
};

static struct iio_device ad4170_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ad4170_channels),
	.channels = ad4170_channels,
	.debug_reg_read = (int32_t (*)()) ad4170_iio_reg_read,
	.debug_reg_write = (int32_t (*)()) ad4170_iio_reg_write,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static int32_t ad4170_iio_reg_read(struct ad4170_iio_device *dev,
				   uint32_t reg, uint32_t *readval)
{
	uint16_t temp;
	uint32_t ri;
	int ret;

	for (ri = 0; ri < NO_OS_ARRAY_SIZE(ad4170_regs); ri++) {
		if (reg == AD4170_ADDR(ri)) {
			ret = ad4170_spi_reg_read(dev->dev, ad4170_regs[ri],
						  &temp);
			break;
		}
	}
	if (!ret)
		*readval = temp;

	return ret;
}

static int32_t ad4170_iio_reg_write(struct ad4170_iio_device *dev,
				    uint32_t reg, uint32_t writeval)
{
	uint32_t ri;
	int ret;

	for (ri = 0; ri < NO_OS_ARRAY_SIZE(ad4170_regs); ri++)
		if (reg == AD4170_ADDR(ri))
			break;

	return ad4170_spi_reg_write(dev->dev, ad4170_regs[ri], writeval)
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
static int ad4170_iio_read_raw(void *ddev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad4170_iio_device *dev = ddev;
	struct ad4170_dev *dev = dev->dev;
	struct ad4170_config *config = &dev->config;
	int32_t val;
	int ret;

	ret = ad4170_set_channel_en(dev->dev, channel->chan_num);
	if (ret)
		return ret;

	ret = ad4170_get_ch_data(dev->dev, channel->cha, &val);
	if (ret)
		return ret;

	if (config->setups.afe.bipolar)
		val = no_os_sign_extend32(val, channel->scan_type.realbits - 1);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
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
static int ad4170_iio_read_scale(void *ddev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad4170_iio_device *dev = ddev;
	struct ad4170_dev *dev = dev->dev;
	struct ad4170_config *config = &dev->config;
	int32_t vals[2];

	switch (config->setups.afe.ref_select) {
	case AD4170_REFIN_REFIN1:
	case AD4170_REFIN_REFIN2:
		return -EOPNOTSUPP;
	case AD4170_REFIN_REFOUT:
		vals[0] = 2500; /* 2.5V internal reference */
		break;
	case AD4170_REFIN_AVDD:
		vals[0] = 5000; /* 5V supply */
		break;
	}

	/*
	 * When bipolar config is set, data is output in two's complement and
	 * one of the output bits is used for sign indication, leaving only 23
	 * bit left for expressing voltage magnitude.
	 */
	if (config->setups.afe.bipolar)
		vals[1] = channel->scan_type.realbits - 1;
	else
		vals[1] = channel->scan_type.realbits;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 1, &val);
}

/**
 * @brief Initializes the AD4170 IIO driver
 * @param iio_device - The iio device structure.
 * @param iio_init_param - Parameters for the initialization of iio_dev
 * @return 0 in case of success, errno errors otherwise
 */
int ad4170_iio_init(struct ad4170_iio_device **iio_device,
		    struct ad4170_iio_init_param *iio_init_param)
{
	struct ad4170_iio_device *iio_device_temp;
	int ret;

	if (!iio_init_param || !iio_init_param->init_param)
		return -EINVAL;

	iio_device_temp = no_os_calloc(1, sizeof(*iio_device_temp));
	if (!iio_device_temp)
		return -ENOMEM;

	ret = ad4170_init(&iio_device_temp->dev, iio_init_param->init_param);
	if (ret)
		goto free_dev;

	iio_device_temp->iio_dev = &ad4170_iio_dev;

	*iio_device = iio_device_temp;

	return 0;

free_dev:
	no_os_free(iio_device_temp);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param iio_device - The iio device structure.
 * @return 0 in case of success, errno errors otherwise
 */
int ad4170_iio_remove(struct ad4170_iio_device *iio_device)
{
	ad4170_remove(iio_device->dev);
	no_os_free(iio_device);
	return 0;
}
