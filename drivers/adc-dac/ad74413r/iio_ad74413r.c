/***************************************************************************//**
 *   @file   iio_ad74413r.c
 *   @brief  Source file of the IIO AD74413r Driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include <string.h>
#include "no_os_error.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "ad74413r.h"
#include "iio_ad74413r.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD74413R_ADC_CHANNEL(type, attrs)                       \
        {                                                       \
                .ch_type = type,                                \
                .indexed = 1,                                   \
                .ch_out = 0,                                    \
                .scan_type = &ad74413r_iio_adc_scan_type,       \
                .attributes = attrs				\
        }

#define AD74413R_DAC_CHANNEL(type)                              \
        {                                                       \
                .ch_type = type,                                \
                .indexed = 1,                                   \
                .ch_out = 1,                                    \
                .attributes = ad74413r_iio_dac_attrs,           \
        }

#define _AD74413R_CHANNELS(_channels)                           \
        {                          				\
                .channels = _channels,	                        \
		.num_channels = NO_OS_ARRAY_SIZE(_channels)	\
        }

#define AD74413R_CHANNELS(name) \
        _AD74413R_CHANNELS(ad74413r_ ## name ## _channels)

static int32_t ad74413r_sample_rate_avail[] = {
	20, 4800, 10, 1200
};

static int32_t ad74412r_sample_rate_avail[] = {
	20, 4800
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int ad74413r_iio_read_reg(struct ad74413r_iio_desc *dev, uint32_t reg,
				 uint32_t *readval);
static int ad74413r_iio_write_reg(struct ad74413r_iio_desc *dev, uint32_t reg,
				  uint32_t writeval);
static int ad74413r_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_write_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_read_sampling_freq_avail(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_read_processed(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_update_channels(void *dev, uint32_t mask);
static int ad74413r_iio_buffer_disable(void *dev);
static int ad74413r_iio_read_samples(void *dev, uint32_t *buf,
				     uint32_t samples);
static int ad74413r_iio_trigger_handler(struct iio_device_data *dev_data);

/******************************************************************************/
/************************ Variable Declarations *******************************/
/******************************************************************************/
static struct scan_type ad74413r_iio_adc_scan_type = {
	.sign = 'u',
	.realbits = 16,
	.storagebits = 32,
	.shift = 8,
	.is_big_endian = true
};

static struct iio_attribute ad74413r_iio_adc_resistance_attrs[] = {
	{
		.name = "raw",
		.show = ad74413r_iio_read_raw
	},
	{
		.name = "sampling_frequency",
		.show = ad74413r_iio_read_sampling_freq,
		.store = ad74413r_iio_write_sampling_freq
	},
	{
		.name = "sampling_frequency_available",
		.show = ad74413r_iio_read_sampling_freq_avail
	},
	{
		.name = "processed",
		.show = ad74413r_iio_read_processed
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad74413r_iio_adc_attrs[] = {
	{
		.name = "sampling_frequency",
		.show = ad74413r_iio_read_sampling_freq,
		.store = ad74413r_iio_write_sampling_freq
	},
	{
		.name = "sampling_frequency_available",
		.show = ad74413r_iio_read_sampling_freq_avail
	},
	{
		.name = "raw",
		.show = ad74413r_iio_read_raw
	},
	{
		.name = "scale",
		.show = ad74413r_iio_read_scale
	},
	{
		.name = "offset",
		.show = ad74413r_iio_read_offset
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad74413r_iio_dac_attrs[] = {
	{
		.name = "raw",
		.show = ad74413r_iio_read_raw,
		.store = ad74413r_iio_write_raw
	},
	{
		.name = "scale",
		.show = ad74413r_iio_read_scale
	},
	{
		.name = "offset",
		.show = ad74413r_iio_read_offset
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ad74413r_voltage_input_channels[] = {
	AD74413R_ADC_CHANNEL(IIO_VOLTAGE, ad74413r_iio_adc_attrs),
};

static struct iio_channel ad74413r_voltage_output_channels[] = {
	AD74413R_ADC_CHANNEL(IIO_CURRENT, ad74413r_iio_adc_attrs),
	AD74413R_DAC_CHANNEL(IIO_VOLTAGE)
};

static struct iio_channel ad74413r_current_input_channels[] = {
	AD74413R_ADC_CHANNEL(IIO_CURRENT, ad74413r_iio_adc_attrs)
};

static struct iio_channel ad74413r_current_output_channels[] = {
	AD74413R_ADC_CHANNEL(IIO_VOLTAGE, ad74413r_iio_adc_attrs),
	AD74413R_DAC_CHANNEL(IIO_CURRENT)
};

static struct iio_channel ad74413r_resistance_input_channels[] = {
	AD74413R_ADC_CHANNEL(IIO_RESISTANCE, ad74413r_iio_adc_resistance_attrs)
};

static struct iio_channel ad74413r_digital_input_channels[] = {
	AD74413R_ADC_CHANNEL(IIO_VOLTAGE, ad74413r_iio_adc_attrs)
};

static struct ad74413r_channel_map channel_map[] = {
	[AD74413R_HIGH_Z] = AD74413R_CHANNELS(voltage_input),
	[AD74413R_VOLTAGE_OUT] = AD74413R_CHANNELS(voltage_output),
	[AD74413R_CURRENT_OUT] = AD74413R_CHANNELS(current_output),
	[AD74413R_VOLTAGE_IN] = AD74413R_CHANNELS(voltage_input),
	[AD74413R_CURRENT_IN_EXT] = AD74413R_CHANNELS(current_input),
	[AD74413R_CURRENT_IN_LOOP] = AD74413R_CHANNELS(current_input),
	[AD74413R_RESISTANCE] = AD74413R_CHANNELS(resistance_input),
	[AD74413R_DIGITAL_INPUT] = AD74413R_CHANNELS(digital_input),
	[AD74413R_DIGITAL_INPUT_LOOP] = AD74413R_CHANNELS(digital_input),
	[AD74413R_CURRENT_IN_EXT_HART] = AD74413R_CHANNELS(current_input),
	[AD74413R_CURRENT_IN_LOOP_HART] = AD74413R_CHANNELS(current_input),
};

static struct iio_device ad74413r_iio_dev = {
	.pre_enable = (int32_t (*)())ad74413r_iio_update_channels,
	.post_disable = (int32_t (*)())ad74413r_iio_buffer_disable,
	.trigger_handler = (int32_t (*)())ad74413r_iio_trigger_handler,
	.read_dev = (int32_t (*)())ad74413r_iio_read_samples,
	.debug_reg_read = (int32_t (*)())ad74413r_iio_read_reg,
	.debug_reg_write = (int32_t (*)())ad74413r_iio_write_reg
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Register read wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_read_reg(struct ad74413r_iio_desc *dev, uint32_t reg,
				 uint32_t *readval)
{
	return ad74413r_reg_read(dev->ad74413r_desc, reg, (uint16_t *)readval);
}

/**
 * @brief Register write wrapper
 * @param dev - The iio device structure.
 * @param reg - The register's address.
 * @param readval - Register value
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_write_reg(struct ad74413r_iio_desc *dev, uint32_t reg,
				  uint32_t writeval)
{
	return ad74413r_reg_write(dev->ad74413r_desc, reg, (uint16_t)writeval);
}

/**
 * @brief Read the offset attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;
	enum ad74413r_adc_range range;

	switch (channel->type) {
	case IIO_VOLTAGE:
		val = 0;
		break;
	case IIO_CURRENT:
		if (channel->ch_out) {
			val = 0;
		} else {
			ad74413r_get_adc_range(((struct ad74413r_iio_desc *)dev)->ad74413r_desc,
					       channel->address, &range);

			switch (range) {
			case AD74413R_ADC_RANGE_10V:
			case AD74413R_ADC_RANGE_2P5V_EXT_POW:
				val = 0;
				break;
			case AD74413R_ADC_RANGE_2P5V_INT_POW:
				val = -AD74413R_ADC_MAX_VALUE;
				break;
			case AD74413R_ADC_RANGE_5V_BI_DIR:
				val = -(AD74413R_ADC_MAX_VALUE / 2);
				break;
			default:
				return -EINVAL;
			}
		}
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Read the raw attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	uint16_t val;

	if (channel->ch_out)
		return -EINVAL;

	ret = ad74413r_get_adc_single(((struct ad74413r_iio_desc *)dev)->ad74413r_desc,
				      channel->address, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Write the raw attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_write_raw(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;

	switch (channel->type) {
	case IIO_VOLTAGE:
		if (!channel->ch_out)
			return -EINVAL;

		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		return ad74413r_set_channel_dac_code(((struct ad74413r_iio_desc *)
						      dev)->ad74413r_desc, channel->address, val);

	default:
		return -EINVAL;
	}
}

/**
 * @brief Read the sampling frequency attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	enum ad74413r_adc_sample val;

	ret = ad74413r_get_adc_rate(((struct ad74413r_iio_desc *)dev)->ad74413r_desc,
				    channel->address, &val);
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

/**
 * @brief Write the sampling frequency attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_write_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t val;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	return ad74413r_set_adc_rate(((struct ad74413r_iio_desc *)dev)->ad74413r_desc,
				     channel->address, val);
}

/**
 * @brief Read available values for the sampling frequency attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_read_sampling_freq_avail(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	if (((struct ad74413r_iio_desc *)dev)->ad74413r_desc->chip_id == AD74412R)
		iio_format_value(buf, len, IIO_VAL_INT_MULTIPLE, 2, ad74412r_sample_rate_avail);
	else
		iio_format_value(buf, len, IIO_VAL_INT_MULTIPLE, 4, ad74413r_sample_rate_avail);

	return strlen(buf);
}

/**
 * @brief Read the scale attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val[2];

	switch (channel->type) {
	case IIO_VOLTAGE:
		if (channel->ch_out) {
			val[0] = 0;
			val[1] = 762940;
		} else {
			val[0] = 0;
			val[1] = 152590;
		}

		break;
	case IIO_CURRENT:
		if (channel->ch_out) {
			val[0] = 0;
			val[1] = 152590 / MILLI;
		} else {
			val[0] = 0;
			val[1] = 381470 / MILLI;
		}
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 1, val);
}

/**
 * @brief Read the processed attribute for a specific channel
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_read_processed(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int ret;
	int32_t val;
	struct ad74413r_decimal decimal_val;

	switch (channel->type) {
	case IIO_RESISTANCE:
		ret = ad74413r_adc_get_value(((struct ad74413r_iio_desc *)dev)->ad74413r_desc,
					     channel->address,
					     &decimal_val);
		if (ret)
			return ret;
		val = decimal_val.integer;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Configure a set if IIO channels based on the operation modes of the enabled
 * physical channels.
 * @param iio_desc - Specific IIO descriptor for AD74413R
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_setup_channels(struct ad74413r_iio_desc *iio_desc)
{
	uint32_t i;
	uint32_t n_chan = 0;
	uint32_t channel_buff_cnt = 0;
	struct ad74413r_desc *ad74413r_desc;
	struct ad74413r_channel_config *config;
	struct iio_channel *chan;
	struct iio_channel *chan_buffer;
	struct ad74413r_channel_map channels_info;

	ad74413r_desc = iio_desc->ad74413r_desc;
	config = ad74413r_desc->channel_configs;

	for (i = 0; i < AD74413R_N_CHANNELS; i++) {
		if (!config[i].enabled)
			continue;
		channel_buff_cnt += channel_map[config[i].function].num_channels;
	}

	chan_buffer = calloc(channel_buff_cnt, sizeof(*chan_buffer));
	if (!chan_buffer)
		return -ENOMEM;

	iio_desc->iio_dev->channels = chan_buffer;

	/** Add the ADC channels */
	for (int i = 0; i < AD74413R_N_CHANNELS; i++) {
		if (!config[i].enabled)
			continue;
		channels_info = channel_map[config[i].function];
		chan = channels_info.channels;

		chan[0].channel = i;
		chan[0].address = i;
		chan[0].scan_index = i;

		memcpy(chan_buffer + n_chan, chan, sizeof(*chan));
		n_chan++;
		iio_desc->iio_dev->num_ch++;
	}

	/** Add the DAC channels */
	for (int i = 0; i < AD74413R_N_CHANNELS; i++) {
		if (!config[i].enabled)
			continue;
		channels_info = channel_map[config[i].function];
		chan = channels_info.channels;
		if (channels_info.num_channels == 1)
			continue;

		chan[1].channel = i;
		chan[1].address = i;

		memcpy(chan_buffer + n_chan, &chan[1], sizeof(chan[1]));
		n_chan++;
		iio_desc->iio_dev->num_ch++;
	}

	return 0;
}

/**
 * @brief Enable IIO channels and start the ADC conversions in continuous mode.
 * @param dev - The iio device structure.
 * @param mask - Bit mask that specifies the enabled channels.
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_update_channels(void *dev, uint32_t mask)
{
	struct ad74413r_iio_desc *iio_desc = dev;

	iio_desc->active_channels = mask;
	iio_desc->no_of_active_channels = no_os_hweight8(mask);

	return ad74413r_set_adc_conv_seq(iio_desc->ad74413r_desc, AD74413R_START_CONT);
}

/**
 * @brief Stop the ADC conversions.
 * @param dev - The iio device structure.
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_buffer_disable(void *dev)
{
	return ad74413r_set_adc_conv_seq(((struct ad74413r_iio_desc *)
					  dev)->ad74413r_desc,
					 AD74413R_STOP_PWR_DOWN);
}

/**
 * @brief Read a number of samples from each enabled channel.
 * @param dev - The iio device structure.
 * @param buf - Buffer to store the samples in.
 * @param samples - The number of samples
 * @return The number of samples in case of success, an error code otherwise
 */
static int ad74413r_iio_read_samples(void *dev, uint32_t *buf, uint32_t samples)
{
	int ret;
	uint32_t j = 0;
	uint32_t val;
	uint32_t i, chan_i;

	for (i = 0; i < samples; i++) {
		for (chan_i = 0; chan_i < AD74413R_N_CHANNELS; chan_i++) {
			if (((struct ad74413r_iio_desc *)dev)->active_channels & NO_OS_BIT(chan_i)) {
				ret = ad74413r_reg_read_raw(((struct ad74413r_iio_desc *)dev)->ad74413r_desc,
							    AD74413R_ADC_RESULT(chan_i),
							    (uint8_t *)&val);
				if (ret)
					return ret;
				buf[j++] = val;
			}
		}
	}

	return samples;
}

/**
 * @brief Read a sample for each enabled channel.
 * @param dev_data - The iio device data structure.
 * @return 0 in case of success, an error code otherwise.
 */
static int ad74413r_iio_trigger_handler(struct iio_device_data *dev_data)
{
	int ret;
	uint32_t i;
	uint32_t val;
	uint32_t buff[4];
	struct ad74413r_iio_desc *iio_desc;
	struct ad74413r_desc *desc;

	iio_desc = dev_data->dev;
	desc = iio_desc->ad74413r_desc;

	for (i = 0; i < AD74413R_N_CHANNELS; i++) {
		if (iio_desc->active_channels & NO_OS_BIT(i)) {
			ret = ad74413r_reg_read_raw(desc, AD74413R_ADC_RESULT(i),
						    (uint8_t *)&val);
			if (ret)
				return ret;
			buff[i] = val;
		}
	}

	return iio_buffer_push_scan(dev_data->buffer, buff);
}

/**
 * @brief Initializes the AD74413R IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int ad74413r_iio_init(struct ad74413r_iio_desc **iio_desc,
		      struct ad74413r_iio_desc_init_param *init_param)
{
	int ret;
	uint32_t i;
	struct ad74413r_iio_desc *descriptor;

	if (!init_param || !init_param->ad74413r_init_param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->iio_dev = &ad74413r_iio_dev;

	ret = ad74413r_init(&descriptor->ad74413r_desc,
			    init_param->ad74413r_init_param);
	if (ret)
		goto err;

	ret = ad74413r_clear_errors(descriptor->ad74413r_desc);
	if (ret)
		goto err;

	/** The operation modes for the physical channels are set only at initialization. */
	for (i = 0; i < AD74413R_N_CHANNELS; i++) {
		if (init_param->channel_configs[i].enabled) {
			ret = ad74413r_set_adc_channel_enable(descriptor->ad74413r_desc, i, true);
			if (ret)
				goto err;

			ret = ad74413r_set_channel_function(descriptor->ad74413r_desc, i,
							    init_param->channel_configs[i].function);
			if (ret)
				goto err;

			ret = ad74413r_set_adc_rate(descriptor->ad74413r_desc, i,
						    AD74413R_ADC_SAMPLE_4800HZ);
			if (ret)
				return ret;
		}
	}

	ret = ad74413r_iio_setup_channels(descriptor);
	if (ret)
		goto err;

	*iio_desc = descriptor;

	return 0;
err:
	ad74413r_remove(descriptor->ad74413r_desc);
	free(descriptor);

	return ret;
}

/**
 * @brief Free an iio descriptor.
 * @param desc - The descriptor to be freed.
 * @return 0 in case of success, an error code otherwise.
 */
int ad74413r_iio_remove(struct ad74413r_iio_desc *desc)
{
	int ret;

	ret = ad74413r_remove(desc->ad74413r_desc);
	if (ret)
		return ret;

	free(desc);

	return 0;
}
