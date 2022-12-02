/***************************************************************************//**
 *   @file   iio_ad7124.c
 *   @brief  Implementation of iio_ad7124.c.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "no_os_error.h"
#include "iio.h"
#include "iio_ad7124.h"
#include "no_os_util.h"
#include "ad7124.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

static int ad7124_iio_read_filter_3db(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv);
static int ad7124_iio_write_filter_3db(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);
static int ad7124_iio_read_offset_chan(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);
static int ad7124_iio_change_offset_chan(void *device, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int ad7124_iio_read_raw_chan(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);
static int ad7124_iio_read_odr_chan(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);
static int ad7124_iio_change_odr_chan(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv);
static int ad7124_iio_read_scale_chan(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv);
static int ad7124_iio_change_scale_chan(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static struct iio_attribute channel_attributes[] = {
	{
		.name = "filter_low_pass_3db_frequency",
		.priv = 0,
		.show = ad7124_iio_read_filter_3db,
		.store = ad7124_iio_write_filter_3db
	},
	{
		.name = "offset",
		.priv = 0,
		.show = ad7124_iio_read_offset_chan,
		.store = ad7124_iio_change_offset_chan
	},
	{
		.name = "raw",
		.priv = 0,
		.show = ad7124_iio_read_raw_chan,
		.store = NULL
	},
	{
		.name = "sampling_frequency",
		.priv = 0,
		.show = ad7124_iio_read_odr_chan,
		.store = ad7124_iio_change_odr_chan
	},
	{
		.name = "scale",
		.priv = 0,
		.show = ad7124_iio_read_scale_chan,
		.store = ad7124_iio_change_scale_chan
	},
	END_ATTRIBUTES_ARRAY
};

struct scan_type ad7124_iio_scan_type = {
	.sign = 's',
	.realbits = 24,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = true
};

#define AD7124_IIO_CHANN_DEF(nm, ch1, ch2) \
	{ \
		.name = nm, \
		.ch_type = IIO_VOLTAGE, \
		.channel = ch1, \
		.channel2 = ch2, \
		.scan_type = &ad7124_iio_scan_type, \
		.attributes = channel_attributes, \
		.ch_out = 0, \
		.indexed = 1, \
		.diferential = true, \
	}

static struct iio_channel ad7124_channels[] = {
	AD7124_IIO_CHANN_DEF("ch0", 0, 1),
	AD7124_IIO_CHANN_DEF("ch1", 2, 3),
	AD7124_IIO_CHANN_DEF("ch2", 4, 5),
	AD7124_IIO_CHANN_DEF("ch3", 6, 7),
	AD7124_IIO_CHANN_DEF("ch4", 8, 9),
	AD7124_IIO_CHANN_DEF("ch5", 10, 11),
	AD7124_IIO_CHANN_DEF("ch6", 12, 13),
	AD7124_IIO_CHANN_DEF("ch7", 14, 15)
};

/**
 * @brief Get cofiguration option of channel.
 * @param desc - Device driver descriptor.
 * @param ch_no - Channel ID.
 * @param config_opt - Pointer to the configuration opt.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t ad7124_iio_get_ch_config_opt(struct ad7124_dev *desc,
		uint8_t ch_no, uint8_t *config_opt)
{
	int32_t ret;
	uint32_t value;

	ret = ad7124_read_register2(desc, (AD7124_CH0_MAP_REG + ch_no),
				    &value);
	if (ret != 0)
		return ret;
	*config_opt = (value & AD7124_CH_MAP_REG_SETUP(0x7)) >> 12;

	return ret;
}

/**
 * @brief Read and display channel offset.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7124_iio_read_offset_chan(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value;
	int32_t ret;
	uint8_t config_opt;

	ret = ad7124_iio_get_ch_config_opt(desc, channel->ch_num, &config_opt);
	if (ret != 0)
		return ret;

	ret = ad7124_read_register2(desc, (AD7124_OFFS0_REG + config_opt),
				    &value);
	if (ret != 0)
		return ret;

	return snprintf(buf, len, "%"PRIX32"", value);
}

/**
 * @brief Change channel offset.
 * @param device - Device driver descriptor.
 * @param buf - Input buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7124_iio_change_offset_chan(void *device, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		reg_val;
	int32_t ret;
	uint8_t config_opt;

	ret = ad7124_iio_get_ch_config_opt(desc, channel->ch_num, &config_opt);
	if (ret != 0)
		return ret;

	sscanf(buf, "%ld", &reg_val);

	ret = ad7124_write_register2(desc, (AD7124_OFFS0_REG + config_opt),
				     reg_val);
	if (ret != 0)
		return ret;

	return len;
}

/**
 * @brief Read and display channel raw value.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7124_iio_read_raw_chan(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	int32_t			value;
	uint32_t reg_temp;
	int32_t ret;

	ret = ad7124_read_register2(desc, (AD7124_CH0_MAP_REG + channel->ch_num),
				    &reg_temp);
	if (ret != 0)
		return ret;
	reg_temp |= AD7124_CH_MAP_REG_CH_ENABLE;
	ret = ad7124_write_register2(desc, (AD7124_CH0_MAP_REG + channel->ch_num),
				     reg_temp);
	if (ret != 0)
		return ret;

	ret = ad7124_wait_for_conv_ready(desc, 10000);
	if (ret != 0)
		return ret;
	ret = ad7124_read_data(desc, &value);
	if (ret != 0)
		return ret;

	reg_temp &= ~AD7124_CH_MAP_REG_CH_ENABLE;
	ret = ad7124_write_register2(desc, (AD7124_CH0_MAP_REG + channel->ch_num),
				     reg_temp);
	if (ret != 0)
		return ret;

	return snprintf(buf, len, "%"PRId32"", (uint32_t)value);
}

/**
 * @brief Calculate and display the channel filter cutoff frequency.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7124_iio_read_filter_3db(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{

	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		value, odr;
	int32_t ret;
	uint32_t reg_temp;
	uint8_t config_opt;

	ret = ad7124_iio_get_ch_config_opt(desc, channel->ch_num, &config_opt);
	if (ret != 0)
		return ret;

	odr = (uint32_t)ad7124_get_odr(desc, config_opt);
	ret = ad7124_read_register2(desc,
				    (AD7124_FILT0_REG + config_opt),
				    &reg_temp);
	if (ret != 0)
		return ret;
	value = (reg_temp >> 21) & 0x7;
	switch (value) {
	case 0:
	case 4:
		value = odr * 262 / 1000;
		break;
	case 2:
	case 5:
		value = odr * 230 / 1000;
		break;
	default:
		return -EINVAL;
	}

	return snprintf(buf, len, "%"PRId32"", value);
}

/**
 * @brief Change the channel filter cutoff frequency.
 * @param device - Device driver descriptor.
 * @param buf - Input buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7124_iio_write_filter_3db(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		sinc4_3db_odr, sinc3_3db_odr, freq;
	uint32_t		new_filter, new_odr;
	int32_t ret;
	uint32_t reg_temp;
	uint8_t config_opt;

	ret = ad7124_iio_get_ch_config_opt(desc, channel->ch_num, &config_opt);
	if (ret != 0)
		return ret;

	sscanf(buf, "%ld", &freq);

	sinc4_3db_odr = (freq * 1000) / 230;
	sinc3_3db_odr = (freq * 1000) / 262;

	if (sinc4_3db_odr > sinc3_3db_odr) {
		new_filter = 2;
		new_odr = sinc3_3db_odr;
	} else {
		new_filter = 0;
		new_odr = sinc4_3db_odr;
	}

	ret = ad7124_read_register2(desc,
				    (AD7124_FILT0_REG + config_opt),
				    &reg_temp);
	if (ret != 0)
		return ret;
	reg_temp &= ~AD7124_FILT_REG_FILTER(~0);
	reg_temp |= AD7124_FILT_REG_FILTER(new_filter);
	ret = ad7124_write_register2(desc,
				     (AD7124_FILT0_REG + config_opt),
				     reg_temp);
	if (ret != 0)
		return ret;

	ret = ad7124_set_odr(desc, (float)new_odr, config_opt);
	if (ret != 0)
		return ret;

	return len;
}

/**
 * @brief Calculate and display the channel ODR.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7124_iio_read_odr_chan(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		odr;
	int32_t ret;
	uint8_t config_opt;

	ret = ad7124_iio_get_ch_config_opt(desc, channel->ch_num, &config_opt);
	if (ret != 0)
		return ret;

	odr = (uint32_t)ad7124_get_odr(desc, config_opt);

	return snprintf(buf, len, "%"PRId32"", odr);
}

/**
 * @brief Change the channel ODR.
 * @param device - Device driver descriptor.
 * @param buf - Input buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7124_iio_change_odr_chan(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		new_odr;
	int32_t ret;
	uint8_t config_opt;

	ret = ad7124_iio_get_ch_config_opt(desc, channel->ch_num, &config_opt);
	if (ret != 0)
		return ret;

	sscanf(buf, "%ld", &new_odr);

	ret = ad7124_set_odr(desc, (float)new_odr, config_opt);
	if (ret != 0)
		return ret;

	return len;
}

/**
 * @brief Calculate and display the channel LSB voltage value.
 * @param device - Device driver descriptor.
 * @param buf - Output buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7124_iio_read_scale_chan(void *device, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	uint32_t		vref_mv = 2500, adc_bit_no = 24;
	uint32_t		pga_bits, bipolar;
	float			lsb_val;
	int32_t ret;
	uint32_t reg_temp;
	uint8_t config_opt;

	ret = ad7124_iio_get_ch_config_opt(desc, channel->ch_num, &config_opt);
	if (ret != 0)
		return ret;

	ret = ad7124_read_register2(desc,
				    (AD7124_CFG0_REG + config_opt),
				    &reg_temp);
	if (ret != 0)
		return ret;
	pga_bits = reg_temp & 0x7;
	bipolar = (reg_temp >> 11) & 0x1;
	if (bipolar != 0)
		lsb_val = vref_mv / pow(2, (adc_bit_no + pga_bits - 1));
	else
		lsb_val = vref_mv / pow(2, (adc_bit_no + pga_bits));

	return snprintf(buf, len, "%f", lsb_val);
}

/**
 * @brief Find closest possible channel gain from a free input one.
 * @param [in] new_gain - Input gain.
 * @return New PGA setting based on the gain.
 */
static uint32_t ad7124_iio_find_closest_gain(uint32_t new_gain)
{
	uint32_t new_pga_bits = 0;
	uint32_t old_diff = 0xFFFFFFFF, new_diff;
	uint32_t i;

	for (i = 0; i < 8; i++) {
		new_diff = abs(pow(2, i) - new_gain);
		if (new_diff < old_diff) {
			old_diff = new_diff;
			new_pga_bits = i;
		}
	}

	return new_pga_bits;
}

/**
 * @brief Change the channel LSB voltage value.
 * @param device - Device driver descriptor.
 * @param buf - Input buffer.
 * @param len - Length of the input buffer (not used in this case).
 * @param channel - IIO channel information.
 * @return Number of bytes printed in the output buffer, or negative error code.
 */
static int ad7124_iio_change_scale_chan(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ad7124_dev	*desc = (struct ad7124_dev *)device;
	float			new_scale;
	uint32_t		vref_mv = 2500, adc_bit_no = 24;
	uint32_t		bipolar;
	float			lsb_val;
	uint32_t		new_gain;
	uint32_t reg_temp;
	int32_t ret;
	uint8_t config_opt;

	ret = ad7124_iio_get_ch_config_opt(desc, channel->ch_num, &config_opt);
	if (ret != 0)
		return ret;

	sscanf(buf, "%f", &new_scale);

	ret = ad7124_read_register2(desc,
				    (AD7124_CFG0_REG + config_opt),
				    &reg_temp);
	if (ret != 0)
		return ret;
	bipolar = (reg_temp >> 11) & 0x1;
	if (bipolar != 0)
		lsb_val = vref_mv / pow(2, (adc_bit_no - 1));
	else
		lsb_val = vref_mv / pow(2, adc_bit_no);

	new_gain = lsb_val / new_scale;

	new_gain = ad7124_iio_find_closest_gain(new_gain);
	reg_temp &= ~AD7124_CFG_REG_PGA(~0);
	reg_temp |= AD7124_CFG_REG_PGA(new_gain);
	ret = ad7124_write_register2(desc,
				     (AD7124_CFG0_REG + config_opt),
				     reg_temp);
	if (ret != 0)
		return ret;

	return len;
}

/**
 * @brief Get index of a bit within a channel mask.
 * @param [in] ch_mask - Channel mask.
 * @param [in] last_idx - Last index found to be set.
 * @param [out] new_index - Next bit that is set or -1 in case there is none.
 * @return true in case a set bit is found, flase otherwise.
 */
static bool get_next_ch_idx(uint32_t ch_mask, uint32_t last_idx,
			    uint32_t *new_idx)
{
	last_idx++;
	if (last_idx >= 32)
		return 0;
	ch_mask >>= last_idx;
	if (!ch_mask) {
		*new_idx = -1;
		return 0;
	}
	while (!(ch_mask & 1)) {
		last_idx++;
		ch_mask >>= 1;
	}
	*new_idx = last_idx;

	return 1;
}

/**
 * @brief Update active channels.
 * @param [in] dev - Application descriptor.
 * @param [in] mask - Number of bytes to transfer.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t iio_ad7124_update_active_channels(void *dev, uint32_t mask)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)dev;
	uint32_t ch_idx = -1;
	int32_t ret;
	uint32_t reg_temp;

	while (get_next_ch_idx(mask, ch_idx, &ch_idx)) {
		ret = ad7124_read_register2(desc,
					    (AD7124_CH0_MAP_REG + ch_idx),
					    &reg_temp);
		if (ret != 0)
			return ret;
		reg_temp |= AD7124_CH_MAP_REG_CH_ENABLE;
		ret = ad7124_write_register2(desc,
					     (AD7124_CH0_MAP_REG + ch_idx),
					     reg_temp);
		if (ret != 0)
			return ret;
	}

	return 0;
}

/**
 * @brief Close active channels.
 * @param [in] dev - Application descriptor.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t iio_ad7124_close_channels(void *dev)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)dev;
	uint32_t ch_idx;
	int32_t ret;
	uint32_t reg_temp;

	for (ch_idx = 0; ch_idx < 16; ch_idx++) {
		ret = ad7124_read_register2(desc,
					    (AD7124_CH0_MAP_REG + ch_idx),
					    &reg_temp);
		if (ret != 0)
			return ret;
		reg_temp &= ~AD7124_CH_MAP_REG_CH_ENABLE;
		ret = ad7124_write_register2(desc,
					     (AD7124_CH0_MAP_REG + ch_idx),
					     reg_temp);
		if (ret != 0)
			return ret;
	}

	return 0;
}

/**
 * @brief Get active channels in the form of a mask.
 * @param [in] desc - Device descriptor.
 * @param [out] mask - Resulting mask representing active channels.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t iio_ad7124_get_active_channels(struct ad7124_dev * desc,
		uint32_t *mask)
{
	int32_t ret, i;
	uint32_t reg_temp;

	*mask = 0;
	for (i = 0; i < 16; i++) {
		ret = ad7124_read_register2(desc,
					    (AD7124_CH0_MAP_REG + i),
					    &reg_temp);
		if (ret != 0)
			return ret;
		if (reg_temp & AD7124_CH_MAP_REG_CH_ENABLE)
			*mask |= 1 << i;
	}

	return 0;
}

/**
 * @brief Get a number of samples from all the active channels.
 * @param [in] dev - Device descriptor.
 * @param [out] buff - Sample buffer.
 * @param [in] bn_samples - Number of samples to get.
 * @return Number of samples read.
 */
static int32_t iio_ad7124_read_samples(void *dev, int32_t *buff,
				       uint32_t nb_samples)
{
	struct ad7124_dev *desc = (struct ad7124_dev *)dev;
	int32_t ret, i = 0, k;
	int32_t value;
	uint32_t ch_id = -1, test;
	uint32_t mask;

	ret = iio_ad7124_get_active_channels(desc, &mask);
	if (ret != 0)
		return ret;

	get_next_ch_idx(mask, ch_id, &ch_id);
	do {
		ret = ad7124_wait_for_conv_ready(desc, 10000);
		if (ret != 0)
			return ret;
		ret = ad7124_read_data(desc, &value);
		if (ret != 0)
			return ret;
		ret = ad7124_get_read_chan_id(desc, &test);
		if (ret != 0)
			return ret;
	} while (ch_id != test);
	buff[i++] = value;

	for (k = 0; k < nb_samples; k++) {
		while (get_next_ch_idx(mask, ch_id, &ch_id)) {
			ret = ad7124_wait_for_conv_ready(desc, 10000);
			if (ret != 0)
				return ret;
			ret = ad7124_read_data(desc, &value);
			if (ret != 0)
				return ret;
			buff[i++] = value;
		}
	}

	return nb_samples;
}

struct iio_device iio_ad7124_device = {
	.num_ch = NO_OS_ARRAY_SIZE(ad7124_channels),
	.channels = ad7124_channels,
	.attributes = NULL,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = iio_ad7124_update_active_channels,
	.post_disable = iio_ad7124_close_channels,
	.read_dev = (int32_t (*)())iio_ad7124_read_samples,
	.debug_reg_read = (int32_t (*)())ad7124_read_register2,
	.debug_reg_write = (int32_t (*)())ad7124_write_register2
};

