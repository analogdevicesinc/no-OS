/*******************************************************************************
 *   @file   iio_ad405x.c
 *   @brief  Implementation of IIO AD405X Driver.
 *   @author George Mois (george.mois@analog.com)
 *   @author Jorge Marques (jorge.marques@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "iio_ad405x.h"
#include "ad405x.h"

enum ad405x_iio_accel_chans {
	CHAN_ADC_MODE = 0,
	CHAN_BURST_AVERAGING_MODE = 1
};

const uint8_t ad405x_realbits_adc[] = {12, 14, 16, 12, 14, 16};
const uint8_t ad405x_realbits_burst[] = {16, 16, 24, 16, 16, 24};
const int32_t ad405x_sample_rate[] = {2000000, 1000000, 333000, 100000, 33000,
				      10000, 3000, 1000, 500, 333, 250, 200,
				      166, 140, 125, 111
				     };

static int ad405x_iio_read_reg(struct ad405x_iio_dev *dev, uint8_t reg,
			       uint8_t *readval);
static int ad405x_iio_write_reg(struct ad405x_iio_dev *dev, uint8_t reg,
				uint8_t writeval);
static int ad405x_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv);
static int ad405x_iio_read_samples(void* dev, int32_t* buff, uint32_t samples);
static int ad405x_iio_update_channels(void* dev, uint32_t mask);
static int ad405x_iio_post_disable(void* dev);
static int ad405x_iio_set_sample_rate(struct ad405x_dev* dev, uint32_t val);
static int ad405x_iio_get_sample_rate(struct ad405x_dev* dev, uint32_t *val);
static int ad405x_iio_set_avg_filter_length(struct ad405x_dev* dev,
		uint32_t val);
static int ad405x_iio_get_avg_filter_length(struct ad405x_dev* dev,
		uint32_t *val);

static struct iio_attribute ad405x_iio_adc_attrs[] = {
	{
		.name = "raw",
		.show = ad405x_iio_read_raw,
	},
	{
		.name   = "sample_rate",
		.shared = IIO_SHARED_BY_ALL,
		.show   = ad405x_iio_read_sample_rate,
		.store  = ad405x_iio_write_sample_rate,
	},
	{
		.name   = "avg_filter_length",
		.shared = IIO_SHARED_BY_ALL,
		.show   = ad405x_iio_read_avg_filter_length,
		.store  = ad405x_iio_write_avg_filter_length,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad405x_debug_attrs[] = {

	{
		.name = "sample_rate",
		.show = ad405x_iio_read_debug_attrs,
		.store = ad405x_iio_write_debug_attrs,
		.priv = AD405X_DEBUG_SAMPLE_RATE,
	},
	{
		.name = "avg_filter_len",
		.show = ad405x_iio_read_debug_attrs,
		.store = ad405x_iio_write_debug_attrs,
		.priv = AD405X_DEBUG_AVG_FILTER_LEN,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type ad405x_iio_adc_scan_type[] = {
	{
		.sign = 's',
		.realbits = 0,
		.storagebits = 32,
		.shift = 0,
		.is_big_endian = false
	},
	{
		.sign = 's',
		.realbits = 0,
		.storagebits = 32,
		.shift = 0,
		.is_big_endian = false
	},

};

static struct iio_channel ad405x_channels[] = {
	{
		.name = "voltage_adc_mode",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.scan_index = 0,
		.channel = 0,
		.address = CHAN_ADC_MODE,
		.scan_type = &ad405x_iio_adc_scan_type[0],
		.attributes = ad405x_iio_adc_attrs,
		.ch_out = IIO_DIRECTION_INPUT,
	},
	{
		.name = "voltage_burst_averaging_mode",
		.ch_type = IIO_VOLTAGE,
		.indexed = 1,
		.scan_index = 1,
		.channel = 1,
		.address = CHAN_BURST_AVERAGING_MODE,
		.scan_type = &ad405x_iio_adc_scan_type[1],
		.attributes = ad405x_iio_adc_attrs,
		.ch_out = IIO_DIRECTION_INPUT,
	},
};

static struct iio_device ad405x_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ad405x_channels),
	.channels = ad405x_channels,
	.debug_attributes = ad405x_debug_attrs,
	.pre_enable = (int32_t (*)())ad405x_iio_update_channels,
	.post_disable = (int32_t (*)())ad405x_iio_post_disable,
	.read_dev = (int32_t (*)())ad405x_iio_read_samples,
	.debug_reg_read = (int32_t (*)())ad405x_iio_read_reg,
	.debug_reg_write = (int32_t (*)())ad405x_iio_write_reg
};

/**
 * @brief Handles the read request for debug attributes.
 * @param dev - The iio device structure.
 * @param buf - Command buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return The size of the read data in case of success, error code otherwise.
 */
int ad405x_iio_read_debug_attrs(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad405x_iio_dev *iio_dev;
	struct ad405x_dev *desc;
	uint32_t res;
	int ret = 0;

	if (!dev)
		return -EINVAL;

	iio_dev = (struct ad405x_iio_dev *)dev;

	if (!iio_dev->ad405x_dev)
		return -EINVAL;

	desc = iio_dev->ad405x_dev;

	switch (priv) {
	case AD405X_DEBUG_SAMPLE_RATE:
		ret = ad405x_iio_get_sample_rate(desc, &res);
		break;
	case AD405X_DEBUG_AVG_FILTER_LEN:
		ret = ad405x_iio_get_avg_filter_length(desc, &res);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t*)&res);
}

/**
 * @brief Handles the write request for debug attributes.
 * @param dev - The iio device structure.
 * @param buf - Command buffer to be filled with the data to be written.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv  - Command attribute id.
 * @return Number of bytes received in case of success, error code otherwise.
 */
int ad405x_iio_write_debug_attrs(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad405x_iio_dev *iio_dev;
	struct ad405x_dev *desc;
	uint32_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_dev = (struct ad405x_iio_dev *)dev;

	if (!iio_dev->ad405x_dev)
		return -EINVAL;

	desc = iio_dev->ad405x_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t*)&val, NULL);
	if (ret)
		return ret;

	switch (priv) {
	case AD405X_DEBUG_SAMPLE_RATE:
		ret = ad405x_iio_set_sample_rate(desc, val);
		break;
	case AD405X_DEBUG_AVG_FILTER_LEN:
		ret = ad405x_iio_set_avg_filter_length(desc, val);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return len;
}

/**
 * @brief Handles the write request for sampling rate attribute.
 * @param dev - The iio device structure.
 * @param buf - Command buffer to be filled with the data to be written.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return Number of bytes received in case of success, error code otherwise.
 */
int ad405x_iio_write_sample_rate(void *dev, char *buf,
				 uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad405x_iio_dev *iio_dev;
	struct ad405x_dev *desc;
	uint32_t freq;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_dev = (struct ad405x_iio_dev *)dev;

	if (!iio_dev->ad405x_dev)
		return -EINVAL;

	desc = iio_dev->ad405x_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t*)&freq, NULL);
	if (ret)
		return ret;

	ret = ad405x_iio_set_sample_rate(desc, freq);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Handles the read request for sample rate attribute.
 * @param dev - The iio device structure.
 * @param buf - Command buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
int ad405x_iio_read_sample_rate(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad405x_iio_dev *iio_dev;
	struct ad405x_dev *desc;
	uint32_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_dev = (struct ad405x_iio_dev*)dev;

	if (!iio_dev->ad405x_dev)
		return -EINVAL;

	desc = iio_dev->ad405x_dev;

	ret = ad405x_iio_get_sample_rate(desc, &val);
	if (ret < 0)
		return ret;
	ret = ad405x_sample_rate[ret];

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t*)&val);
}

/**
 * @brief Handles the write request for average filter length attribute.
 * @param dev - The iio device structure.
 * @param buf - Command buffer to be filled with the data to be written.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return Number of bytes received in case of success, error code otherwise.
 */
int ad405x_iio_write_avg_filter_length(void *dev, char *buf,
				       uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad405x_iio_dev *iio_dev;
	struct ad405x_dev *desc;
	uint32_t freq;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_dev = (struct ad405x_iio_dev *)dev;

	if (!iio_dev->ad405x_dev)
		return -EINVAL;

	desc = iio_dev->ad405x_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t*)&freq, NULL);
	if (ret)
		return ret;

	ret = ad405x_iio_set_avg_filter_length(desc, freq);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Handles the read request for average filter length attribute.
 * @param dev - The iio device structure.
 * @param buf - Command buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return The size of the read data in case of success, error code otherwise.
 */
int ad405x_iio_read_avg_filter_length(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad405x_iio_dev *iio_dev;
	struct ad405x_dev *desc;
	uint32_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_dev = (struct ad405x_iio_dev*)dev;

	if (!iio_dev->ad405x_dev)
		return -EINVAL;

	desc = iio_dev->ad405x_dev;

	ret = ad405x_iio_get_avg_filter_length(desc, &val);
	if (ret < 0)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Initializes the AD405X IIO driver.
 * @param iio_dev - The iio device structure.
 * @param init_param - The structure that contains the device initial
 *                     parameters.
 * @return Result of the initialization procedure.
 */
int ad405x_iio_init(struct ad405x_iio_dev **iio_dev,
		    struct ad405x_iio_dev_init_param *init_param)
{
	int ret;
	struct ad405x_iio_dev *desc;
	const struct iio_channel *channels = ad405x_iio_dev.channels;
	const uint8_t dev_type = init_param->ad405x_dev_init->dev_type;

	desc = (struct ad405x_iio_dev *)calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &ad405x_iio_dev;

	/* Initialize device specific values */
	channels[0].scan_type->realbits = ad405x_realbits_adc[dev_type];
	channels[1].scan_type->realbits = ad405x_realbits_burst[dev_type];

	/* Initialize AD405X driver */
	ret = ad405x_init(&desc->ad405x_dev, *(init_param->ad405x_dev_init));
	if (ret)
		goto error_ad405x_init;

	/* Set GP0 as DRDY */
	ret = ad405x_set_gp_mode(desc->ad405x_dev, AD405X_GP_0, AD405X_GP_MODE_DRDY);
	if (ret)
		goto error_config;

	*iio_dev = desc;

	return 0;

error_config:
	ad405x_remove(desc->ad405x_dev);

error_ad405x_init:
	free(desc);
	return ret;
}

/**
 * @brief Free the resources allocated by ad405x_iio_init().
 * @param desc - The IIO device structure.
 * @return Result of the remove procedure.
 */
int ad405x_iio_remove(struct ad405x_iio_dev *desc)
{
	int ret;

	ret = ad405x_remove(desc->ad405x_dev);
	if (ret)
		return ret;

	free(desc);

	return 0;
}

/**
 * @brief Wrapper for reading AD405X register.
 * @param device - The iio device structure.
 * @param reg - Address of the register to be read from.
 * @param readval - Read data.
 * @return Result of the reading procedure.
 */
static int ad405x_iio_read_reg(struct ad405x_iio_dev *dev, uint8_t reg,
			       uint8_t *readval)
{
	int ret;
	enum ad405x_operation_mode mode;

	ret = ad405x_set_config_mode(dev->ad405x_dev, &mode);
	if (ret)
		return ret;

	ret = ad405x_read(dev->ad405x_dev, reg, (uint8_t *)readval, 1);
	if (ret)
		return ret;

	return ad405x_set_operation_mode(dev->ad405x_dev, mode);
}

/**
 * @brief Wrapper for writing to AD405X register.
 * @param device  - The iio device structure.
 * @param reg - Address of the register to be written to.
 * @param writeval - Data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
static int ad405x_iio_write_reg(struct ad405x_iio_dev *dev, uint8_t reg,
				uint8_t writeval)
{
	uint8_t val = writeval;
	int ret;
	enum ad405x_operation_mode mode;

	ret = ad405x_set_config_mode(dev->ad405x_dev, &mode);
	if (ret)
		return ret;

	ret = ad405x_write(dev->ad405x_dev, reg, &val, 1);
	if (ret)
		return ret;

	return ad405x_set_operation_mode(dev->ad405x_dev, mode);
}

/**
 * @brief Handles the read request for raw attribute.
 * @param dev - The iio device structure.
 * @param buf - Command buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Command attribute id.
 * @return The size of the read data in case of success, error code otherwise.
 */
static int ad405x_iio_read_raw(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	struct ad405x_iio_dev *iio_dev;
	struct ad405x_dev *desc;
	int32_t data;

	if (!dev)
		return -EINVAL;

	iio_dev = (struct ad405x_iio_dev *)dev;

	if (!iio_dev->ad405x_dev)
		return -EINVAL;

	desc = iio_dev->ad405x_dev;

	if (channel->type == IIO_VOLTAGE) {
		/* Check device mode */
		switch (channel->address) {
		case 0:
			if (desc->operation_mode != AD405X_ADC_MODE_OP) {
				ret = ad405x_set_operation_mode(desc, AD405X_ADC_MODE_OP);
				if (ret)
					return ret;
			}
			break;
		case 1:
			if (desc->operation_mode != AD405X_BURST_AVERAGING_MODE_OP) {
				ret = ad405x_set_operation_mode(desc, AD405X_BURST_AVERAGING_MODE_OP);
				if (ret)
					return ret;
			}
			break;
		default:
			return -EINVAL;
		}

		ret = ad405x_get_adc(desc, &data);
		if (ret)
			return ret;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &data);
	}

	return -EINVAL;
}

/**
 * @brief Reads the number of given samples for the selected channels.
 * @param dev - The iio device structure.
 * @param buf - Command buffer to be filled with requested data.
 * @param samples - Number of samples to be returned
 * @return The size of the read data in case of success, error code otherwise.
 */
static int ad405x_iio_read_samples(void* dev, int32_t* buff, uint32_t samples)
{
	int ret;
	struct ad405x_iio_dev *iio_dev;
	struct ad405x_dev *desc;
	int32_t data;

	if (!dev)
		return -EINVAL;

	iio_dev = (struct ad405x_iio_dev *)dev;

	if (!iio_dev->ad405x_dev)
		return -EINVAL;

	desc = iio_dev->ad405x_dev;

	for (uint32_t i = 0; i < samples;) {
		ret = ad405x_get_adc(desc, &data);
		if (ret)
			return ret;

		buff[i] = data;
		i++;
	}

	return samples;
}


/**
 * @brief Updates the number of active channels and the total number of
 * active channels,
 * @param dev  - The iio device structure.
 * @param mask - Mask of the active channels
 * @return ret - Result of the updating procedure.
 */
static int ad405x_iio_update_channels(void* dev, uint32_t mask)
{
	int ret;
	struct ad405x_iio_dev *iio_ad405x;

	if (!dev)
		return -EINVAL;

	iio_ad405x = (struct ad405x_iio_dev *)dev;

	iio_ad405x->active_channels = mask;

	if (mask & NO_OS_BIT(0)) {
		ret = ad405x_set_operation_mode(iio_ad405x->ad405x_dev,
						AD405X_ADC_MODE_OP);
		if (ret)
			return ret;
	} else if (mask & NO_OS_BIT(1)) {
		ret = ad405x_set_operation_mode(iio_ad405x->ad405x_dev,
						AD405X_BURST_AVERAGING_MODE_OP);
		if (ret)
			return ret;
	}

	iio_ad405x->no_of_active_channels = 1;

	return 0;
}

/**
 * @brief Returns to config mode.
 * @param dev - The iio device structure.
 * @return Result of the updating procedure.
 */
static int ad405x_iio_post_disable(void* dev)
{
	enum ad405x_operation_mode mode;
	struct ad405x_iio_dev *iio_ad405x;

	if (!dev)
		return -EINVAL;

	iio_ad405x = (struct ad405x_iio_dev *)dev;

	if (iio_ad405x->ad405x_dev->comm_type == AD405X_I3C_COMM)
		mode = AD405X_ADC_MODE_OP;
	else
		mode = AD405X_CONFIG_MODE_OP;

	return ad405x_set_operation_mode(iio_ad405x->ad405x_dev, mode);
}

/**
 * @brief Set sample rate for Burst and Autonomous Modes.
 * @param dev - The iio device structure.
 * @param val - The sample rate.
 * @return 0 in case of success, error code otherwise.
 */
static int ad405x_iio_set_sample_rate(struct ad405x_dev* dev, uint32_t val)
{
	int ret;
	enum ad405x_operation_mode mode;
	enum ad405x_sample_rate rate;
	rate = no_os_find_closest(val, ad405x_sample_rate,
				  sizeof(ad405x_sample_rate) / sizeof(*ad405x_sample_rate));

	ret = ad405x_set_config_mode(dev, &mode);
	if (ret)
		return ret;

	ret = ad405x_set_sample_rate(dev, rate);
	if (ret)
		return ret;

	return ad405x_set_operation_mode(dev, mode);
}

/**
 * @brief Get sample rate for Burst and Autonomous Modes.
 * @param dev - The iio device structure.
 * @param val - The sample rate.
 * @return 0 in case of success, error code otherwise.
 */
static int ad405x_iio_get_sample_rate(struct ad405x_dev* dev, uint32_t *val)
{
	int32_t rate = ad405x_get_sample_rate(dev);

	if (rate < 0)
		return -EIO;

	*val = ad405x_sample_rate[rate];
	return 0;
}

/**
 * @brief Set average filter length for Burst and Autonomous Modes.
 * @param dev - The iio device structure.
 * @param val - The filter length.
 * @return 0 in case of success, error code otherwise.
 */
static int ad405x_iio_set_avg_filter_length(struct ad405x_dev* dev,
		uint32_t val)
{
	uint8_t f_len = 0;
	int ret;
	enum ad405x_operation_mode mode;

	/* Crude log */
	while (val >>= 1)
		f_len++;
	if (f_len)
		f_len--;

	ret = ad405x_set_config_mode(dev, &mode);
	if (ret)
		return ret;

	ret = ad405x_set_avg_filter_length(dev, f_len);
	if (ret)
		return ret;

	return ad405x_set_operation_mode(dev, mode);
}

/**
 * @brief Get average filter length for Burst and Autonomous Modes.
 * @param dev  - The iio device structure.
 * @param len - The filter length.
 * @return 0 in case of success, error code otherwise.
 */
static int ad405x_iio_get_avg_filter_length(struct ad405x_dev* dev,
		uint32_t *len)
{
	int32_t f_len = ad405x_get_avg_filter_length(dev);

	if (f_len < 0)
		return -EIO;

	*len = NO_OS_BIT(f_len + 1);
	return 0;
}
