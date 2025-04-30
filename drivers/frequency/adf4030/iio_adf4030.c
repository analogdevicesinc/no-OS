/***************************************************************************//**
 *   @file   iio_adf4030.c
 *   @brief  Implementation of IIO ADF4030 Driver.
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_adf4030.h"
#include "adf4030.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

/**
 * @brief Wrapper for reading adf4030 register.
 * @param dev	  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 * @return	  - Result of the reading procedure.
 */
static int adf4030_iio_read_reg(struct adf4030_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	uint8_t tmp;
	int ret;

	ret = adf4030_spi_read(dev->adf4030_dev, (uint16_t)reg, &tmp);
	if (ret)
		return ret;

	*readval = tmp;
	return 0;
}

/**
 * @brief Wrapper for writing adf4030 register.
 * @param dev	   - The iio device structure.
 * @param reg	   - Address of the register to be written to.
 * @param writeval - Data to be written.
 * @return	   - Result of the writing procedure.
 */
static int adf4030_iio_write_reg(struct adf4030_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	return adf4030_spi_write(dev->adf4030_dev, (uint16_t)reg,
				 (uint8_t)writeval);
}

/**
 * @brief Handles the read request for the device attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int adf4030_iio_read_device_attr(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{

	struct adf4030_iio_dev *iio_adf4030 = (struct adf4030_iio_dev *)dev;
	struct adf4030_dev *adf4030;
	int64_t val = 0;
	int ret;

	if (!iio_adf4030)
		return -EINVAL;

	adf4030 = iio_adf4030->adf4030_dev;

	if (!adf4030)
		return -EINVAL;

	switch (priv) {
	case ADF4030_IIO_DEV_ATTR_DEFAULT_REGS:
		val = -99; // DUMMY DATA
		break;

	case ADF4030_IIO_DEV_ATTR_CHIP_ADDR:
		val = adf4030->chip_addr;
		break;

	case ADF4030_IIO_DEV_ATTR_REF_FREQ:
		val = adf4030->ref_freq;
		break;

	case ADF4030_IIO_DEV_ATTR_VCO_FREQ:
		ret = adf4030_get_vco_freq(adf4030, &val);
		break;

	case ADF4030_IIO_DEV_ATTR_BSYNCA_FREQ:
		ret = adf4030_get_bsync_freq(adf4030, &val, false);
		break;

	case ADF4030_IIO_DEV_ATTR_BSYNCB_FREQ:
		ret = adf4030_get_bsync_freq(adf4030, &val, true);
		break;

	case ADF4030_IIO_DEV_ATTR_TDC_SOURCE_CH:
		ret = adf4030_get_tdc_source(adf4030, &val);
		break;

	case ADF4030_IIO_DEV_ATTR_TDC_MEASUREMENT:
		ret = adf4030_get_tdc_measurement(adf4030, &val);
		break;

	case ADF4030_IIO_DEV_ATTR_ALIGNMENT_ITER:
		ret = adf4030_get_alignment_iter(adf4030, &val);
		break;

	case ADF4030_IIO_DEV_ATTR_ALIGNMENT_THRESHOLD:
		ret = adf4030_get_alignment_threshold(adf4030, &val);
		break;

	case ADF4030_IIO_DEV_ATTR_SINGLE_CH_ALINGMENT:
		val = 1; // DUMMY DATA
		break;

	case ADF4030_IIO_DEV_ATTR_SERIAL_CH_ALINGMENT:
		ret = adf4030_get_serial_alignment(adf4030, &val);
		break;

	case ADF4030_IIO_DEV_ATTR_BACKGROUND_SERIAL_ALINGMENT:
		ret = adf4030_get_background_serial_alignment(adf4030, &val);
		break;

	case ADF4030_IIO_DEV_ATTR_TEMP:
		ret = adf4030_get_temperature(adf4030, &val);
		break;

	default:
		return -EINVAL;
	};

	if (ret)
		return ret;

	return snprintf(buf, len, "%lli", val);
}

/**
 * @brief Handles the write request for the device attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int adf4030_iio_write_device_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{

	struct adf4030_iio_dev *iio_adf4030 = (struct adf4030_iio_dev *)dev;
	struct adf4030_dev *adf4030;
	int64_t val = 0;
	int ret;

	if (!iio_adf4030)
		return -EINVAL;

	adf4030 = iio_adf4030->adf4030_dev;

	if (!adf4030)
		return -EINVAL;

	sscanf(buf, "%lli", &val);

	switch (priv) {
	case ADF4030_IIO_DEV_ATTR_DEFAULT_REGS:
		ret = adf4030_set_default_regs(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_CHIP_ADDR:
		ret = adf4030_set_chip_address(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_REF_FREQ:
		ret = adf4030_set_ref_clk(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_VCO_FREQ:
		ret = adf4030_set_vco_freq(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_BSYNCA_FREQ:
		ret = adf4030_set_bsync_freq(adf4030, val, false);
		break;

	case ADF4030_IIO_DEV_ATTR_BSYNCB_FREQ:
		ret = adf4030_set_bsync_freq(adf4030, val, true);
		break;

	case ADF4030_IIO_DEV_ATTR_TDC_SOURCE_CH:
		ret = adf4030_set_tdc_source(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_TDC_MEASUREMENT:
		ret = adf4030_set_tdc_measurement(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_ALIGNMENT_ITER:
		ret = adf4030_set_alignment_iter(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_ALIGNMENT_THRESHOLD:
		ret = adf4030_set_alignment_threshold(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_SINGLE_CH_ALINGMENT:
		ret = adf4030_set_single_ch_alignment(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_SERIAL_CH_ALINGMENT:
		ret = adf4030_set_serial_alignment(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_BACKGROUND_SERIAL_ALINGMENT:
		ret = adf4030_set_background_serial_alignment(adf4030, val);
		break;

	case ADF4030_IIO_DEV_ATTR_TEMP:
		ret = adf4030_set_temperature(adf4030, val);
		break;

	default:
		return -EINVAL;
	};

	if (ret)
		return ret;

	return ret;
}

/**
 * @brief Handles the read request for the channel attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int adf4030_iio_read_channel_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{

	struct adf4030_iio_dev *iio_adf4030 = (struct adf4030_iio_dev *)dev;
	struct adf4030_dev *adf4030;
	int64_t val = 0;
	int ret;

	if (!iio_adf4030)
		return -EINVAL;

	adf4030 = iio_adf4030->adf4030_dev;

	if (!adf4030)
		return -EINVAL;

	switch (priv) {
	case ADF4030_IIO_CH_ATTR_DIRECTION:
		ret = adf4030_get_channel_direction(adf4030, channel->ch_num, &val);
		break;

	case ADF4030_IIO_CH_ATTR_DELAY:
		ret = adf4030_get_channel_delay(adf4030, channel->ch_num, &val);
		break;

	case ADF4030_IIO_CH_ATTR_ODIVB_EN:
		ret = adf4030_get_channel_odivb(adf4030, channel->ch_num, &val);
		break;

	case ADF4030_IIO_CH_ATTR_TERMINATION:
		ret = adf4030_get_channel_termination(adf4030, channel->ch_num, &val);
		break;

	case ADF4030_IIO_CH_ATTR_PRBS:
		ret = adf4030_get_channel_prbs(adf4030, channel->ch_num, &val);
		break;

	case ADF4030_IIO_CH_ATTR_INVERT:
		ret = adf4030_get_channel_invert(adf4030, channel->ch_num, &val);
		break;

	case ADF4030_IIO_CH_ATTR_VOLTAGE:
		ret = adf4030_get_channel_voltage(adf4030, channel->ch_num, &val);
		break;

	default:
		return -EINVAL;
	};

	if (ret)
		return ret;

	return snprintf(buf, len, "%lli", val);
}

/**
 * @brief Handles the write request for the channel attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return 	  - The size of the read data in case of success, error code
 * 		    otherwise.
 */
static int adf4030_iio_write_channel_attr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{

	struct adf4030_iio_dev *iio_adf4030 = (struct adf4030_iio_dev *)dev;
	struct adf4030_dev *adf4030;
	int64_t val = 0;
	int ret;

	if (!iio_adf4030)
		return -EINVAL;

	adf4030 = iio_adf4030->adf4030_dev;

	if (!adf4030)
		return -EINVAL;

	sscanf(buf, "%lli", &val);

	switch (priv) {
	case ADF4030_IIO_CH_ATTR_DIRECTION:
		ret = adf4030_set_channel_direction(adf4030, channel->ch_num, val);
		break;

	case ADF4030_IIO_CH_ATTR_DELAY:
		ret = adf4030_set_channel_delay(adf4030, channel->ch_num, val);
		break;

	case ADF4030_IIO_CH_ATTR_ODIVB_EN:
		ret = adf4030_set_channel_odivb(adf4030, channel->ch_num, val);
		break;

	case ADF4030_IIO_CH_ATTR_TERMINATION:
		ret = adf4030_set_channel_termination(adf4030, channel->ch_num, val);
		break;

	case ADF4030_IIO_CH_ATTR_PRBS:
		ret = adf4030_set_channel_prbs(adf4030, channel->ch_num, val);
		break;

	case ADF4030_IIO_CH_ATTR_INVERT:
		ret = adf4030_set_channel_invert(adf4030, channel->ch_num, val);
		break;

	case ADF4030_IIO_CH_ATTR_VOLTAGE:
		ret = adf4030_set_channel_voltage(adf4030, channel->ch_num, val);
		break;

	default:
		return -EINVAL;
	};

	if (ret)
		return ret;

	return ret;
}

static struct iio_attribute adf4030_iio_attrs[] = {
	{
		.name = "default_register",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_DEFAULT_REGS,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "chip_address",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_CHIP_ADDR,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "reference_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_REF_FREQ,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "vco_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_VCO_FREQ,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "bsync_freq_odiva",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_BSYNCA_FREQ,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "bsync_freq_odivb",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_BSYNCB_FREQ,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "tdc_source_ch",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_TDC_SOURCE_CH,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "tdc_measurement",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_TDC_MEASUREMENT,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "alignment_iter",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_ALIGNMENT_ITER,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "alignment_threshold",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_ALIGNMENT_THRESHOLD,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "single_ch_alignment",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_SINGLE_CH_ALINGMENT,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "serial_alignment",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_SERIAL_CH_ALINGMENT,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "background_serial_alignment",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_BACKGROUND_SERIAL_ALINGMENT,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	{
		.name = "temperature",
		.shared = IIO_SHARED_BY_ALL,
		.priv = ADF4030_IIO_DEV_ATTR_TEMP,
		.show = adf4030_iio_read_device_attr,
		.store = adf4030_iio_write_device_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adf4030_iio_ch_attrs[] = {
	{
		.name = "direction",
		.shared = IIO_SEPARATE,
		.priv = ADF4030_IIO_CH_ATTR_DIRECTION,
		.show = adf4030_iio_read_channel_attr,
		.store = adf4030_iio_write_channel_attr,
	},
	{
		.name = "delay",
		.shared = IIO_SEPARATE,
		.priv = ADF4030_IIO_CH_ATTR_DELAY,
		.show = adf4030_iio_read_channel_attr,
		.store = adf4030_iio_write_channel_attr,
	},
	{
		.name = "odivb_en",
		.shared = IIO_SEPARATE,
		.priv = ADF4030_IIO_CH_ATTR_ODIVB_EN,
		.show = adf4030_iio_read_channel_attr,
		.store = adf4030_iio_write_channel_attr,
	},
	{
		.name = "termination",
		.shared = IIO_SEPARATE,
		.priv = ADF4030_IIO_CH_ATTR_TERMINATION,
		.show = adf4030_iio_read_channel_attr,
		.store = adf4030_iio_write_channel_attr,
	},
	{
		.name = "prbs",
		.shared = IIO_SEPARATE,
		.priv = ADF4030_IIO_CH_ATTR_PRBS,
		.show = adf4030_iio_read_channel_attr,
		.store = adf4030_iio_write_channel_attr,
	},
	{
		.name = "invert",
		.shared = IIO_SEPARATE,
		.priv = ADF4030_IIO_CH_ATTR_INVERT,
		.show = adf4030_iio_read_channel_attr,
		.store = adf4030_iio_write_channel_attr,
	},
	{
		.name = "voltage",
		.shared = IIO_SEPARATE,
		.priv = ADF4030_IIO_CH_ATTR_VOLTAGE,
		.show = adf4030_iio_read_channel_attr,
		.store = adf4030_iio_write_channel_attr,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adf4030_channels[] = {
	{
		.name = "bsync0",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 0,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4030_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.name = "bsync1",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 1,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4030_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.name = "bsync2",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 2,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4030_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.name = "bsync3",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 3,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4030_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.name = "bsync4",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 4,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4030_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.name = "bsync5",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 5,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4030_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.name = "bsync6",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 6,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4030_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.name = "bsync7",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 7,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4030_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.name = "bsync8",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 8,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4030_iio_ch_attrs,
		.scan_type = NULL,
	},
	{
		.name = "bsync9",
		.ch_type = IIO_ALTVOLTAGE,
		.channel = 9,
		.ch_out = true,
		.indexed = true,
		.attributes = adf4030_iio_ch_attrs,
		.scan_type = NULL,
	},
};

static struct iio_device adf4030_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adf4030_channels),
	.channels = adf4030_channels,
	.attributes = adf4030_iio_attrs,
	.debug_reg_read = (int32_t (*)())adf4030_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adf4030_iio_write_reg
};

/**
 * @brief Initializes the ADF4030 IIO driver
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial parameters.
 * @return ret	     - Result of the initialization procedure.
 */
int adf4030_iio_init(struct adf4030_iio_dev **iio_dev,
		     struct adf4030_iio_dev_init_param *init_param)
{
	struct adf4030_iio_dev *dev;
	int ret;

	dev = (struct adf4030_iio_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->iio_dev = &adf4030_iio_dev;

	ret = adf4030_init(&dev->adf4030_dev, init_param->adf4030_dev_init);
	if (ret)
		goto error;

	*iio_dev = dev;

	return ret;

error:
	no_os_free(dev);
	return ret;
}

/**
 * @brief Free the resources allocated by adf4030_iio_init().
 * @param dev	- The IIO device structure.
 * @return ret 	- Result of the remove procedure.
*/
int adf4030_iio_remove(struct adf4030_iio_dev *dev)
{
	int ret;

	ret = adf4030_remove(dev->adf4030_dev);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}
