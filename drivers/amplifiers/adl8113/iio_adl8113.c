/***************************************************************************//**
 * @file iio_adl8113.c
 * @brief Implementation of ADL8113 IIO Driver.
 * @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
 ********************************************************************************
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <stdio.h>

#include "iio_adl8113.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"

/**
 * @brief Find a gain configuration by its gain_db value.
 * @param[in] iio_dev  - IIO descriptor.
 * @param[in] gain_db  - Gain in dB to search for.
 * @return Index of the matching config, negative error code otherwise.
 */
static int adl8113_find_gain_config(struct adl8113_iio_dev *iio_dev,
				    int gain_db)
{
	uint8_t i;

	if (!iio_dev || !iio_dev->gain_configs)
		return -EINVAL;

	for (i = 0; i < iio_dev->num_gain_configs; i++) {
		if (iio_dev->gain_configs[i].gain_db == gain_db)
			return (int)i;
	}

	return -EINVAL;
}

/**
 * @brief Get the gain_db corresponding to the currently selected path/mode.
 * @param[in]  iio_dev  - IIO descriptor.
 * @param[out] gain_db  - Gain in dB for the current mode.
 * @return 0 in case of success, negative error code otherwise.
 */
static int adl8113_get_current_gain(struct adl8113_iio_dev *iio_dev,
				    int *gain_db)
{
	enum adl8113_mode mode;
	uint8_t i;
	int ret;

	if (!iio_dev || !gain_db)
		return -EINVAL;

	ret = adl8113_get_mode(iio_dev->adl8113_dev, &mode);
	if (ret)
		return ret;

	for (i = 0; i < iio_dev->num_gain_configs; i++) {
		if (iio_dev->gain_configs[i].path == mode) {
			*gain_db = iio_dev->gain_configs[i].gain_db;
			return 0;
		}
	}

	return -EINVAL;
}

/**
 * @brief IIO callback: read "hardwaregain".
 * @param[in]  device  - IIO device descriptor (#adl8113_iio_dev).
 * @param[out] buf     - Output buffer.
 * @param[in]  len     - Length of \p buf.
 * @param[in]  channel - Channel info.
 * @param[in]  priv    - Private data.
 * @return Length of buffer used, negative error code otherwise.
 *
 * Returns a Linux-compatible numeric format (e.g. "14.000000").
 */
static int adl8113_iio_read_hardwaregain(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct adl8113_iio_dev *iio_dev = device;
	int gain_db;
	int ret;

	(void)channel;
	(void)priv;

	if (!iio_dev || !buf)
		return -EINVAL;

	ret = adl8113_get_current_gain(iio_dev, &gain_db);
	if (ret)
		return ret;

	return snprintf(buf, len, "%d.000000", gain_db);
}

/**
 * @brief IIO callback: write "hardwaregain".
 * @param[in] device   - IIO device descriptor (#adl8113_iio_dev).
 * @param[in] buf      - Input buffer.
 * @param[in] len      - Length of \p buf.
 * @param[in] channel  - Channel info.
 * @param[in] priv     - Private data.
 * @return 0 in case of success, negative error code otherwise.
 *
 * Linux-style behavior: writing a valid gain selects the corresponding path/mode.
 */
static int adl8113_iio_write_hardwaregain(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct adl8113_iio_dev *iio_dev = device;
	int gain_db;
	int config_idx;
	int ret;

	(void)len;
	(void)channel;
	(void)priv;

	if (!iio_dev || !buf)
		return -EINVAL;

	/* Allow "14", "14.0", "14.000000", "14.000000 dB" etc. */
	if (sscanf(buf, "%d", &gain_db) != 1)
		return -EINVAL;

	config_idx = adl8113_find_gain_config(iio_dev, gain_db);
	if (config_idx < 0)
		return config_idx;

	ret = adl8113_set_mode(iio_dev->adl8113_dev,
			       iio_dev->gain_configs[config_idx].path);

	return ret;
}

/* Channel attributes */
static struct iio_attribute adl8113_iio_ch_attrs[] = {
	{
		.name = "hardwaregain",
		.shared = IIO_SHARED_BY_TYPE,
		.show = adl8113_iio_read_hardwaregain,
		.store = adl8113_iio_write_hardwaregain,
	},
	END_ATTRIBUTES_ARRAY
};

/* Single global gain-control channel */
static struct iio_channel adl8113_iio_channels[] = {
	{
		.ch_type = IIO_VOLTAGE,
		.ch_out = IIO_DIRECTION_INPUT,
		.indexed = false,
		.channel = 0,
		.scan_index = -1,
		.scan_type = NULL,
		.attributes = adl8113_iio_ch_attrs,
	},
};

/* Device descriptor template */
static struct iio_device adl8113_iio_device_template = {
	.num_ch = NO_OS_ARRAY_SIZE(adl8113_iio_channels),
	.channels = adl8113_iio_channels,
	.attributes = NULL,
};

/**
 * @brief Initialize the gain configuration table (Linux-style).
 * @param[in,out] desc       - IIO descriptor.
 * @param[in]     init_param - Initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 *
 * Always adds:
 * - Internal amplifier: 14 dB
 * - Internal bypass:    -2 dB
 *
 * Optionally adds:
 * - External bypass A/B gains (if enabled in init_param)
 */
static int adl8113_iio_init_gain_configs(struct adl8113_iio_dev *desc,
		struct adl8113_iio_init_param *init_param)
{
	uint8_t n = 2;
	uint8_t i = 0;

	if (!desc || !init_param)
		return -EINVAL;

	if (init_param->has_external_bypass_a)
		n++;

	if (init_param->has_external_bypass_b)
		n++;

	desc->gain_configs = no_os_calloc(n, sizeof(*desc->gain_configs));
	if (!desc->gain_configs)
		return -ENOMEM;

	/* Always include internal amplifier (14 dB) */
	desc->gain_configs[i++] = (struct adl8113_iio_gain_config) {
		.path = ADL8113_INTERNAL_AMPLIFIER,
		.gain_db = 14,
	};

	/* Always include internal bypass (-2 dB insertion loss) */
	desc->gain_configs[i++] = (struct adl8113_iio_gain_config) {
		.path = ADL8113_INTERNAL_BYPASS,
		.gain_db = -2,
	};

	/* Optional external bypass A */
	if (init_param->has_external_bypass_a) {
		desc->gain_configs[i++] = (struct adl8113_iio_gain_config) {
			.path = ADL8113_EXTERNAL_BYPASS_A,
			.gain_db = init_param->external_bypass_a_gain_db,
		};
	}

	/* Optional external bypass B */
	if (init_param->has_external_bypass_b) {
		desc->gain_configs[i++] = (struct adl8113_iio_gain_config) {
			.path = ADL8113_EXTERNAL_BYPASS_B,
			.gain_db = init_param->external_bypass_b_gain_db,
		};
	}

	desc->num_gain_configs = i;

	/* Sanity */
	if (desc->num_gain_configs != n)
		return -EINVAL;

	return 0;
}

/**
 * @brief Initialize the ADL8113 IIO driver.
 * @param[out] iio_dev    - Pointer to the allocated IIO descriptor.
 * @param[in]  init_param - Initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adl8113_iio_init(struct adl8113_iio_dev **iio_dev,
		     struct adl8113_iio_init_param *init_param)
{
	struct adl8113_iio_dev *desc;
	int ret;

	if (!iio_dev || !init_param || !init_param->adl8113_init_param)
		return -EINVAL;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	ret = adl8113_iio_init_gain_configs(desc, init_param);
	if (ret)
		goto err_desc;

	/* Core driver init (VA/VB GPIO truth table lives in core). */
	ret = adl8113_init(&desc->adl8113_dev, init_param->adl8113_init_param);
	if (ret)
		goto err_gain;

	desc->iio_dev = &adl8113_iio_device_template;
	*iio_dev = desc;

	return 0;

err_gain:
	no_os_free(desc->gain_configs);
err_desc:
	no_os_free(desc);
	return ret;
}

/**
 * @brief Remove the ADL8113 IIO driver.
 * @param[in] iio_dev - IIO descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int adl8113_iio_remove(struct adl8113_iio_dev *iio_dev)
{
	if (!iio_dev)
		return -EINVAL;

	adl8113_remove(iio_dev->adl8113_dev);
	no_os_free(iio_dev->gain_configs);
	no_os_free(iio_dev);

	return 0;
}
