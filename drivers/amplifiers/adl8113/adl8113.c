/***************************************************************************//**
 *   @file   adl8113.c
 *   @brief  Implementation of ADL8113 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
#include <stdlib.h>
#include "adl8113.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/**
 * Set operation mode (LNA or Bypass).
 * @param dev - The device structure.
 * @param mode - Operation mode (ADL8113_LNA_MODE or ADL8113_BYPASS_MODE).
 * @return 0 in case of success, negative error code otherwise.
 */
int adl8113_set_mode(struct adl8113_dev *dev, enum adl8113_mode mode)
{
	int ret;

	if (!dev)
		return -EINVAL;

	switch (mode) {
	case ADL8113_INTERNAL_AMPLIFIER:
		ret = no_os_gpio_set_value(dev->gpio_va, 0);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->gpio_vb, 0);
		if (ret)
			return ret;
		break;

	case ADL8113_INTERNAL_BYPASS:
		ret = no_os_gpio_set_value(dev->gpio_va, 1);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->gpio_vb, 1);
		if (ret)
			return ret;
		break;

	case ADL8113_EXTERNAL_BYPASS_A:
		ret = no_os_gpio_set_value(dev->gpio_va, 0);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->gpio_vb, 1);
		if (ret)
			return ret;
		break;

	case ADL8113_EXTERNAL_BYPASS_B:
		ret = no_os_gpio_set_value(dev->gpio_va, 1);
		if (ret)
			return ret;
		ret = no_os_gpio_set_value(dev->gpio_vb, 0);
		if (ret)
			return ret;
		break;

	default:
		return -EINVAL;
	}

	dev->current_mode = mode;

	return 0;
}

/**
 * Get current operation mode.
 * @param dev - The device structure.
 * @param mode - Pointer to store the current operation mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int adl8113_get_mode(struct adl8113_dev *dev, enum adl8113_mode *mode)
{
	if (!dev || !mode)
		return -EINVAL;

	*mode = dev->current_mode;

	return 0;
}


/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adl8113_init(struct adl8113_dev **device,
		 struct adl8113_init_param *init_param)
{
	struct adl8113_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct adl8113_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_gpio_get(&dev->gpio_va, &init_param->gpio_va);
	if (ret)
		goto error_va;

	ret = no_os_gpio_get(&dev->gpio_vb, &init_param->gpio_vb);
	if (ret)
		goto error_vb;

	ret = no_os_gpio_direction_output(dev->gpio_va, 0);
	if (ret)
		goto error_config;

	ret = no_os_gpio_direction_output(dev->gpio_vb, 0);
	if (ret)
		goto error_config;

	ret = adl8113_set_mode(dev, init_param->initial_mode);
	if (ret)
		goto error_config;

	*device = dev;

	return 0;

error_config:
	no_os_gpio_remove(dev->gpio_vb);
error_vb:
	no_os_gpio_remove(dev->gpio_va);
error_va:
	no_os_free(dev);

	return ret;
}

/**
 * Free the resources allocated by adl8113_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adl8113_remove(struct adl8113_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_gpio_remove(dev->gpio_va);
	no_os_gpio_remove(dev->gpio_vb);

	no_os_free(dev);

	return 0;
}