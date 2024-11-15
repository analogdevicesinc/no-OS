/***************************************************************************//**
 *   @file   no_os_gpio.c
 *   @brief  Implementation of the GPIO Interface
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <inttypes.h>
#include "no_os_gpio.h"
#include <stdlib.h>
#include "no_os_error.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_gpio_get(struct no_os_gpio_desc **desc,
		       const struct no_os_gpio_init_param *param)
{
	int32_t ret;

	if (!param || !param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->gpio_ops_get)
		return -ENOSYS;

	ret = param->platform_ops->gpio_ops_get(desc, param);
	if (ret)
		return ret;

	(*desc)->platform_ops = param->platform_ops;

	return 0;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_gpio_get_optional(struct no_os_gpio_desc **desc,
				const struct no_os_gpio_init_param *param)
{
	int32_t ret;

	if (!param || (param->number == -1)) {
		*desc = NULL;
		return 0;
	}

	if (!param->platform_ops)
		return -EINVAL;

	if (!param->platform_ops->gpio_ops_get_optional)
		return -ENOSYS;

	ret = param->platform_ops->gpio_ops_get_optional(desc, param);
	if (ret)
		return ret;

	(*desc)->platform_ops = param->platform_ops;

	return 0;
}
/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (desc) {
		if (!desc->platform_ops)
			return -EINVAL;

		if (!desc->platform_ops->gpio_ops_remove)
			return -ENOSYS;

		return desc->platform_ops->gpio_ops_remove(desc);
	}

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	if (desc) {
		if (!desc->platform_ops)
			return -EINVAL;

		if (!desc->platform_ops->gpio_ops_direction_input)
			return -ENOSYS;

		return desc->platform_ops->gpio_ops_direction_input(desc);
	}

	return 0;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_gpio_direction_output(struct no_os_gpio_desc *desc,
				    uint8_t value)
{
	if (desc) {
		if (!desc->platform_ops)
			return -EINVAL;

		if (!desc->platform_ops->gpio_ops_direction_output)
			return -ENOSYS;

		return desc->platform_ops->
		       gpio_ops_direction_output(desc, value);
	}

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: NO_OS_GPIO_OUT
 *                             NO_OS_GPIO_IN
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_gpio_get_direction(struct no_os_gpio_desc *desc,
				 uint8_t *direction)
{
	if (desc) {
		if (!desc->platform_ops)
			return -EINVAL;

		if (!desc->platform_ops->gpio_ops_get_direction)
			return -ENOSYS;

		return desc->platform_ops->
		       gpio_ops_get_direction(desc, direction);
	}

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_gpio_set_value(struct no_os_gpio_desc *desc,
			     uint8_t value)
{
	if (desc) {
		if (!desc->platform_ops)
			return -EINVAL;

		if (!desc->platform_ops->gpio_ops_set_value)
			return -ENOSYS;

		return desc->platform_ops->gpio_ops_set_value(desc, value);
	}

	return 0;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_gpio_get_value(struct no_os_gpio_desc *desc,
			     uint8_t *value)
{
	if (desc) {
		if (!desc->platform_ops)
			return -EINVAL;

		if (!desc->platform_ops->gpio_ops_set_value)
			return -ENOSYS;

		return desc->platform_ops->gpio_ops_get_value(desc, value);
	}

	return 0;
}
