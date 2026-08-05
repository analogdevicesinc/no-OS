/***************************************************************************//**
 *   @file   adg1736.c
 *   @brief  Implementation of ADG1736/ADG2736 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "adg1736.h"

/**
 * @brief Set the state of a specific switch.
 * @param dev - The device structure.
 * @param sw - The switch to control (ADG1736_SW1 or ADG1736_SW2).
 * @param state - The desired state (ADG1736_CONNECT_A or ADG1736_CONNECT_B).
 * @return 0 in case of success, negative error code otherwise.
 */
int adg1736_set_switch_state(struct adg1736_dev *dev,
			     enum adg1736_switch sw,
			     enum adg1736_state state)
{
	struct no_os_gpio_desc *gpio;
	uint8_t value;

	if (!dev)
		return -EINVAL;

	switch (sw) {
	case ADG1736_SW1:
		gpio = dev->gpio_in1;
		break;
	case ADG1736_SW2:
		gpio = dev->gpio_in2;
		break;
	default:
		return -EINVAL;
	}

	if (!gpio)
		return -EINVAL;

	value = (state == ADG1736_CONNECT_B) ? NO_OS_GPIO_LOW : NO_OS_GPIO_HIGH;

	return no_os_gpio_set_value(gpio, value);
}

/**
 * @brief Get the current state of a specific switch.
 * @param dev - The device structure.
 * @param sw - The switch to read (ADG1736_SW1 or ADG1736_SW2).
 * @param state - Pointer to store the current state.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg1736_get_switch_state(struct adg1736_dev *dev,
			     enum adg1736_switch sw,
			     enum adg1736_state *state)
{
	struct no_os_gpio_desc *gpio;
	uint8_t value;
	int ret;

	if (!dev || !state)
		return -EINVAL;

	switch (sw) {
	case ADG1736_SW1:
		gpio = dev->gpio_in1;
		break;
	case ADG1736_SW2:
		gpio = dev->gpio_in2;
		break;
	default:
		return -EINVAL;
	}

	if (!gpio)
		return -EINVAL;

	ret = no_os_gpio_get_value(gpio, &value);
	if (ret)
		return ret;

	*state = (value == NO_OS_GPIO_HIGH) ? ADG1736_CONNECT_A : ADG1736_CONNECT_B;

	return 0;
}

/**
 * @brief Initialize the ADG1736 device.
 * @param device - Pointer to the device structure pointer.
 * @param init_param - Pointer to the initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg1736_init(struct adg1736_dev **device,
		 struct adg1736_init_param *init_param)
{
	struct adg1736_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	if (init_param->type != ADG1736 && init_param->type != ADG2736 &&
	    init_param->gpio_en)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->type = init_param->type;

	ret = no_os_gpio_get(&dev->gpio_in1, &init_param->gpio_in1);
	if (ret)
		goto error_dev;

	ret = no_os_gpio_get(&dev->gpio_in2, &init_param->gpio_in2);
	if (ret)
		goto error_gpio1;

	ret = no_os_gpio_direction_output(dev->gpio_in1, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio2;

	ret = no_os_gpio_direction_output(dev->gpio_in2, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio2;

	if (dev->type == ADG1736 || dev->type == ADG2736) {
		ret = no_os_gpio_get_optional(&dev->gpio_en, init_param->gpio_en);
		if (ret)
			goto error_gpio2;

		if (dev->gpio_en) {
			ret = no_os_gpio_direction_output(dev->gpio_en, NO_OS_GPIO_HIGH);
			if (ret)
				goto error_gpio_en;
		}
	}

	*device = dev;

	return 0;

error_gpio_en:
	no_os_gpio_remove(dev->gpio_en);
error_gpio2:
	no_os_gpio_remove(dev->gpio_in2);
error_gpio1:
	no_os_gpio_remove(dev->gpio_in1);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free resources allocated by adg1736_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg1736_remove(struct adg1736_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_gpio_remove(dev->gpio_in1);
	no_os_gpio_remove(dev->gpio_in2);
	if ((dev->type == ADG1736 || dev->type == ADG2736) && dev->gpio_en)
		no_os_gpio_remove(dev->gpio_en);
	no_os_free(dev);

	return 0;
}

/**
 * @brief Enable the mux (ADG1736/ADG2736 only, requires EN pin).
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg1736_enable(struct adg1736_dev *dev)
{
	if (!dev)
		return -EINVAL;

	if (dev->type != ADG1736 && dev->type != ADG2736)
		return -ENOTSUP;

	if (!dev->gpio_en)
		return -EINVAL;

	return no_os_gpio_set_value(dev->gpio_en, NO_OS_GPIO_HIGH);
}

/**
 * @brief Disable the mux (ADG1736/ADG2736 only, requires EN pin).
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg1736_disable(struct adg1736_dev *dev)
{
	if (!dev)
		return -EINVAL;

	if (dev->type != ADG1736 && dev->type != ADG2736)
		return -ENOTSUP;

	if (!dev->gpio_en)
		return -EINVAL;

	return no_os_gpio_set_value(dev->gpio_en, NO_OS_GPIO_LOW);
}
