/***************************************************************************//**
 *   @file   max14914.c
 *   @brief  Source file of MAX14914 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdlib.h>
#include <string.h>
#include "max14914.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/**
 * @brief Set the state of the MAX14914
 * @param desc - MAX14914 device descriptor
 * @param state - State for the device to be set in.
 * @return 0 in case of succes, negative error code otherwise
*/
int max14914_set_state(struct max14914_desc *desc, enum max14914_state state)
{
	int ret;

	switch (state) {
	case MAX14914_DIGITAL_OUTPUT_PP:
		ret = no_os_gpio_set_value(desc->di_en_gpio, NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(desc->pp_gpio, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		break;
	case MAX14914_DIGITAL_OUTPUT_HIGH_SIDE:
		ret = no_os_gpio_set_value(desc->di_en_gpio, NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(desc->pp_gpio, NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(desc->in_gpio, NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		break;
	case MAX14914_DIGITAL_OUTPUT_LOW_SIDE:
		ret = no_os_gpio_set_value(desc->di_en_gpio, NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(desc->pp_gpio, NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(desc->in_gpio, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		break;
	case MAX14914_DIGITAL_INPUT_IEC_TYPE1_3:
		ret = no_os_gpio_set_value(desc->di_en_gpio, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(desc->pp_gpio, NO_OS_GPIO_LOW);
		if (ret)
			return ret;

		break;
	case MAX14914_DIGITAL_INPUT_IEC_TYPE2:
		ret = no_os_gpio_set_value(desc->di_en_gpio, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		ret = no_os_gpio_set_value(desc->pp_gpio, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;

		break;
	default:
		return -EINVAL;
	}

	desc->current_state = state;

	return 0;
}

/**
 * @brief Initialize the MAX14914 device
 * @param desc - MAX14914 device descriptor
 * @param init_param - MAX14914 initialization parameter
 * @return 0 in case of succes, negative error code otherwise
*/
int max14914_init(struct max14914_desc **desc,
		  struct max14914_init_param *init_param)
{
	struct max14914_desc *descriptor;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_gpio_get(&descriptor->di_en_gpio,
			     init_param->di_en_init_param);
	if (ret)
		goto error;

	ret = no_os_gpio_direction_output(descriptor->di_en_gpio,
					  NO_OS_GPIO_LOW);
	if (ret)
		goto di_en_error;

	ret = no_os_gpio_get(&descriptor->pp_gpio, init_param->pp_init_param);
	if (ret)
		goto di_en_error;

	ret = no_os_gpio_direction_output(descriptor->pp_gpio, NO_OS_GPIO_LOW);
	if (ret)
		goto pp_error;

	ret = no_os_gpio_get(&descriptor->in_gpio, init_param->in_init_param);
	if (ret)
		goto pp_error;

	ret = no_os_gpio_direction_output(descriptor->in_gpio, NO_OS_GPIO_LOW);
	if (ret)
		goto in_error;

	ret = no_os_gpio_get(&descriptor->doilvl_gpio,
			     init_param->doilvl_init_param);
	if (ret)
		goto in_error;

	ret = no_os_gpio_direction_input(descriptor->doilvl_gpio);
	if (ret)
		goto doilvl_error;

	ret = no_os_gpio_get(&descriptor->fault_gpio,
			     init_param->fault_init_param);
	if (ret)
		goto doilvl_error;

	ret = no_os_gpio_direction_input(descriptor->fault_gpio);
	if (ret)
		goto fault_error;

	descriptor->current_state = MAX14914_DIGITAL_OUTPUT_HIGH_SIDE;
	*desc = descriptor;

	return 0;

fault_error:
	no_os_gpio_remove(descriptor->fault_gpio);
doilvl_error:
	no_os_gpio_remove(descriptor->doilvl_gpio);
in_error:
	no_os_gpio_remove(descriptor->in_gpio);
pp_error:
	no_os_gpio_remove(descriptor->pp_gpio);
di_en_error:
	no_os_gpio_remove(descriptor->di_en_gpio);
error:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param desc - MAX14914 device descriptor
 * @return 0 in case of succes, negative error code otherwise
*/
int max14914_remove(struct max14914_desc *desc)
{
	if (!desc)
		return -ENODEV;

	no_os_gpio_remove(desc->fault_gpio);
	no_os_gpio_remove(desc->doilvl_gpio);
	no_os_gpio_remove(desc->in_gpio);
	no_os_gpio_remove(desc->pp_gpio);
	no_os_gpio_remove(desc->di_en_gpio);
	no_os_free(desc);

	return 0;
}
