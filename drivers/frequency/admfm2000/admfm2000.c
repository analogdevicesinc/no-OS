/***************************************************************************//**
 *   @file   admfm2000.c
 *   @brief  Implementation of admfm2000 Driver.
 *   @author Ramona Nechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "admfm2000.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_print_log.h"

/**
 * @brief Get the value of the dsa pins of the device.
 * @param dev - The device structure.
 * @param chan - The channel for which the value is read.
 * @param dsa_raw - The raw value from DSA gpios.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admfm2000_get_dsa_raw(struct admfm2000_dev *dev, uint8_t chan,
			  int32_t *dsa_raw)
{
	int32_t ret;
	uint8_t i;
	uint8_t bit;
	int32_t tmp = 0;

	if (chan > 1)
		return -EINVAL;

	for (i = 0; i < ADMFM2000_DSA_GPIOS; i++) {
		ret = no_os_gpio_get_value(dev->gpio_dsa[chan][i], &bit);
		if (ret != 0)
			return ret;
		tmp |= (bit << i);
	}

	*dsa_raw = tmp;

	return 0;
}

/**
 * @brief Set the DSA GPIOs of the device.
 * @param dev - The device structure.
 * @param chan - The channel for which the value is set.
 * @param dsa_raw - The raw value for GPIOs.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admfm2000_set_dsa_raw(struct admfm2000_dev *dev, uint8_t chan,
			  int32_t dsa_raw)
{
	int32_t ret;
	uint8_t i;

	if (dsa_raw > ADMFM2000_MAX_GAIN_RAW || dsa_raw < ADMFM2000_MIN_GAIN_RAW)
		return -EINVAL;

	for (i = 0; i < ADMFM2000_DSA_GPIOS; i++) {
		ret = no_os_gpio_set_value(dev->gpio_dsa[chan][i],
					   (dsa_raw & (1 << i)) >> i);
		if (ret != 0)
			return ret;
	}

	return 0;
}

/**
 * @brief Set the channel configuration.
 * @param dev - The device structure.
 * @param config - The config
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admfm2000_set_channel_config(struct admfm2000_dev *dev, uint8_t config)
{
	int32_t ret;
	int32_t i;

	if (config > 1)
		return -EINVAL;

	if (config) {
		ret = no_os_gpio_set_value(dev->gpio_sw[0][0], 0);
		ret = no_os_gpio_set_value(dev->gpio_sw[0][1], 1);
		ret = no_os_gpio_set_value(dev->gpio_sw[1][0], 1);
		ret = no_os_gpio_set_value(dev->gpio_sw[1][1], 0);
	} else {
		ret = no_os_gpio_set_value(dev->gpio_sw[0][0], 1);
		ret = no_os_gpio_set_value(dev->gpio_sw[0][1], 0);
		ret = no_os_gpio_set_value(dev->gpio_sw[1][0], 0);
		ret = no_os_gpio_set_value(dev->gpio_sw[1][1], 1);
	}

	return ret;
}

/**
 * @brief Initialize the admfm2000 device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial parameters.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int admfm2000_init(struct admfm2000_dev **device,
		   struct admfm2000_init_param *init_param)
{
	int32_t ret;
	uint8_t i, j;
	struct admfm2000_dev *dev;

	dev = (struct admfm2000_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	for (i = 0; i < ADMFM2000_NUM_CHANNELS; i++) {
		for (j = 0; j < ADMFM2000_MODE_GPIOS; j++) {
			ret = no_os_gpio_get(&dev->gpio_sw[i][j],
					     init_param->gpio_sw_param[i][j]);
			if (ret)
				goto error;
			ret = no_os_gpio_direction_output(dev->gpio_sw[i][j],
							  NO_OS_GPIO_HIGH);
			if (ret)
				goto error;
		}
	}

	for (i = 0; i < ADMFM2000_NUM_CHANNELS; i++) {
		for (j = 0; j < ADMFM2000_DSA_GPIOS; j++) {
			ret = no_os_gpio_get(&dev->gpio_dsa[i][j],
					     init_param->gpio_dsa_param[i][j]);
			if (ret)
				goto error;
			ret = no_os_gpio_direction_output(dev->gpio_dsa[i][j],
							  NO_OS_GPIO_HIGH);
			if (ret)
				goto error;
		}
	}

	ret = admfm2000_set_channel_config(dev, init_param->mixer_mode);
	if (ret)
		goto error;

	ret = admfm2000_set_dsa_raw(dev, 0, init_param->dsa_gain);
	if (ret)
		goto error;

	ret = admfm2000_set_dsa_raw(dev, 1, init_param->dsa_gain);
	if (ret)
		goto error;

	*device = dev;

	return 0;

error:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by admfm2000_init().
 * @param dev - The device structure.
 * @return ret - Result of the remove procedure.
 */
int admfm2000_remove(struct admfm2000_dev *dev)
{
	uint8_t i, j;

	for (i = 0; i < ADMFM2000_NUM_CHANNELS; i++)
		for (j = 0; j < ADMFM2000_MODE_GPIOS; j++)
			no_os_gpio_remove(dev->gpio_sw[i][j]);

	for (i = 0; i < ADMFM2000_NUM_CHANNELS; i++)
		for (j = 0; j < ADMFM2000_DSA_GPIOS; j++)
			no_os_gpio_remove(dev->gpio_dsa[i][j]);

	no_os_free(dev);

	return 0;
}
