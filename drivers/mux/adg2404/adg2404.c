/***************************************************************************//**
 *   @file   adg2404.c
 *   @brief  Implementation of ADG2404 Driver.
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
#include "adg2404.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/**
 * Select the multiplexer channel.
 * @param dev - The device structure.
 * @param channel - Channel to select (ADG2404_CH_OFF, ADG2404_S1 to ADG2404_S4).
 * @return 0 in case of success, negative error code otherwise.
 */
int adg2404_select_channel(struct adg2404_dev *dev,
			   enum adg2404_channel channel)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (channel > ADG2404_S4)
		return -EINVAL;

	if (channel == ADG2404_CH_OFF)
		return adg2404_enable(dev, false);

	ret = adg2404_enable(dev, true);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_a0, (channel - 1) & 0x01);
	if (ret)
		return ret;

	return no_os_gpio_set_value(dev->gpio_a1, ((channel - 1) >> 1) & 0x01);
}

/**
 * Enable/disable the multiplexer.
 * @param dev - The device structure.
 * @param enable - true to enable, false to disable.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg2404_enable(struct adg2404_dev *dev, bool enable)
{
	if (!dev)
		return -EINVAL;

	return no_os_gpio_set_value(dev->gpio_en, enable);
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg2404_init(struct adg2404_dev **device,
		 struct adg2404_init_param *init_param)
{
	struct adg2404_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct adg2404_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_gpio_get(&dev->gpio_a0, &init_param->gpio_a0);
	if (ret)
		goto error_a0;

	ret = no_os_gpio_get(&dev->gpio_a1, &init_param->gpio_a1);
	if (ret)
		goto error_a1;

	ret = no_os_gpio_get(&dev->gpio_en, &init_param->gpio_en);
	if (ret)
		goto error_en;

	ret = no_os_gpio_direction_output(dev->gpio_a0, NO_OS_GPIO_LOW);
	if (ret)
		goto error_config;

	ret = no_os_gpio_direction_output(dev->gpio_a1, NO_OS_GPIO_LOW);
	if (ret)
		goto error_config;

	ret = no_os_gpio_direction_output(dev->gpio_en, NO_OS_GPIO_LOW);
	if (ret)
		goto error_config;


	*device = dev;

	return 0;

error_config:
	no_os_gpio_remove(dev->gpio_en);
error_en:
	no_os_gpio_remove(dev->gpio_a1);
error_a1:
	no_os_gpio_remove(dev->gpio_a0);
error_a0:
	no_os_free(dev);

	return ret;
}

/**
 * Free the resources allocated by adg2404_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg2404_remove(struct adg2404_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_gpio_remove(dev->gpio_a0);
	no_os_gpio_remove(dev->gpio_a1);
	no_os_gpio_remove(dev->gpio_en);

	no_os_free(dev);

	return 0;
}
