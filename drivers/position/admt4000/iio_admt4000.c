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
#include <string.h>
#include "iio_admt4000.h"
#include "admt4000.h"
#include "iio.h"
#include "iio_trigger.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_util.h"


static int admt4000_iio_read_raw(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv);

static int admt4000_iio_read_scale(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

static int admt4000_iio_read_offset(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

static int admt4000_iio_reg_read(struct admt4000_iio_dev *dev, uint32_t reg,
				 uint32_t *readval);

static int admt4000_iio_reg_write(struct admt4000_iio_dev *dev, uint32_t reg,
				  uint32_t writeval);

static int admt4000_iio_read_gpio_raw(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv);

static int admt4000_iio_write_gpio_raw(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int admt4000_iio_debug_reset_store(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int admt4000_iio_show_conv_sync_mode_avail(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int admt4000_iio_show_conv_sync_mode(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_conv_sync_mode(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_store_conv_mode(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int admt4000_iio_show_conv_mode(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int admt4000_iio_store_angle_filt_en(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int admt4000_iio_show_angle_filt_en(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int admt4000_iio_show_harmonic_corr_src_avail(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int admt4000_iio_store_harmonic_corr_src(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

static int admt4000_iio_show_harmonic_corr_src(void *dev, char *buf,
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);


static int admt4000_iio_read_harmonics(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel,
				       intptr_t priv);

static int admt4000_iio_write_harmonics(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv);

static int admt4000_iio_read_harmonics_magnitude_scale(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_read_harmonics_phase_scale(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);

static int admt4000_iio_read_faults(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv);

static int admt4000_iio_reinitialize(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv);

static int admt4000_iio_trigger_handler(struct iio_device_data *dev_data);

static int admt4000_iio_submit_buffer(struct iio_device_data *dev_data);

static const char *const admt4000_conv_sync_mode_avail[] = {
	[ADMT4000_SEQ_CTRL] = "seq_ctrl",
	[ADMT4000_START_EDGE] = "start_edge",
};

static const char *const admt4000_harmonic_corr_src_avail[] = {
	[ADMT4000_FACTORY] = "factory",
	[ADMT4000_USER] = "user",
};


#define ADMT4000_GPIO_RAW_ATTR(_gpio_num) {\
	.name = "gpio" # _gpio_num "_raw",\
	.priv = _gpio_num,\
	.show = admt4000_iio_read_gpio_raw,\
	.store = admt4000_iio_write_gpio_raw \
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
	{
		.name = "harmonics",
		.show = admt4000_iio_read_harmonics,
		.store = admt4000_iio_write_harmonics,
	},
	{
		.name = "harmonics_magnitude_scale",
		.show = admt4000_iio_read_harmonics_magnitude_scale,
		.store = NULL,
	},
	{
		.name = "harmonics_phase_scale",
		.show = admt4000_iio_read_harmonics_phase_scale,
		.store = NULL,
	},
	{
		.name = "faults",
		.show = admt4000_iio_read_faults,
		.store = NULL,
	},
	{
		.name = "reinitialize",
		.store = admt4000_iio_reinitialize,
		.show = NULL,
	},
	ADMT4000_GPIO_RAW_ATTR(0),
	ADMT4000_GPIO_RAW_ATTR(1),
	ADMT4000_GPIO_RAW_ATTR(2),
	ADMT4000_GPIO_RAW_ATTR(3),
	ADMT4000_GPIO_RAW_ATTR(4),
	ADMT4000_GPIO_RAW_ATTR(5),
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

static struct iio_attribute admt4000_radius_attrs[] = {
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

static struct scan_type admt4000_iio_radius_scan_type = {
	.sign = 'u',
	.realbits = 15,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

static struct iio_channel admt4000_channels[] = {
	{
		.name = "turns",
		.ch_type = IIO_ROT,
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
		.address = ADMT4000_ANGLE,
		.scan_index = ADMT4000_ANGLE,
		.attributes = admt4000_angle_attrs,
		.scan_type = &admt4000_iio_angle_scan_type,
		.ch_out = false
	},
	{
		.name = "temp",
		.ch_type = IIO_TEMP,
		.address = ADMT4000_TEMP,
		.scan_index = ADMT4000_TEMP,
		.attributes = admt4000_temp_attrs,
		.scan_type = &admt4000_iio_temp_scan_type,
		.ch_out = false
	},
	{
		.name = "cosine",
		.ch_type = IIO_COUNT, /* Quadrature signal for position sensing */
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
		.ch_type = IIO_COUNT, /* Quadrature signal for position sensing */
		.channel = 2,
		.address = ADMT4000_SINE,
		.scan_index = ADMT4000_SINE,
		.attributes = admt4000_sincos_attrs,
		.scan_type = &admt4000_iio_sincos_scan_type,
		.indexed = 1,
		.ch_out = false
	},
	{
		.name = "radius",
		.ch_type = IIO_VOLTAGE,
		.address = ADMT4000_RADIUS,
		.scan_index = ADMT4000_RADIUS,
		.attributes = admt4000_radius_attrs,
		.scan_type = &admt4000_iio_radius_scan_type,
		.ch_out = false
	},
};

static struct iio_attribute admt4000_debug_attrs[] = {
	{
		.name = "reset",
		.store = admt4000_iio_debug_reset_store,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device admt4000_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(admt4000_channels),
	.attributes = admt4000_iio_attrs,
	.channels = admt4000_channels,
	.debug_attributes = admt4000_debug_attrs,
	.debug_reg_read = (int32_t (*)())admt4000_iio_reg_read,
	.debug_reg_write = (int32_t (*)())admt4000_iio_reg_write,
	.trigger_handler = (int32_t (*)())admt4000_iio_trigger_handler,
	.submit = (int32_t (*)())admt4000_iio_submit_buffer,
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
	struct admt4000_iio_dev *descriptor;
	int ret;

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
	admt4000_remove(desc->admt4000_desc);
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

	ret = admt4000_reg_read(dev->admt4000_desc, (uint8_t)reg, &temp, NULL);
	*readval = temp;

	return ret;
}

/**
 * @brief ADMT4000 reg write wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param writeval - Register value to write
 * @return 0 in case of success, errno errors otherwise
 */
static int admt4000_iio_reg_write(struct admt4000_iio_dev *dev, uint32_t reg,
				  uint32_t writeval)
{
	return admt4000_reg_write(dev->admt4000_desc, (uint8_t)reg,
				  (uint16_t) writeval);
}

/**
 * @brief Read GPIO raw value (IIO attribute handler)
 * @param dev - IIO device structure
 * @param buf - Output buffer
 * @param len - Buffer length
 * @param channel - Channel info (unused)
 * @param priv - GPIO number (0-5)
 * @return Number of bytes written, or negative error code
 */
static int admt4000_iio_read_gpio_raw(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_dev = dev;
	struct admt4000_dev *device = iio_dev->admt4000_desc;
	int ret;
	int32_t val;
	uint8_t gpio = (uint8_t)priv;
	bool logic;

	if (gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	ret = admt4000_get_gpio(device, gpio, &logic);
	if (ret)
		return ret;

	val = (int32_t)logic;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Write GPIO raw value (IIO attribute handler)
 * @param dev - IIO device structure
 * @param buf - Input buffer
 * @param len - Buffer length
 * @param channel - Channel info (unused)
 * @param priv - GPIO number (0-5)
 * @return 0 on success, negative error code otherwise
 *         -EBUSY if GPIO is in alternate function mode
 *         -EPERM if GPIO is configured as input
 */
static int admt4000_iio_write_gpio_raw(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_dev = dev;
	struct admt4000_dev *device = iio_dev->admt4000_desc;
	int ret;
	uint32_t value;
	uint8_t gpio = (uint8_t)priv;

	if (gpio > ADMT4000_MAX_GPIO_INDEX)
		return -EINVAL;

	/* Check if GPIO is in alternate function mode */
	if (device->gpios[gpio].alt_mode)
		return -EBUSY;

	/* Check if GPIO is configured as output */
	if (!device->gpios[gpio].output_en)
		return -EPERM;

	ret = iio_parse_value(buf, IIO_VAL_INT, &value, NULL);
	if (ret)
		return ret;

	return admt4000_set_gpio(device, gpio, (bool)value);
}

/**
 * @brief IIO debug attribute handler for hardware reset.
 * @param dev - The iio device structure.
 * @param buf - Input buffer.
 * @param len - Length of the input buffer.
 * @param channel - IIO channel information.
 * @param priv - Private data.
 * @return 0 in case of success, negative error code otherwise.
 */
static int admt4000_iio_debug_reset_store(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	struct admt4000_iio_dev *iio_dev = dev;
	struct admt4000_dev *device = iio_dev->admt4000_desc;
	int ret;
	uint32_t value;

	ret = iio_parse_value(buf, IIO_VAL_INT, &value, NULL);
	if (ret)
		return ret;

	if (value != 1)
		return -EINVAL;

	/* Perform hardware reset (ignore errors) */
	admt4000_hard_reset(device);

	/* Reinitialize device and return result */
	return admt4000_reinitialize(device);
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
	int ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(admt4000_conv_sync_mode_avail); i++) {
		ret = snprintf(buf + length, len - length, "%s ",
			       admt4000_conv_sync_mode_avail[i]);
		if (ret < 0 || ret >= (int)(len - length))
			return -ENOMEM;

		length += ret;
	}

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

	return snprintf(buf, len, "%s", admt4000_conv_sync_mode_avail[mode]);
}

/**
 * @brief Handles the write request for conversion sync mode attribute.
 * @param dev - The iio device structure.
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
	int i;

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
 * @param dev - The iio device structure.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_conv_mode(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	uint8_t conv_mode = no_os_str_to_uint32(buf);

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
				       const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	int32_t conv_mode = 0;
	bool is_one_shot;

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
 * @param dev - The iio device structure.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 * @return Number of bytes written to device, or negative value on failure.
 */
static int admt4000_iio_store_angle_filt_en(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	uint8_t en = no_os_str_to_uint32(buf);

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
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	int32_t en = 0;
	bool temp;

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
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	int length = 0;
	int i;
	int ret;

	for (i = 0; i < NO_OS_ARRAY_SIZE(admt4000_harmonic_corr_src_avail); i++) {
		ret = snprintf(buf + length, len - length, "%s ",
			       admt4000_harmonic_corr_src_avail[i]);
		if (ret < 0 || ret >= (int)(len - length))
			return -ENOMEM;

		length += ret;
	}

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
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	int i;

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
		uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
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

	return snprintf(buf, len, "%s", admt4000_harmonic_corr_src_avail[source]);
}



/**
 * @brief Handles the read request for all harmonic coefficients (magnitude and phase).
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written to buf, or negative value on failure.
 * @details Format: "mag1 phase1 mag2 phase2 mag3 phase3 mag8 phase8"
 *          Example: "500 120 600 240 45 180 23 90"
 *          Represents H1, H2, H3, H8 harmonics in order.
 */
static int admt4000_iio_read_harmonics(void *dev, char *buf,
				       uint32_t len, const struct iio_ch_info *channel,
				       intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000 = (struct admt4000_iio_dev *)dev;
	struct admt4000_harmonics_calib calib;
	int ret;

	ret = admt4000_get_harmonics_calib(iio_admt4000->admt4000_desc, &calib);
	if (ret)
		return ret;

	/* Return raw register values (interleaved mag/phase pairs) */
	return snprintf(buf, len, "%u %u %u %u %u %u %u %u",
			calib.h1.magnitude_raw, calib.h1.phase_raw,   /* H1 */
			calib.h2.magnitude_raw, calib.h2.phase_raw,   /* H2 */
			calib.h3.magnitude_raw, calib.h3.phase_raw,   /* H3 */
			calib.h8.magnitude_raw, calib.h8.phase_raw);  /* H8 */
}

/**
 * @brief Handles the write request for all harmonic coefficients (magnitude and phase).
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer containing space-separated raw register values.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written, or negative value on failure.
 * @details Format: "mag1 phase1 mag2 phase2 mag3 phase3 mag8 phase8" (raw values)
 *          Example: "1024 2048 1536 3072 64 1024 32 512"
 *          All 8 values must be provided. This performs an atomic update
 *          of all harmonics in a single ECC cycle.
 *          Valid ranges:
 *            H1/H2 magnitude: 0-2047 (11-bit)
 *            H3/H8 magnitude: 0-127 (8-bit)
 *            All phases: 0-4095 (12-bit)
 *          Use harmonics_magnitude_scale and harmonics_phase_scale to convert
 *          to physical units.
 */
static int admt4000_iio_write_harmonics(void *dev, char *buf,
					uint32_t len, const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000 = (struct admt4000_iio_dev *)dev;
	struct admt4000_harmonics_calib calib;
	int ret;

	/* Parse interleaved mag/phase pairs - all 8 values required */
	ret = sscanf(buf, "%hu %hu %hu %hu %hu %hu %hu %hu",
		     &calib.h1.magnitude_raw, &calib.h1.phase_raw,   /* H1 */
		     &calib.h2.magnitude_raw, &calib.h2.phase_raw,   /* H2 */
		     &calib.h3.magnitude_raw, &calib.h3.phase_raw,   /* H3 */
		     &calib.h8.magnitude_raw, &calib.h8.phase_raw);  /* H8 */

	if (ret != 8)
		return -EINVAL;

	/* Validate bit-width constraints */
	if (calib.h1.magnitude_raw > ADMT4000_11BIT_MAX ||
	    calib.h2.magnitude_raw > ADMT4000_11BIT_MAX ||
	    calib.h3.magnitude_raw > ADMT4000_8BIT_MAX ||
	    calib.h8.magnitude_raw > ADMT4000_8BIT_MAX ||
	    calib.h1.phase_raw > ADMT4000_12BIT_MAX ||
	    calib.h2.phase_raw > ADMT4000_12BIT_MAX ||
	    calib.h3.phase_raw > ADMT4000_12BIT_MAX ||
	    calib.h8.phase_raw > ADMT4000_12BIT_MAX)
		return -EINVAL;

	/* Single atomic write - 1 ECC cycle! */
	ret = admt4000_set_harmonics_calib(iio_admt4000->admt4000_desc, &calib);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Handles the read request for harmonic magnitude scale factor.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with the scale value.
 * @param len     - Length of the buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written, or negative value on failure.
 * @details Returns the scale factor to convert raw magnitude values to
 *          physical units.
 */
static int admt4000_iio_read_harmonics_magnitude_scale(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	/* Scale factor for magnitude = resolution/(cordic scale) = 0.005493/0.6072 = 0.009046 */
	return snprintf(buf, len, "%f", ADMT4000_HMAG_SCALER);
}

/**
 * @brief Handles the read request for harmonic phase scale factor.
 * @param dev     - The iio device structure.
 * @param buf     - Command buffer to be filled with the scale value.
 * @param len     - Length of the buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written, or negative value on failure.
 * @details Returns the scale factor to convert raw phase values to
 *          physical units.
 */
static int admt4000_iio_read_harmonics_phase_scale(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	/* Scale factor for phase: resolution = 0.087891 */
	return snprintf(buf, len, "%f", ADMT4000_HPHA_SCALER);
}

/**
 * @brief Handles the read request for fault register.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return Number of bytes written to buf, or negative value on failure.
 */
static int admt4000_iio_read_faults(void *dev, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000 = (struct admt4000_iio_dev *)dev;
	struct admt4000_dev *admt4000 = iio_admt4000->admt4000_desc;
	int ret;
	int32_t out_val;
	uint16_t fault_reg;

	ret = admt4000_get_faults(admt4000, &fault_reg);
	if (ret)
		return ret;

	out_val = (int32_t)fault_reg;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &out_val);
}

static int admt4000_iio_reinitialize(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	uint8_t en = no_os_str_to_uint32(buf);

	if (en != 0 && en != 1)
		return -EINVAL;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;
	admt4000 = iio_admt4000->admt4000_desc;
	if (en) {
		ret = admt4000_reinitialize(admt4000);
		if (ret)
			return ret;
	}

	return len;

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
				   const struct iio_ch_info *channel,
				   intptr_t priv)
{
	int32_t vals[2];

	switch (channel->type) {
	case IIO_ROT:
		if (channel->address == ADMT4000_TURNS) {
			/* Turns scale: 1 turn = 4 quarter turns */
			vals[0] = 1;
			vals[1] = 4;

			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
		} else
			return -EINVAL;
	case IIO_ANGL:
		/* Angle scale: raw * scale = degrees */
		vals[0] = ADMT4000_FULL_TURN_DEGREES;
		vals[1] = ADMT4000_ANGLE_RES;

		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
	case IIO_TEMP:
		/* Temperature scale: raw * scale = millidegrees Celsius */
		vals[0] = MICRO;
		vals[1] = ADMT4000_TEMP_SCALE_MILLIS;

		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
	case IIO_VOLTAGE:
		/* Radius scale: raw * scale = millivolts */
		if (channel->address == ADMT4000_RADIUS) {
			vals[0] = ADMT4000_RADIUS_RES;
			/* Results in 0.000924 scale for mV output */
			vals[1] = MICRO;

			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
		}

		return -EINVAL;
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
				    const struct iio_ch_info *channel,
				    intptr_t priv)
{
	int32_t val;

	switch (channel->type) {
	case IIO_TEMP:
		val = -ADMT4000_TEMP_OFFSET;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
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
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;
	uint16_t angle[2];
	int16_t sincos_val;
	uint16_t temp;
	int16_t quarter_turns;
	uint8_t raw_quarter_turns;

	if (!dev)
		return -EINVAL;

	iio_admt4000 = (struct admt4000_iio_dev *)dev;

	if (!iio_admt4000->admt4000_desc)
		return -EINVAL;

	admt4000 = iio_admt4000->admt4000_desc;

	switch (channel->type) {
	case IIO_ANGL:
	case IIO_ROT:
	case IIO_COUNT:
		if (admt4000->is_one_shot) {
			ret = admt4000_toggle_cnv(admt4000);
			if (ret)
				return ret;

			no_os_mdelay(ADMT4000_CONVERSION_DELAY_MS);
		}

		ret = admt4000_get_raw_turns_and_angle(admt4000, &raw_quarter_turns, angle);
		if (ret)
			return ret;

		switch (channel->type) {
		case IIO_ANGL:
			ret = (int32_t)angle[1];
			break;
		case IIO_ROT:
			ret = admt4000_quarter_turns_cnt(raw_quarter_turns, &quarter_turns);
			if (ret)
				return ret;

			ret = no_os_sign_extend32(quarter_turns, 15);
			break;
		case IIO_COUNT:
			switch (channel->ch_num) {
			case 1:
				ret = admt4000_get_cos(admt4000, &sincos_val, NULL);
				if (ret)
					return ret;

				ret = (int32_t)no_os_sign_extend32(sincos_val, 15);
				break;
			case 2:
				ret = admt4000_get_sin(admt4000, &sincos_val, NULL);
				if (ret)
					return ret;
				ret = (int32_t)no_os_sign_extend32(sincos_val, 15);
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
		ret = admt4000_get_temp(admt4000, &temp);
		if (ret)
			return ret;

		ret = (int32_t)temp;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);
	case IIO_VOLTAGE:
		/* Radius is a voltage measurement */
		if (channel->address == ADMT4000_RADIUS) {
			uint16_t radius_val;
			ret = admt4000_get_radius(admt4000, &radius_val, NULL);
			if (ret)
				return ret;

			ret = (int32_t)radius_val;

			return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);
		}

		return -EINVAL;
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
	int i = 0;
	int ret;
	uint16_t angles[2];
	uint8_t turns;

	if (!dev_data)
		return -EINVAL;

	iio_admt4000 = (struct admt4000_iio_dev *)dev_data->dev;

	admt4000 = iio_admt4000->admt4000_desc;

	ret = admt4000_get_raw_turns_and_angle(admt4000, &turns, angles);
	if (ret)
		return ret;

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_TURNS)) {
		ret = admt4000_quarter_turns_cnt(turns, &iio_admt4000->data[i]);
		i++;
		if (ret)
			return ret;
	}

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_ANGLE))
		iio_admt4000->data[i++] = (int16_t) angles[1];

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_TEMP)) {
		ret = admt4000_get_temp(admt4000, &iio_admt4000->data[i]);
		i++;
		if (ret)
			return ret;
	}

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_COSINE)) {
		ret = admt4000_get_cos(admt4000, &iio_admt4000->data[i], NULL);
		i++;
		if (ret)
			return ret;
	}

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_SINE)) {
		ret = admt4000_get_sin(admt4000, &iio_admt4000->data[i], NULL);
		i++;
		if (ret)
			return ret;
	}

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADMT4000_RADIUS)) {
		ret = admt4000_get_radius(admt4000, &iio_admt4000->data[i], NULL);
		i++;
		if (ret)
			return ret;
	}

	return iio_buffer_push_scan(dev_data->buffer, &iio_admt4000->data[0]);
}

/**
 * @brief Submit buffer handler for manual conversion triggering
 * @param dev_data - IIO device data structure
 * @return 0 on success, negative error code otherwise
 * @details This function is called when ACALC is not configured as interrupt source.
 *          It manually toggles CNV, waits for conversion, then calls trigger_handler.
 */
static int admt4000_iio_submit_buffer(struct iio_device_data *dev_data)
{
	struct admt4000_iio_dev *iio_admt4000;
	struct admt4000_dev *admt4000;
	int ret;

	if (!dev_data)
		return -EINVAL;

	iio_admt4000 = (struct admt4000_iio_dev *)dev_data->dev;
	admt4000 = iio_admt4000->admt4000_desc;

	/* Only toggle CNV if ACALC is not configured (not interrupt mode) */
	if (!admt4000->gpios[3].alt_mode) {
		/* Trigger conversion manually */
		ret = admt4000_toggle_cnv(admt4000);
		if (ret)
			return ret;

		/* Wait for conversion to complete */
		no_os_mdelay(ADMT4000_CONVERSION_DELAY_MS);
	}

	/* Call trigger handler to read and push data */
	return admt4000_iio_trigger_handler(dev_data);
}
