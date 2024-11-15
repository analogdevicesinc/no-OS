/******************************************************************************
 *   @file   chibios/chibios_gpio.c
 *   @brief  Implementation of chibios gpio functionality.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdlib.h>
#include <errno.h>
#include "no_os_util.h"
#include "no_os_gpio.h"
#include "no_os_alloc.h"
#include "chibios_gpio.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Prepare the GPIO descriptor.
 * @param desc - The GPIO descriptor.
 * @param param - The structure that contains the GPIO parameters.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t _gpio_init(struct no_os_gpio_desc *desc,
			  const struct no_os_gpio_init_param *param)
{
	struct chibios_gpio_desc *extra = desc->extra;
	struct chibios_gpio_init_param *pextra = param->extra;

	if (!param)
		return -EINVAL;

	//set GPIO pad to according mode
	palSetPadMode(pextra->port, pextra->pad, pextra->mode);

	/* copy the settings to gpio descriptor */
	desc->port = (uint32_t) pextra->port;
	desc->number = (uint32_t) pextra->pad;
	desc->pull = (((uint32_t)pextra->mode & PAL_MODE_OUTPUT_PUSHPULL)
		      || ((uint32_t)pextra->mode & PAL_MODE_OUTPUT_OPENDRAIN)) ? NO_OS_GPIO_OUT :
		     NO_OS_GPIO_IN;
	extra->port = pextra->port;
	extra->pad = pextra->pad;
	extra->mode = pextra->mode;

	return 0;
}

/**
 * @brief Obtain the GPIO descriptor.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO initialization parameters
 * @return 0 in case of success, -1 otherwise.
 */
int32_t chibios_gpio_get(struct no_os_gpio_desc **desc,
			 const struct no_os_gpio_init_param *param)
{
	int32_t ret;
	struct no_os_gpio_desc *descriptor;
	struct chibios_gpio_desc *extra;

	if (!desc || !param)
		return -EINVAL;

	descriptor = (struct no_os_gpio_desc *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor) {
		ret = -ENOMEM;
		goto error_1;
	}
	extra = (struct chibios_gpio_desc*)no_os_calloc(1, sizeof(*extra));
	if (!extra) {
		ret = -ENOMEM;
		goto error_2;
	}

	descriptor->extra = extra;
	ret = _gpio_init(descriptor, param);
	if (ret)
		goto error_2;

	*desc = descriptor;

	return 0;
error_2:
	no_os_free(extra);
error_1:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Get the value of an optional GPIO.
 * @param desc - The GPIO descriptor.
 * @param param - GPIO Initialization parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t chibios_gpio_get_optional(struct no_os_gpio_desc **desc,
				  const struct no_os_gpio_init_param *param)
{
	if (param == NULL) {
		*desc = NULL;
		return 0;
	}

	return chibios_gpio_get(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gpio_get().
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t chibios_gpio_remove(struct no_os_gpio_desc *desc)
{
	if (desc != NULL)
		no_os_free(desc->extra);

	no_os_free(desc);

	return 0;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t chibios_gpio_direction_input(struct no_os_gpio_desc *desc)
{
	struct chibios_gpio_desc *extra;

	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EFAULT;

	extra = desc->extra;

	/* configure gpio with user configuration */
	extra->mode =  PAL_MODE_INPUT;
	palSetPadMode(extra->port, extra->pad, extra->mode);

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
int32_t chibios_gpio_direction_output(struct no_os_gpio_desc *desc,
				      uint8_t value)
{
	struct chibios_gpio_desc *extra;

	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EFAULT;
	extra = desc->extra;

	/* configure gpio with user configuration */
	extra->mode = PAL_MODE_OUTPUT_PUSHPULL;
	palSetPadMode(extra->port, extra->pad, extra->mode);

	/* configure gpio output level */
	palWritePad(extra->port, extra->pad, value);

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
int32_t chibios_gpio_get_direction(struct no_os_gpio_desc *desc,
				   uint8_t *direction)
{
	if (!desc || !direction)
		return -EINVAL;

	struct chibios_gpio_desc *extra = desc->extra;
	*direction = (((uint32_t)extra->mode & PAL_MODE_OUTPUT_PUSHPULL)
		      || ((uint32_t)extra->mode & PAL_MODE_OUTPUT_OPENDRAIN)) ? NO_OS_GPIO_OUT :
		     NO_OS_GPIO_IN;
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
int32_t chibios_gpio_set_value(struct no_os_gpio_desc *desc,
			       uint8_t value)
{
	struct chibios_gpio_desc *extra;

	if (!desc)
		return -EINVAL;

	if (!desc->extra)
		return -EFAULT;

	extra = desc->extra;

	/* configure gpio output level */
	palWritePad(extra->port, extra->pad, value);

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
int32_t chibios_gpio_get_value(struct no_os_gpio_desc *desc,
			       uint8_t *value)
{
	struct chibios_gpio_desc *extra;

	if (!desc || !value)
		return -EINVAL;

	if (!desc->extra)
		return -EFAULT;

	extra = desc->extra;

	*value = (uint8_t)palReadPad(extra->port, extra->pad);

	return 0;
}

/**
 * @brief chibios platform specific GPIO platform ops structure
 */
const struct no_os_gpio_platform_ops chibios_gpio_ops = {
	.gpio_ops_get = &chibios_gpio_get,
	.gpio_ops_get_optional = &chibios_gpio_get_optional,
	.gpio_ops_remove = &chibios_gpio_remove,
	.gpio_ops_direction_input = &chibios_gpio_direction_input,
	.gpio_ops_direction_output = &chibios_gpio_direction_output,
	.gpio_ops_get_direction = &chibios_gpio_get_direction,
	.gpio_ops_set_value = &chibios_gpio_set_value,
	.gpio_ops_get_value = &chibios_gpio_get_value,
};
