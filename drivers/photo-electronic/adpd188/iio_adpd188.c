/***************************************************************************//**
 *   @file   iio_adpd188.c
 *   @brief  Implementation of the ADPD188 IIO driver.
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "iio_adpd188.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include <stdio.h>

static int adpd188_iio_read_offset_chan(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);
static int adpd188_iio_change_offset_chan(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adpd188_iio_read_raw_chan(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);
static int adpd188_iio_read_odr_chan(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);
static int adpd188_iio_change_odr_chan(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static struct iio_attribute adpd188_channel_attributes[] = {
	{
		.name = "offset",
		.priv = 0,
		.show = adpd188_iio_read_offset_chan,
		.store = adpd188_iio_change_offset_chan
	},
	{
		.name = "raw",
		.priv = 0,
		.show = adpd188_iio_read_raw_chan,
		.store = NULL
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adpd188_device_attributes[] = {
	{
		.name = "sampling_frequency",
		.priv = 0,
		.show = adpd188_iio_read_odr_chan,
		.store = adpd188_iio_change_odr_chan
	},
	END_ATTRIBUTES_ARRAY
};

struct scan_type adpd188_iio_scan_type = {
	.sign = 'u',
	.realbits = 27,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = true
};

#define ADPD188_IIO_CHANN_DEF(nm, ch1) \
	{ \
		.name = nm, \
		.ch_type = IIO_CURRENT, \
		.channel = ch1, \
		.scan_index = ch1, \
		.scan_type = &adpd188_iio_scan_type, \
		.attributes = adpd188_channel_attributes, \
		.indexed = 1, \
	}

static struct iio_channel adpd188_channels[] = {
	ADPD188_IIO_CHANN_DEF("pd0", 0),
	ADPD188_IIO_CHANN_DEF("pd1", 1),
	ADPD188_IIO_CHANN_DEF("pd2", 2),
	ADPD188_IIO_CHANN_DEF("pd3", 3),
	ADPD188_IIO_CHANN_DEF("pd4", 4),
	ADPD188_IIO_CHANN_DEF("pd5", 5),
	ADPD188_IIO_CHANN_DEF("pd6", 6),
	ADPD188_IIO_CHANN_DEF("pd7", 7)
};

/**
 * @brief Read offset for the channel.
 * @param device - Pointer to the IIO device structure.
 * @param buf - Pointer to the ASCII buffer that will contain the response.
 * @param len - Length of the output buffer.
 * @param channel - Structure containing the channel ID data.
 * @param priv - Private pointer not used in this case.
 * @return Length of the response or 0 in case of error.
 */
static int adpd188_iio_read_offset_chan(void *device, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	int32_t ret;
	struct adpd188_iio_desc *iio_desc = (struct adpd188_iio_desc *)device;
	struct adpd188_dev *desc = iio_desc->drv_dev;
	uint8_t ch_offset = (channel->ch_num < 4) ? channel->ch_num :
			    (channel->ch_num + 2);
	uint16_t reg_val;

	ret = adpd188_reg_read(desc, (ADPD188_REG_SLOTA_CH1_OFFSET + ch_offset),
			       &reg_val);
	if (ret != 0)
		return ret;

	return snprintf(buf, len, "%"PRIX16"", reg_val);
}

/**
 * @brief Change the offset for the channel.
 * @param device - Pointer to the IIO device structure.
 * @param buf - Pointer to the ASCII buffer that contains the new value.
 * @param len - Length of the input buffer.
 * @param channel - Structure containing the channel ID data.
 * @param priv - Private pointer not used in this case.
 * @return Length of the input buffer.
 */
static int adpd188_iio_change_offset_chan(void *device, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	struct adpd188_iio_desc *iio_desc = (struct adpd188_iio_desc *)device;
	struct adpd188_dev *desc = iio_desc->drv_dev;
	uint8_t ch_offset = (channel->ch_num < 4) ? channel->ch_num :
			    (channel->ch_num + 2);
	uint16_t reg_val;

	sscanf(buf, "%hx", &reg_val);

	ret = adpd188_mode_set(desc, ADPD188_PROGRAM);
	if (ret != 0)
		return ret;

	ret = adpd188_reg_write(desc, (ADPD188_REG_SLOTA_CH1_OFFSET + ch_offset),
				reg_val);
	if (ret != 0)
		return ret;

	ret = adpd188_mode_set(desc, ADPD188_STANDBY);
	if (ret != 0)
		return ret;

	return len;
}

/**
 * @brief Read the output data rate.
 * @param device - Pointer to the IIO device structure.
 * @param buf - Pointer to the ASCII buffer that will contain the response.
 * @param len - Length of the output buffer.
 * @param channel - Structure containing the channel ID data.
 * @param priv - Private pointer not used in this case.
 * @return Length of the response or 0 in case of error.
 */
static int adpd188_iio_read_odr_chan(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t ret;
	struct adpd188_iio_desc *iio_desc = (struct adpd188_iio_desc *)device;
	struct adpd188_dev *desc = iio_desc->drv_dev;
	uint16_t freq_hz;

	ret = adpd188_adc_fsample_get(desc, &freq_hz);
	if (ret != 0)
		return ret;

	return snprintf(buf, len, "%"PRId16"", freq_hz);
}

/**
 * @brief Change the output data rate.
 * @param device - Pointer to the IIO device structure.
 * @param buf - Pointer to the ASCII buffer that contains the new value.
 * @param len - Length of the output buffer.
 * @param channel - Structure containing the channel ID data.
 * @param priv - Private pointer not used in this case.
 * @return Length of the output buffer.
 */
static int adpd188_iio_change_odr_chan(void *device, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	int32_t ret;
	struct adpd188_iio_desc *iio_desc = (struct adpd188_iio_desc *)device;
	struct adpd188_dev *desc = iio_desc->drv_dev;
	uint16_t freq_hz;

	sscanf(buf, "%hu", &freq_hz);

	ret = adpd188_mode_set(desc, ADPD188_PROGRAM);
	if (ret != 0)
		return ret;

	ret = adpd188_adc_fsample_set(desc, freq_hz);
	if (ret != 0)
		return ret;

	ret = adpd188_mode_set(desc, ADPD188_STANDBY);
	if (ret != 0)
		return ret;

	return len;
}

/**
 * @brief Change device into normal mode.
 * @param desc - Pointer to the device driver handler.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t adpd188_iio_normal_mode(struct adpd188_dev *desc)
{
	int32_t ret;

	ret = adpd188_mode_set(desc, ADPD188_PROGRAM);
	if (ret != 0)
		return ret;
	return adpd188_mode_set(desc, ADPD188_NORMAL);
}

/**
 * @brief Change device into standby mode.
 * @param desc - Pointer to the device driver handler.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t adpd188_iio_standby_mode(struct adpd188_dev *desc)
{
	int32_t ret;

	ret = adpd188_mode_set(desc, ADPD188_PROGRAM);
	if (ret != 0)
		return ret;
	return adpd188_mode_set(desc, ADPD188_STANDBY);
}

/**
 * @brief Read raw value of the channel.
 * @param device - Pointer to the IIO device structure.
 * @param buf - Pointer to the ASCII buffer that will contain the response.
 * @param len - Length of the output buffer.
 * @param channel - Structure containing the channel ID data.
 * @param priv - Private pointer not used in this case.
 * @return Length of the response or 0 in case of error.
 */
static int adpd188_iio_read_raw_chan(void *device, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	int32_t ret;
	struct adpd188_iio_desc *iio_desc = (struct adpd188_iio_desc *)device;
	struct adpd188_dev *desc = iio_desc->drv_dev;
	uint8_t fifo_bytes, i;
	uint16_t data[ADPD1080_WORDS_PER_SAMPLE * ADPD1080_CHANNEL_NO];
	uint32_t req_sample;

	ret = adpd188_iio_normal_mode(desc);
	if (ret != 0)
		return ret;

	do {
		ret = adpd188_fifo_status_get(desc, &fifo_bytes);
		if (ret != 0)
			return ret;
	} while (fifo_bytes < 32);

	ret = adpd188_iio_standby_mode(desc);
	if (ret != 0)
		return ret;

	for (i = 0;
	     i < (ADPD1080_WORDS_PER_SAMPLE * ADPD1080_CHANNEL_NO);
	     i++) {
		ret = adpd188_reg_read(desc, ADPD188_REG_FIFO_ACCESS,
				       (data + i));
		if (ret != 0)
			return ret;
	}

	req_sample = data[(2 * channel->ch_num)] |
		     (data[(2 * channel->ch_num + 1)] << 16);

	return snprintf(buf, len, "%"PRIX32"", req_sample);
}

/**
 * @brief Enable channels before buffer read.
 * @param dev - Pointer to the IIO driver structure.
 * @param mask - Mask of the enabled channels.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t iio_adpd188_prepare_data_read(void *dev, uint32_t mask)
{
	struct adpd188_iio_desc *iio_desc = (struct adpd188_iio_desc *)dev;
	struct adpd188_dev *desc = iio_desc->drv_dev;

	iio_desc->ch_mask = mask;

	return adpd188_iio_normal_mode(desc);
}

/**
 * @brief Disable all channels after buffer read.
 * @param dev - Pointer to the IIO driver structure.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t iio_adpd188_end_data_read(void *dev)
{
	struct adpd188_iio_desc *iio_desc = (struct adpd188_iio_desc *)dev;
	struct adpd188_dev *desc = iio_desc->drv_dev;

	iio_desc->ch_mask = 0;

	return adpd188_iio_standby_mode(desc);
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
 * @brief Read consecutive samples in a buffer.
 * @param dev - Pointer to the IIO driver structure.
 * @param buff - Output buffer.
 * @param nb_samples - Number of samples to take.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t iio_adpd188_read_samples(void *dev, int32_t *buff,
					uint32_t nb_samples)
{
	struct adpd188_iio_desc *iio_desc = (struct adpd188_iio_desc *)dev;
	struct adpd188_dev *desc = iio_desc->drv_dev;
	uint8_t byte_no;
	uint32_t i, k, new_idx, last_idx, buff_idx = 0;
	uint16_t data_buff[16];
	int32_t ret;

	for (i = 0; i < nb_samples; i++) {
		do {
			ret = adpd188_fifo_status_get(desc, &byte_no);
			if (NO_OS_IS_ERR_VALUE(ret))
				return -1;
		} while (byte_no < 32);

		for (k = 0; k < 16; k++) {
			ret = adpd188_reg_read(desc, ADPD188_REG_FIFO_ACCESS,
					       (data_buff + k));
			if (NO_OS_IS_ERR_VALUE(ret))
				return -1;
		}
		last_idx = -1;
		while (get_next_ch_idx(iio_desc->ch_mask, last_idx,
				       &new_idx)) {
			buff[buff_idx] = data_buff[new_idx * 2];
			buff[buff_idx++] |= data_buff[new_idx * 2 + 1] << 16;
			last_idx = new_idx;
		}
	}

	return 0;
}

/**
 * @brief  Register read access wrapper.
 * @param dev - Pointer to the IIO driver structure.
 * @param reg_addr - Register address.
 * @param reg_val - Pointer to the register value.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t iio_adpd188_reg_read(struct adpd188_iio_desc *dev,
				    uint8_t reg_addr,
				    uint16_t *reg_val)
{
	struct adpd188_dev *desc = dev->drv_dev;

	return adpd188_reg_read(desc, reg_addr, reg_val);
}

/**
 * @brief  Register read access wrapper.
 * @param dev - Pointer to the IIO driver structure.
 * @param reg_addr - Register address.
 * @param reg_val - New register value.
 * @return 0 in case of success, error code otherwise.
 */
static int32_t iio_adpd188_reg_write(struct adpd188_iio_desc *dev,
				     uint8_t reg_addr,
				     uint16_t reg_val)
{
	struct adpd188_dev *desc = dev->drv_dev;

	return adpd188_reg_write(desc, reg_addr, reg_val);
}

/**
 * @brief Initialize the IIO driver and allocate memory.
 * @param device - Pointer to the IIO driver handler structure.
 * @param init_param - Pointer to the IIO driver initialization structure.
 * @return 0 in case of success, error code otherwise.
 */
int32_t adpd188_iio_init(struct adpd188_iio_desc **device,
			 struct adpd188_iio_init_param *init_param)
{
	struct adpd188_iio_desc *dev =
		(struct adpd188_iio_desc *)no_os_calloc(1, sizeof(*dev));
	int32_t ret;
	uint16_t reg_data;
	struct adpd188_slot_config slota = {
		.slot_id = ADPD188_SLOTA,
		.slot_en = true,
		.sot_fifo_mode = ADPD188_32BIT_4CHAN
	};
	struct adpd188_slot_config slotb = {
		.slot_id = ADPD188_SLOTB,
		.slot_en = true,
		.sot_fifo_mode = ADPD188_32BIT_4CHAN
	};

	if (!dev)
		return -1;

	dev->ch_mask = 0;

	ret = adpd188_init(&dev->drv_dev, &init_param->drv_init_param);
	if (ret != 0)
		goto error_dev;

	/* Enable state machine clock */
	ret = adpd188_reg_read(dev->drv_dev, ADPD188_REG_SAMPLE_CLK, &reg_data);
	if (ret != 0)
		goto error_drv;
	reg_data |= ADPD188_SAMPLE_CLK_CLK32K_EN_MASK;
	ret = adpd188_reg_write(dev->drv_dev, ADPD188_REG_SAMPLE_CLK, reg_data);
	if (ret != 0)
		goto error_drv;

	ret = adpd188_mode_set(dev->drv_dev, ADPD188_PROGRAM);
	if (ret != 0)
		goto error_drv;

	ret = adpd188_slot_setup(dev->drv_dev, slota);
	if (ret != 0)
		goto error_drv;
	ret = adpd188_slot_setup(dev->drv_dev, slotb);
	if (ret != 0)
		goto error_drv;

	ret = adpd188_reg_read(dev->drv_dev, ADPD188_REG_PD_LED_SELECT, &reg_data);
	if (ret != 0)
		goto error_drv;
	/* Select LEDx1 for both timeslots */
	reg_data &= ~(ADPD188_PD_LED_SELECT_SLOTA_LED_SEL_MASK |
		      ADPD188_PD_LED_SELECT_SLOTB_LED_SEL_MASK |
		      ADPD188_PD_LED_SELECT_SLOTA_PD_SEL_MASK |
		      ADPD188_PD_LED_SELECT_SLOTB_PD_SEL_MASK);
	reg_data |= (1 << ADPD188_PD_LED_SELECT_SLOTA_LED_SEL_POS) &
		    ADPD188_PD_LED_SELECT_SLOTA_LED_SEL_MASK;
	reg_data |= (1 << ADPD188_PD_LED_SELECT_SLOTB_LED_SEL_POS) &
		    ADPD188_PD_LED_SELECT_SLOTB_LED_SEL_MASK;
	/* Timeslot A has PD1-4 on channels 1-4 */
	reg_data |= (5 << ADPD188_PD_LED_SELECT_SLOTA_PD_SEL_POS) &
		    ADPD188_PD_LED_SELECT_SLOTA_PD_SEL_MASK;
	/* Timeslot B has PD5-8 on channels 1-4 */
	reg_data |= (4 << ADPD188_PD_LED_SELECT_SLOTB_PD_SEL_POS) &
		    ADPD188_PD_LED_SELECT_SLOTB_PD_SEL_MASK;
	ret = adpd188_reg_write(dev->drv_dev, ADPD188_REG_PD_LED_SELECT, reg_data);
	if (ret != 0)
		goto error_drv;

	/* No averaging for any slot */
	ret = adpd188_reg_write(dev->drv_dev, ADPD188_REG_NUM_AVG, 0);
	if(ret != 0)
		goto error_drv;

	*device = dev;

	return 0;
error_drv:
	adpd188_remove(dev->drv_dev);
error_dev:
	no_os_free(dev);

	return -1;
}

/**
 * @brief Free mmory allocated by adpd188_iio_init().
 * @param dev - Pointer to the IIO driver structure.
 * @return 0 in case of success, error code otherwise.
 */
int32_t adpd188_iio_remove(struct adpd188_iio_desc *dev)
{
	int32_t ret;

	ret = adpd188_remove(dev->drv_dev);
	if (ret != 0)
		return ret;

	no_os_free(dev);

	return 0;
}

struct iio_device iio_adpd188_device = {
	.num_ch = NO_OS_ARRAY_SIZE(adpd188_channels),
	.channels = adpd188_channels,
	.attributes = adpd188_device_attributes,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = iio_adpd188_prepare_data_read,
	.post_disable = iio_adpd188_end_data_read,
	.read_dev = (int32_t (*)())iio_adpd188_read_samples,
	.debug_reg_read = (int32_t (*)())iio_adpd188_reg_read,
	.debug_reg_write = (int32_t (*)())iio_adpd188_reg_write
};

