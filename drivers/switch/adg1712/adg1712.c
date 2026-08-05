/***************************************************************************//**
 *   @file   adg1712.c
 *   @brief  Implementation of ADG1712/ADG2712 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "adg1712.h"

/**
 * @brief Set the state of a specific switch.
 * @param dev - The device structure.
 * @param sw - The switch to control (ADG1712_SW1 to ADG1712_SW4).
 * @param enable - true to enable the switch, false to disable.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg1712_set_switch_state(struct adg1712_dev *dev,
			     enum adg1712_switch sw,
			     bool enable)
{
	struct no_os_gpio_desc *gpio;

	if (!dev)
		return -EINVAL;

	switch (sw) {
	case ADG1712_SW1:
		gpio = dev->gpio_in1;
		break;
	case ADG1712_SW2:
		gpio = dev->gpio_in2;
		break;
	case ADG1712_SW3:
		gpio = dev->gpio_in3;
		break;
	case ADG1712_SW4:
		gpio = dev->gpio_in4;
		break;
	default:
		return -EINVAL;
	}

	if (!gpio)
		return -EINVAL;

	return no_os_gpio_set_value(gpio, enable);
}

/**
 * @brief Get the current state of a specific switch.
 * @param dev - The device structure.
 * @param sw - The switch to read (ADG1712_SW1 to ADG1712_SW4).
 * @param enable - Pointer to store the current state (true if enabled).
 * @return 0 in case of success, negative error code otherwise.
 */
int adg1712_get_switch_state(struct adg1712_dev *dev,
			     enum adg1712_switch sw,
			     bool *enable)
{
	struct no_os_gpio_desc *gpio;
	uint8_t value;
	int ret;

	if (!dev || !enable)
		return -EINVAL;

	switch (sw) {
	case ADG1712_SW1:
		gpio = dev->gpio_in1;
		break;
	case ADG1712_SW2:
		gpio = dev->gpio_in2;
		break;
	case ADG1712_SW3:
		gpio = dev->gpio_in3;
		break;
	case ADG1712_SW4:
		gpio = dev->gpio_in4;
		break;
	default:
		return -EINVAL;
	}

	if (!gpio)
		return -EINVAL;

	ret = no_os_gpio_get_value(gpio, &value);
	if (ret)
		return ret;

	*enable = (value == NO_OS_GPIO_HIGH);

	return 0;
}

/**
 * @brief Initialize the ADG1712 device.
 * @param device - Pointer to the device structure pointer.
 * @param init_param - Pointer to the initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg1712_init(struct adg1712_dev **device,
		 struct adg1712_init_param *init_param)
{
	struct adg1712_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_gpio_get(&dev->gpio_in1, &init_param->gpio_in1);
	if (ret)
		goto error_dev;

	ret = no_os_gpio_get(&dev->gpio_in2, &init_param->gpio_in2);
	if (ret)
		goto error_gpio1;

	ret = no_os_gpio_get(&dev->gpio_in3, &init_param->gpio_in3);
	if (ret)
		goto error_gpio2;

	ret = no_os_gpio_get(&dev->gpio_in4, &init_param->gpio_in4);
	if (ret)
		goto error_gpio3;

	ret = no_os_gpio_direction_output(dev->gpio_in1, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio4;

	ret = no_os_gpio_direction_output(dev->gpio_in2, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio4;

	ret = no_os_gpio_direction_output(dev->gpio_in3, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio4;

	ret = no_os_gpio_direction_output(dev->gpio_in4, NO_OS_GPIO_LOW);
	if (ret)
		goto error_gpio4;

	dev->chip_id = init_param->chip_id;

	*device = dev;

	return 0;

error_gpio4:
	no_os_gpio_remove(dev->gpio_in4);
error_gpio3:
	no_os_gpio_remove(dev->gpio_in3);
error_gpio2:
	no_os_gpio_remove(dev->gpio_in2);
error_gpio1:
	no_os_gpio_remove(dev->gpio_in1);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free resources allocated by adg1712_init().
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adg1712_remove(struct adg1712_dev *dev)
{
	if (!dev)
		return -EINVAL;

	no_os_gpio_remove(dev->gpio_in1);
	no_os_gpio_remove(dev->gpio_in2);
	no_os_gpio_remove(dev->gpio_in3);
	no_os_gpio_remove(dev->gpio_in4);
	no_os_free(dev);

	return 0;
}
