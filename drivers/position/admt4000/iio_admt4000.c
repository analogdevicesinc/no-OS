/***************************************************************************//**
 *   @file   iio_admt4000.c
 *   @brief  Implementation of IIO ADMT4000 driver.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 ******************************************************************************/

#include <errno.h>
#include <stdlib.h>
#include "iio_admt4000.h"
#include "admt4000.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "iio.h"
#include "iio_trigger.h"

static int admt4000_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_update_channels(void* dev, uint32_t mask);

static int admt4000_iio_reg_read(struct admt4000_iio_dev *dev, uint32_t reg,
				 uint32_t *readval);

static int admt4000_iio_reg_write(struct admt4000_iio_dev *dev, uint32_t reg,
				  uint32_t writeval);

static int admt4000_iio_show_conv_sync_mode_avail(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_conv_sync_mode(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_conv_sync_mode(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_conv_mode(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_conv_mode(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_angle_filt_en(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_angle_filt_en(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_harmonic_corr_src_avail(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_harmonic_corr_src(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_harmonic_corr_src(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_harmonic_corr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_show_harmonic_corr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_trigger_handler(struct iio_device_data *dev_data);

static const char *const admt4000_conv_sync_mode_avail[] = {
	[ADMT4000_SEQ_CTRL] = "seq_ctrl",
	[ADMT4000_START_EDGE] = "start_edge",
};

static const char *const admt4000_harmonic_corr_src_avail[] = {
	[ADMT4000_FACTORY] = "factory",
	[ADMT4000_USER] = "user",
};

#define HARMONIC_MAG_COEFF_ATTR(_num) {\
	.name = "h" # _num "mag_corr" ,\
	.priv = (_num - 1) * 2,\
	.show = admt4000_iio_show_harmonic_corr,\
	.store = admt4000_iio_store_harmonic_corr \
}

#define HARMONIC_PH_COEFF_ATTR(_num) {\
	.name = "h" # _num "ph_corr" ,\
	.priv = (_num - 1) * 2 + 1,\
	.show = admt4000_iio_show_harmonic_corr,\
	.store = admt4000_iio_store_harmonic_corr \
}

static struct iio_attribute admt4000_iio_attrs[] = {
	{
		.name = "conv_sync_mode",
		.store = admt4000_iio_store_conv_sync_mode,
		.show = admt4000_iio_show_conv_sync_mode,
	},
	{
		.name = "conv_sync_mode_available",
		.store = NULL,
		.show = admt4000_iio_show_conv_sync_mode_avail,
	},
	{
		.name = "angle_filter_enable",
		.store = admt4000_iio_store_angle_filt_en,
		.show = admt4000_iio_show_angle_filt_en,
	},
	{
		.name = "conversion_mode",
		.store = admt4000_iio_store_conv_mode,
		.show = admt4000_iio_show_conv_mode,
	},
	{
		.name = "h8_corr_src",
		.store = admt4000_iio_store_harmonic_corr_src,
		.show = admt4000_iio_show_harmonic_corr_src,
	},
	{
		.name = "h8_corr_src_available",
		.store = NULL,
		.show = admt4000_iio_show_harmonic_corr_src_avail,
	},
	HARMONIC_MAG_COEFF_ATTR(1),
	HARMONIC_PH_COEFF_ATTR(1),
	HARMONIC_MAG_COEFF_ATTR(2),
	HARMONIC_PH_COEFF_ATTR(2),
	HARMONIC_MAG_COEFF_ATTR(3),
	HARMONIC_PH_COEFF_ATTR(3),
	HARMONIC_MAG_COEFF_ATTR(8),
	HARMONIC_PH_COEFF_ATTR(8),
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute admt4000_turn_attrs[] = {
	{
		.name = "raw",
		.show = admt4000_iio_read_raw,
	},
	{
		.name = "scale",
		.show = admt4000_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute admt4000_angle_attrs[] = {
	{
		.name = "raw",
		.show = admt4000_iio_read_raw,
	},
	{
		.name = "scale",
		.show = admt4000_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute admt4000_temp_attrs[] = {
	{
		.name = "raw",
		.show = admt4000_iio_read_raw,
	},
	{
		.name = "scale",
		.show = admt4000_iio_read_scale,
	},
	{
		.name = "offset",
		.show = admt4000_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute admt4000_sincos_attrs[] = {
	{
		.name = "raw",
		.show = admt4000_iio_read_raw,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type admt4000_iio_angle_scan_type = {
	.sign = 'u',
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

static struct scan_type admt4000_iio_temp_scan_type = {
	.sign = 'u',
	.realbits = 12,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

static struct scan_type admt4000_iio_turns_scan_type = {
	.sign = 's',
	.realbits = 8,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

static struct scan_type admt4000_iio_sincos_scan_type = {
	.sign = 's',
	.realbits = 14,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

static struct iio_channel admt4000_channels[] = {
	{
		.name = "turns",
		.ch_type = IIO_COUNT,
		.channel = 0,
		.address = ADMT4000_TURNS,
		.scan_index = ADMT4000_TURNS,
		.attributes = admt4000_turn_attrs,
		.scan_type = &admt4000_iio_turns_scan_type,
		.ch_out = false
	},
	{
		.name = "angle",
		.ch_type = IIO_ANGL,
		.channel = 0,
		.address = ADMT4000_ANGLE,
		.scan_index = ADMT4000_ANGLE,
		.attributes = admt4000_angle_attrs,
		.scan_type = &admt4000_iio_angle_scan_type,
		.indexed = 1,
		.ch_out = false
	},
	{
		.ch_type = IIO_TEMP,
		.address = ADMT4000_TEMP,
		.scan_index = ADMT4000_TEMP,
		.attributes = admt4000_temp_attrs,
		.scan_type = &admt4000_iio_temp_scan_type,
		.ch_out = false
	},
	{
		.name = "cosine",
		.ch_type = IIO_COUNT,
		.channel = 1,
		.address = ADMT4000_COSINE,
		.scan_index = ADMT4000_COSINE,
		.attributes = admt4000_sincos_attrs,
		.scan_type = &admt4000_iio_sincos_scan_type,
		.indexed = 1,
		.ch_out = false
	},
	{
		.name = "sine",
		.ch_type = IIO_COUNT,
		.channel = 2,
		.address = ADMT4000_SINE,
		.scan_index = ADMT4000_SINE,
		.attributes = admt4000_sincos_attrs,
		.scan_type = &admt4000_iio_sincos_scan_type,
		.indexed = 1,
		.ch_out = false
	},
};

static struct iio_device admt4000_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(admt4000_channels),
	.attributes = admt4000_iio_attrs,
	.channels = admt4000_channels,
	.debug_reg_read = (int32_t (*)())admt4000_iio_reg_read,
	.debug_reg_write = (int32_t (*)())admt4000_iio_reg_write,
	.trigger_handler = (int32_t (*)())admt4000_iio_trigger_handler,
};

/**
 * @brief Initializes the ADMT4000 IIO driver
 * @param iio_dev - The iio device structure.
 * @param init_param - Parameters for the initialization of iio_dev
 * @return 0 in case of success, errno errors otherwise
 */
int admt4000_iio_init(struct admt4000_iio_dev **iio_dev,
		      struct admt4000_iio_dev_init_param *init_param)
{
	int ret;
	struct admt4000_iio_dev *descriptor;

	if (!init_param)
		return -EINVAL;

	descriptor = (struct admt4000_iio_dev *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = admt4000_init(&descriptor->admt4000_desc,
			    init_param->admt4000_init_param);
	if (ret)
		goto init_err;

	descriptor->iio_dev = &admt4000_iio_dev;

	*iio_dev = descriptor;

	return 0;
init_err:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - The iio device structure.
 * @return 0 in case of success, errno errors otherwise
 */
int admt4000_iio_remove(struct admt4000_iio_dev *desc)
{
	int ret;

	ret = admt4000_remove(desc->admt4000_desc);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

/**
 * @brief ADMT4000 reg read wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 * @return 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_reg_read(struct admt4000_iio_dev *dev, uint32_t reg,
				 uint32_t *readval)
{
	int ret;
	uint16_t temp;

	ret = admt4000_read(dev->admt4000_desc, (uint8_t)reg,
			    &temp, NULL);
	*readval = temp;

	return ret;
}

/**
 * @brief ADMT4000 reg write wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 * @return 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_reg_write(struct admt4000_iio_dev *dev, uint32_t reg,
				  uint32_t writeval)
{
	return admt4000_write(dev->admt4000_desc, (uint8_t)reg,
			      (uint16_t) writeval);
}

/**
 * @brief Get the list of available values for conversion sync mode attribute.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int admt4000_iio_show_conv_sync_mode_avail(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	int i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(admt4000_conv_sync_mode_avail); i++)
		length += sprintf(buf + length, "%s ", admt4000_conv_sync_mode_avail[i]);

	return length;
}

/**
 * @brief Handles the read request for conversion sync  mode attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_show_conv_sync_mode(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	int32_t mode = 0;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	ret = admt4000_get_conv_sync_mode(admt4000, &mode);
	if (ret)
		return ret;

	return sprintf(buf, "%s", admt4000_conv_sync_mode_avail[mode]);
}

/**
 * @brief Handles the write request for conversion sync mode attribute.
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_conv_sync_mode(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	uint16_t temp = 0;
	uint16_t i;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	for (i = 0; i < NO_OS_ARRAY_SIZE(admt4000_conv_sync_mode_avail); i++)
		if (!strcmp(buf, admt4000_conv_sync_mode_avail[i])) {
			ret = admt4000_set_conv_sync_mode(admt4000, i);
			if (ret)
				return ret;
			return len;
		}

	return -EINVAL;
}

/**
 * @brief Handles the write request for conversion mode attribute.
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_conv_mode(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	uint64_t conv_mode = no_os_str_to_uint32(buf);
	int ret;

	if (conv_mode != 0 && conv_mode != 1)
		return -EINVAL;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	ret = admt4000_set_conv_mode(admt4000, (bool) conv_mode);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Handles the read request for conversion mode attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_show_conv_mode(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	bool is_one_shot;
	uint16_t temp;
	int32_t conv_mode = 0;
	int ret;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	ret = admt4000_get_conv_mode(admt4000, &is_one_shot);
	if (ret)
		return ret;

	if (!is_one_shot)
		conv_mode = 0;
	else
		conv_mode = 1;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &conv_mode);
}

/**
 * @brief Handles the write request for angle filt enable attribute.
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_angle_filt_en(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	uint64_t en = no_os_str_to_uint32(buf);
	int ret;

	if (en != 0 && en != 1)
		return -EINVAL;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	ret = admt4000_set_angle_filt(admt4000, (bool)en);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Handles the read request for angle filt enable attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_show_angle_filt_en(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	bool temp;
	int32_t en = 0;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	ret = admt4000_get_angle_filt(admt4000, &temp);
	if (ret)
		return ret;

	if (!temp)
		en = 0;
	else
		en = 1;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &en);
}

/**
 * @brief Get the list of available values for harmonic corr source attribute.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int admt4000_iio_show_harmonic_corr_src_avail(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int length = 0;
	int i;

	for (i = 0; i < NO_OS_ARRAY_SIZE(admt4000_harmonic_corr_src_avail); i++)
		length += sprintf(buf + length, "%s ", admt4000_harmonic_corr_src_avail[i]);

	return length;
}

/**
 * @brief Handles the write request for harmonic correction source attribute.
 * @param dev - The iio device structure.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_harmonic_corr_src(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	uint16_t i;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	for (i = 0; i < NO_OS_ARRAY_SIZE(admt4000_harmonic_corr_src_avail); i++)
		if (!strcmp(buf, admt4000_harmonic_corr_src_avail[i])) {
			ret = admt4000_set_h8_ctrl(admt4000, i);
			if (ret)
				return ret;
			return len;
		}

	return -EINVAL;
}

/**
 * @brief Handles the read request for harmonic correction source attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_show_harmonic_corr_src(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	uint8_t source;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	ret = admt4000_get_h8_ctrl(admt4000, &source);
	if (ret)
		return ret;

	return sprintf(buf, "%s", admt4000_harmonic_corr_src_avail[source]);
}

/**
 * @brief Handles the write request for harmonic corrections coefficients attribute.
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_harmonic_corr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	uint16_t val = (uint16_t) no_os_str_to_uint32(buf);
	int ret;
	int harmonic;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	harmonic = ((int)priv) / 2 + 1;

	switch (priv) {
	case ADMT4000_H1MAG_CORR:
	case ADMT4000_H2MAG_CORR:
	case ADMT4000_H3MAG_CORR:
	case ADMT4000_H8MAG_CORR:
		ret = admt4000_set_hmag_config(admt4000, harmonic, val);
		if (ret)
			return ret;
		break;
	case ADMT4000_H1PH_CORR:
	case ADMT4000_H2PH_CORR:
	case ADMT4000_H3PH_CORR:
	case ADMT4000_H8PH_CORR:
		ret = admt4000_set_hphase_config(admt4000, harmonic, val);
		if (ret)
			return ret;
		break;
	default:
		return -EINVAL;
	}

	return len;
}

/**
 * @brief Handles the read request for harmonic corrections coefficients attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_show_harmonic_corr(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	int harmonic;
	uint16_t val = 0;
	int32_t out_val;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	harmonic = (((int)priv) / 2) + 1;

	switch (priv) {
	case ADMT4000_H1MAG_CORR:
	case ADMT4000_H2MAG_CORR:
	case ADMT4000_H3MAG_CORR:
	case ADMT4000_H8MAG_CORR:
		ret = admt4000_get_hmag_config(admt4000, harmonic, &val);
		if (ret)
			return ret;
		break;
	case ADMT4000_H1PH_CORR:
	case ADMT4000_H2PH_CORR:
	case ADMT4000_H3PH_CORR:
	case ADMT4000_H8PH_CORR:
		ret = admt4000_get_hphase_config(admt4000, harmonic, &val);
		if (ret)
			return ret;
		break;
	default:
		return -EINVAL;
	}

	out_val = (int32_t)val;
	return iio_format_value(buf, len, IIO_VAL_INT, 1, &out_val);
}

/**
 * @brief Handles the write request for pulsing coil_rs.
 * @param device - Physical instance of a iio_axi_dac device.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_sdp_pulse_coil_rs(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	ret = admt4000_sdp_pulse_coil_rs(admt4000);

	return ret;
}

/**
 * @brief Handles the read request for pulsing coil_rs.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_show_sdp_pulse_coil_rs(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;

	return 0;
}

/**
 * @brief Handles the read request for scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure. In case of success, the size of the read data is returned.
 */
static int admt4000_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;

	if (!dev)
		return -EINVAL;

	iio_admt4000 = dev;

	if (!iio_admt4000->admt4000_desc)
		return -EINVAL;

	admt4000 = iio_admt4000->admt4000_desc;

	switch (channel->type) {
	case IIO_COUNT:
		if (channel->address == ADMT4000_TURNS) {
			vals[0] = 1;
			vals[1] = 4;
			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
		} else
			return -EINVAL;
	case IIO_ANGL:
		vals[0] = 360;
		vals[1] = 4096;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
	case IIO_TEMP:
		vals[0] = 100000;
		vals[1] = 1632;
		if (admt4000->dev_vdd == ADMT4000_3P3V)
			vals[1] = 1632;
		else if (admt4000->dev_vdd == ADMT4000_5V)
			vals[1] = 1627;
		else
			return -EINVAL;

		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for offset attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure. In case of success, the size of the read data is returned.
 */
static int admt4000_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;

	if (!dev)
		return -EINVAL;

	iio_admt4000 = dev;

	if (!iio_admt4000->admt4000_desc)
		return -EINVAL;

	admt4000 = iio_admt4000->admt4000_desc;

	switch (channel->type) {
	case IIO_TEMP:
		if (admt4000->dev_vdd == ADMT4000_3P3V)
			vals[0] = - 1150;
		else if (admt4000->dev_vdd == ADMT4000_5V)
			vals[0] = - 1145;
		else
			return -EINVAL;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return ret    - Result of the reading procedure. In case of success, the size of the read data is returned.
 */
static int admt4000_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	int32_t vals[2];
	int8_t turns;
	uint16_t angle[2];
	uint16_t cos_val;
	uint16_t sin_val;
	uint16_t temp;
	bool is_one_shot;
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;

	if (!dev)
		return -EINVAL;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;

	if (!iio_admt4000->admt4000_desc)
		return -EINVAL;

	admt4000 = iio_admt4000->admt4000_desc;

	switch (channel->type) {
	case IIO_ANGL:
	case IIO_COUNT:
		ret = admt4000_get_conv_mode(admt4000, &is_one_shot);
		if (ret)
			return ret;

		ret = admt4000_toggle_cnv(admt4000);
		if (ret)
			return ret;
		no_os_mdelay(20);
		ret = admt4000_get_raw_turns_and_angle(admt4000, &turns, angle);
		if (ret)
			return ret;

		switch (channel->type) {
		case IIO_ANGL:
			ret = (int32_t)angle[1];
			break;
		case IIO_COUNT:
			switch (channel->ch_num) {
			case 0:
				ret = no_os_sign_extend32(admt4000_signed_qturn_cnt(turns), 15);
				break;
			case 1:
				ret = admt4000_get_cos(admt4000, &cos_val);
				ret = (int32_t)cos_val;
				break;
			case 2:
				ret = admt4000_get_sin(admt4000, &sin_val);
				ret = (int32_t)sin_val;
				break;
			default:
				return -EINVAL;
			}

			break;
		default:
			return -EINVAL;
		}
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);
	case IIO_TEMP:
		ret = admt4000_get_temp(admt4000, &temp, true);
		if (ret)
			return ret;

		ret = (int32_t)temp;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);

	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles trigger: reads one data-set and writes it to the buffer.
 * @param dev_data  - The iio device data structure.
 * @return the size of the written data in case of success, error code otherwise.
 */
static int admt4000_iio_trigger_handler(struct iio_device_data *dev_data)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	bool is_one_shot, cnv;
	int i = 0, ret;
	uint16_t angles[2];
	int8_t turns;

	if (!dev_data)
		return -EINVAL;

	iio_admt4000 = (struct admt4000_iio_dev *)dev_data->dev;

	admt4000 = iio_admt4000->admt4000_desc;

	ret = admt4000_get_raw_turns_and_angle(admt4000, &turns, angles);
	if (ret)
		return -EINVAL;

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_TURNS)) {
		iio_admt4000->data[i++] = admt4000_signed_qturn_cnt(turns);
	}

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_ANGLE))
		iio_admt4000->data[i++] = (int16_t) angles[1];

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_TEMP)) {
		ret = admt4000_get_temp(admt4000, &iio_admt4000->data[i], true);
		i++;
		if (ret)
			return -EINVAL;
	}

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_COSINE)) {
		ret = admt4000_get_cos(admt4000, &iio_admt4000->data[i]);
		i++;
		if (ret)
			return -EINVAL;
	}

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_SINE)) {
		ret = admt4000_get_sin(admt4000, &iio_admt4000->data[i]);
		i++;
		if (ret)
			return -EINVAL;
	}

	return iio_buffer_push_scan(dev_data->buffer, &iio_admt4000->data[0]);
}
