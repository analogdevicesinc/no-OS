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
#include "no_os_alloc.h"
#include "ad74413r.h"
#include "iio_ad74413r.h"
#include "mxc_device.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AD74413R_DIAG_CH_OFFSET		4

#define AD74413R_ADC_CHANNEL(type, attrs)                	\
        {                                                       \
                .ch_type = type,                                \
                .indexed = 1,                                   \
                .ch_out = 0,                                    \
                .scan_type = &ad74413r_iio_adc_scan_type,	\
                .attributes = attrs				\
        }

#define AD74413R_DIAG_CHANNEL(_addr, _name)	           	\
        {                                                       \
                .ch_type = IIO_VOLTAGE,                         \
		.name = _name,					\
                .indexed = 1,                                   \
                .ch_out = 0,					\
		.channel = _addr,				\
		.address = _addr - 4,				\
                .scan_type = &ad74413r_iio_adc_scan_type,	\
                .attributes = ad74413r_iio_adc_diag_attrs	\
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

enum channel_type {
	AD74413R_ADC,
	AD74413R_DIAG,
};

static int32_t ad74413r_sample_rate_avail[] = {
	20, 4800, 10, 1200
};

static int32_t ad74412r_sample_rate_avail[] = {
	20, 4800
};

static int32_t ad74413r_slew_rate_avail[] = {
	4, 64, 150, 240,
};

static int32_t ad74413r_slew_step_avail[] = {
	64, 120, 500, 1820,
};

static char *ad74413r_diag_available[14] = {
	[AD74413R_DIAG_AGND] = "agnd",
	[AD74413R_DIAG_TEMP] = "temp",
	[AD74413R_DIAG_AVDD] = "avdd",
	[AD74413R_DIAG_AVSS] = "avss",
	[AD74413R_DIAG_REFOUT] = "refout",
	[AD74413R_DIAG_ALDO_5V] = "aldo_5v",
	[AD74413R_DIAG_ALDO_1V8] = "aldo_1v8",
	[AD74413R_DIAG_DLDO_1V8] = "dldo_1v8",
	[AD74413R_DIAG_DVCC] = "dvcc",
	[AD74413R_DIAG_IOVDD] = "iovdd",
	[AD74413R_SENSEL_A] = "sensel_a",
	[AD74413R_SENSEL_B] = "sensel_b",
	[AD74413R_SENSEL_C] = "sensel_c",
	[AD74413R_SENSEL_D] = "sensel_d",
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
static int ad74413r_iio_read_slew_en(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_write_slew_en(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_read_slew_rate(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_write_slew_rate(void *dev, char *buf, uint32_t len,
				        const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_read_slew_step(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_write_slew_step(void *dev, char *buf, uint32_t len,
				        const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_read_slew_rate_avail(void *dev, char *buf, uint32_t len,
					     const struct iio_ch_info *channel,
					     intptr_t priv);
static int ad74413r_iio_read_slew_step_avail(void *dev, char *buf, uint32_t len,
					     const struct iio_ch_info *channel,
					     intptr_t priv);
static int ad74413r_iio_read_threshold(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_write_threshold(void *dev, char *buf, uint32_t len,
				        const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_update_channels(void *dev, uint32_t mask);
static int ad74413r_iio_buffer_disable(void *dev);
static int ad74413r_iio_read_samples(void *dev, uint32_t *buf,
				     uint32_t samples);
static int ad74413r_iio_trigger_handler(struct iio_device_data *dev_data);

static int ad74413r_iio_read_diag_function(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_write_diag_function(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int ad74413r_iio_read_diag_function_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int ad74413r_iio_read_fault_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);

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
		.name = "offset",
		.show = ad74413r_iio_read_offset
	},
		{
		.name = "scale",
		.show = ad74413r_iio_read_scale
	},
	{
		.name = "sampling_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad74413r_iio_read_sampling_freq,
		.store = ad74413r_iio_write_sampling_freq
	},
	{
		.name = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_ALL,
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
		.shared = IIO_SHARED_BY_ALL,
		.show = ad74413r_iio_read_sampling_freq,
		.store = ad74413r_iio_write_sampling_freq,
	},
	{
		.name = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad74413r_iio_read_sampling_freq_avail
	},
	{
		.name = "raw",
		.show = ad74413r_iio_read_raw,
		.priv = AD74413R_ADC,
	},
	{
		.name = "scale",
		.show = ad74413r_iio_read_scale,
		.priv = AD74413R_ADC,
	},
	{
		.name = "offset",
		.show = ad74413r_iio_read_offset,
		.priv = AD74413R_ADC,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad74413r_iio_digital_attrs[] = {
	{
		.name = "sampling_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad74413r_iio_read_sampling_freq,
		.store = ad74413r_iio_write_sampling_freq,
	},
	{
		.name = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad74413r_iio_read_sampling_freq_avail
	},
	{
		.name = "raw",
		.show = ad74413r_iio_read_raw,
		.priv = AD74413R_ADC,
	},
	{
		.name = "threshold",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad74413r_iio_read_threshold,
		.store = ad74413r_iio_write_threshold,
		.priv = AD74413R_ADC,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad74413r_iio_adc_diag_attrs[] = {
	{
		.name = "sampling_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad74413r_iio_read_sampling_freq,
		.store = ad74413r_iio_write_sampling_freq,
	},
	{
		.name = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad74413r_iio_read_sampling_freq_avail
	},
	{
		.name = "diag_function",
		.show = ad74413r_iio_read_diag_function,
		.store = ad74413r_iio_write_diag_function,
	},
	{
		.name = "diag_function_available",
		.shared = IIO_SHARED_BY_ALL,
		.show = ad74413r_iio_read_diag_function_avail,
	},
	{
		.name = "raw",
		.show = ad74413r_iio_read_raw,
		.priv = AD74413R_DIAG,
	},
	{
		.name = "scale",
		.show = ad74413r_iio_read_scale,
		.priv = AD74413R_DIAG,
	},
	{
		.name = "offset",
		.show = ad74413r_iio_read_offset,
		.priv = AD74413R_DIAG,
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
	{
		.name = "slew_en",
		.show = ad74413r_iio_read_slew_en,
		.store = ad74413r_iio_write_slew_en,
	},
	{
		.name = "slew_rate",
		.show = ad74413r_iio_read_slew_step,
		.store = ad74413r_iio_write_slew_step,
	},
	{
		.name = "slew_rate_available",
		.shared = IIO_SHARED_BY_DIR,
		.show = ad74413r_iio_read_slew_rate_avail,
	},
	{
		.name = "slew_step",
		.show = ad74413r_iio_read_slew_rate,
		.store = ad74413r_iio_write_slew_rate,
	},
	{
		.name = "slew_step_available",
		.shared = IIO_SHARED_BY_DIR,
		.show = ad74413r_iio_read_slew_step_avail,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad74413r_fault_attrs[] = {
	{
		.name = "raw",
		.show = ad74413r_iio_read_fault_raw,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ad74413r_voltage_input_channels[] = {
	AD74413R_ADC_CHANNEL(IIO_VOLTAGE, ad74413r_iio_adc_attrs),
};

static struct iio_channel ad74413r_voltage_output_channels[] = {
	AD74413R_ADC_CHANNEL(IIO_CURRENT, ad74413r_iio_adc_attrs),
	AD74413R_DAC_CHANNEL(IIO_VOLTAGE),
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
	AD74413R_ADC_CHANNEL(IIO_VOLTAGE, ad74413r_iio_digital_attrs)
};

static struct iio_channel ad74413r_digital_input_loop_channels[] = {
	AD74413R_ADC_CHANNEL(IIO_VOLTAGE, ad74413r_iio_digital_attrs),
	AD74413R_DAC_CHANNEL(IIO_CURRENT)
};

static struct iio_channel ad74413r_current_input_loop_channels[] = {
	AD74413R_ADC_CHANNEL(IIO_CURRENT, ad74413r_iio_adc_attrs),
	AD74413R_DAC_CHANNEL(IIO_CURRENT)
};

static struct iio_channel ad74413r_diag_channels[] = {
	AD74413R_DIAG_CHANNEL(4, "diag0"),
	AD74413R_DIAG_CHANNEL(5, "diag1"),
	AD74413R_DIAG_CHANNEL(6, "diag2"),
	AD74413R_DIAG_CHANNEL(7, "diag3"),
};

static struct iio_channel ad74413r_fault_channels[] = {
	{
		.ch_type = IIO_VOLTAGE,
		.name = "fault",
		.ch_out = 0,
		.attributes = ad74413r_fault_attrs,
	}
};

static struct ad74413r_channel_map channel_map[] = {
	[AD74413R_HIGH_Z] = AD74413R_CHANNELS(voltage_input),
	[AD74413R_VOLTAGE_OUT] = AD74413R_CHANNELS(voltage_output),
	[AD74413R_CURRENT_OUT] = AD74413R_CHANNELS(current_output),
	[AD74413R_VOLTAGE_IN] = AD74413R_CHANNELS(voltage_input),
	[AD74413R_CURRENT_IN_EXT] = AD74413R_CHANNELS(current_input),
	[AD74413R_CURRENT_IN_LOOP] = AD74413R_CHANNELS(current_input_loop),
	[AD74413R_RESISTANCE] = AD74413R_CHANNELS(resistance_input),
	[AD74413R_DIGITAL_INPUT] = AD74413R_CHANNELS(digital_input),
	[AD74413R_DIGITAL_INPUT_LOOP] = AD74413R_CHANNELS(digital_input_loop),
	[AD74413R_CURRENT_IN_EXT_HART] = AD74413R_CHANNELS(current_input),
	[AD74413R_CURRENT_IN_LOOP_HART] = AD74413R_CHANNELS(current_input_loop),
};

static struct iio_device ad74413r_iio_dev = {
	.pre_enable = (int32_t (*)())ad74413r_iio_update_channels,
	.post_disable = (int32_t (*)())ad74413r_iio_buffer_disable,
	.trigger_handler = (int32_t (*)())ad74413r_iio_trigger_handler,
	.read_dev = (int32_t (*)())ad74413r_iio_read_samples,
	.debug_reg_read = (int32_t (*)())ad74413r_iio_read_reg,
	.debug_reg_write = (int32_t (*)())ad74413r_iio_write_reg,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

static int _get_ch_by_idx(struct iio_device *iio_dev, int scan_index, uint32_t *ch)
{
	size_t i;
	struct iio_channel *chan;

	for (i = 0; i < iio_dev->num_ch; i++) {
		chan = &iio_dev->channels[i];
		if (chan->ch_out)
			continue;

		if (chan->scan_index == scan_index) {
			*ch = chan->channel;

			return 0;
		}
	}

	return -EINVAL;
}

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
	return ad74413r_reg_read(dev->ad74413r_desc, reg, (uint32_t *)readval);
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
	return ad74413r_reg_write(dev->ad74413r_desc, reg, (uint32_t)writeval);
}

static int ad74413r_range_to_voltage_offset(enum ad74413r_adc_range range, uint32_t *val)
{
	switch (range)
	{
	case AD74413R_ADC_RANGE_10V:
	case AD74413R_ADC_RANGE_2P5V_EXT_POW:
		*val = 0;
		break;
	case AD74413R_ADC_RANGE_2P5V_INT_POW:
		*val = (-(int)AD74413R_ADC_MAX_VALUE);
		break;
	case AD74413R_ADC_RANGE_5V_BI_DIR:
		*val = (-(int)AD74413R_ADC_MAX_VALUE / 2);
		break;
	default:
		return -EINVAL;
	}

	return 0;
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
	struct ad74413r_iio_desc *desc = dev;
	enum ad74413r_diag_mode diag_func;
	enum ad74413r_adc_range range;
	int32_t raw_offset;
	int32_t range_val;
	uint32_t reg_val;
	int32_t val;
	int ret;

	__disable_irq();
	ret = ad74413r_get_adc_range(desc->ad74413r_desc, channel->address, &range);
	if (ret)
		return ret;

	switch (priv) {
	case AD74413R_ADC:
		switch (channel->type) {
		case IIO_VOLTAGE:
			ret = ad74413r_range_to_voltage_offset(range, &val);
			break;
		case IIO_CURRENT:
			if (channel->ch_out) {
				val = 0;
			} else {
				ret = ad74413r_range_to_voltage_offset(range, &val);
			}
			break;
		default:
			__enable_irq();
			return -EINVAL;
		}
		break;
	case AD74413R_DIAG:
		ret = ad74413r_reg_read(desc->ad74413r_desc, AD74413R_DIAG_ASSIGN, &reg_val);
		if (ret) {
			__enable_irq();
			return ret;
		}

		diag_func = reg_val;
		switch (diag_func) {
		case AD74413R_DIAG_REFOUT:
			val = 33671;
			break;
		case AD74413R_DIAG_AGND:
			/* fallthrough */
		case AD74413R_DIAG_AVDD:
		case AD74413R_DIAG_AVSS:
		case AD74413R_DIAG_ALDO_5V:
		case AD74413R_DIAG_ALDO_1V8:
		case AD74413R_DIAG_DLDO_1V8:
		case AD74413R_DIAG_DVCC:
		case AD74413R_DIAG_IOVDD:
		case AD74413R_SENSEL_A:
		case AD74413R_SENSEL_B:
		case AD74413R_SENSEL_C:
		case AD74413R_SENSEL_D:
			val = 0;
			break;
		default:
			__enable_irq();
			return -EINVAL;
		}
		break;
	default:
		__enable_irq();
		return -EINVAL;
	}

	__enable_irq();
	if (ret)
		return ret;

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
	uint32_t val = 0;

	if (channel->ch_out)
		ret = ad74413r_reg_read(((struct ad74413r_iio_desc *)dev)->ad74413r_desc,
					AD74413R_DAC_CODE(channel->address), &val);
	else if (priv)
		ret = ad74413r_get_diag(((struct ad74413r_iio_desc *)dev)->ad74413r_desc,
					channel->address, &val);
	else
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
	case IIO_VOLTAGE: /* fallthrough */
	case IIO_CURRENT:
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
	int32_t sample_rate;
	enum ad74413r_adc_sample val;

	__disable_irq();
	ret = ad74413r_get_adc_rate(((struct ad74413r_iio_desc *)dev)->ad74413r_desc,
				    0, &val);
	if (ret) {
		__enable_irq();
		return ret;
	}

	sample_rate = val;

	ret = iio_format_value(buf, len, IIO_VAL_INT, 1, &sample_rate);
	__enable_irq();

	return ret;
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
	struct ad74413r_iio_desc *iio_desc = dev;
	uint32_t val;
	size_t i;
	int ret;

	iio_parse_value(buf, IIO_VAL_INT, (int32_t *)&val, NULL);

	for (i = 0; i < AD74413R_N_CHANNELS; i++) {
		ret = ad74413r_set_adc_rate(iio_desc->ad74413r_desc, i, val);
		if (ret)
			return ret;
	}

	/* Set the sampling frequency for all the diagnostics channels */
	switch (val) {
	case AD74413R_ADC_SAMPLE_1200HZ:
	case AD74413R_ADC_SAMPLE_4800HZ:
		return ad74413r_reg_update(iio_desc->ad74413r_desc, AD74413R_ADC_CONV_CTRL,
					   AD74413R_EN_REJ_DIAG_MASK, 0);
	case AD74413R_ADC_SAMPLE_10HZ:
	case AD74413R_ADC_SAMPLE_20HZ:
		return ad74413r_reg_update(iio_desc->ad74413r_desc, AD74413R_ADC_CONV_CTRL,
					   AD74413R_EN_REJ_DIAG_MASK, 1);
	default:
		return -EINVAL;
	}
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
	struct ad74413r_iio_desc *desc = dev;
	enum ad74413r_diag_mode diag_func;
	enum ad74413r_adc_range range;
	uint32_t range_val;
	uint32_t reg_val;
	int32_t val[2];
	int ret;

	__disable_irq();

	switch (priv) {
	case AD74413R_ADC:
		switch (channel->type) {
		case IIO_VOLTAGE:
			if (channel->ch_out) {
				val[0] = AD74413R_DAC_RANGE;
				val[1] = AD74413R_DAC_RESOLUTION;
			} else {
				ret = ad74413r_get_adc_range(desc->ad74413r_desc,
							     channel->address, &range);
				if (ret)
					return ret;

				ret = ad74413r_range_to_voltage_range(range, &range_val);
				if (ret)
					return ret;

				val[0] = range_val;
				val[1] = AD74413R_ADC_RESOLUTION;
			}
			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 1, val);
		case IIO_CURRENT:
			if (channel->ch_out) {
				val[0] = 2500000;
				val[1] = AD74413R_SENSE_RESISTOR_OHMS * AD74413R_DAC_CODE_MAX * 1000;
			} else {
				ret = ad74413r_get_adc_range(desc->ad74413r_desc,
							     channel->address, &range);
				if (ret)
					return ret;

				ret = ad74413r_range_to_voltage_range(range, &range_val);
				if (ret)
					return ret;

				val[0] = range_val;
				val[1] = AD74413R_ADC_CODE_MAX * AD74413R_SENSE_RESISTOR_OHMS;
			}
			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 1, val);
		default:
			return -EINVAL;
		}
		break;
	case AD74413R_DIAG:
		ret = ad74413r_reg_read(desc->ad74413r_desc, AD74413R_DIAG_ASSIGN,
					&reg_val);
		if (ret)
			return ret;

		diag_func = reg_val;
		switch (diag_func) {
		case AD74413R_DIAG_AGND:
			val[0] = 0;
			val[1] = 38147;
			break;
		case AD74413R_DIAG_AVDD:
			val[0] = 0;
			val[1] = 610360;
			break;
		case AD74413R_DIAG_AVSS:
			val[0] = 0;
			val[1] = 177;
			break;
		case AD74413R_DIAG_REFOUT:
			val[0] = 0;
			val[1] = 50062;
			break;
		case AD74413R_DIAG_ALDO_5V:
			val[0] = 0;
			val[1] = 267029;
			break;
		case AD74413R_DIAG_ALDO_1V8:
			val[0] = 0;
			val[1] = 88883;
			break;
		case AD74413R_DIAG_DLDO_1V8:
			val[0] = 0;
			val[1] = 114442;
			break;
		case AD74413R_DIAG_DVCC:
			val[0] = 0;
			val[1] = 125886;
			break;
		case AD74413R_DIAG_IOVDD:
			val[0] = 0;
			val[1] = 125886;
			break;
		case AD74413R_SENSEL_A:
			/* fallthrough */
		case AD74413R_SENSEL_B:
		case AD74413R_SENSEL_C:
		case AD74413R_SENSEL_D:
			val[0] = 0;
			val[1] = 457770;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	__enable_irq();
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
					     channel->address, &decimal_val);
		if (ret)
			return ret;
		val = decimal_val.integer;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	default:
		return -EINVAL;
	}
}

static int ad74413r_iio_read_slew_en(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *desc = dev;
	uint32_t val;
	int ret;

	ret = ad74413r_reg_read(desc->ad74413r_desc, AD74413R_OUTPUT_CONFIG(channel->address),
				&val);
	if (ret)
		return ret;

	val = !!no_os_field_get(AD74413R_SLEW_EN_MASK, val);

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

static int ad74413r_iio_write_slew_en(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *desc = dev;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	return ad74413r_reg_update(desc->ad74413r_desc, AD74413R_OUTPUT_CONFIG(channel->address),
				   AD74413R_SLEW_EN_MASK, !!val);
}

static int ad74413r_iio_read_slew_step(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *desc = dev;
	uint32_t val;
	int ret;

	ret = ad74413r_reg_read(desc->ad74413r_desc,
				AD74413R_OUTPUT_CONFIG(channel->address), &val);
	if (ret)
		return ret;

	val = no_os_field_get(AD74413R_SLEW_LIN_STEP_MASK, val);
	val = ad74413r_slew_step_avail[val];

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

static int ad74413r_iio_write_slew_step(void *dev, char *buf, uint32_t len,
				        const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *desc = dev;
	enum ad74413r_slew_lin_step step;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	switch (val){
		case 64:
			step = AD74413R_STEP_64;
			break;
		case 120:
			step = AD74413R_STEP_120;
			break;
		case 500:
			step = AD74413R_STEP_500;
			break;
		case 1820:
			step = AD74413R_STEP_1820;
			break;
		default:
			return -EINVAL;
	}

	return ad74413r_reg_update(desc->ad74413r_desc,
				   AD74413R_OUTPUT_CONFIG(channel->address),
				   AD74413R_SLEW_LIN_STEP_MASK, step);
}

static int ad74413r_iio_read_slew_rate(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *desc = dev;
	uint32_t val;
	int ret;

	ret = ad74413r_reg_read(desc->ad74413r_desc,
				AD74413R_OUTPUT_CONFIG(channel->address), &val);
	if (ret)
		return ret;

	val = no_os_field_get(AD74413R_SLEW_LIN_RATE_MASK, val);
	val = ad74413r_slew_rate_avail[val];

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&val);
}

static int ad74413r_iio_write_slew_rate(void *dev, char *buf, uint32_t len,
				        const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *desc = dev;
	enum ad74413r_lin_rate rate;
	int32_t val;

	iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

	switch (val){
		case 4:
			rate = AD74413R_LIN_RATE_4KHZ;
			break;
		case 64:
			rate = AD74413R_LIN_RATE_64KHZ;
			break;
		case 150:
			rate = AD74413R_LIN_RATE_150KHZ;
			break;
		case 240:
			rate = AD74413R_LIN_RATE_240KHZ;
			break;
		default:
			return -EINVAL;
	}

	return ad74413r_reg_update(desc->ad74413r_desc,
				   AD74413R_OUTPUT_CONFIG(channel->address),
				   AD74413R_SLEW_LIN_RATE_MASK, rate);
}

static int ad74413r_iio_read_slew_rate_avail(void *dev, char *buf, uint32_t len,
					     const struct iio_ch_info *channel,
					     intptr_t priv)
{
	iio_format_value(buf, len, IIO_VAL_INT_MULTIPLE, 4, ad74413r_slew_rate_avail);

	return strlen(buf);
}

static int ad74413r_iio_read_slew_step_avail(void *dev, char *buf, uint32_t len,
					     const struct iio_ch_info *channel,
					     intptr_t priv)
{
	iio_format_value(buf, len, IIO_VAL_INT_MULTIPLE, 4, ad74413r_slew_step_avail);

	return strlen(buf);
}

static int ad74413r_iio_read_threshold(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *desc = dev;
	uint32_t reg_val;
	int ret;

	if (desc->ad74413_conv_mode != AD74413R_STOP_PWR_DOWN)
		return -EINVAL;

	ret = ad74413r_reg_read(desc->ad74413r_desc, AD74413R_DIN_THRESH, &reg_val);
	if (ret)
		return ret;

	reg_val = no_os_field_get(AD74413R_COMP_THRESH_MASK, reg_val);

	/* Convert to mV */
	reg_val = reg_val * AD74413R_THRESHOLD_RANGE / AD74413R_THRESHOLD_DAC_RANGE;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&reg_val);
}

static int ad74413r_iio_write_threshold(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *desc = dev;
	uint32_t reg_val;
	int ret;

	if (desc->ad74413_conv_mode != AD74413R_STOP_PWR_DOWN)
		return -EINVAL;

	iio_parse_value(buf, IIO_VAL_INT, &reg_val, NULL);

	return ad74413r_set_threshold(desc->ad74413r_desc, channel->address, reg_val);
}

static int ad74413r_iio_read_diag_function(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *iio_desc = dev;
	struct ad74413r_desc *desc = iio_desc->ad74413r_desc;
	enum ad74413r_diag_mode mode;
	uint32_t reg_val = 0;
	uint32_t ch_idx;
	int ret;

	ch_idx = channel->address;
	ret = ad74413r_reg_read(desc, AD74413R_DIAG_ASSIGN, &reg_val);
	if (ret)
		return ret;

	mode = no_os_field_get(AD74413R_DIAG_ASSIGN_MASK(ch_idx), reg_val);
	strcpy(buf, ad74413r_diag_available[mode]);

	return strlen(buf); 
}

static int ad74413r_iio_write_diag_function(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *iio_desc = dev;
	struct ad74413r_desc *desc = iio_desc->ad74413r_desc;
	uint32_t i;
	int ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(ad74413r_diag_available); i++) {
		if (!strcmp(ad74413r_diag_available[i], buf)) {
			ret = ad74413r_set_diag(desc, channel->address, i);
			if (ret)
				return ret;
			break;
		}

		if (i == NO_OS_ARRAY_SIZE(ad74413r_diag_available) - 1)
			return -EINVAL;
	}

	return 0;
}

static int ad74413r_iio_read_diag_function_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	size_t i;

	strcpy(buf, "");
	for (i = 0; i < NO_OS_ARRAY_SIZE(ad74413r_diag_available); i++) {
		strcat(buf, ad74413r_diag_available[i]);

		if (i != NO_OS_ARRAY_SIZE(ad74413r_diag_available) - 1)
			strcat(buf, " ");
	}

	return strlen(buf);
}

static int ad74413r_iio_read_fault_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad74413r_iio_desc *iio_desc = dev;
	struct ad74413r_desc *desc = iio_desc->ad74413r_desc;
	uint32_t fault;
	uint32_t ret;

	ret = ad74413r_reg_read(desc, AD74413R_ALERT_STATUS, &fault);
	if (ret)
		return ret;

	/* Ignore the RESET_OCCURED bit */
	fault = no_os_field_get(NO_OS_GENMASK(14, 0), fault);

	/* Clear the fault bits */
	ret = ad74413r_reg_write(desc, AD74413R_ALERT_STATUS, NO_OS_GENMASK(14, 0));
	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t *)&fault);
}

/**
 * @brief Configure a set if IIO channels based on the operation modes of the enabled
 * physical channels.
 * @param iio_desc - Specific IIO descriptor for AD74413R
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_setup_channels(struct ad74413r_iio_desc *iio_desc,
				       struct ad74413r_iio_desc_init_param *init_param)
{
	uint32_t i;
	uint32_t n_chan = 0;
	uint32_t scan_idx = 0;
	uint32_t channel_buff_cnt = 0;
	struct ad74413r_channel_config *config;
	struct iio_channel *chan;
	struct iio_channel *chan_buffer;
	struct ad74413r_channel_map channels_info;
	int ret;

	config = &init_param->channel_configs[0];

	for (i = 0; i < AD74413R_N_CHANNELS; i++) {
		if (!config[i].enabled)
			continue;
		channel_buff_cnt += channel_map[config[i].function].num_channels;
	}

	chan_buffer = no_os_calloc(channel_buff_cnt + AD74413R_DIAG_CH,
						 sizeof(*chan_buffer));
	if (!chan_buffer)
		return -ENOMEM;

	iio_desc->iio_dev->channels = chan_buffer;
	iio_desc->iio_dev->num_ch = 0;

	/* Both the ADC and DAC channels have to be added in one iteration */
	for (int i = 0; i < AD74413R_N_CHANNELS; i++) {
		if (!config[i].enabled)
			continue;

		channels_info = channel_map[config[i].function];
		chan = channels_info.channels;

		for (uint32_t ch_idx = 0; ch_idx < channels_info.num_channels; ch_idx++) {
			/* Only add a scan index to the ADC channels */
			if (chan[ch_idx].ch_out == 1)
				continue;

			chan[ch_idx].scan_index = scan_idx++;
			chan[ch_idx].channel = i;
			chan[ch_idx].address = i;

			chan_buffer[n_chan] = chan[ch_idx];
			iio_desc->iio_dev->num_ch++;
			n_chan++;

			if (config[i].function == AD74413R_DIGITAL_INPUT ||
			    config[i].function == AD74413R_DIGITAL_INPUT_LOOP) {
				ret = ad74413r_reg_update(iio_desc->ad74413r_desc, AD74413R_DIN_THRESH,
							  AD74413R_DIN_THRESH_MODE_MASK, 1);
				if (ret)
					return ret;
			}
		}

		iio_desc->no_of_active_adc_channels++;
	}

	/* Add the diagnostics channels */
	for (int i = 0; i < AD74413R_N_CHANNELS; i++) {
		chan_buffer[n_chan] = ad74413r_diag_channels[i];
		chan_buffer[n_chan].scan_index = scan_idx++;
		iio_desc->iio_dev->num_ch++;
		n_chan++;
	}

	for (int i = 0; i < AD74413R_N_CHANNELS; i++) {
		if (!config[i].enabled)
			continue;

		channels_info = channel_map[config[i].function];
		chan = channels_info.channels;

		for (uint32_t ch_idx = 0; ch_idx < channels_info.num_channels; ch_idx++) {
			/* Only add a scan index to the ADC channels */
			if (chan[ch_idx].ch_out == 0)
				continue;

			chan[ch_idx].channel = i;
			chan[ch_idx].address = i;

			chan_buffer[n_chan] = chan[ch_idx];
			iio_desc->iio_dev->num_ch++;
			n_chan++;
		}
	}

	/* Add the fault channel */
	chan_buffer[n_chan++] = ad74413r_fault_channels[0];
	iio_desc->iio_dev->num_ch++;

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
	uint32_t ch;
	size_t i;
	int ret;

	__disable_irq();
	iio_desc->active_channels = mask;
	iio_desc->no_of_active_channels = no_os_hweight32(mask);

	/* Disable all channels */
	ret = ad74413r_reg_update(iio_desc->ad74413r_desc, AD74413R_ADC_CONV_CTRL,
				  NO_OS_GENMASK(7, 0), 0);
	if (ret)
		goto out;

	for (i = 0; i < iio_desc->no_of_active_adc_channels + AD74413R_DIAG_CH_OFFSET; i++) {
		if (mask & NO_OS_BIT(i)) {
			ret = _get_ch_by_idx(iio_desc->iio_dev, i, &ch);
			if (ret)
				goto out;

			ret = ad74413r_set_adc_channel_enable(iio_desc->ad74413r_desc, ch, true);
			if (ret)
				goto out;
		}
	}

	ret = ad74413r_set_adc_conv_seq(iio_desc->ad74413r_desc, AD74413R_START_CONT);
out:
	if (!ret)
		iio_desc->ad74413_conv_mode = AD74413R_START_CONT;

	__enable_irq();

	return ret;
	// if (ret)
	// 	return ret;

	// addr_val = no_os_field_prep(0xF, AD74413R_ADC_RESULT(0));
	// addr_val |= no_os_field_prep(NO_OS_BIT(9), 0x1);

	// return ad74413r_reg_write(iio_desc->ad74413r_desc, AD74413R_READ_SELECT, addr_val);
}

/**
 * @brief Stop the ADC conversions.
 * @param dev - The iio device structure.
 * @return 0 in case of success, error code otherwise
 */
static int ad74413r_iio_buffer_disable(void *dev)
{
	struct ad74413r_iio_desc *iio_desc = dev;
	int ret;

	__disable_irq();
	ret = ad74413r_set_adc_conv_seq(iio_desc->ad74413r_desc, AD74413R_STOP_PWR_DOWN);
	if (!ret)
		iio_desc->ad74413_conv_mode = AD74413R_STOP_PWR_DOWN;
	__enable_irq();
	
	return ret;
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
	struct ad74413r_iio_desc *iio_desc = dev;
	int ret;
	uint32_t j = 0;
	uint8_t val[4];
	uint32_t i, chan_i;

	for (i = 0; i < samples; i++) {
		for (chan_i = 0; chan_i < iio_desc->no_of_active_adc_channels; chan_i++) {
			if (iio_desc->active_channels & NO_OS_BIT(chan_i)) {
				ret = ad74413r_reg_read_raw(((struct ad74413r_iio_desc *)dev)->ad74413r_desc,
							    AD74413R_ADC_RESULT(chan_i), val);
				if (ret)
					return ret;

				memcpy(&buf[j++], val, NO_OS_ARRAY_SIZE(val));
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
	uint32_t ch;
	uint8_t buff[32] = {0};
	uint32_t buffer_idx = 0;
	struct ad74413r_desc *desc;
	struct ad74413r_channel_config *config;
	struct ad74413r_iio_desc *iio_desc;
	uint32_t active_adc_ch;
	uint32_t digital_val;

	iio_desc = dev_data->dev;
	desc = iio_desc->ad74413r_desc;
	active_adc_ch = iio_desc->no_of_active_adc_channels;
	config = iio_desc->channel_configs;

	for (i = 0; i < iio_desc->no_of_active_adc_channels + AD74413R_DIAG_CH_OFFSET; i++) {
		if (iio_desc->active_channels & NO_OS_BIT(i)) {
			ret = _get_ch_by_idx(iio_desc->iio_dev, i, &ch);
			if (ret)
				continue;

			if (ch < 4) {
				if (config[ch].function == AD74413R_DIGITAL_INPUT ||
				    config[ch].function == AD74413R_DIGITAL_INPUT_LOOP) {
					ret = ad74413r_reg_read_raw(desc, AD74413R_DIN_COMP_OUT,
								    &buff[buffer_idx]);
					digital_val = no_os_field_get((1 << ch), buff[buffer_idx + 2]);
					buff[buffer_idx + 1] = 0x0;
					buff[buffer_idx + 2] = digital_val;
				} else {
					ret = ad74413r_reg_read_raw(desc, AD74413R_ADC_RESULT(ch),
								    &buff[buffer_idx]);
				}
			} else {
				ret = ad74413r_reg_read_raw(desc,
							    AD74413R_DIAG_RESULT(ch - 4),
							    &buff[buffer_idx]);
			}
			if (ret)
				goto out;

			buffer_idx += 4;
		}
	}

	iio_buffer_push_scan(dev_data->buffer, buff);

	return 0;
out:
	return ret;
}

/**
 * @brief Initializes the AD74413R IIO descriptor.
 * @param iio_desc - The iio device descriptor.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, an error code otherwise.
 */
int ad74413r_iio_init(struct ad74413r_iio_desc **iio_desc,
		      struct ad74413r_iio_desc_init_param *init_param,
		      bool config)
{
	int ret;
	uint32_t i;
	struct ad74413r_iio_desc *descriptor;

	if (!init_param || !init_param->ad74413r_init_param)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->iio_dev = &ad74413r_iio_dev;

	ret = ad74413r_init(&descriptor->ad74413r_desc,
			    init_param->ad74413r_init_param);
	if (ret) {
		no_os_mdelay(10000);
		goto err;
	}

	ret = ad74413r_clear_errors(descriptor->ad74413r_desc);
	if (ret)
		goto err;

	/** The operation modes for the physical channels are set only at initialization. */
	for (i = 0; i < AD74413R_N_CHANNELS; i++) {
		if ((*init_param->channel_configs)[i].enabled) {
			ret = ad74413r_set_adc_channel_enable(descriptor->ad74413r_desc, i, true);
			if (ret)
				goto err;

			ret = ad74413r_set_channel_function(descriptor->ad74413r_desc, i,
							    (*init_param->channel_configs)[i].function);
			if (ret)
				goto err;

			ret = ad74413r_set_adc_rate(descriptor->ad74413r_desc, i,
						    AD74413R_ADC_SAMPLE_4800HZ);
			if (ret)
				goto err;
		} else {
			ret = ad74413r_set_adc_channel_enable(descriptor->ad74413r_desc, i, false);
			if (ret)
				goto err;
		}

		ret = ad74413r_set_diag_channel_enable(descriptor->ad74413r_desc, i, true);
		if (ret)
			goto err;
	}

	ret = ad74413r_iio_setup_channels(descriptor, init_param);
	if (ret)
		goto err;

	descriptor->channel_configs = init_param->channel_configs;
	descriptor->ad74413_conv_mode = AD74413R_STOP_PWR_DOWN;

	*iio_desc = descriptor;

	return 0;
err:
	ad74413r_remove(descriptor->ad74413r_desc);
	no_os_free(descriptor);

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

	no_os_free(desc);

	return 0;
}
