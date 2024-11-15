/***************************************************************************//**
 *   @file   generic/generic_gpio.c
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_gpio.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_gpio_get(struct no_os_gpio_desc **desc,
			 const struct no_os_gpio_init_param *param)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(param);

	return 0;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_gpio_get_optional(struct no_os_gpio_desc **desc,
				  const struct no_os_gpio_init_param *param)
{
	if(param == NULL) {
		*desc = NULL;
		return 0;
	}

	return no_os_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_gpio_remove(struct no_os_gpio_desc *desc)
{
	NO_OS_UNUSED_PARAM(desc);

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	NO_OS_UNUSED_PARAM(desc);

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
int32_t generic_gpio_direction_output(struct no_os_gpio_desc *desc,
				      uint8_t value)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(value);

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
int32_t generic_gpio_get_direction(struct no_os_gpio_desc *desc,
				   uint8_t *direction)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(direction);

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value taken from the enum gpio_values members.
 *                Example: NO_OS_GPIO_HIGH
 *                         NO_OS_GPIO_LOW
 *                         NO_OS_GPIO_HIGH_Z
 * @return 0 in case of success, -1 otherwise.
 */
int32_t generic_gpio_set_value(struct no_os_gpio_desc *desc,
			       uint8_t value)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(value);

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
int32_t generic_gpio_get_value(struct no_os_gpio_desc *desc,
			       uint8_t *value)
{
	NO_OS_UNUSED_PARAM(desc);
	NO_OS_UNUSED_PARAM(value);

	return 0;
}

/**
 * @brief Generic platform GPIO ops
 */
const struct no_os_gpio_platform_ops generic_gpio_ops = {
	.gpio_ops_get = &generic_gpio_get,
	.gpio_ops_get_optional = &generic_gpio_get_optional,
	.gpio_ops_remove = &generic_gpio_remove,
	.gpio_ops_direction_input = &generic_gpio_direction_input,
	.gpio_ops_direction_output = &generic_gpio_direction_output,
	.gpio_ops_get_direction = &generic_gpio_get_direction,
	.gpio_ops_set_value = &generic_gpio_set_value,
	.gpio_ops_get_value = &generic_gpio_get_value,
};
